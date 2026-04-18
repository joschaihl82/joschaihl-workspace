// unused-optimizer1.cpp
// Single-file tool: inline only #include "file.h" (no macro expansion) using Clang preprocessor callbacks,
// then run AST-based cleanup (print used enums, remove unused static functions/types).
// This version does NOT use any external compilation database. All compile flags are provided
// via command-line fallback or defaults inside this file.
//
// Usage:
//   ./unused-optimizer1 file1.c file2.c -- -Iinclude -DDEBUG -std=c11
// If no "--" and flags are provided, the tool uses the built-in defaults: -std=c11 -I.
//
// Build with LLVM/Clang dev environment (point -DLLVM_DIR to your LLVM cmake dir).
// Example build steps (adjust LLVM_DIR):
//   rm -rf build
//   cmake -S . -B build -DLLVM_DIR=/usr/lib/llvm-20/lib/cmake/llvm
//   cmake --build build -- -j

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Basic/DiagnosticOptions.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/PPCallbacks.h>
#include <clang/Lex/Token.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Basic/FileManager.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/LangOptions.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/TypeLoc.h>
#include <clang/Lex/Lexer.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>

#include <set>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <system_error>

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

static const char *DEFAULT_FLAGS[] = { "-std=c11", "-I." };

// Range info for removal
struct RangeInfo {
  unsigned startOffset;
  unsigned endOffset; // exclusive
  std::string name;
};

// Helper: read file to string
static std::string readFile(const std::string &path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) return {};
  std::ostringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

// Canonicalize path if possible
static std::string canonicalize(const std::string &p) {
  std::error_code ec;
  SmallString<256> can;
  ec = llvm::sys::fs::real_path(p, can);
  if (!ec) return std::string(can.str());
  return p;
}

// -------------------- Include discovery (no macro expansion) --------------------

// PPCallbacks subclass that records double-quoted includes and their source locations
class IncludeRecorder : public PPCallbacks {
public:
  struct IncludeEntry {
    SourceLocation loc;      // location of the '#' token (start of directive)
    std::string resolved;    // resolved file path (canonical if possible)
  };
  std::vector<IncludeEntry> includes;

  // Do not use 'override' to avoid signature mismatch across Clang versions
  void InclusionDirective(SourceLocation HashLoc,
                          const Token &IncludeTok,
                          StringRef FileName,
                          bool IsAngled,
                          CharSourceRange FilenameRange,
                          const FileEntry *File,
                          StringRef SearchPath,
                          StringRef RelativePath,
                          const Module *Imported) {
    // Only record double-quoted includes (IsAngled == false)
    if (IsAngled) return;

    // Prefer SearchPath + RelativePath (this is how the preprocessor reports the resolved path).
    std::string resolved;
    if (!SearchPath.empty() && !RelativePath.empty()) {
      SmallString<256> tmp(SearchPath);
      llvm::sys::path::append(tmp, RelativePath);
      resolved = std::string(tmp.str());
    } else if (!RelativePath.empty()) {
      resolved = std::string(RelativePath);
    } else if (!SearchPath.empty()) {
      resolved = std::string(SearchPath);
    } else {
      // If both are empty, we cannot reliably resolve; skip recording.
      return;
    }

    // Canonicalize and record
    resolved = canonicalize(resolved);
    includes.push_back({HashLoc, resolved});
  }
};

// Compute the full line range (start offset, end offset exclusive) in buffer for a SourceLocation
static bool computeLineRange(const SourceManager &SM, SourceLocation Loc, unsigned &outStart, unsigned &outEnd) {
  if (!Loc.isValid()) return false;
  SourceLocation SpellingLoc = SM.getSpellingLoc(Loc);
  FileID fid = SM.getFileID(SpellingLoc);
  if (fid.isInvalid()) return false;
  StringRef buf = SM.getBufferData(fid);
  unsigned off = SM.getFileOffset(SpellingLoc);
  // find line start
  unsigned s = off;
  while (s > 0 && buf[s-1] != '\n' && buf[s-1] != '\r') --s;
  // find line end (first newline after off)
  unsigned e = off;
  while (e < buf.size() && buf[e] != '\n' && buf[e] != '\r') ++e;
  // include the newline(s)
  if (e < buf.size() && buf[e] == '\r') ++e;
  if (e < buf.size() && buf[e] == '\n') ++e;
  outStart = s;
  outEnd = e;
  return true;
}

// Inline includes recursively into a buffer string. `inlined` is a set of canonicalized paths already inlined.
static std::string inlineIncludesText(const std::string &content, std::set<std::string> &inlined) {
  // Create a temporary CompilerInstance to run preprocessor only to discover includes
  CompilerInstance CI;

  IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts(new DiagnosticOptions());
  TextDiagnosticPrinter *DiagClient = new TextDiagnosticPrinter(llvm::errs(), &*DiagOpts);

  // Create FileManager first
  CI.createFileManager();

  // Create diagnostics (VFS-aware overload) BEFORE creating the SourceManager.
  CI.createDiagnostics(CI.getFileManager().getVirtualFileSystem(),
                       DiagOpts.get(),
                       DiagClient,
                       /*ShouldOwnClient=*/false,
                       /*CodeGenOpts=*/nullptr);

  // Now create SourceManager (diagnostics are available)
  CI.createSourceManager(CI.getFileManager());

  // Minimal invocation: set language to C
  CI.getInvocation().getLangOpts().C11 = 1;

  // Create preprocessor
  CI.createPreprocessor(TU_Complete);
  Preprocessor &PP = CI.getPreprocessor();

  // Create a memory buffer for the content and set as main file
  std::unique_ptr<llvm::MemoryBuffer> buf = llvm::MemoryBuffer::getMemBuffer(content, "<inline-temp>");
  FileID fid = CI.getSourceManager().createFileID(std::move(buf));
  CI.getSourceManager().setMainFileID(fid);

  // Create a recorder and add it to PP callbacks, keep pointer to it
  auto recPtr = std::make_unique<IncludeRecorder>();
  IncludeRecorder *rec = recPtr.get();
  PP.addPPCallbacks(std::move(recPtr));

  // Enter main file and lex to trigger callbacks (we don't use tokens)
  PP.EnterMainSourceFile();
  Token tok;
  while (true) {
    PP.Lex(tok);
    if (tok.is(tok::eof)) break;
  }

  // If no includes, return original content
  if (rec->includes.empty()) return content;

  // Build a list of replacements: for each include, compute line range and resolved path
  struct Replacement { unsigned s,e; std::string replacement; };
  std::vector<Replacement> reps;
  for (auto &inc : rec->includes) {
    unsigned s,e;
    if (!computeLineRange(CI.getSourceManager(), inc.loc, s, e)) continue;
    // If resolved path already inlined, skip (guard)
    if (inlined.count(inc.resolved)) {
      // remove the include line entirely
      reps.push_back({s,e,""});
      continue;
    }
    // Read included file raw contents
    std::string incText = readFile(inc.resolved);
    if (incText.empty()) {
      // cannot read: keep original include line (do nothing)
      continue;
    }
    // Mark as inlined
    inlined.insert(inc.resolved);
    // Recursively inline includes inside the included file.
    std::string recursed = inlineIncludesText(incText, inlined);
    // Ensure trailing newline
    if (!recursed.empty() && recursed.back() != '\n') recursed.push_back('\n');
    reps.push_back({s,e,recursed});
  }

  if (reps.empty()) return content;

  // Merge overlapping replacements and apply them descending order
  std::sort(reps.begin(), reps.end(), [](auto &a, auto &b){ return a.s < b.s; });
  std::vector<Replacement> merged;
  merged.push_back(reps[0]);
  for (size_t i=1;i<reps.size();++i) {
    auto &cur = merged.back();
    if (reps[i].s <= cur.e) {
      cur.e = std::max(cur.e, reps[i].e);
      // keep cur.replacement as-is
    } else {
      merged.push_back(reps[i]);
    }
  }

  // Apply replacements from end to start
  std::string out = content;
  for (int i = (int)merged.size()-1; i>=0; --i) {
    auto &r = merged[i];
    if (r.s >= out.size()) continue;
    unsigned e = std::min<unsigned>(r.e, out.size());
    out.erase(r.s, e - r.s);
    out.insert(r.s, r.replacement);
  }

  return out;
}

// -------------------- AST-based collector and cleaner (conservative) --------------------

struct CollectorState {
  std::map<const FunctionDecl*, RangeInfo> funcDefs;
  std::set<const FunctionDecl*> funcCalled;
  std::set<const FunctionDecl*> funcAddrTaken;

  std::map<const TagDecl*, RangeInfo> tagDefs;
  std::map<const TypedefNameDecl*, RangeInfo> typedefDefs;
  std::set<const TagDecl*> tagUsed;
  std::set<const TypedefNameDecl*> typedefUsed;

  std::map<const EnumDecl*, std::vector<std::string>> enumConstantsUsed;

  std::string fileName;
};

class PerFileCollector : public RecursiveASTVisitor<PerFileCollector> {
  ASTContext &Ctx;
  SourceManager &SM;
  LangOptions LO;
  CollectorState &State;

public:
  PerFileCollector(ASTContext &Context, CollectorState &S)
    : Ctx(Context), SM(Context.getSourceManager()), LO(Context.getLangOpts()), State(S) {}

  bool recordRangeForDecl(SourceLocation B, SourceLocation E, RangeInfo &out) {
    if (!B.isValid() || !E.isValid()) return false;
    if (!SM.isWrittenInSameFile(B, E)) return false;
    FileID fid = SM.getFileID(B);
    if (fid.isInvalid()) return false;
    SourceLocation realEnd = Lexer::getLocForEndOfToken(E, 0, SM, LO);
    if (!realEnd.isValid()) return false;
    unsigned start = SM.getFileOffset(B);
    unsigned end = SM.getFileOffset(realEnd);
    // include trailing semicolon/newline if present
    StringRef buf = SM.getBufferData(fid);
    unsigned i = end;
    while (i < buf.size() && (buf[i]==' '||buf[i]=='\t')) ++i;
    if (i < buf.size() && buf[i]==';') {
      unsigned j = i+1;
      while (j < buf.size() && (buf[j]==' '||buf[j]=='\t')) ++j;
      if (j < buf.size() && (buf[j]=='\r' || buf[j]=='\n')) {
        while (j < buf.size() && (buf[j]=='\r' || buf[j]=='\n')) ++j;
      }
      end = j;
    }
    out.startOffset = start;
    out.endOffset = end;
    return true;
  }

  bool VisitFunctionDecl(FunctionDecl *FD) {
    if (FD->isThisDeclarationADefinition()) {
      SourceLocation B = FD->getSourceRange().getBegin();
      SourceLocation E = FD->getSourceRange().getEnd();
      RangeInfo ri{0,0,FD->getNameAsString()};
      if (recordRangeForDecl(B,E,ri)) {
        State.funcDefs[FD] = ri;
        State.fileName = SM.getFilename(B).str();
      }
    }
    if (TypeSourceInfo *TSI = FD->getTypeSourceInfo()) VisitTypeLoc(TSI->getTypeLoc());
    return true;
  }

  bool VisitCallExpr(CallExpr *CE) {
    if (FunctionDecl *Callee = CE->getDirectCallee()) State.funcCalled.insert(Callee);
    return true;
  }

  bool VisitUnaryOperator(UnaryOperator *UO) {
    if (UO->getOpcode() == UO_AddrOf) {
      Expr *sub = UO->getSubExpr()->IgnoreParenImpCasts();
      if (DeclRefExpr *DRE = dyn_cast<DeclRefExpr>(sub)) {
        if (FunctionDecl *FD = dyn_cast<FunctionDecl>(DRE->getDecl())) State.funcAddrTaken.insert(FD);
      }
    }
    return true;
  }

  bool VisitDeclRefExpr(DeclRefExpr *DRE) {
    if (ValueDecl *VD = DRE->getDecl()) {
      if (FunctionDecl *FD = dyn_cast<FunctionDecl>(VD)) {
        QualType QT = DRE->getType();
        if (!QT.isNull() && (QT->isPointerType() || QT->isFunctionPointerType())) State.funcAddrTaken.insert(FD);
      }
      if (EnumConstantDecl *ECD = dyn_cast<EnumConstantDecl>(VD)) {
        if (DeclContext *DC = ECD->getDeclContext()) {
          if (EnumDecl *ED = dyn_cast<EnumDecl>(DC)) {
            const EnumDecl *def = ED->getDefinition() ? ED->getDefinition() : ED;
            State.enumConstantsUsed[def].push_back(ECD->getNameAsString());
            State.tagUsed.insert(def);
          }
        }
      }
    }
    return true;
  }

  bool VisitTagDecl(TagDecl *TD) {
    if (!TD->isThisDeclarationADefinition()) return true;
    SourceLocation B = TD->getBeginLoc();
    SourceLocation E = TD->getEndLoc();
    RangeInfo ri{0,0,TD->getNameAsString()};
    if (!recordRangeForDecl(B,E,ri)) return true;
    State.tagDefs[TD] = ri;
    State.fileName = SM.getFilename(B).str();
    return true;
  }

  bool VisitTypedefNameDecl(TypedefNameDecl *TD) {
    SourceLocation B = TD->getBeginLoc();
    SourceLocation E = TD->getEndLoc();
    RangeInfo ri{0,0,TD->getNameAsString()};
    if (!recordRangeForDecl(B,E,ri)) return true;
    State.typedefDefs[TD] = ri;
    State.fileName = SM.getFilename(B).str();
    return true;
  }

  bool VisitTypeLoc(TypeLoc TL) {
    if (TL.getType().isNull()) return true;
    if (TagTypeLoc TTL = TL.getAs<TagTypeLoc>()) {
      if (TagDecl *TD = TTL.getDecl()) {
        const TagDecl *def = TD->getDefinition() ? TD->getDefinition() : TD;
        State.tagUsed.insert(def);
      }
    }
    if (TypedefTypeLoc TDL = TL.getAs<TypedefTypeLoc>()) {
      if (TypedefNameDecl *TD = TDL.getTypedefNameDecl()) State.typedefUsed.insert(TD);
    }
    return true;
  }

  bool VisitCXXBaseSpecifier(CXXBaseSpecifier const &B) {
    if (TypeSourceInfo *TSI = B.getTypeSourceInfo()) VisitTypeLoc(TSI->getTypeLoc());
    return true;
  }

  bool VisitVarDecl(VarDecl *VD) {
    if (TypeSourceInfo *TSI = VD->getTypeSourceInfo()) VisitTypeLoc(TSI->getTypeLoc());
    return true;
  }
  bool VisitFieldDecl(FieldDecl *FD) {
    if (TypeSourceInfo *TSI = FD->getTypeSourceInfo()) VisitTypeLoc(TSI->getTypeLoc());
    return true;
  }
  bool VisitParmVarDecl(ParmVarDecl *PD) {
    if (TypeSourceInfo *TSI = PD->getTypeSourceInfo()) VisitTypeLoc(TSI->getTypeLoc());
    return true;
  }
};

class PerFileConsumer : public ASTConsumer {
  ASTContext *Ctx;
public:
  PerFileConsumer(ASTContext *C) : Ctx(C) {}

  void HandleTranslationUnit(ASTContext &Context) override {
    CollectorState S;
    PerFileCollector V(Context, S);
    V.TraverseDecl(Context.getTranslationUnitDecl());

    if (S.fileName.empty()) {
      llvm::outs() << "Skipping TU with no file backing\n";
      return;
    }

    // Print enums that have enumerators used in this file
    if (!S.enumConstantsUsed.empty()) {
      SourceManager &SM = Context.getSourceManager();
      LangOptions LO = Context.getLangOpts();
      for (auto &entry : S.enumConstantsUsed) {
        const EnumDecl *ED = entry.first;
        const TagDecl *TD = ED;
        SourceRange SR = TD->getSourceRange();
        SourceLocation B = SR.getBegin();
        SourceLocation E = TD->getEndLoc();
        if (B.isValid() && E.isValid() && SM.isWrittenInSameFile(B, E)) {
          SourceLocation realEnd = Lexer::getLocForEndOfToken(E, 0, SM, LO);
          if (realEnd.isValid()) {
            CharSourceRange CR = CharSourceRange::getCharRange(B, realEnd);
            StringRef src = Lexer::getSourceText(CR, SM, LO);
            llvm::outs() << src << "\n";
          }
        }
      }
    }

    // Decide removable functions (conservative)
    std::vector<RangeInfo> funcRemovals;
    for (auto &p : S.funcDefs) {
      const FunctionDecl *FD = p.first;
      const RangeInfo &ri = p.second;
      if (FD->getStorageClass() != SC_Static) continue;
      if (S.funcCalled.count(FD)) continue;
      if (S.funcAddrTaken.count(FD)) continue;
      if (FD->hasAttr<UsedAttr>() || FD->hasAttr<ConstructorAttr>() || FD->hasAttr<DestructorAttr>()) continue;
      funcRemovals.push_back(ri);
    }

    // Decide removable types
    std::vector<RangeInfo> typeRemovals;
    for (auto &p : S.tagDefs) {
      const TagDecl *TD = p.first;
      const RangeInfo &ri = p.second;
      const TagDecl *def = TD->getDefinition() ? TD->getDefinition() : TD;
      if (S.tagUsed.count(def)) continue;
      if (const EnumDecl *ED = dyn_cast<EnumDecl>(def)) {
        if (S.enumConstantsUsed.count(ED)) continue;
      }
      if (TD->getName().empty()) continue;
      if (TD->hasAttr<UsedAttr>()) continue;
      typeRemovals.push_back(ri);
    }
    for (auto &p : S.typedefDefs) {
      const TypedefNameDecl *TD = p.first;
      const RangeInfo &ri = p.second;
      if (S.typedefUsed.count(TD)) continue;
      if (TD->hasAttr<UsedAttr>()) continue;
      if (TD->getName().empty()) continue;
      typeRemovals.push_back(ri);
    }

    if (funcRemovals.empty() && typeRemovals.empty()) {
      llvm::outs() << "No removable functions or types in file: " << S.fileName << "\n";
      return;
    }

    // Read the in-memory buffer (we parsed the inlined text as main file)
    SourceManager &SM = Context.getSourceManager();
    FileID fid = SM.getMainFileID();
    StringRef buf = SM.getBufferData(fid);
    std::string content = buf.str();

    // Collect ranges to remove and merge
    std::vector<std::pair<unsigned,unsigned>> ranges;
    auto addRange = [&](const RangeInfo &r) {
      unsigned s = r.startOffset;
      unsigned e = r.endOffset;
      if (s >= content.size()) return;
      e = std::min(e, (unsigned)content.size());
      ranges.emplace_back(s, e);
    };
    for (auto &r : funcRemovals) addRange(r);
    for (auto &r : typeRemovals) addRange(r);

    if (ranges.empty()) {
      llvm::outs() << "No valid ranges to remove in file: " << S.fileName << "\n";
      return;
    }

    std::sort(ranges.begin(), ranges.end());
    std::vector<std::pair<unsigned,unsigned>> merged;
    auto cur = ranges[0];
    for (size_t i = 1; i < ranges.size(); ++i) {
      if (ranges[i].first <= cur.second) {
        cur.second = std::max(cur.second, ranges[i].second);
      } else {
        merged.push_back(cur);
        cur = ranges[i];
      }
    }
    merged.push_back(cur);

    // Remove ranges from content (descending order)
    std::sort(merged.begin(), merged.end(), [](auto &a, auto &b){ return a.first > b.first; });
    std::string newContent = content;
    for (auto &r : merged) {
      unsigned s = r.first;
      unsigned e = r.second;
      if (s >= e || s >= newContent.size()) continue;
      unsigned safeEnd = std::min(e, (unsigned)newContent.size());
      newContent.erase(s, safeEnd - s);
    }

    // Post-process: remove lines that contain only semicolons and whitespace.
    auto is_semicolon_only_line = [](llvm::StringRef line) -> bool {
      line = line.trim();
      if (line.empty()) return false; // keep blank lines
      size_t commentPos = line.find("//");
      if (commentPos != llvm::StringRef::npos) line = line.substr(0, commentPos).rtrim();
      if (line.find("/*") != llvm::StringRef::npos || line.find("*/") != llvm::StringRef::npos) return false;
      bool hasSemi = false;
      for (char c : line) {
        if (c == ';') hasSemi = true;
        else if (!isspace((unsigned char)c)) return false;
      }
      return hasSemi;
    };

    // Split into lines preserving newline boundaries
    std::vector<std::string> lines;
    lines.reserve(1024);
    {
      size_t pos = 0;
      size_t len = newContent.size();
      while (pos < len) {
        size_t nl = newContent.find_first_of("\r\n", pos);
        if (nl == std::string::npos) {
          lines.emplace_back(newContent.substr(pos));
          break;
        }
        lines.emplace_back(newContent.substr(pos, nl - pos));
        // consume newline sequence (CRLF or single)
        if (newContent[nl] == '\r' && nl + 1 < len && newContent[nl+1] == '\n') pos = nl + 2;
        else pos = nl + 1;
      }
    }

    // Filter lines: drop lines that are semicolon-only (but keep blank lines)
    std::string filtered;
    filtered.reserve(newContent.size());
    for (size_t i = 0; i < lines.size(); ++i) {
      llvm::StringRef lr(lines[i]);
      if (is_semicolon_only_line(lr)) {
        continue;
      }
      filtered.append(lines[i]);
      if (i + 1 < lines.size()) filtered.push_back('\n');
    }

    // Collapse excessive leading newlines to at most one
    size_t p = 0;
    while (p < filtered.size() && (filtered[p] == '\n' || filtered[p] == '\r')) ++p;
    if (p > 1) filtered.erase(0, p - 1);

    newContent.swap(filtered);

    // Write cleaned file
    std::string outFile = S.fileName + ".clean.c";
    std::ofstream fout(outFile, std::ios::binary);
    if (!fout) {
      llvm::errs() << "Cannot write output file: " << outFile << "\n";
      return;
    }
    fout << newContent;
    fout.close();

    llvm::outs() << "Wrote cleaned file: " << outFile << " (removed " << merged.size() << " ranges)\n";
  }
};

// FrontendAction that uses PerFileConsumer
class PerFileAction : public ASTFrontendAction {
public:
  PerFileAction() {}
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef) override {
    return std::make_unique<PerFileConsumer>(&CI.getASTContext());
  }
};

// -------------------- Command-line parsing helpers --------------------

static void splitFlagsString(const std::string &s, std::vector<std::string> &out) {
  // Simple split by spaces; does not handle quoting. For more complex needs, pass flags after "--".
  std::istringstream iss(s);
  std::string tok;
  while (iss >> tok) out.push_back(tok);
}

// -------------------- Main flow (no compilation database) --------------------

int main(int argc, const char **argv) {
  if (argc < 2) {
    llvm::errs() << "Usage: " << argv[0] << " <source-files...> [-- <compiler-flags...>]\n";
    llvm::errs() << "Example: " << argv[0] << " test.c -- -std=c11 -Iinclude\n";
    return 1;
  }

  // Parse arguments: files until "--", flags after "--"
  std::vector<std::string> files;
  std::vector<std::string> flags;
  bool sawSep = false;
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (!sawSep && a == "--") { sawSep = true; continue; }
    if (!sawSep) files.push_back(a);
    else flags.push_back(a);
  }

  if (files.empty()) {
    llvm::errs() << "No source files provided.\n";
    return 1;
  }

  // If no flags provided, use defaults
  if (flags.empty()) {
    flags.assign(DEFAULT_FLAGS, DEFAULT_FLAGS + (sizeof(DEFAULT_FLAGS)/sizeof(DEFAULT_FLAGS[0])));
    llvm::outs() << "No compiler flags provided; using defaults: -std=c11 -I.\n";
  }

  // Global include-guard set (canonical paths)
  std::set<std::string> globalInlined;

  for (const std::string &srcPath : files) {
    // Read original file
    std::string orig = readFile(srcPath);
    if (orig.empty()) {
      llvm::errs() << "Cannot open source file: " << srcPath << "\n";
      continue;
    }

    // Inline includes (double-quoted only), recursively, without macro expansion
    std::set<std::string> localInlined = globalInlined;
    std::string inlined = inlineIncludesText(orig, localInlined);
    // merge local into global
    for (auto &s : localInlined) globalInlined.insert(s);

    // Run the AST consumer on the inlined code; use runToolOnCodeWithArgs with a PerFileAction instance
    bool ok = tooling::runToolOnCodeWithArgs(std::make_unique<PerFileAction>(), inlined, flags, srcPath);
    if (!ok) {
      llvm::errs() << "Parsing inlined content failed for: " << srcPath << "\n";
      continue;
    }
  }

  llvm::outs() << "Done. Review the .clean.c files before replacing originals.\n";
  return 0;
}
