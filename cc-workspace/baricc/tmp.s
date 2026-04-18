.intel_syntax noprefix
.bss
token:
  .zero 8
user_input:
  .zero 8
filename:
  .zero 8
locals:
  .zero 800
globals:
  .zero 800
strings:
  .zero 8
tags:
  .zero 8
enum_vars:
  .zero 8
code:
  .zero 8000
.data
.LC457:
  .string "tried to read unexpected token kind"
.LC456:
  .string "TK_STDERR"
.LC455:
  .string "TK_ERRNO"
.LC454:
  .string "TK_SEEKEND"
.LC453:
  .string "TK_SEEKCUR"
.LC452:
  .string "TK_SEEKSET"
.LC451:
  .string "TK_NULL"
.LC450:
  .string "TK_FALSE"
.LC449:
  .string "TK_TRUE"
.LC448:
  .string "TK_DEFAULT"
.LC447:
  .string "TK_CASE"
.LC446:
  .string "TK_SWITCH"
.LC445:
  .string "TK_CONTINUE"
.LC444:
  .string "TK_BREAK"
.LC443:
  .string "TK_ENUM"
.LC442:
  .string "TK_TYPEDEF"
.LC441:
  .string "TK_STRUCT"
.LC440:
  .string "TK_STRING"
.LC439:
  .string "TK_SIZEOF"
.LC438:
  .string "TK_TYPE"
.LC437:
  .string "TK_FOR"
.LC436:
  .string "TK_WHILE"
.LC435:
  .string "TK_ELSE"
.LC434:
  .string "TK_IF"
.LC433:
  .string "TK_RETURN"
.LC432:
  .string "TK_IDENT"
.LC431:
  .string "TK_EOF"
.LC430:
  .string "TK_NUM"
.LC429:
  .string "TK_RESERVED"
.LC428:
  .string ""
.LC427:
  .string "%*s"
.LC426:
  .string "%.*s\n"
.LC425:
  .string "%s:%d: "
.LC424:
  .string ""
.LC423:
  .string "%*s"
.LC422:
  .string "%.*s\n"
.LC421:
  .string "%s:%d: "
.LC420:
  .string ""
.LC419:
  .string "%*s"
.LC418:
  .string "%.*s\n"
.LC417:
  .string "%s:%d: "
.LC416:
  .string "\n"
.LC415:
  .string "\n"
.LC414:
  .string "\n"
.LC413:
  .string "[DEBUG] node->kind = %d\n"
.LC412:
  .string "[DEBUG] node->varsize = %d\n"
.LC411:
  .string "[DEBUG] node->varname = %s\n"
.LC410:
  .string "[DEBUG] node->funcname = %s\n"
.LC409:
  .string "[DEBUG] node is null\n"
.LC408:
  .string "\n"
.LC407:
  .string "[DEBUG] type = %s "
.LC406:
  .string "STRUCT"
.LC405:
  .string "ARRAY"
.LC404:
  .string "PTR"
.LC403:
  .string "CHAR"
.LC402:
  .string "INT"
.LC401:
  .string "[DEBUG] type is null"
.LC400:
  .string "[DEBUG] token: %2d %s\n"
.LC399:
  .string "[DEBUG] token is null\n"
.LC398:
  .string "%s: fseek: %s"
.LC397:
  .string "%s: fseek: %s"
.LC396:
  .string "cannot open %s: %s"
.LC395:
  .string "r"
.LC394:
  .string ".globl main\n"
.LC393:
  .string ".text\n"
.LC392:
  .string "  .string \"%s\"\n"
.LC391:
  .string ".LC%d:\n"
.LC390:
  .string ".data\n"
.LC389:
  .string ".bss\n"
.LC388:
  .string ".intel_syntax noprefix\n"
.LC387:
  .string "; i = %d, filename=%s\n"
.LC386:
  .string "not variable"
.LC385:
  .string "  push rax\n"
.LC384:
  .string "  add rax, %d\n"
.LC383:
  .string "  pop rax\n"
.LC382:
  .string "  push offset %s\n"
.LC381:
  .string "  push rax\n"
.LC380:
  .string "  sub rax, %d\n"
.LC379:
  .string "  mov rax, rbp\n"
.LC378:
  .string "  push rax\n"
.LC377:
  .string "  and rax, rdi\n"
.LC376:
  .string "  or rax, rdi\n"
.LC375:
  .string "  xor rax, rdi\n"
.LC374:
  .string "  movzb rax, al\n"
.LC373:
  .string "  setle al\n"
.LC372:
  .string "  cmp rax, rdi\n"
.LC371:
  .string "  movzb rax, al\n"
.LC370:
  .string "  setl al\n"
.LC369:
  .string "  cmp rax, rdi\n"
.LC368:
  .string "  movzb rax, al\n"
.LC367:
  .string "  setne al\n"
.LC366:
  .string "  cmp rax, rdi\n"
.LC365:
  .string "  movzb rax, al\n"
.LC364:
  .string "  sete al\n"
.LC363:
  .string "  cmp rax, rdi\n"
.LC362:
  .string "  idiv rdi\n"
.LC361:
  .string "  cqo\n"
.LC360:
  .string "  imul rax, rdi\n"
.LC359:
  .string "  sub rax, rdi\n"
.LC358:
  .string "  add rax, rdi\n"
.LC357:
  .string "  pop rax\n"
.LC356:
  .string "  pop rdi\n"
.LC355:
  .string "  push rax\n"
.LC354:
  .string "  mov rax, [rax]\n"
.LC353:
  .string "  movsxd rax, DWORD PTR [rax]\n"
.LC352:
  .string "  movsx rax, BYTE PTR [rax]\n"
.LC351:
  .string "  pop rax\n"
.LC350:
  .string "  ret\n"
.LC349:
  .string "  pop rbp\n"
.LC348:
  .string "  mov rsp, rbp\n"
.LC347:
  .string "  mov [rbp-%d], %s\n"
.LC346:
  .string "  mov [rbp-%d], %s\n"
.LC345:
  .string "  mov [rbp-%d], %s\n"
.LC344:
  .string "  sub rsp, %d\n"
.LC343:
  .string "  mov rbp, rsp\n"
.LC342:
  .string "  push rbp\n"
.LC341:
  .string "%s:\n"
.LC340:
  .string ".global %s\n"
.LC339:
  .string "  push rax\n"
.LC338:
  .string ".L.end.%03d:\n"
.LC337:
  .string "  add rsp, 8\n"
.LC336:
  .string "  call %s\n"
.LC335:
  .string "  mov rax, 0\n"
.LC334:
  .string "  sub rsp, 8\n"
.LC333:
  .string ".L.call.%03d:\n"
.LC332:
  .string "  jmp .L.end.%03d\n"
.LC331:
  .string "  call %s\n"
.LC330:
  .string "  mov rax, 0\n"
.LC329:
  .string "  jnz .L.call.%03d\n"
.LC328:
  .string "  and rax, 15\n"
.LC327:
  .string "  mov rax, rsp\n"
.LC326:
  .string "  pop %s\n"
.LC325:
  .string "invalid number of args. lteq 6."
.LC324:
  .string ".Lcase%d:\n"
.LC323:
  .string ".Lend%d:\n"
.LC322:
  .string "  jmp .Lend%d\n"
.LC321:
  .string "  jmp .Lcase%d\n"
.LC320:
  .string "  je .Lcase%d\n"
.LC319:
  .string "  cmp rax, %d\n"
.LC318:
  .string "  pop rax\n"
.LC317:
  .string ".Lend%d:\n"
.LC316:
  .string "  jmp .Lbegin%d\n"
.LC315:
  .string ".Lcontinue%d:\n"
.LC314:
  .string "  je .Lend%d\n"
.LC313:
  .string "  cmp rax, 0\n"
.LC312:
  .string "  pop rax\n"
.LC311:
  .string "  push 1\n"
.LC310:
  .string ".Lbegin%d:\n"
.LC309:
  .string ".Lend%d:\n"
.LC308:
  .string "  jmp .Lbegin%d\n"
.LC307:
  .string "  je .Lend%d\n"
.LC306:
  .string "  cmp rax, 0\n"
.LC305:
  .string "  pop rax\n"
.LC304:
  .string ".Lcontinue%d:\n"
.LC303:
  .string ".Lbegin%d:\n"
.LC302:
  .string ".Lend%d:\n"
.LC301:
  .string "  jmp .Lend%d\n"
.LC300:
  .string ".Lelse%d:\n"
.LC299:
  .string "  jmp .Lend%d\n"
.LC298:
  .string "  je .Lelse%d\n"
.LC297:
  .string "  cmp rax, 0\n"
.LC296:
  .string "  pop rax\n"
.LC295:
  .string ".Lend%d:\n"
.LC294:
  .string ".Lelse%d:\n"
.LC293:
  .string "  jmp .Lend%d\n"
.LC292:
  .string "  jmp .Lend%d\n"
.LC291:
  .string ".Lelse%d:\n"
.LC290:
  .string "  jmp .Lend%d\n"
.LC289:
  .string "  je .Lelse%d\n"
.LC288:
  .string "  cmp rax, 0\n"
.LC287:
  .string "  pop rax\n"
.LC286:
  .string "  jmp .Lcontinue%d\n"
.LC285:
  .string "currently not in for, while"
.LC284:
  .string "  jmp .Lend%d\n"
.LC283:
  .string "currently not in for, while"
.LC282:
  .string "  ret\n"
.LC281:
  .string "  pop rbp\n"
.LC280:
  .string "  mov rsp, rbp\n"
.LC279:
  .string "  pop rax\n"
.LC278:
  .string "  push rdi\n"
.LC277:
  .string "  mov [rax], rdi\n"
.LC276:
  .string "  mov [rax], edi\n"
.LC275:
  .string "  mov [rax], dil\n"
.LC274:
  .string "  pop rax\n"
.LC273:
  .string "  pop rdi\n"
.LC272:
  .string "  push rax\n"
.LC271:
  .string "  mov rax, [rax]\n"
.LC270:
  .string "  mov rax, [rax]\n"
.LC269:
  .string "  movsxd rax, DWORD PTR [rax]\n"
.LC268:
  .string "  movsx rax, BYTE PTR [rax]\n"
.LC267:
  .string "  pop rax\n"
.LC266:
  .string "  .long 0x%x\n"
.LC265:
  .string "  .quad .LC%d\n"
.LC264:
  .string "  .string \"%s\"\n"
.LC263:
  .string "  .quad .LC%d\n"
.LC262:
  .string "  .byte 0x%x\n"
.LC261:
  .string "  .long 0x%x\n"
.LC260:
  .string "  .zero 0x%x\n"
.LC259:
  .string "  .zero %d\n"
.LC258:
  .string "%s:\n"
.LC257:
  .string "  push %d\n"
.LC256:
  .string "  push offset .LC%d\n"
.LC255:
  .string "  push rax\n"
.LC254:
  .string "  add rax, rdi\n"
.LC253:
  .string "  pop rax\n"
.LC252:
  .string "  pop rdi\n"
.LC251:
  .string "  push 1\n"
.LC250:
  .string "  push rax\n"
.LC249:
  .string "  sub rax, rdi\n"
.LC248:
  .string "  pop rax\n"
.LC247:
  .string "  pop rdi\n"
.LC246:
  .string "  push 1\n"
.LC245:
  .string "  push rax\n"
.LC244:
  .string "  movzb rax, al\n"
.LC243:
  .string "  sete al\n"
.LC242:
  .string "  cmp rax, 0\n"
.LC241:
  .string "  pop rax\n"
.LC240:
  .string "  push rax\n"
.LC239:
  .string "  not rax\n"
.LC238:
  .string "  pop rax\n"
.LC237:
  .string ".Lend%d:\n"
.LC236:
  .string "  push 1\n"
.LC235:
  .string ".Ltrue%d:\n"
.LC234:
  .string "  jmp .Lend%d\n"
.LC233:
  .string "  push 0\n"
.LC232:
  .string "  jne .Ltrue%d\n"
.LC231:
  .string "  cmp rax, 0\n"
.LC230:
  .string "  pop rax\n"
.LC229:
  .string "  jne .Ltrue%d\n"
.LC228:
  .string "  cmp rax, 0\n"
.LC227:
  .string "  pop rax\n"
.LC226:
  .string ".Lend%d:\n"
.LC225:
  .string "  push 0\n"
.LC224:
  .string ".Lfalse%d:\n"
.LC223:
  .string "  jmp .Lend%d\n"
.LC222:
  .string "  push 1\n"
.LC221:
  .string "  je .Lfalse%d\n"
.LC220:
  .string "  cmp rax, 0\n"
.LC219:
  .string "  pop rax\n"
.LC218:
  .string "  je .Lfalse%d\n"
.LC217:
  .string "  cmp rax, 0\n"
.LC216:
  .string "  pop rax\n"
.LC215:
  .string "node must not be NULL"
.LC214:
  .string "r9"
.LC213:
  .string "r8"
.LC212:
  .string "rcx"
.LC211:
  .string "rdx"
.LC210:
  .string "rsi"
.LC209:
  .string "rdi"
.LC208:
  .string "r9d"
.LC207:
  .string "r8d"
.LC206:
  .string "ecx"
.LC205:
  .string "edx"
.LC204:
  .string "esi"
.LC203:
  .string "edi"
.LC202:
  .string "r9w"
.LC201:
  .string "r8w"
.LC200:
  .string "cx"
.LC199:
  .string "dx"
.LC198:
  .string "si"
.LC197:
  .string "di"
.LC196:
  .string "r9b"
.LC195:
  .string "r8b"
.LC194:
  .string "cl"
.LC193:
  .string "dl"
.LC192:
  .string "sil"
.LC191:
  .string "dil"
.LC190:
  .string "]"
.LC189:
  .string "["
.LC188:
  .string "invalid definition of function or variable"
.LC187:
  .string "ident should come here."
.LC186:
  .string "*"
.LC185:
  .string "char"
.LC184:
  .string "unexpected Type->ty comes here"
.LC183:
  .string "array size is zero."
.LC182:
  .string "type should be non null"
.LC181:
  .string "invalid dereference"
.LC180:
  .string "member ident is not found"
.LC179:
  .string "this type doesn't have members."
.LC178:
  .string "member type is not passed."
.LC177:
  .string "member ident must come here"
.LC176:
  .string "unexpected type is passed."
.LC175:
  .string ","
.LC174:
  .string "{"
.LC173:
  .string "}"
.LC172:
  .string ","
.LC171:
  .string "{"
.LC170:
  .string "}"
.LC169:
  .string "{"
.LC168:
  .string "{"
.LC167:
  .string "="
.LC166:
  .string "invalid definition of function or variable"
.LC165:
  .string "--"
.LC164:
  .string "++"
.LC163:
  .string "->"
.LC162:
  .string "."
.LC161:
  .string "]"
.LC160:
  .string "["
.LC159:
  .string "variable %s is not defined yet"
.LC158:
  .string "; [DEBUG] %s name\n"
.LC157:
  .string ","
.LC156:
  .string ")"
.LC155:
  .string "("
.LC154:
  .string ")"
.LC153:
  .string "("
.LC152:
  .string ")"
.LC151:
  .string "*"
.LC150:
  .string "it's not allowed to use sizeof for incomplete type"
.LC149:
  .string "("
.LC148:
  .string "("
.LC147:
  .string "("
.LC146:
  .string ")"
.LC145:
  .string "("
.LC144:
  .string "("
.LC143:
  .string ")"
.LC142:
  .string "invalid type name is passed here. %s"
.LC141:
  .string "char"
.LC140:
  .string "int"
.LC139:
  .string "("
.LC138:
  .string "("
.LC137:
  .string "~"
.LC136:
  .string "!"
.LC135:
  .string "&"
.LC134:
  .string "*"
.LC133:
  .string "-"
.LC132:
  .string "+"
.LC131:
  .string "--"
.LC130:
  .string "++"
.LC129:
  .string "/"
.LC128:
  .string "*"
.LC127:
  .string "-"
.LC126:
  .string "+"
.LC125:
  .string ">="
.LC124:
  .string ">"
.LC123:
  .string "<="
.LC122:
  .string "<"
.LC121:
  .string "!="
.LC120:
  .string "=="
.LC119:
  .string "&"
.LC118:
  .string "^"
.LC117:
  .string "|"
.LC116:
  .string "&&"
.LC115:
  .string "||"
.LC114:
  .string ":"
.LC113:
  .string "?"
.LC112:
  .string "/="
.LC111:
  .string "*="
.LC110:
  .string "-="
.LC109:
  .string "+="
.LC108:
  .string "="
.LC107:
  .string "size of statements in one block is over the limitation."
.LC106:
  .string "}"
.LC105:
  .string "{"
.LC104:
  .string ";"
.LC103:
  .string ";"
.LC102:
  .string "{"
.LC101:
  .string ";"
.LC100:
  .string ";"
.LC99:
  .string ")"
.LC98:
  .string ")"
.LC97:
  .string ";"
.LC96:
  .string ";"
.LC95:
  .string ";"
.LC94:
  .string "("
.LC93:
  .string ":"
.LC92:
  .string "stray case"
.LC91:
  .string ":"
.LC90:
  .string "stray case"
.LC89:
  .string ")"
.LC88:
  .string "("
.LC87:
  .string ")"
.LC86:
  .string "("
.LC85:
  .string ")"
.LC84:
  .string "("
.LC83:
  .string ";"
.LC82:
  .string ";"
.LC81:
  .string "invalid token comes here. %d"
.LC80:
  .string ";"
.LC79:
  .string ","
.LC78:
  .string ")"
.LC77:
  .string ";"
.LC76:
  .string "}"
.LC75:
  .string "{"
.LC74:
  .string "{"
.LC73:
  .string "="
.LC72:
  .string ","
.LC71:
  .string "}"
.LC70:
  .string "}"
.LC69:
  .string "{"
.LC68:
  .string "type not found."
.LC67:
  .string "{"
.LC66:
  .string ";"
.LC65:
  .string ";"
.LC64:
  .string ";"
.LC63:
  .string "["
.LC62:
  .string "("
.LC61:
  .string "def is NULL"
.LC60:
  .string ";"
.LC59:
  .string ";"
.LC58:
  .string "char literal must be one length"
.LC57:
  .string "unclosed char literal"
.LC56:
  .string "failed to tokenize. %s\n"
.LC55:
  .string "\\"
.LC54:
  .string ".+-*/()<>;={},&[]!~|^?:"
.LC53:
  .string "||"
.LC52:
  .string "&&"
.LC51:
  .string "--"
.LC50:
  .string "++"
.LC49:
  .string "/="
.LC48:
  .string "*="
.LC47:
  .string "-="
.LC46:
  .string "+="
.LC45:
  .string "->"
.LC44:
  .string ">="
.LC43:
  .string "<="
.LC42:
  .string "!="
.LC41:
  .string "=="
.LC40:
  .string "\n"
.LC39:
  .string "extern"
.LC38:
  .string "\n"
.LC37:
  .string "#include"
.LC36:
  .string "\n"
.LC35:
  .string "//"
.LC34:
  .string "block comment is not closed.\n"
.LC33:
  .string "*/"
.LC32:
  .string "/*"
.LC31:
  .string ""
.LC30:
  .string "stderr"
.LC29:
  .string "errno"
.LC28:
  .string "default"
.LC27:
  .string "SEEK_CUR"
.LC26:
  .string "SEEK_SET"
.LC25:
  .string "SEEK_END"
.LC24:
  .string "NULL"
.LC23:
  .string "true"
.LC22:
  .string "false"
.LC21:
  .string "case"
.LC20:
  .string "switch"
.LC19:
  .string "continue"
.LC18:
  .string "break"
.LC17:
  .string "enum"
.LC16:
  .string "typedef"
.LC15:
  .string "struct"
.LC14:
  .string "sizeof"
.LC13:
  .string "FILE"
.LC12:
  .string "size_t"
.LC11:
  .string "bool"
.LC10:
  .string "char"
.LC9:
  .string "void"
.LC8:
  .string "int"
.LC7:
  .string "for"
.LC6:
  .string "while"
.LC5:
  .string "else"
.LC4:
  .string "if"
.LC3:
  .string "return"
.LC2:
  .string "数ではありません\n"
.LC1:
  .string "unexpected token: '%s'\nactual: '%s'\n"
.LC0:
  .string "expected: '%c'\nactual: '%s'\n"
reserved_words:
  .quad .LC3
  .long 0x5
  .zero 0x4
  .quad .LC4
  .long 0x6
  .zero 0x4
  .quad .LC5
  .long 0x7
  .zero 0x4
  .quad .LC6
  .long 0x8
  .zero 0x4
  .quad .LC7
  .long 0x9
  .zero 0x4
  .quad .LC8
  .long 0xa
  .zero 0x4
  .quad .LC9
  .long 0xa
  .zero 0x4
  .quad .LC10
  .long 0xa
  .zero 0x4
  .quad .LC11
  .long 0xa
  .zero 0x4
  .quad .LC12
  .long 0xa
  .zero 0x4
  .quad .LC13
  .long 0xa
  .zero 0x4
  .quad .LC14
  .long 0xb
  .zero 0x4
  .quad .LC15
  .long 0xd
  .zero 0x4
  .quad .LC16
  .long 0xe
  .zero 0x4
  .quad .LC17
  .long 0xf
  .zero 0x4
  .quad .LC18
  .long 0x10
  .zero 0x4
  .quad .LC19
  .long 0x11
  .zero 0x4
  .quad .LC20
  .long 0x12
  .zero 0x4
  .quad .LC21
  .long 0x13
  .zero 0x4
  .quad .LC22
  .long 0x16
  .zero 0x4
  .quad .LC23
  .long 0x15
  .zero 0x4
  .quad .LC24
  .long 0x17
  .zero 0x4
  .quad .LC25
  .long 0x1a
  .zero 0x4
  .quad .LC26
  .long 0x18
  .zero 0x4
  .quad .LC27
  .long 0x19
  .zero 0x4
  .quad .LC28
  .long 0x14
  .zero 0x4
  .quad .LC29
  .long 0x1b
  .zero 0x4
  .quad .LC30
  .long 0x1c
  .zero 0x4
  .quad .LC31
  .long 0x3
  .zero 0x4
cur_scope_depth:
  .long 0x0
current_switch:
  .long 0x0
argreg1:
  .quad .LC191
  .quad .LC192
  .quad .LC193
  .quad .LC194
  .quad .LC195
  .quad .LC196
argreg2:
  .quad .LC197
  .quad .LC198
  .quad .LC199
  .quad .LC200
  .quad .LC201
  .quad .LC202
argreg4:
  .quad .LC203
  .quad .LC204
  .quad .LC205
  .quad .LC206
  .quad .LC207
  .quad .LC208
argreg8:
  .quad .LC209
  .quad .LC210
  .quad .LC211
  .quad .LC212
  .quad .LC213
  .quad .LC214
if_sequence:
  .long 0x0
break_sequence:
  .long 0x0
continue_sequence:
  .long 0x0
.text
.globl main
.global consume
consume:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.001
  mov rax, 0
  call check
  jmp .L.end.001
.L.call.001:
  sub rsp, 8
  mov rax, 0
  call check
  add rsp, 8
.L.end.001:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse0
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend0
.Lelse0:
.Lend0:
  mov rax, rsp
  and rax, 15
  jnz .L.call.002
  mov rax, 0
  call advance_token
  jmp .L.end.002
.L.call.002:
  sub rsp, 8
  mov rax, 0
  call advance_token
  add rsp, 8
.L.end.002:
  push rax
  push 1
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global consume_kind
consume_kind:
  push rbp
  mov rbp, rsp
  sub rsp, 12
  mov [rbp-4], edi
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.004
  mov rax, 0
  call check_kind
  jmp .L.end.004
.L.call.004:
  sub rsp, 8
  mov rax, 0
  call check_kind
  add rsp, 8
.L.end.004:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse3
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend3
.Lelse3:
.Lend3:
  mov rax, rbp
  sub rax, 12
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.005
  mov rax, 0
  call advance_token
  jmp .L.end.005
.L.call.005:
  sub rsp, 8
  mov rax, 0
  call advance_token
  add rsp, 8
.L.end.005:
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global check
check:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov [rbp-8], rdi
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse6
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.008
  mov rax, 0
  call strlen
  jmp .L.end.008
.L.call.008:
  sub rsp, 8
  mov rax, 0
  call strlen
  add rsp, 8
.L.end.008:
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse7
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.009
  mov rax, 0
  call memcmp
  jmp .L.end.009
.L.call.009:
  sub rsp, 8
  mov rax, 0
  call memcmp
  add rsp, 8
.L.end.009:
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse7
  push 1
  jmp .Lend7
.Lfalse7:
  push 0
.Lend7:
  pop rax
  cmp rax, 0
  je .Lfalse6
  push 1
  jmp .Lend6
.Lfalse6:
  push 0
.Lend6:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global check_kind
check_kind:
  push rbp
  mov rbp, rsp
  sub rsp, 4
  mov [rbp-4], edi
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global expect
expect:
  push rbp
  mov rbp, rsp
  sub rsp, 808
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.011
  mov rax, 0
  call check
  jmp .L.end.011
.L.call.011:
  sub rsp, 8
  mov rax, 0
  call check
  add rsp, 8
.L.end.011:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse10
  mov rax, rbp
  sub rax, 808
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 40
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 48
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 56
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 64
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 72
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 80
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 88
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 96
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 104
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 112
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 120
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 128
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 136
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 144
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 152
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 160
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 168
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 176
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 184
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 192
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 200
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 208
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 216
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 224
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 232
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 240
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 248
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 256
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 264
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 272
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 280
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 288
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 296
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 304
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 312
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 320
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 328
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 336
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 344
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 352
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 360
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 368
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 376
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 384
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 392
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 400
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 408
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 416
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 424
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 432
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 440
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 448
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 456
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 464
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 472
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 480
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 488
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 496
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 504
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 512
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 520
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 528
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 536
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 544
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 552
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 560
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 568
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 576
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 584
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 592
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 600
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 608
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 616
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 624
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 632
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 640
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 648
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 656
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 664
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 672
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 680
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 688
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 696
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 704
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 712
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 720
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 728
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 736
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 744
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 752
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 760
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 768
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 776
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 784
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 792
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.012
  mov rax, 0
  call memcpy
  jmp .L.end.012
.L.call.012:
  sub rsp, 8
  mov rax, 0
  call memcpy
  add rsp, 8
.L.end.012:
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC0
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 808
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.013
  mov rax, 0
  call error_at2
  jmp .L.end.013
.L.call.013:
  sub rsp, 8
  mov rax, 0
  call error_at2
  add rsp, 8
.L.end.013:
  push rax
  jmp .Lend10
.Lelse10:
.Lend10:
  mov rax, rsp
  and rax, 15
  jnz .L.call.014
  mov rax, 0
  call advance_token
  jmp .L.end.014
.L.call.014:
  sub rsp, 8
  mov rax, 0
  call advance_token
  add rsp, 8
.L.end.014:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global expect_kind
expect_kind:
  push rbp
  mov rbp, rsp
  sub rsp, 12
  mov [rbp-4], edi
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.016
  mov rax, 0
  call check_kind
  jmp .L.end.016
.L.call.016:
  sub rsp, 8
  mov rax, 0
  call check_kind
  add rsp, 8
.L.end.016:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse15
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC1
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.018
  mov rax, 0
  call get_token_kind_name
  jmp .L.end.018
.L.call.018:
  sub rsp, 8
  mov rax, 0
  call get_token_kind_name
  add rsp, 8
.L.end.018:
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.019
  mov rax, 0
  call get_token_kind_name
  jmp .L.end.019
.L.call.019:
  sub rsp, 8
  mov rax, 0
  call get_token_kind_name
  add rsp, 8
.L.end.019:
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.017
  mov rax, 0
  call error_at2
  jmp .L.end.017
.L.call.017:
  sub rsp, 8
  mov rax, 0
  call error_at2
  add rsp, 8
.L.end.017:
  push rax
  jmp .Lend15
.Lelse15:
.Lend15:
  mov rax, rbp
  sub rax, 12
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.020
  mov rax, 0
  call advance_token
  jmp .L.end.020
.L.call.020:
  sub rsp, 8
  mov rax, 0
  call advance_token
  add rsp, 8
.L.end.020:
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global expect_number
expect_number:
  push rbp
  mov rbp, rsp
  sub rsp, 4
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 2
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse21
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.022
  mov rax, 0
  call print_token
  jmp .L.end.022
.L.call.022:
  sub rsp, 8
  mov rax, 0
  call print_token
  add rsp, 8
.L.end.022:
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC2
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.023
  mov rax, 0
  call error_at0
  jmp .L.end.023
.L.call.023:
  sub rsp, 8
  mov rax, 0
  call error_at0
  add rsp, 8
.L.end.023:
  push rax
  jmp .Lend21
.Lelse21:
.Lend21:
  mov rax, rbp
  sub rax, 4
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.024
  mov rax, 0
  call advance_token
  jmp .L.end.024
.L.call.024:
  sub rsp, 8
  mov rax, 0
  call advance_token
  add rsp, 8
.L.end.024:
  push rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global advance_token
advance_token:
  push rbp
  mov rbp, rsp
  push offset token
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global at_eof
at_eof:
  push rbp
  mov rbp, rsp
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 3
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global new_token
new_token:
  push rbp
  mov rbp, rsp
  sub rsp, 32
  mov [rbp-4], edi
  mov [rbp-12], rsi
  mov [rbp-20], rdx
  mov [rbp-24], ecx
  mov rax, rbp
  sub rax, 32
  push rax
  push 1
  push 40
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.025
  mov rax, 0
  call calloc
  jmp .L.end.025
.L.call.025:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.025:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global startswith
startswith:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov [rbp-8], rdi
  mov [rbp-16], rsi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.027
  mov rax, 0
  call strlen
  jmp .L.end.027
.L.call.027:
  sub rsp, 8
  mov rax, 0
  call strlen
  add rsp, 8
.L.end.027:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.026
  mov rax, 0
  call memcmp
  jmp .L.end.026
.L.call.026:
  sub rsp, 8
  mov rax, 0
  call memcmp
  add rsp, 8
.L.end.026:
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global is_alnum
is_alnum:
  push rbp
  mov rbp, rsp
  sub rsp, 1
  mov [rbp-1], dil
  push 97
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse29
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 122
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse29
  push 1
  jmp .Lend29
.Lfalse29:
  push 0
.Lend29:
  pop rax
  cmp rax, 0
  jne .Ltrue28
  push 65
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse31
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 90
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse31
  push 1
  jmp .Lend31
.Lfalse31:
  push 0
.Lend31:
  pop rax
  cmp rax, 0
  jne .Ltrue30
  push 48
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse33
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 57
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse33
  push 1
  jmp .Lend33
.Lfalse33:
  push 0
.Lend33:
  pop rax
  cmp rax, 0
  jne .Ltrue32
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 95
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue32
  push 0
  jmp .Lend32
.Ltrue32:
  push 1
.Lend32:
  pop rax
  cmp rax, 0
  jne .Ltrue30
  push 0
  jmp .Lend30
.Ltrue30:
  push 1
.Lend30:
  pop rax
  cmp rax, 0
  jne .Ltrue28
  push 0
  jmp .Lend28
.Ltrue28:
  push 1
.Lend28:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global tokenize
tokenize:
  push rbp
  mov rbp, rsp
  sub rsp, 116
  mov rax, rbp
  sub rax, 8
  push rax
  push offset user_input
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  add rax, 8
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  add rax, 32
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  add rax, 24
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 56
  push rax
  mov rax, rbp
  sub rax, 48
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin34:
.Lcontinue34:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend34
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.036
  mov rax, 0
  call isspace
  jmp .L.end.036
.L.call.036:
  sub rsp, 8
  mov rax, 0
  call isspace
  add rsp, 8
.L.end.036:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse35
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lcontinue34
  jmp .Lend35
.Lelse35:
.Lend35:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC32
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.038
  mov rax, 0
  call startswith
  jmp .L.end.038
.L.call.038:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.038:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse37
  mov rax, rbp
  sub rax, 64
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 2
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push offset .LC33
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.039
  mov rax, 0
  call strstr
  jmp .L.end.039
.L.call.039:
  sub rsp, 8
  mov rax, 0
  call strstr
  add rsp, 8
.L.end.039:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse40
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC34
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.041
  mov rax, 0
  call error_at0
  jmp .L.end.041
.L.call.041:
  sub rsp, 8
  mov rax, 0
  call error_at0
  add rsp, 8
.L.end.041:
  push rax
  jmp .Lend40
.Lelse40:
.Lend40:
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 2
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lcontinue34
  jmp .Lend37
.Lelse37:
.Lend37:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC35
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.043
  mov rax, 0
  call startswith
  jmp .L.end.043
.L.call.043:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.043:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse42
.Lbegin44:
.Lcontinue44:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC36
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.045
  mov rax, 0
  call startswith
  jmp .L.end.045
.L.call.045:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.045:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend44
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin44
.Lend44:
  jmp .Lcontinue34
  jmp .Lend42
.Lelse42:
.Lend42:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC37
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.047
  mov rax, 0
  call startswith
  jmp .L.end.047
.L.call.047:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.047:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse46
.Lbegin48:
.Lcontinue48:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC38
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.049
  mov rax, 0
  call startswith
  jmp .L.end.049
.L.call.049:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.049:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend48
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin48
.Lend48:
  jmp .Lcontinue34
  jmp .Lend46
.Lelse46:
.Lend46:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC39
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.051
  mov rax, 0
  call startswith
  jmp .L.end.051
.L.call.051:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.051:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse50
.Lbegin52:
.Lcontinue52:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC40
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.053
  mov rax, 0
  call startswith
  jmp .L.end.053
.L.call.053:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.053:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend52
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin52
.Lend52:
  jmp .Lcontinue34
  jmp .Lend50
.Lelse50:
.Lend50:
  mov rax, rbp
  sub rax, 68
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin54:
  push offset reserved_words
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 16
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 3
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend54
  mov rax, rbp
  sub rax, 80
  push rax
  push offset reserved_words
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 16
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 84
  push rax
  mov rax, rbp
  sub rax, 80
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.055
  mov rax, 0
  call strlen
  jmp .L.end.055
.L.call.055:
  sub rsp, 8
  mov rax, 0
  call strlen
  add rsp, 8
.L.end.055:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 88
  push rax
  push offset reserved_words
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 16
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 80
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.058
  mov rax, 0
  call startswith
  jmp .L.end.058
.L.call.058:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.058:
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse57
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 84
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.059
  mov rax, 0
  call is_alnum
  jmp .L.end.059
.L.call.059:
  sub rsp, 8
  mov rax, 0
  call is_alnum
  add rsp, 8
.L.end.059:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse57
  push 1
  jmp .Lend57
.Lfalse57:
  push 0
.Lend57:
  pop rax
  cmp rax, 0
  je .Lelse56
  mov rax, rbp
  sub rax, 56
  push rax
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 84
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.060
  mov rax, 0
  call new_token
  jmp .L.end.060
.L.call.060:
  sub rsp, 8
  mov rax, 0
  call new_token
  add rsp, 8
.L.end.060:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 84
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 68
  push rax
  push 1
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend54
  jmp .Lend56
.Lelse56:
.Lend56:
.Lcontinue54:
  mov rax, rbp
  sub rax, 72
  push rax
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin54
.Lend54:
  mov rax, rbp
  sub rax, 68
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse61
  jmp .Lcontinue34
  jmp .Lend61
.Lelse61:
.Lend61:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC41
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.064
  mov rax, 0
  call startswith
  jmp .L.end.064
.L.call.064:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.064:
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue63
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC42
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.066
  mov rax, 0
  call startswith
  jmp .L.end.066
.L.call.066:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.066:
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue65
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC43
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.068
  mov rax, 0
  call startswith
  jmp .L.end.068
.L.call.068:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.068:
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue67
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC44
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.070
  mov rax, 0
  call startswith
  jmp .L.end.070
.L.call.070:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.070:
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue69
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC45
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.072
  mov rax, 0
  call startswith
  jmp .L.end.072
.L.call.072:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.072:
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue71
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC46
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.074
  mov rax, 0
  call startswith
  jmp .L.end.074
.L.call.074:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.074:
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue73
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC47
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.076
  mov rax, 0
  call startswith
  jmp .L.end.076
.L.call.076:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.076:
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue75
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC48
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.078
  mov rax, 0
  call startswith
  jmp .L.end.078
.L.call.078:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.078:
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue77
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC49
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.080
  mov rax, 0
  call startswith
  jmp .L.end.080
.L.call.080:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.080:
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue79
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC50
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.082
  mov rax, 0
  call startswith
  jmp .L.end.082
.L.call.082:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.082:
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue81
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC51
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.084
  mov rax, 0
  call startswith
  jmp .L.end.084
.L.call.084:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.084:
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue83
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC52
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.086
  mov rax, 0
  call startswith
  jmp .L.end.086
.L.call.086:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.086:
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue85
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC53
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.087
  mov rax, 0
  call startswith
  jmp .L.end.087
.L.call.087:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.087:
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue85
  push 0
  jmp .Lend85
.Ltrue85:
  push 1
.Lend85:
  pop rax
  cmp rax, 0
  jne .Ltrue83
  push 0
  jmp .Lend83
.Ltrue83:
  push 1
.Lend83:
  pop rax
  cmp rax, 0
  jne .Ltrue81
  push 0
  jmp .Lend81
.Ltrue81:
  push 1
.Lend81:
  pop rax
  cmp rax, 0
  jne .Ltrue79
  push 0
  jmp .Lend79
.Ltrue79:
  push 1
.Lend79:
  pop rax
  cmp rax, 0
  jne .Ltrue77
  push 0
  jmp .Lend77
.Ltrue77:
  push 1
.Lend77:
  pop rax
  cmp rax, 0
  jne .Ltrue75
  push 0
  jmp .Lend75
.Ltrue75:
  push 1
.Lend75:
  pop rax
  cmp rax, 0
  jne .Ltrue73
  push 0
  jmp .Lend73
.Ltrue73:
  push 1
.Lend73:
  pop rax
  cmp rax, 0
  jne .Ltrue71
  push 0
  jmp .Lend71
.Ltrue71:
  push 1
.Lend71:
  pop rax
  cmp rax, 0
  jne .Ltrue69
  push 0
  jmp .Lend69
.Ltrue69:
  push 1
.Lend69:
  pop rax
  cmp rax, 0
  jne .Ltrue67
  push 0
  jmp .Lend67
.Ltrue67:
  push 1
.Lend67:
  pop rax
  cmp rax, 0
  jne .Ltrue65
  push 0
  jmp .Lend65
.Ltrue65:
  push 1
.Lend65:
  pop rax
  cmp rax, 0
  jne .Ltrue63
  push 0
  jmp .Lend63
.Ltrue63:
  push 1
.Lend63:
  pop rax
  cmp rax, 0
  je .Lelse62
  mov rax, rbp
  sub rax, 56
  push rax
  push 1
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 2
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.088
  mov rax, 0
  call new_token
  jmp .L.end.088
.L.call.088:
  sub rsp, 8
  mov rax, 0
  call new_token
  add rsp, 8
.L.end.088:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 2
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lcontinue34
  jmp .Lend62
.Lelse62:
.Lend62:
  push offset .LC54
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.090
  mov rax, 0
  call strchr
  jmp .L.end.090
.L.call.090:
  sub rsp, 8
  mov rax, 0
  call strchr
  add rsp, 8
.L.end.090:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse89
  mov rax, rbp
  sub rax, 56
  push rax
  push 1
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.091
  mov rax, 0
  call new_token
  jmp .L.end.091
.L.call.091:
  sub rsp, 8
  mov rax, 0
  call new_token
  add rsp, 8
.L.end.091:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lcontinue34
  jmp .Lend89
.Lelse89:
.Lend89:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 39
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse92
  mov rax, rbp
  sub rax, 56
  push rax
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.093
  mov rax, 0
  call read_char_literal
  jmp .L.end.093
.L.call.093:
  sub rsp, 8
  mov rax, 0
  call read_char_literal
  add rsp, 8
.L.end.093:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lcontinue34
  jmp .Lend92
.Lelse92:
.Lend92:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 34
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse94
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 96
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin95:
.Lcontinue95:
  push 1
  pop rax
  cmp rax, 0
  je .Lend95
  mov rax, rbp
  sub rax, 96
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC55
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.097
  mov rax, 0
  call startswith
  jmp .L.end.097
.L.call.097:
  sub rsp, 8
  mov rax, 0
  call startswith
  add rsp, 8
.L.end.097:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse96
  mov rax, rbp
  sub rax, 96
  push rax
  mov rax, rbp
  sub rax, 96
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 2
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lcontinue95
  jmp .Lend96
.Lelse96:
.Lend96:
  mov rax, rbp
  sub rax, 96
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 34
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse98
  jmp .Lend95
  jmp .Lend98
.Lelse98:
.Lend98:
  mov rax, rbp
  sub rax, 96
  push rax
  mov rax, rbp
  sub rax, 96
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin95
.Lend95:
  mov rax, rbp
  sub rax, 100
  push rax
  mov rax, rbp
  sub rax, 96
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 56
  push rax
  push 12
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 100
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.099
  mov rax, 0
  call new_token
  jmp .L.end.099
.L.call.099:
  sub rsp, 8
  mov rax, 0
  call new_token
  add rsp, 8
.L.end.099:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 96
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lcontinue34
  jmp .Lend94
.Lelse94:
.Lend94:
  push 97
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse102
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 122
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse102
  push 1
  jmp .Lend102
.Lfalse102:
  push 0
.Lend102:
  pop rax
  cmp rax, 0
  jne .Ltrue101
  push 65
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse103
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 90
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse103
  push 1
  jmp .Lend103
.Lfalse103:
  push 0
.Lend103:
  pop rax
  cmp rax, 0
  jne .Ltrue101
  push 0
  jmp .Lend101
.Ltrue101:
  push 1
.Lend101:
  pop rax
  cmp rax, 0
  je .Lelse100
  mov rax, rbp
  sub rax, 108
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin104:
.Lcontinue104:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.105
  mov rax, 0
  call is_alnum
  jmp .L.end.105
.L.call.105:
  sub rsp, 8
  mov rax, 0
  call is_alnum
  add rsp, 8
.L.end.105:
  push rax
  pop rax
  cmp rax, 0
  je .Lend104
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin104
.Lend104:
  mov rax, rbp
  sub rax, 56
  push rax
  push 4
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 108
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 108
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.106
  mov rax, 0
  call new_token
  jmp .L.end.106
.L.call.106:
  sub rsp, 8
  mov rax, 0
  call new_token
  add rsp, 8
.L.end.106:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lcontinue34
  jmp .Lend100
.Lelse100:
.Lend100:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.108
  mov rax, 0
  call isdigit
  jmp .L.end.108
.L.call.108:
  sub rsp, 8
  mov rax, 0
  call isdigit
  add rsp, 8
.L.end.108:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse107
  mov rax, rbp
  sub rax, 56
  push rax
  push 2
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.109
  mov rax, 0
  call new_token
  jmp .L.end.109
.L.call.109:
  sub rsp, 8
  mov rax, 0
  call new_token
  add rsp, 8
.L.end.109:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 116
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  push 10
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.110
  mov rax, 0
  call strtol
  jmp .L.end.110
.L.call.110:
  sub rsp, 8
  mov rax, 0
  call strtol
  add rsp, 8
.L.end.110:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 116
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lcontinue34
  jmp .Lend107
.Lelse107:
.Lend107:
  push offset .LC56
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.111
  mov rax, 0
  call error_at0
  jmp .L.end.111
.L.call.111:
  sub rsp, 8
  mov rax, 0
  call error_at0
  add rsp, 8
.L.end.111:
  push rax
  jmp .Lbegin34
.Lend34:
  push 3
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.112
  mov rax, 0
  call new_token
  jmp .L.end.112
.L.call.112:
  sub rsp, 8
  mov rax, 0
  call new_token
  add rsp, 8
.L.end.112:
  push rax
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global read_char_literal
read_char_literal:
  push rbp
  mov rbp, rsp
  sub rsp, 33
  mov [rbp-8], rdi
  mov [rbp-16], rsi
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse113
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC57
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.114
  mov rax, 0
  call error_at0
  jmp .L.end.114
.L.call.114:
  sub rsp, 8
  mov rax, 0
  call error_at0
  add rsp, 8
.L.end.114:
  push rax
  jmp .Lend113
.Lelse113:
.Lend113:
  mov rax, rbp
  sub rax, 25
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 92
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse115
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 25
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.116
  mov rax, 0
  call get_escape_char
  jmp .L.end.116
.L.call.116:
  sub rsp, 8
  mov rax, 0
  call get_escape_char
  add rsp, 8
.L.end.116:
  push rax
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lend115
.Lelse115:
  mov rax, rbp
  sub rax, 25
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lend115
.Lend115:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 39
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse117
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC58
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.118
  mov rax, 0
  call error_at0
  jmp .L.end.118
.L.call.118:
  sub rsp, 8
  mov rax, 0
  call error_at0
  add rsp, 8
.L.end.118:
  push rax
  jmp .Lend117
.Lelse117:
.Lend117:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 33
  push rax
  push 2
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.119
  mov rax, 0
  call new_token
  jmp .L.end.119
.L.call.119:
  sub rsp, 8
  mov rax, 0
  call new_token
  add rsp, 8
.L.end.119:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 33
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 25
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 33
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global get_escape_char
get_escape_char:
  push rbp
  mov rbp, rsp
  sub rsp, 1
  mov [rbp-1], dil
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rax
  cmp rax, 48
  je .Lcase121
  cmp rax, 101
  je .Lcase122
  cmp rax, 114
  je .Lcase123
  cmp rax, 102
  je .Lcase124
  cmp rax, 118
  je .Lcase125
  cmp rax, 110
  je .Lcase126
  cmp rax, 116
  je .Lcase127
  cmp rax, 98
  je .Lcase128
  cmp rax, 97
  je .Lcase129
  jmp .Lcase130
  jmp .Lend120
.Lcase129:
  push 7
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase128:
  push 8
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase127:
  push 9
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase126:
  push 10
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase125:
  push 11
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase124:
  push 12
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase123:
  push 13
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase122:
  push 27
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase121:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase130:
  mov rax, rbp
  sub rax, 1
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend120:
  mov rsp, rbp
  pop rbp
  ret
.global new_node
new_node:
  push rbp
  mov rbp, rsp
  sub rsp, 12
  mov [rbp-4], edi
  mov rax, rbp
  sub rax, 12
  push rax
  push 1
  push 128
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.130
  mov rax, 0
  call calloc
  jmp .L.end.130
.L.call.130:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.130:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global new_binary
new_binary:
  push rbp
  mov rbp, rsp
  sub rsp, 28
  mov [rbp-4], edi
  mov [rbp-12], rsi
  mov [rbp-20], rdx
  mov rax, rbp
  sub rax, 28
  push rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.131
  mov rax, 0
  call new_node
  jmp .L.end.131
.L.call.131:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.131:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global new_unary
new_unary:
  push rbp
  mov rbp, rsp
  sub rsp, 20
  mov [rbp-4], edi
  mov [rbp-12], rsi
  mov rax, rbp
  sub rax, 20
  push rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.132
  mov rax, 0
  call new_node
  jmp .L.end.132
.L.call.132:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.132:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global new_num
new_num:
  push rbp
  mov rbp, rsp
  sub rsp, 12
  mov [rbp-4], edi
  mov rax, rbp
  sub rax, 12
  push rax
  push 5
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.133
  mov rax, 0
  call new_node
  jmp .L.end.133
.L.call.133:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.133:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global new_string
new_string:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  push 26
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.134
  mov rax, 0
  call new_node
  jmp .L.end.134
.L.call.134:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.134:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 80
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global program
program:
  push rbp
  mov rbp, rsp
  sub rsp, 44
  mov rax, rbp
  sub rax, 4
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin135:
.Lcontinue135:
  mov rax, rsp
  and rax, 15
  jnz .L.call.136
  mov rax, 0
  call at_eof
  jmp .L.end.136
.L.call.136:
  sub rsp, 8
  mov rax, 0
  call at_eof
  add rsp, 8
.L.end.136:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend135
  mov rax, rbp
  sub rax, 12
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 14
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.138
  mov rax, 0
  call consume_kind
  jmp .L.end.138
.L.call.138:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.138:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse137
  mov rax, rsp
  and rax, 15
  jnz .L.call.139
  mov rax, 0
  call define_typedef
  jmp .L.end.139
.L.call.139:
  sub rsp, 8
  mov rax, 0
  call define_typedef
  add rsp, 8
.L.end.139:
  push rax
  push offset .LC59
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.140
  mov rax, 0
  call expect
  jmp .L.end.140
.L.call.140:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.140:
  push rax
  jmp .Lcontinue135
  jmp .Lend137
.Lelse137:
.Lend137:
  push 15
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.142
  mov rax, 0
  call check_kind
  jmp .L.end.142
.L.call.142:
  sub rsp, 8
  mov rax, 0
  call check_kind
  add rsp, 8
.L.end.142:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse141
  mov rax, rsp
  and rax, 15
  jnz .L.call.143
  mov rax, 0
  call define_enum
  jmp .L.end.143
.L.call.143:
  sub rsp, 8
  mov rax, 0
  call define_enum
  add rsp, 8
.L.end.143:
  push rax
  push offset .LC60
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.144
  mov rax, 0
  call expect
  jmp .L.end.144
.L.call.144:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.144:
  push rax
  jmp .Lcontinue135
  jmp .Lend141
.Lelse141:
.Lend141:
  mov rax, rbp
  sub rax, 20
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.145
  mov rax, 0
  call read_define_head
  jmp .L.end.145
.L.call.145:
  sub rsp, 8
  mov rax, 0
  call read_define_head
  add rsp, 8
.L.end.145:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse146
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.147
  mov rax, 0
  call print_token
  jmp .L.end.147
.L.call.147:
  sub rsp, 8
  mov rax, 0
  call print_token
  add rsp, 8
.L.end.147:
  push rax
  push offset .LC61
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.148
  mov rax, 0
  call error0
  jmp .L.end.148
.L.call.148:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.148:
  push rax
  jmp .Lend146
.Lelse146:
.Lend146:
  push offset .LC62
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.150
  mov rax, 0
  call consume
  jmp .L.end.150
.L.call.150:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.150:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse149
  mov rax, rbp
  sub rax, 28
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.151
  mov rax, 0
  call func_def
  jmp .L.end.151
.L.call.151:
  sub rsp, 8
  mov rax, 0
  call func_def
  add rsp, 8
.L.end.151:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse152
  push offset code
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend152
.Lelse152:
  jmp .Lcontinue135
  jmp .Lend152
.Lend152:
  jmp .Lend149
.Lelse149:
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 5
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse153
  push offset .LC63
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.155
  mov rax, 0
  call check
  jmp .L.end.155
.L.call.155:
  sub rsp, 8
  mov rax, 0
  call check
  add rsp, 8
.L.end.155:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse154
  mov rax, rbp
  sub rax, 36
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset globals
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.156
  mov rax, 0
  call define_variable
  jmp .L.end.156
.L.call.156:
  sub rsp, 8
  mov rax, 0
  call define_variable
  add rsp, 8
.L.end.156:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  push 12
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push offset code
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC64
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.157
  mov rax, 0
  call expect
  jmp .L.end.157
.L.call.157:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.157:
  push rax
  jmp .Lend154
.Lelse154:
  mov rax, rsp
  and rax, 15
  jnz .L.call.158
  mov rax, 0
  call define_struct
  jmp .L.end.158
.L.call.158:
  sub rsp, 8
  mov rax, 0
  call define_struct
  add rsp, 8
.L.end.158:
  push rax
  push offset .LC65
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.159
  mov rax, 0
  call expect
  jmp .L.end.159
.L.call.159:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.159:
  push rax
  jmp .Lcontinue135
  jmp .Lend154
.Lend154:
  jmp .Lend153
.Lelse153:
  mov rax, rbp
  sub rax, 44
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset globals
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.160
  mov rax, 0
  call define_variable
  jmp .L.end.160
.L.call.160:
  sub rsp, 8
  mov rax, 0
  call define_variable
  add rsp, 8
.L.end.160:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  push 12
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push offset code
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC66
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.161
  mov rax, 0
  call expect
  jmp .L.end.161
.L.call.161:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.161:
  push rax
  jmp .Lend153
.Lend153:
  jmp .Lend149
.Lend149:
  mov rax, rbp
  sub rax, 4
  push rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin135
.Lend135:
  push offset code
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rsp, rbp
  pop rbp
  ret
.global define_typedef
define_typedef:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.162
  mov rax, 0
  call read_define_head
  jmp .L.end.162
.L.call.162:
  sub rsp, 8
  mov rax, 0
  call read_define_head
  add rsp, 8
.L.end.162:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.163
  mov rax, 0
  call read_define_head
  jmp .L.end.163
.L.call.163:
  sub rsp, 8
  mov rax, 0
  call read_define_head
  add rsp, 8
.L.end.163:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.164
  mov rax, 0
  call push_tag
  jmp .L.end.164
.L.call.164:
  sub rsp, 8
  mov rax, 0
  call push_tag
  add rsp, 8
.L.end.164:
  push rax
  push 1
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global define_enum
define_enum:
  push rbp
  mov rbp, rsp
  sub rsp, 20
  push 15
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.166
  mov rax, 0
  call consume_kind
  jmp .L.end.166
.L.call.166:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.166:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse165
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend165
.Lelse165:
.Lend165:
  mov rax, rbp
  sub rax, 8
  push rax
  push 4
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.167
  mov rax, 0
  call consume_kind
  jmp .L.end.167
.L.call.167:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.167:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse169
  push offset .LC67
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.170
  mov rax, 0
  call check
  jmp .L.end.170
.L.call.170:
  sub rsp, 8
  mov rax, 0
  call check
  add rsp, 8
.L.end.170:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse169
  push 1
  jmp .Lend169
.Lfalse169:
  push 0
.Lend169:
  pop rax
  cmp rax, 0
  je .Lelse168
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.171
  mov rax, 0
  call find_or_register_tag
  jmp .L.end.171
.L.call.171:
  sub rsp, 8
  mov rax, 0
  call find_or_register_tag
  add rsp, 8
.L.end.171:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse172
  push offset .LC68
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.173
  mov rax, 0
  call error0
  jmp .L.end.173
.L.call.173:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.173:
  push rax
  jmp .Lend172
.Lelse172:
.Lend172:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend168
.Lelse168:
.Lend168:
  push offset .LC69
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.174
  mov rax, 0
  call expect
  jmp .L.end.174
.L.call.174:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.174:
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin175:
.Lcontinue175:
  push 1
  pop rax
  cmp rax, 0
  je .Lend175
  push offset .LC70
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.177
  mov rax, 0
  call consume
  jmp .L.end.177
.L.call.177:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.177:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse176
  jmp .Lend175
  jmp .Lend176
.Lelse176:
.Lend176:
  mov rax, rbp
  sub rax, 20
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.178
  mov rax, 0
  call define_enum_member
  jmp .L.end.178
.L.call.178:
  sub rsp, 8
  mov rax, 0
  call define_enum_member
  add rsp, 8
.L.end.178:
  push rax
  push offset .LC71
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.180
  mov rax, 0
  call consume
  jmp .L.end.180
.L.call.180:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.180:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse179
  jmp .Lend175
  jmp .Lend179
.Lelse179:
.Lend179:
  push offset .LC72
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.181
  mov rax, 0
  call expect
  jmp .L.end.181
.L.call.181:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.181:
  push rax
  jmp .Lbegin175
.Lend175:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse182
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.184
  mov rax, 0
  call int_type
  jmp .L.end.184
.L.call.184:
  sub rsp, 8
  mov rax, 0
  call int_type
  add rsp, 8
.L.end.184:
  push rax
  push 1
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.183
  mov rax, 0
  call push_tag
  jmp .L.end.183
.L.call.183:
  sub rsp, 8
  mov rax, 0
  call push_tag
  add rsp, 8
.L.end.183:
  push rax
  jmp .Lend182
.Lelse182:
.Lend182:
  mov rax, rsp
  and rax, 15
  jnz .L.call.185
  mov rax, 0
  call int_type
  jmp .L.end.185
.L.call.185:
  sub rsp, 8
  mov rax, 0
  call int_type
  add rsp, 8
.L.end.185:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global define_enum_member
define_enum_member:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  push 4
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.186
  mov rax, 0
  call expect_kind
  jmp .L.end.186
.L.call.186:
  sub rsp, 8
  mov rax, 0
  call expect_kind
  add rsp, 8
.L.end.186:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC73
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.188
  mov rax, 0
  call consume
  jmp .L.end.188
.L.call.188:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.188:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse187
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.189
  mov rax, 0
  call expect_number
  jmp .L.end.189
.L.call.189:
  sub rsp, 8
  mov rax, 0
  call expect_number
  add rsp, 8
.L.end.189:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend187
.Lelse187:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend187
.Lend187:
  mov rax, rbp
  sub rax, 24
  push rax
  push 1
  push 24
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.190
  mov rax, 0
  call calloc
  jmp .L.end.190
.L.call.190:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.190:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  push 100
  push 1
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.191
  mov rax, 0
  call calloc
  jmp .L.end.191
.L.call.191:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.191:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.192
  mov rax, 0
  call memcpy
  jmp .L.end.192
.L.call.192:
  sub rsp, 8
  mov rax, 0
  call memcpy
  add rsp, 8
.L.end.192:
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  push offset enum_vars
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset enum_vars
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global int_type
int_type:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, rbp
  sub rax, 8
  push rax
  push 1
  push 40
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.193
  mov rax, 0
  call calloc
  jmp .L.end.193
.L.call.193:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.193:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  push 1
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  push 4
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global char_type
char_type:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, rbp
  sub rax, 8
  push rax
  push 1
  push 40
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.194
  mov rax, 0
  call calloc
  jmp .L.end.194
.L.call.194:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.194:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  push 2
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  push 1
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global ptr_type
ptr_type:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, rbp
  sub rax, 8
  push rax
  push 1
  push 40
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.195
  mov rax, 0
  call calloc
  jmp .L.end.195
.L.call.195:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.195:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  push 3
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  push 8
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global define_struct
define_struct:
  push rbp
  mov rbp, rsp
  sub rsp, 92
  push 13
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.197
  mov rax, 0
  call consume_kind
  jmp .L.end.197
.L.call.197:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.197:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse196
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend196
.Lelse196:
.Lend196:
  mov rax, rbp
  sub rax, 8
  push rax
  push 4
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.198
  mov rax, 0
  call consume_kind
  jmp .L.end.198
.L.call.198:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.198:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse200
  push offset .LC74
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.201
  mov rax, 0
  call check
  jmp .L.end.201
.L.call.201:
  sub rsp, 8
  mov rax, 0
  call check
  add rsp, 8
.L.end.201:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse200
  push 1
  jmp .Lend200
.Lfalse200:
  push 0
.Lend200:
  pop rax
  cmp rax, 0
  je .Lelse199
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.202
  mov rax, 0
  call find_or_register_tag
  jmp .L.end.202
.L.call.202:
  sub rsp, 8
  mov rax, 0
  call find_or_register_tag
  add rsp, 8
.L.end.202:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  push 5
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend199
.Lelse199:
.Lend199:
  push offset .LC75
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.203
  mov rax, 0
  call expect
  jmp .L.end.203
.L.call.203:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.203:
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  push 1
  push 40
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.204
  mov rax, 0
  call calloc
  jmp .L.end.204
.L.call.204:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.204:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  push 5
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  mov rax, rbp
  sub rax, 56
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 68
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin205:
.Lcontinue205:
  push offset .LC76
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.206
  mov rax, 0
  call consume
  jmp .L.end.206
.L.call.206:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.206:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend205
  mov rax, rbp
  sub rax, 80
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.207
  mov rax, 0
  call read_define
  jmp .L.end.207
.L.call.207:
  sub rsp, 8
  mov rax, 0
  call read_define
  add rsp, 8
.L.end.207:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC77
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.208
  mov rax, 0
  call expect
  jmp .L.end.208
.L.call.208:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.208:
  push rax
  mov rax, rbp
  sub rax, 88
  push rax
  push 1
  push 32
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.209
  mov rax, 0
  call calloc
  jmp .L.end.209
.L.call.209:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.209:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  push 100
  push 1
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.210
  mov rax, 0
  call calloc
  jmp .L.end.210
.L.call.210:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.210:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 80
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 80
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.211
  mov rax, 0
  call memcpy
  jmp .L.end.211
.L.call.211:
  sub rsp, 8
  mov rax, 0
  call memcpy
  add rsp, 8
.L.end.211:
  push rax
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 80
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 92
  push rax
  mov rax, rbp
  sub rax, 80
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.212
  mov rax, 0
  call get_type_size
  jmp .L.end.212
.L.call.212:
  sub rsp, 8
  mov rax, 0
  call get_type_size
  add rsp, 8
.L.end.212:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 68
  push rax
  mov rax, rbp
  sub rax, 68
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 92
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.213
  mov rax, 0
  call align_to
  jmp .L.end.213
.L.call.213:
  sub rsp, 8
  mov rax, 0
  call align_to
  add rsp, 8
.L.end.213:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  mov rax, rbp
  sub rax, 68
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 68
  push rax
  mov rax, rbp
  sub rax, 68
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 92
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 64
  push rax
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse215
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 92
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse215
  push 1
  jmp .Lend215
.Lfalse215:
  push 0
.Lend215:
  pop rax
  cmp rax, 0
  je .Lelse214
  mov rax, rbp
  sub rax, 72
  push rax
  mov rax, rbp
  sub rax, 92
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend214
.Lelse214:
.Lend214:
  jmp .Lbegin205
.Lend205:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  mov rax, rbp
  sub rax, 68
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.216
  mov rax, 0
  call align_to
  jmp .L.end.216
.L.call.216:
  sub rsp, 8
  mov rax, 0
  call align_to
  add rsp, 8
.L.end.216:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse217
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.218
  mov rax, 0
  call push_tag
  jmp .L.end.218
.L.call.218:
  sub rsp, 8
  mov rax, 0
  call push_tag
  add rsp, 8
.L.end.218:
  push rax
  jmp .Lend217
.Lelse217:
.Lend217:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global func_def
func_def:
  push rbp
  mov rbp, rsp
  sub rsp, 20
  mov [rbp-8], rdi
  push offset cur_scope_depth
  push offset cur_scope_depth
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  push 23
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.219
  mov rax, 0
  call new_node
  jmp .L.end.219
.L.call.219:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.219:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  push 100
  push 1
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.220
  mov rax, 0
  call calloc
  jmp .L.end.220
.L.call.220:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.220:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.221
  mov rax, 0
  call memcpy
  jmp .L.end.221
.L.call.221:
  sub rsp, 8
  mov rax, 0
  call memcpy
  add rsp, 8
.L.end.221:
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 48
  push rax
  push 10
  push 8
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.222
  mov rax, 0
  call calloc
  jmp .L.end.222
.L.call.222:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.222:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 20
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin223:
  push offset .LC78
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.224
  mov rax, 0
  call consume
  jmp .L.end.224
.L.call.224:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.224:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend223
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse225
  push offset .LC79
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.226
  mov rax, 0
  call expect
  jmp .L.end.226
.L.call.226:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.226:
  push rax
  jmp .Lend225
.Lelse225:
.Lend225:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 48
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.227
  mov rax, 0
  call define_arg
  jmp .L.end.227
.L.call.227:
  sub rsp, 8
  mov rax, 0
  call define_arg
  add rsp, 8
.L.end.227:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lcontinue223:
  mov rax, rbp
  sub rax, 20
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin223
.Lend223:
  push offset .LC80
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.229
  mov rax, 0
  call consume
  jmp .L.end.229
.L.call.229:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.229:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse228
  push offset locals
  push offset cur_scope_depth
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset cur_scope_depth
  push offset cur_scope_depth
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend228
.Lelse228:
.Lend228:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.230
  mov rax, 0
  call block
  jmp .L.end.230
.L.call.230:
  sub rsp, 8
  mov rax, 0
  call block
  add rsp, 8
.L.end.230:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global define_arg
define_arg:
  push rbp
  mov rbp, rsp
  sub rsp, 808
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.231
  mov rax, 0
  call read_define_head
  jmp .L.end.231
.L.call.231:
  sub rsp, 8
  mov rax, 0
  call read_define_head
  add rsp, 8
.L.end.231:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse232
  mov rax, rbp
  sub rax, 808
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 40
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 48
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 56
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 64
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 72
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 80
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 88
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 96
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 104
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 112
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 120
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 128
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 136
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 144
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 152
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 160
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 168
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 176
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 184
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 192
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 200
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 208
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 216
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 224
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 232
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 240
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 248
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 256
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 264
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 272
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 280
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 288
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 296
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 304
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 312
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 320
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 328
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 336
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 344
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 352
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 360
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 368
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 376
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 384
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 392
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 400
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 408
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 416
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 424
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 432
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 440
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 448
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 456
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 464
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 472
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 480
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 488
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 496
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 504
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 512
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 520
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 528
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 536
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 544
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 552
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 560
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 568
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 576
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 584
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 592
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 600
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 608
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 616
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 624
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 632
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 640
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 648
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 656
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 664
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 672
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 680
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 688
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 696
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 704
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 712
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 720
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 728
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 736
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 744
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 752
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 760
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 768
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 776
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 784
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push 792
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 808
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.233
  mov rax, 0
  call memcpy
  jmp .L.end.233
.L.call.233:
  sub rsp, 8
  mov rax, 0
  call memcpy
  add rsp, 8
.L.end.233:
  push rax
  push offset .LC81
  mov rax, rbp
  sub rax, 808
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.234
  mov rax, 0
  call error1
  jmp .L.end.234
.L.call.234:
  sub rsp, 8
  mov rax, 0
  call error1
  add rsp, 8
.L.end.234:
  push rax
  jmp .Lend232
.Lelse232:
.Lend232:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset locals
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.235
  mov rax, 0
  call define_variable
  jmp .L.end.235
.L.call.235:
  sub rsp, 8
  mov rax, 0
  call define_variable
  add rsp, 8
.L.end.235:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global stmt
stmt:
  push rbp
  mov rbp, rsp
  sub rsp, 140
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 5
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.237
  mov rax, 0
  call consume_kind
  jmp .L.end.237
.L.call.237:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.237:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse236
  mov rax, rbp
  sub rax, 8
  push rax
  push 14
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.238
  mov rax, 0
  call new_node
  jmp .L.end.238
.L.call.238:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.238:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC82
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.240
  mov rax, 0
  call consume
  jmp .L.end.240
.L.call.240:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.240:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse239
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.241
  mov rax, 0
  call expr
  jmp .L.end.241
.L.call.241:
  sub rsp, 8
  mov rax, 0
  call expr
  add rsp, 8
.L.end.241:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC83
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.242
  mov rax, 0
  call expect
  jmp .L.end.242
.L.call.242:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.242:
  push rax
  jmp .Lend239
.Lelse239:
.Lend239:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend236
.Lelse236:
.Lend236:
  push 6
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.244
  mov rax, 0
  call consume_kind
  jmp .L.end.244
.L.call.244:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.244:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse243
  mov rax, rbp
  sub rax, 8
  push rax
  push 15
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.245
  mov rax, 0
  call new_node
  jmp .L.end.245
.L.call.245:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.245:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC84
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.246
  mov rax, 0
  call expect
  jmp .L.end.246
.L.call.246:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.246:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.247
  mov rax, 0
  call expr
  jmp .L.end.247
.L.call.247:
  sub rsp, 8
  mov rax, 0
  call expr
  add rsp, 8
.L.end.247:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC85
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.248
  mov rax, 0
  call expect
  jmp .L.end.248
.L.call.248:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.248:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.249
  mov rax, 0
  call stmt
  jmp .L.end.249
.L.call.249:
  sub rsp, 8
  mov rax, 0
  call stmt
  add rsp, 8
.L.end.249:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 7
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.251
  mov rax, 0
  call consume_kind
  jmp .L.end.251
.L.call.251:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.251:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse250
  mov rax, rbp
  sub rax, 16
  push rax
  push 16
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.252
  mov rax, 0
  call new_node
  jmp .L.end.252
.L.call.252:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.252:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.253
  mov rax, 0
  call stmt
  jmp .L.end.253
.L.call.253:
  sub rsp, 8
  mov rax, 0
  call stmt
  add rsp, 8
.L.end.253:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend250
.Lelse250:
.Lend250:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend243
.Lelse243:
.Lend243:
  push 8
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.255
  mov rax, 0
  call consume_kind
  jmp .L.end.255
.L.call.255:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.255:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse254
  mov rax, rbp
  sub rax, 24
  push rax
  push 17
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.256
  mov rax, 0
  call new_node
  jmp .L.end.256
.L.call.256:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.256:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC86
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.257
  mov rax, 0
  call expect
  jmp .L.end.257
.L.call.257:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.257:
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.258
  mov rax, 0
  call expr
  jmp .L.end.258
.L.call.258:
  sub rsp, 8
  mov rax, 0
  call expr
  add rsp, 8
.L.end.258:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC87
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.259
  mov rax, 0
  call expect
  jmp .L.end.259
.L.call.259:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.259:
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.260
  mov rax, 0
  call stmt
  jmp .L.end.260
.L.call.260:
  sub rsp, 8
  mov rax, 0
  call stmt
  add rsp, 8
.L.end.260:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend254
.Lelse254:
.Lend254:
  push 18
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.262
  mov rax, 0
  call consume_kind
  jmp .L.end.262
.L.call.262:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.262:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse261
  mov rax, rbp
  sub rax, 32
  push rax
  push 42
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.263
  mov rax, 0
  call new_node
  jmp .L.end.263
.L.call.263:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.263:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC88
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.264
  mov rax, 0
  call expect
  jmp .L.end.264
.L.call.264:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.264:
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.265
  mov rax, 0
  call expr
  jmp .L.end.265
.L.call.265:
  sub rsp, 8
  mov rax, 0
  call expr
  add rsp, 8
.L.end.265:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC89
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.266
  mov rax, 0
  call expect
  jmp .L.end.266
.L.call.266:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.266:
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  push offset current_switch
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset current_switch
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.267
  mov rax, 0
  call stmt
  jmp .L.end.267
.L.call.267:
  sub rsp, 8
  mov rax, 0
  call stmt
  add rsp, 8
.L.end.267:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset current_switch
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend261
.Lelse261:
.Lend261:
  push 19
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.269
  mov rax, 0
  call consume_kind
  jmp .L.end.269
.L.call.269:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.269:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse268
  push offset current_switch
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse270
  push offset .LC90
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.271
  mov rax, 0
  call error0
  jmp .L.end.271
.L.call.271:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.271:
  push rax
  jmp .Lend270
.Lelse270:
.Lend270:
  mov rax, rbp
  sub rax, 48
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 56
  push rax
  push 4
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.272
  mov rax, 0
  call consume_kind
  jmp .L.end.272
.L.call.272:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.272:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 64
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse273
  mov rax, rbp
  sub rax, 64
  push rax
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.274
  mov rax, 0
  call find_enum_var
  jmp .L.end.274
.L.call.274:
  sub rsp, 8
  mov rax, 0
  call find_enum_var
  add rsp, 8
.L.end.274:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend273
.Lelse273:
.Lend273:
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse275
  mov rax, rbp
  sub rax, 64
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.276
  mov rax, 0
  call convert_predefined_keyword_to_num
  jmp .L.end.276
.L.call.276:
  sub rsp, 8
  mov rax, 0
  call convert_predefined_keyword_to_num
  add rsp, 8
.L.end.276:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend275
.Lelse275:
.Lend275:
  mov rax, rbp
  sub rax, 68
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse277
  mov rax, rbp
  sub rax, 68
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend277
.Lelse277:
  mov rax, rbp
  sub rax, 68
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.278
  mov rax, 0
  call expect_number
  jmp .L.end.278
.L.call.278:
  sub rsp, 8
  mov rax, 0
  call expect_number
  add rsp, 8
.L.end.278:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend277
.Lend277:
  push offset .LC91
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.279
  mov rax, 0
  call expect
  jmp .L.end.279
.L.call.279:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.279:
  push rax
  mov rax, rbp
  sub rax, 76
  push rax
  push 43
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.280
  mov rax, 0
  call new_node
  jmp .L.end.280
.L.call.280:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.280:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 76
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  mov rax, rbp
  sub rax, 68
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 76
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 104
  push rax
  push offset current_switch
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 104
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset current_switch
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 104
  push rax
  mov rax, rbp
  sub rax, 76
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 76
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend268
.Lelse268:
.Lend268:
  push 20
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.282
  mov rax, 0
  call consume_kind
  jmp .L.end.282
.L.call.282:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.282:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse281
  push offset current_switch
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse283
  push offset .LC92
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.284
  mov rax, 0
  call error0
  jmp .L.end.284
.L.call.284:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.284:
  push rax
  jmp .Lend283
.Lelse283:
.Lend283:
  mov rax, rbp
  sub rax, 84
  push rax
  push 43
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.285
  mov rax, 0
  call new_node
  jmp .L.end.285
.L.call.285:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.285:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC93
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.286
  mov rax, 0
  call expect
  jmp .L.end.286
.L.call.286:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.286:
  push rax
  push offset current_switch
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 112
  push rax
  mov rax, rbp
  sub rax, 84
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 84
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend281
.Lelse281:
.Lend281:
  push 9
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.288
  mov rax, 0
  call consume_kind
  jmp .L.end.288
.L.call.288:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.288:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse287
  mov rax, rbp
  sub rax, 92
  push rax
  push 18
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.289
  mov rax, 0
  call new_node
  jmp .L.end.289
.L.call.289:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.289:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 100
  push rax
  push 19
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.290
  mov rax, 0
  call new_node
  jmp .L.end.290
.L.call.290:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.290:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 20
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.291
  mov rax, 0
  call new_node
  jmp .L.end.291
.L.call.291:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.291:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 92
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 100
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 92
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 108
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC94
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.292
  mov rax, 0
  call expect
  jmp .L.end.292
.L.call.292:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.292:
  push rax
  push offset .LC95
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.294
  mov rax, 0
  call consume
  jmp .L.end.294
.L.call.294:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.294:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse293
  mov rax, rbp
  sub rax, 100
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.295
  mov rax, 0
  call stmt
  jmp .L.end.295
.L.call.295:
  sub rsp, 8
  mov rax, 0
  call stmt
  add rsp, 8
.L.end.295:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend293
.Lelse293:
.Lend293:
  push offset .LC96
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.297
  mov rax, 0
  call consume
  jmp .L.end.297
.L.call.297:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.297:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse296
  mov rax, rbp
  sub rax, 100
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.298
  mov rax, 0
  call expr
  jmp .L.end.298
.L.call.298:
  sub rsp, 8
  mov rax, 0
  call expr
  add rsp, 8
.L.end.298:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC97
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.299
  mov rax, 0
  call expect
  jmp .L.end.299
.L.call.299:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.299:
  push rax
  jmp .Lend296
.Lelse296:
.Lend296:
  push offset .LC98
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.301
  mov rax, 0
  call consume
  jmp .L.end.301
.L.call.301:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.301:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse300
  mov rax, rbp
  sub rax, 108
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.302
  mov rax, 0
  call expr
  jmp .L.end.302
.L.call.302:
  sub rsp, 8
  mov rax, 0
  call expr
  add rsp, 8
.L.end.302:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC99
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.303
  mov rax, 0
  call expect
  jmp .L.end.303
.L.call.303:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.303:
  push rax
  jmp .Lend300
.Lelse300:
.Lend300:
  mov rax, rbp
  sub rax, 108
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.304
  mov rax, 0
  call stmt
  jmp .L.end.304
.L.call.304:
  sub rsp, 8
  mov rax, 0
  call stmt
  add rsp, 8
.L.end.304:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 92
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend287
.Lelse287:
.Lend287:
  push 16
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.306
  mov rax, 0
  call consume_kind
  jmp .L.end.306
.L.call.306:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.306:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse305
  mov rax, rbp
  sub rax, 116
  push rax
  push 28
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.307
  mov rax, 0
  call new_node
  jmp .L.end.307
.L.call.307:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.307:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC100
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.308
  mov rax, 0
  call expect
  jmp .L.end.308
.L.call.308:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.308:
  push rax
  mov rax, rbp
  sub rax, 116
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend305
.Lelse305:
.Lend305:
  push 17
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.310
  mov rax, 0
  call consume_kind
  jmp .L.end.310
.L.call.310:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.310:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse309
  mov rax, rbp
  sub rax, 124
  push rax
  push 29
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.311
  mov rax, 0
  call new_node
  jmp .L.end.311
.L.call.311:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.311:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC101
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.312
  mov rax, 0
  call expect
  jmp .L.end.312
.L.call.312:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.312:
  push rax
  mov rax, rbp
  sub rax, 124
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend309
.Lelse309:
.Lend309:
  push offset .LC102
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.314
  mov rax, 0
  call check
  jmp .L.end.314
.L.call.314:
  sub rsp, 8
  mov rax, 0
  call check
  add rsp, 8
.L.end.314:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse313
  mov rax, rsp
  and rax, 15
  jnz .L.call.315
  mov rax, 0
  call block
  jmp .L.end.315
.L.call.315:
  sub rsp, 8
  mov rax, 0
  call block
  add rsp, 8
.L.end.315:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend313
.Lelse313:
.Lend313:
  mov rax, rbp
  sub rax, 132
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.316
  mov rax, 0
  call read_define_head
  jmp .L.end.316
.L.call.316:
  sub rsp, 8
  mov rax, 0
  call read_define_head
  add rsp, 8
.L.end.316:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 132
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse317
  mov rax, rbp
  sub rax, 140
  push rax
  mov rax, rbp
  sub rax, 132
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset locals
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.318
  mov rax, 0
  call define_variable
  jmp .L.end.318
.L.call.318:
  sub rsp, 8
  mov rax, 0
  call define_variable
  add rsp, 8
.L.end.318:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 140
  push rax
  mov rax, rbp
  sub rax, 140
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.319
  mov rax, 0
  call local_variable_init
  jmp .L.end.319
.L.call.319:
  sub rsp, 8
  mov rax, 0
  call local_variable_init
  add rsp, 8
.L.end.319:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC103
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.320
  mov rax, 0
  call expect
  jmp .L.end.320
.L.call.320:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.320:
  push rax
  mov rax, rbp
  sub rax, 140
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend317
.Lelse317:
.Lend317:
  mov rax, rbp
  sub rax, 140
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.321
  mov rax, 0
  call expr
  jmp .L.end.321
.L.call.321:
  sub rsp, 8
  mov rax, 0
  call expr
  add rsp, 8
.L.end.321:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC104
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.322
  mov rax, 0
  call expect
  jmp .L.end.322
.L.call.322:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.322:
  push rax
  mov rax, rbp
  sub rax, 140
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global block
block:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov rax, rbp
  sub rax, 8
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC105
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.324
  mov rax, 0
  call consume
  jmp .L.end.324
.L.call.324:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.324:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse323
  mov rax, rbp
  sub rax, 8
  push rax
  push 21
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.325
  mov rax, 0
  call new_node
  jmp .L.end.325
.L.call.325:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.325:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 12
  push rax
  push 1000
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 128
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.326
  mov rax, 0
  call calloc
  jmp .L.end.326
.L.call.326:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.326:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin327:
  push offset .LC106
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.328
  mov rax, 0
  call consume
  jmp .L.end.328
.L.call.328:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.328:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend327
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse329
  push offset .LC107
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.330
  mov rax, 0
  call error0
  jmp .L.end.330
.L.call.330:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.330:
  push rax
  jmp .Lend329
.Lelse329:
.Lend329:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.331
  mov rax, 0
  call stmt
  jmp .L.end.331
.L.call.331:
  sub rsp, 8
  mov rax, 0
  call stmt
  add rsp, 8
.L.end.331:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lcontinue327:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin327
.Lend327:
  jmp .Lend323
.Lelse323:
.Lend323:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global expr
expr:
  push rbp
  mov rbp, rsp
  mov rax, rsp
  and rax, 15
  jnz .L.call.332
  mov rax, 0
  call assign
  jmp .L.end.332
.L.call.332:
  sub rsp, 8
  mov rax, 0
  call assign
  add rsp, 8
.L.end.332:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global assign
assign:
  push rbp
  mov rbp, rsp
  sub rsp, 56
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.333
  mov rax, 0
  call conditional
  jmp .L.end.333
.L.call.333:
  sub rsp, 8
  mov rax, 0
  call conditional
  add rsp, 8
.L.end.333:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC108
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.335
  mov rax, 0
  call consume
  jmp .L.end.335
.L.call.335:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.335:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse334
  mov rax, rbp
  sub rax, 8
  push rax
  push 10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.337
  mov rax, 0
  call conditional
  jmp .L.end.337
.L.call.337:
  sub rsp, 8
  mov rax, 0
  call conditional
  add rsp, 8
.L.end.337:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.336
  mov rax, 0
  call new_binary
  jmp .L.end.336
.L.call.336:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.336:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend334
.Lelse334:
.Lend334:
  push offset .LC109
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.339
  mov rax, 0
  call consume
  jmp .L.end.339
.L.call.339:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.339:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse338
  mov rax, rbp
  sub rax, 16
  push rax
  push 1
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.342
  mov rax, 0
  call conditional
  jmp .L.end.342
.L.call.342:
  sub rsp, 8
  mov rax, 0
  call conditional
  add rsp, 8
.L.end.342:
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.341
  mov rax, 0
  call ptr_conversion
  jmp .L.end.341
.L.call.341:
  sub rsp, 8
  mov rax, 0
  call ptr_conversion
  add rsp, 8
.L.end.341:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.340
  mov rax, 0
  call new_binary
  jmp .L.end.340
.L.call.340:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.340:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  push 10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.343
  mov rax, 0
  call new_binary
  jmp .L.end.343
.L.call.343:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.343:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend338
.Lelse338:
.Lend338:
  push offset .LC110
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.345
  mov rax, 0
  call consume
  jmp .L.end.345
.L.call.345:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.345:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse344
  mov rax, rbp
  sub rax, 24
  push rax
  push 2
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.348
  mov rax, 0
  call conditional
  jmp .L.end.348
.L.call.348:
  sub rsp, 8
  mov rax, 0
  call conditional
  add rsp, 8
.L.end.348:
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.347
  mov rax, 0
  call ptr_conversion
  jmp .L.end.347
.L.call.347:
  sub rsp, 8
  mov rax, 0
  call ptr_conversion
  add rsp, 8
.L.end.347:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.346
  mov rax, 0
  call new_binary
  jmp .L.end.346
.L.call.346:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.346:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  push 10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.349
  mov rax, 0
  call new_binary
  jmp .L.end.349
.L.call.349:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.349:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend344
.Lelse344:
.Lend344:
  push offset .LC111
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.351
  mov rax, 0
  call consume
  jmp .L.end.351
.L.call.351:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.351:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse350
  mov rax, rbp
  sub rax, 32
  push rax
  push 3
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.352
  mov rax, 0
  call new_node
  jmp .L.end.352
.L.call.352:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.352:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 40
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.353
  mov rax, 0
  call conditional
  jmp .L.end.353
.L.call.353:
  sub rsp, 8
  mov rax, 0
  call conditional
  add rsp, 8
.L.end.353:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  push 10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.354
  mov rax, 0
  call new_binary
  jmp .L.end.354
.L.call.354:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.354:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend350
.Lelse350:
.Lend350:
  push offset .LC112
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.356
  mov rax, 0
  call consume
  jmp .L.end.356
.L.call.356:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.356:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse355
  mov rax, rbp
  sub rax, 48
  push rax
  push 4
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.357
  mov rax, 0
  call new_node
  jmp .L.end.357
.L.call.357:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.357:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 56
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.358
  mov rax, 0
  call conditional
  jmp .L.end.358
.L.call.358:
  sub rsp, 8
  mov rax, 0
  call conditional
  add rsp, 8
.L.end.358:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  push 10
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.359
  mov rax, 0
  call new_binary
  jmp .L.end.359
.L.call.359:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.359:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend355
.Lelse355:
.Lend355:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global conditional
conditional:
  push rbp
  mov rbp, rsp
  sub rsp, 48
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.360
  mov rax, 0
  call logor
  jmp .L.end.360
.L.call.360:
  sub rsp, 8
  mov rax, 0
  call logor
  add rsp, 8
.L.end.360:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC113
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.362
  mov rax, 0
  call consume
  jmp .L.end.362
.L.call.362:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.362:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse361
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.363
  mov rax, 0
  call logor
  jmp .L.end.363
.L.call.363:
  sub rsp, 8
  mov rax, 0
  call logor
  add rsp, 8
.L.end.363:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC114
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.364
  mov rax, 0
  call expect
  jmp .L.end.364
.L.call.364:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.364:
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.365
  mov rax, 0
  call logor
  jmp .L.end.365
.L.call.365:
  sub rsp, 8
  mov rax, 0
  call logor
  add rsp, 8
.L.end.365:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 40
  push rax
  push 16
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.366
  mov rax, 0
  call new_node
  jmp .L.end.366
.L.call.366:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.366:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 48
  push rax
  push 41
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.367
  mov rax, 0
  call new_node
  jmp .L.end.367
.L.call.367:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.367:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend361
.Lelse361:
.Lend361:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global logor
logor:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.368
  mov rax, 0
  call logand
  jmp .L.end.368
.L.call.368:
  sub rsp, 8
  mov rax, 0
  call logand
  add rsp, 8
.L.end.368:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin369:
.Lcontinue369:
  push offset .LC115
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.370
  mov rax, 0
  call consume
  jmp .L.end.370
.L.call.370:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.370:
  push rax
  pop rax
  cmp rax, 0
  je .Lend369
  mov rax, rbp
  sub rax, 8
  push rax
  push 39
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.372
  mov rax, 0
  call logor
  jmp .L.end.372
.L.call.372:
  sub rsp, 8
  mov rax, 0
  call logor
  add rsp, 8
.L.end.372:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.371
  mov rax, 0
  call new_binary
  jmp .L.end.371
.L.call.371:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.371:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lbegin369
.Lend369:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global logand
logand:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.373
  mov rax, 0
  call bitor
  jmp .L.end.373
.L.call.373:
  sub rsp, 8
  mov rax, 0
  call bitor
  add rsp, 8
.L.end.373:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin374:
.Lcontinue374:
  push offset .LC116
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.375
  mov rax, 0
  call consume
  jmp .L.end.375
.L.call.375:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.375:
  push rax
  pop rax
  cmp rax, 0
  je .Lend374
  mov rax, rbp
  sub rax, 8
  push rax
  push 40
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.377
  mov rax, 0
  call logand
  jmp .L.end.377
.L.call.377:
  sub rsp, 8
  mov rax, 0
  call logand
  add rsp, 8
.L.end.377:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.376
  mov rax, 0
  call new_binary
  jmp .L.end.376
.L.call.376:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.376:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lbegin374
.Lend374:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global bitor
bitor:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.378
  mov rax, 0
  call bitxor
  jmp .L.end.378
.L.call.378:
  sub rsp, 8
  mov rax, 0
  call bitxor
  add rsp, 8
.L.end.378:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin379:
.Lcontinue379:
  push offset .LC117
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.380
  mov rax, 0
  call consume
  jmp .L.end.380
.L.call.380:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.380:
  push rax
  pop rax
  cmp rax, 0
  je .Lend379
  mov rax, rbp
  sub rax, 8
  push rax
  push 36
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.382
  mov rax, 0
  call bitor
  jmp .L.end.382
.L.call.382:
  sub rsp, 8
  mov rax, 0
  call bitor
  add rsp, 8
.L.end.382:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.381
  mov rax, 0
  call new_binary
  jmp .L.end.381
.L.call.381:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.381:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lbegin379
.Lend379:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global bitxor
bitxor:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.383
  mov rax, 0
  call bitand
  jmp .L.end.383
.L.call.383:
  sub rsp, 8
  mov rax, 0
  call bitand
  add rsp, 8
.L.end.383:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin384:
.Lcontinue384:
  push offset .LC118
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.385
  mov rax, 0
  call consume
  jmp .L.end.385
.L.call.385:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.385:
  push rax
  pop rax
  cmp rax, 0
  je .Lend384
  mov rax, rbp
  sub rax, 8
  push rax
  push 37
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.387
  mov rax, 0
  call bitxor
  jmp .L.end.387
.L.call.387:
  sub rsp, 8
  mov rax, 0
  call bitxor
  add rsp, 8
.L.end.387:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.386
  mov rax, 0
  call new_binary
  jmp .L.end.386
.L.call.386:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.386:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lbegin384
.Lend384:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global bitand
bitand:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.388
  mov rax, 0
  call equality
  jmp .L.end.388
.L.call.388:
  sub rsp, 8
  mov rax, 0
  call equality
  add rsp, 8
.L.end.388:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin389:
.Lcontinue389:
  push offset .LC119
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.390
  mov rax, 0
  call consume
  jmp .L.end.390
.L.call.390:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.390:
  push rax
  pop rax
  cmp rax, 0
  je .Lend389
  mov rax, rbp
  sub rax, 8
  push rax
  push 38
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.392
  mov rax, 0
  call bitand
  jmp .L.end.392
.L.call.392:
  sub rsp, 8
  mov rax, 0
  call bitand
  add rsp, 8
.L.end.392:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.391
  mov rax, 0
  call new_binary
  jmp .L.end.391
.L.call.391:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.391:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lbegin389
.Lend389:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global equality
equality:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.393
  mov rax, 0
  call relational
  jmp .L.end.393
.L.call.393:
  sub rsp, 8
  mov rax, 0
  call relational
  add rsp, 8
.L.end.393:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin394:
  push 1
  pop rax
  cmp rax, 0
  je .Lend394
  push offset .LC120
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.396
  mov rax, 0
  call consume
  jmp .L.end.396
.L.call.396:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.396:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse395
  mov rax, rbp
  sub rax, 8
  push rax
  push 6
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.398
  mov rax, 0
  call relational
  jmp .L.end.398
.L.call.398:
  sub rsp, 8
  mov rax, 0
  call relational
  add rsp, 8
.L.end.398:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.397
  mov rax, 0
  call new_binary
  jmp .L.end.397
.L.call.397:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.397:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend395
.Lelse395:
  push offset .LC121
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.400
  mov rax, 0
  call consume
  jmp .L.end.400
.L.call.400:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.400:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse399
  mov rax, rbp
  sub rax, 8
  push rax
  push 7
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.402
  mov rax, 0
  call relational
  jmp .L.end.402
.L.call.402:
  sub rsp, 8
  mov rax, 0
  call relational
  add rsp, 8
.L.end.402:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.401
  mov rax, 0
  call new_binary
  jmp .L.end.401
.L.call.401:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.401:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend399
.Lelse399:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend399
.Lend399:
  jmp .Lend395
.Lend395:
.Lcontinue394:
  jmp .Lbegin394
.Lend394:
  mov rsp, rbp
  pop rbp
  ret
.global relational
relational:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.403
  mov rax, 0
  call add
  jmp .L.end.403
.L.call.403:
  sub rsp, 8
  mov rax, 0
  call add
  add rsp, 8
.L.end.403:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin404:
  push 1
  pop rax
  cmp rax, 0
  je .Lend404
  push offset .LC122
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.406
  mov rax, 0
  call consume
  jmp .L.end.406
.L.call.406:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.406:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse405
  mov rax, rbp
  sub rax, 8
  push rax
  push 8
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.408
  mov rax, 0
  call add
  jmp .L.end.408
.L.call.408:
  sub rsp, 8
  mov rax, 0
  call add
  add rsp, 8
.L.end.408:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.407
  mov rax, 0
  call new_binary
  jmp .L.end.407
.L.call.407:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.407:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend405
.Lelse405:
  push offset .LC123
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.410
  mov rax, 0
  call consume
  jmp .L.end.410
.L.call.410:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.410:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse409
  mov rax, rbp
  sub rax, 8
  push rax
  push 9
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.412
  mov rax, 0
  call add
  jmp .L.end.412
.L.call.412:
  sub rsp, 8
  mov rax, 0
  call add
  add rsp, 8
.L.end.412:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.411
  mov rax, 0
  call new_binary
  jmp .L.end.411
.L.call.411:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.411:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend409
.Lelse409:
  push offset .LC124
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.414
  mov rax, 0
  call consume
  jmp .L.end.414
.L.call.414:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.414:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse413
  mov rax, rbp
  sub rax, 8
  push rax
  push 8
  mov rax, rsp
  and rax, 15
  jnz .L.call.416
  mov rax, 0
  call add
  jmp .L.end.416
.L.call.416:
  sub rsp, 8
  mov rax, 0
  call add
  add rsp, 8
.L.end.416:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.415
  mov rax, 0
  call new_binary
  jmp .L.end.415
.L.call.415:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.415:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend413
.Lelse413:
  push offset .LC125
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.418
  mov rax, 0
  call consume
  jmp .L.end.418
.L.call.418:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.418:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse417
  mov rax, rbp
  sub rax, 8
  push rax
  push 9
  mov rax, rsp
  and rax, 15
  jnz .L.call.420
  mov rax, 0
  call add
  jmp .L.end.420
.L.call.420:
  sub rsp, 8
  mov rax, 0
  call add
  add rsp, 8
.L.end.420:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.419
  mov rax, 0
  call new_binary
  jmp .L.end.419
.L.call.419:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.419:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend417
.Lelse417:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend417
.Lend417:
  jmp .Lend413
.Lend413:
  jmp .Lend409
.Lend409:
  jmp .Lend405
.Lend405:
.Lcontinue404:
  jmp .Lbegin404
.Lend404:
  mov rsp, rbp
  pop rbp
  ret
.global add
add:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.421
  mov rax, 0
  call mul
  jmp .L.end.421
.L.call.421:
  sub rsp, 8
  mov rax, 0
  call mul
  add rsp, 8
.L.end.421:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin422:
  push 1
  pop rax
  cmp rax, 0
  je .Lend422
  push offset .LC126
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.424
  mov rax, 0
  call consume
  jmp .L.end.424
.L.call.424:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.424:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse423
  mov rax, rbp
  sub rax, 8
  push rax
  push 1
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.427
  mov rax, 0
  call mul
  jmp .L.end.427
.L.call.427:
  sub rsp, 8
  mov rax, 0
  call mul
  add rsp, 8
.L.end.427:
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.426
  mov rax, 0
  call ptr_conversion
  jmp .L.end.426
.L.call.426:
  sub rsp, 8
  mov rax, 0
  call ptr_conversion
  add rsp, 8
.L.end.426:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.425
  mov rax, 0
  call new_binary
  jmp .L.end.425
.L.call.425:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.425:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend423
.Lelse423:
  push offset .LC127
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.429
  mov rax, 0
  call consume
  jmp .L.end.429
.L.call.429:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.429:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse428
  mov rax, rbp
  sub rax, 8
  push rax
  push 2
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.432
  mov rax, 0
  call mul
  jmp .L.end.432
.L.call.432:
  sub rsp, 8
  mov rax, 0
  call mul
  add rsp, 8
.L.end.432:
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.431
  mov rax, 0
  call ptr_conversion
  jmp .L.end.431
.L.call.431:
  sub rsp, 8
  mov rax, 0
  call ptr_conversion
  add rsp, 8
.L.end.431:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.430
  mov rax, 0
  call new_binary
  jmp .L.end.430
.L.call.430:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.430:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend428
.Lelse428:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend428
.Lend428:
  jmp .Lend423
.Lend423:
.Lcontinue422:
  jmp .Lbegin422
.Lend422:
  mov rsp, rbp
  pop rbp
  ret
.global mul
mul:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.433
  mov rax, 0
  call unary
  jmp .L.end.433
.L.call.433:
  sub rsp, 8
  mov rax, 0
  call unary
  add rsp, 8
.L.end.433:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin434:
  push 1
  pop rax
  cmp rax, 0
  je .Lend434
  push offset .LC128
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.436
  mov rax, 0
  call consume
  jmp .L.end.436
.L.call.436:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.436:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse435
  mov rax, rbp
  sub rax, 8
  push rax
  push 3
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.438
  mov rax, 0
  call unary
  jmp .L.end.438
.L.call.438:
  sub rsp, 8
  mov rax, 0
  call unary
  add rsp, 8
.L.end.438:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.437
  mov rax, 0
  call new_binary
  jmp .L.end.437
.L.call.437:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.437:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend435
.Lelse435:
  push offset .LC129
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.440
  mov rax, 0
  call consume
  jmp .L.end.440
.L.call.440:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.440:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse439
  mov rax, rbp
  sub rax, 8
  push rax
  push 4
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.442
  mov rax, 0
  call unary
  jmp .L.end.442
.L.call.442:
  sub rsp, 8
  mov rax, 0
  call unary
  add rsp, 8
.L.end.442:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.441
  mov rax, 0
  call new_binary
  jmp .L.end.441
.L.call.441:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.441:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend439
.Lelse439:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend439
.Lend439:
  jmp .Lend435
.Lend435:
.Lcontinue434:
  jmp .Lbegin434
.Lend434:
  mov rsp, rbp
  pop rbp
  ret
.global unary
unary:
  push rbp
  mov rbp, rsp
  sub rsp, 80
  push offset .LC130
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.444
  mov rax, 0
  call consume
  jmp .L.end.444
.L.call.444:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.444:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse443
  push 30
  mov rax, rsp
  and rax, 15
  jnz .L.call.446
  mov rax, 0
  call unary
  jmp .L.end.446
.L.call.446:
  sub rsp, 8
  mov rax, 0
  call unary
  add rsp, 8
.L.end.446:
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.445
  mov rax, 0
  call new_binary
  jmp .L.end.445
.L.call.445:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.445:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend443
.Lelse443:
.Lend443:
  push offset .LC131
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.448
  mov rax, 0
  call consume
  jmp .L.end.448
.L.call.448:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.448:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse447
  push 31
  mov rax, rsp
  and rax, 15
  jnz .L.call.450
  mov rax, 0
  call unary
  jmp .L.end.450
.L.call.450:
  sub rsp, 8
  mov rax, 0
  call unary
  add rsp, 8
.L.end.450:
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.449
  mov rax, 0
  call new_binary
  jmp .L.end.449
.L.call.449:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.449:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend447
.Lelse447:
.Lend447:
  push offset .LC132
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.452
  mov rax, 0
  call consume
  jmp .L.end.452
.L.call.452:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.452:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse451
  mov rax, rsp
  and rax, 15
  jnz .L.call.453
  mov rax, 0
  call unary
  jmp .L.end.453
.L.call.453:
  sub rsp, 8
  mov rax, 0
  call unary
  add rsp, 8
.L.end.453:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend451
.Lelse451:
.Lend451:
  push offset .LC133
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.455
  mov rax, 0
  call consume
  jmp .L.end.455
.L.call.455:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.455:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse454
  push 2
  push 0
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.457
  mov rax, 0
  call new_num
  jmp .L.end.457
.L.call.457:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.457:
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.458
  mov rax, 0
  call unary
  jmp .L.end.458
.L.call.458:
  sub rsp, 8
  mov rax, 0
  call unary
  add rsp, 8
.L.end.458:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.456
  mov rax, 0
  call new_binary
  jmp .L.end.456
.L.call.456:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.456:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend454
.Lelse454:
.Lend454:
  push offset .LC134
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.460
  mov rax, 0
  call consume
  jmp .L.end.460
.L.call.460:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.460:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse459
  push 25
  mov rax, rsp
  and rax, 15
  jnz .L.call.462
  mov rax, 0
  call unary
  jmp .L.end.462
.L.call.462:
  sub rsp, 8
  mov rax, 0
  call unary
  add rsp, 8
.L.end.462:
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.461
  mov rax, 0
  call new_binary
  jmp .L.end.461
.L.call.461:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.461:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend459
.Lelse459:
.Lend459:
  push offset .LC135
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.464
  mov rax, 0
  call consume
  jmp .L.end.464
.L.call.464:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.464:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse463
  push 24
  mov rax, rsp
  and rax, 15
  jnz .L.call.466
  mov rax, 0
  call unary
  jmp .L.end.466
.L.call.466:
  sub rsp, 8
  mov rax, 0
  call unary
  add rsp, 8
.L.end.466:
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.465
  mov rax, 0
  call new_binary
  jmp .L.end.465
.L.call.465:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.465:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend463
.Lelse463:
.Lend463:
  push offset .LC136
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.468
  mov rax, 0
  call consume
  jmp .L.end.468
.L.call.468:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.468:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse467
  push 34
  mov rax, rsp
  and rax, 15
  jnz .L.call.470
  mov rax, 0
  call unary
  jmp .L.end.470
.L.call.470:
  sub rsp, 8
  mov rax, 0
  call unary
  add rsp, 8
.L.end.470:
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.469
  mov rax, 0
  call new_binary
  jmp .L.end.469
.L.call.469:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.469:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend467
.Lelse467:
.Lend467:
  push offset .LC137
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.472
  mov rax, 0
  call consume
  jmp .L.end.472
.L.call.472:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.472:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse471
  push 35
  mov rax, rsp
  and rax, 15
  jnz .L.call.474
  mov rax, 0
  call unary
  jmp .L.end.474
.L.call.474:
  sub rsp, 8
  mov rax, 0
  call unary
  add rsp, 8
.L.end.474:
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.473
  mov rax, 0
  call new_binary
  jmp .L.end.473
.L.call.473:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.473:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend471
.Lelse471:
.Lend471:
  push 11
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.476
  mov rax, 0
  call consume_kind
  jmp .L.end.476
.L.call.476:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.476:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse475
  push offset .LC138
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.480
  mov rax, 0
  call check
  jmp .L.end.480
.L.call.480:
  sub rsp, 8
  mov rax, 0
  call check
  add rsp, 8
.L.end.480:
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse479
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse479
  push 1
  jmp .Lend479
.Lfalse479:
  push 0
.Lend479:
  pop rax
  cmp rax, 0
  jne .Ltrue478
  push 10
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.481
  mov rax, 0
  call check_kind
  jmp .L.end.481
.L.call.481:
  sub rsp, 8
  mov rax, 0
  call check_kind
  add rsp, 8
.L.end.481:
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue478
  push 0
  jmp .Lend478
.Ltrue478:
  push 1
.Lend478:
  pop rax
  cmp rax, 0
  je .Lelse477
  mov rax, rbp
  sub rax, 4
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push offset .LC139
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.483
  mov rax, 0
  call consume
  jmp .L.end.483
.L.call.483:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.483:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse482
  mov rax, rbp
  sub rax, 4
  push rax
  push 1
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend482
.Lelse482:
.Lend482:
  mov rax, rbp
  sub rax, 12
  push rax
  push 100
  push 1
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.484
  mov rax, 0
  call calloc
  jmp .L.end.484
.L.call.484:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.484:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.485
  mov rax, 0
  call memcpy
  jmp .L.end.485
.L.call.485:
  sub rsp, 8
  mov rax, 0
  call memcpy
  add rsp, 8
.L.end.485:
  push rax
  push 10
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.486
  mov rax, 0
  call consume_kind
  jmp .L.end.486
.L.call.486:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.486:
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC140
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.488
  mov rax, 0
  call strcmp
  jmp .L.end.488
.L.call.488:
  sub rsp, 8
  mov rax, 0
  call strcmp
  add rsp, 8
.L.end.488:
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse487
  mov rax, rbp
  sub rax, 20
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.491
  mov rax, 0
  call int_type
  jmp .L.end.491
.L.call.491:
  sub rsp, 8
  mov rax, 0
  call int_type
  add rsp, 8
.L.end.491:
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.490
  mov rax, 0
  call get_type_size
  jmp .L.end.490
.L.call.490:
  sub rsp, 8
  mov rax, 0
  call get_type_size
  add rsp, 8
.L.end.490:
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.489
  mov rax, 0
  call new_num
  jmp .L.end.489
.L.call.489:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.489:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend487
.Lelse487:
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC141
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.493
  mov rax, 0
  call strcmp
  jmp .L.end.493
.L.call.493:
  sub rsp, 8
  mov rax, 0
  call strcmp
  add rsp, 8
.L.end.493:
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse492
  mov rax, rbp
  sub rax, 20
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.496
  mov rax, 0
  call char_type
  jmp .L.end.496
.L.call.496:
  sub rsp, 8
  mov rax, 0
  call char_type
  add rsp, 8
.L.end.496:
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.495
  mov rax, 0
  call get_type_size
  jmp .L.end.495
.L.call.495:
  sub rsp, 8
  mov rax, 0
  call get_type_size
  add rsp, 8
.L.end.495:
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.494
  mov rax, 0
  call new_num
  jmp .L.end.494
.L.call.494:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.494:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend492
.Lelse492:
  push offset .LC142
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.497
  mov rax, 0
  call error1
  jmp .L.end.497
.L.call.497:
  sub rsp, 8
  mov rax, 0
  call error1
  add rsp, 8
.L.end.497:
  push rax
  jmp .Lend492
.Lend492:
  jmp .Lend487
.Lend487:
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse498
  push offset .LC143
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.499
  mov rax, 0
  call consume
  jmp .L.end.499
.L.call.499:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.499:
  push rax
  jmp .Lend498
.Lelse498:
.Lend498:
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend477
.Lelse477:
.Lend477:
  push offset .LC144
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.503
  mov rax, 0
  call check
  jmp .L.end.503
.L.call.503:
  sub rsp, 8
  mov rax, 0
  call check
  add rsp, 8
.L.end.503:
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse502
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 15
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue504
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 13
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue504
  push 0
  jmp .Lend504
.Ltrue504:
  push 1
.Lend504:
  pop rax
  cmp rax, 0
  je .Lfalse502
  push 1
  jmp .Lend502
.Lfalse502:
  push 0
.Lend502:
  pop rax
  cmp rax, 0
  jne .Ltrue501
  push 15
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.506
  mov rax, 0
  call check_kind
  jmp .L.end.506
.L.call.506:
  sub rsp, 8
  mov rax, 0
  call check_kind
  add rsp, 8
.L.end.506:
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue505
  push 13
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.507
  mov rax, 0
  call check_kind
  jmp .L.end.507
.L.call.507:
  sub rsp, 8
  mov rax, 0
  call check_kind
  add rsp, 8
.L.end.507:
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue505
  push 0
  jmp .Lend505
.Ltrue505:
  push 1
.Lend505:
  pop rax
  cmp rax, 0
  jne .Ltrue501
  push 0
  jmp .Lend501
.Ltrue501:
  push 1
.Lend501:
  pop rax
  cmp rax, 0
  je .Lelse500
  mov rax, rbp
  sub rax, 24
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push offset .LC145
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.509
  mov rax, 0
  call consume
  jmp .L.end.509
.L.call.509:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.509:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse508
  mov rax, rbp
  sub rax, 24
  push rax
  push 1
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend508
.Lelse508:
.Lend508:
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.510
  mov rax, 0
  call read_define_head
  jmp .L.end.510
.L.call.510:
  sub rsp, 8
  mov rax, 0
  call read_define_head
  add rsp, 8
.L.end.510:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 40
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.512
  mov rax, 0
  call get_type_size
  jmp .L.end.512
.L.call.512:
  sub rsp, 8
  mov rax, 0
  call get_type_size
  add rsp, 8
.L.end.512:
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.511
  mov rax, 0
  call new_num
  jmp .L.end.511
.L.call.511:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.511:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse513
  push offset .LC146
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.514
  mov rax, 0
  call consume
  jmp .L.end.514
.L.call.514:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.514:
  push rax
  jmp .Lend513
.Lelse513:
.Lend513:
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend500
.Lelse500:
.Lend500:
  push offset .LC147
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.518
  mov rax, 0
  call check
  jmp .L.end.518
.L.call.518:
  sub rsp, 8
  mov rax, 0
  call check
  add rsp, 8
.L.end.518:
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse517
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 4
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse517
  push 1
  jmp .Lend517
.Lfalse517:
  push 0
.Lend517:
  pop rax
  cmp rax, 0
  jne .Ltrue516
  push 4
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.519
  mov rax, 0
  call check_kind
  jmp .L.end.519
.L.call.519:
  sub rsp, 8
  mov rax, 0
  call check_kind
  add rsp, 8
.L.end.519:
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue516
  push 0
  jmp .Lend516
.Ltrue516:
  push 1
.Lend516:
  pop rax
  cmp rax, 0
  je .Lelse515
  mov rax, rbp
  sub rax, 44
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC148
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.521
  mov rax, 0
  call check
  jmp .L.end.521
.L.call.521:
  sub rsp, 8
  mov rax, 0
  call check
  add rsp, 8
.L.end.521:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse520
  mov rax, rbp
  sub rax, 52
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.522
  mov rax, 0
  call find_tag
  jmp .L.end.522
.L.call.522:
  sub rsp, 8
  mov rax, 0
  call find_tag
  add rsp, 8
.L.end.522:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse523
  push offset .LC149
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.524
  mov rax, 0
  call consume
  jmp .L.end.524
.L.call.524:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.524:
  push rax
  jmp .Lend523
.Lelse523:
.Lend523:
  mov rax, rbp
  sub rax, 44
  push rax
  push 1
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend520
.Lelse520:
  mov rax, rbp
  sub rax, 52
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.525
  mov rax, 0
  call find_tag
  jmp .L.end.525
.L.call.525:
  sub rsp, 8
  mov rax, 0
  call find_tag
  add rsp, 8
.L.end.525:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend520
.Lend520:
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse526
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 5
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse528
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse528
  push 1
  jmp .Lend528
.Lfalse528:
  push 0
.Lend528:
  pop rax
  cmp rax, 0
  je .Lelse527
  push offset .LC150
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.529
  mov rax, 0
  call error0
  jmp .L.end.529
.L.call.529:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.529:
  push rax
  jmp .Lend527
.Lelse527:
.Lend527:
  push 4
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.530
  mov rax, 0
  call consume_kind
  jmp .L.end.530
.L.call.530:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.530:
  push rax
  mov rax, rbp
  sub rax, 60
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC151
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.532
  mov rax, 0
  call consume
  jmp .L.end.532
.L.call.532:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.532:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse531
  mov rax, rbp
  sub rax, 60
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.535
  mov rax, 0
  call ptr_type
  jmp .L.end.535
.L.call.535:
  sub rsp, 8
  mov rax, 0
  call ptr_type
  add rsp, 8
.L.end.535:
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.534
  mov rax, 0
  call get_type_size
  jmp .L.end.534
.L.call.534:
  sub rsp, 8
  mov rax, 0
  call get_type_size
  add rsp, 8
.L.end.534:
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.533
  mov rax, 0
  call new_num
  jmp .L.end.533
.L.call.533:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.533:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend531
.Lelse531:
  mov rax, rbp
  sub rax, 60
  push rax
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.537
  mov rax, 0
  call get_type_size
  jmp .L.end.537
.L.call.537:
  sub rsp, 8
  mov rax, 0
  call get_type_size
  add rsp, 8
.L.end.537:
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.536
  mov rax, 0
  call new_num
  jmp .L.end.536
.L.call.536:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.536:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend531
.Lend531:
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse538
  push offset .LC152
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.539
  mov rax, 0
  call consume
  jmp .L.end.539
.L.call.539:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.539:
  push rax
  jmp .Lend538
.Lelse538:
.Lend538:
  mov rax, rbp
  sub rax, 60
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend526
.Lelse526:
.Lend526:
  jmp .Lend515
.Lelse515:
.Lend515:
  mov rax, rbp
  sub rax, 68
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.540
  mov rax, 0
  call unary
  jmp .L.end.540
.L.call.540:
  sub rsp, 8
  mov rax, 0
  call unary
  add rsp, 8
.L.end.540:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 68
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 5
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse541
  mov rax, rbp
  sub rax, 72
  push rax
  push 4
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend541
.Lelse541:
  mov rax, rbp
  sub rax, 80
  push rax
  mov rax, rbp
  sub rax, 68
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.542
  mov rax, 0
  call get_type
  jmp .L.end.542
.L.call.542:
  sub rsp, 8
  mov rax, 0
  call get_type
  add rsp, 8
.L.end.542:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  mov rax, rbp
  sub rax, 80
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.543
  mov rax, 0
  call get_type_size
  jmp .L.end.543
.L.call.543:
  sub rsp, 8
  mov rax, 0
  call get_type_size
  add rsp, 8
.L.end.543:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend541
.Lend541:
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.544
  mov rax, 0
  call new_num
  jmp .L.end.544
.L.call.544:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.544:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend475
.Lelse475:
.Lend475:
  mov rax, rsp
  and rax, 15
  jnz .L.call.545
  mov rax, 0
  call primary
  jmp .L.end.545
.L.call.545:
  sub rsp, 8
  mov rax, 0
  call primary
  add rsp, 8
.L.end.545:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global primary
primary:
  push rbp
  mov rbp, rsp
  sub rsp, 60
  push offset .LC153
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.547
  mov rax, 0
  call consume
  jmp .L.end.547
.L.call.547:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.547:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse546
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.548
  mov rax, 0
  call expr
  jmp .L.end.548
.L.call.548:
  sub rsp, 8
  mov rax, 0
  call expr
  add rsp, 8
.L.end.548:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC154
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.549
  mov rax, 0
  call expect
  jmp .L.end.549
.L.call.549:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.549:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend546
.Lelse546:
.Lend546:
  mov rax, rbp
  sub rax, 16
  push rax
  push 4
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.550
  mov rax, 0
  call consume_kind
  jmp .L.end.550
.L.call.550:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.550:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse551
  push offset .LC155
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.553
  mov rax, 0
  call consume
  jmp .L.end.553
.L.call.553:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.553:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse552
  mov rax, rbp
  sub rax, 24
  push rax
  push 22
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.554
  mov rax, 0
  call new_node
  jmp .L.end.554
.L.call.554:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.554:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  push 100
  push 1
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.555
  mov rax, 0
  call calloc
  jmp .L.end.555
.L.call.555:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.555:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.556
  mov rax, 0
  call memcpy
  jmp .L.end.556
.L.call.556:
  sub rsp, 8
  mov rax, 0
  call memcpy
  add rsp, 8
.L.end.556:
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  push 10
  push 128
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.557
  mov rax, 0
  call calloc
  jmp .L.end.557
.L.call.557:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.557:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 28
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin558:
  push offset .LC156
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.559
  mov rax, 0
  call consume
  jmp .L.end.559
.L.call.559:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.559:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend558
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse560
  push offset .LC157
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.561
  mov rax, 0
  call expect
  jmp .L.end.561
.L.call.561:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.561:
  push rax
  jmp .Lend560
.Lelse560:
.Lend560:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.562
  mov rax, 0
  call expr
  jmp .L.end.562
.L.call.562:
  sub rsp, 8
  mov rax, 0
  call expr
  add rsp, 8
.L.end.562:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lcontinue558:
  mov rax, rbp
  sub rax, 28
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin558
.Lend558:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend552
.Lelse552:
.Lend552:
  mov rax, rbp
  sub rax, 36
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.563
  mov rax, 0
  call find_enum_var
  jmp .L.end.563
.L.call.563:
  sub rsp, 8
  mov rax, 0
  call find_enum_var
  add rsp, 8
.L.end.563:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse564
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend564
.Lelse564:
.Lend564:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.565
  mov rax, 0
  call variable
  jmp .L.end.565
.L.call.565:
  sub rsp, 8
  mov rax, 0
  call variable
  add rsp, 8
.L.end.565:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend551
.Lelse551:
.Lend551:
  mov rax, rbp
  sub rax, 44
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.566
  mov rax, 0
  call convert_predefined_keyword_to_num
  jmp .L.end.566
.L.call.566:
  sub rsp, 8
  mov rax, 0
  call convert_predefined_keyword_to_num
  add rsp, 8
.L.end.566:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse567
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend567
.Lelse567:
.Lend567:
  mov rax, rbp
  sub rax, 16
  push rax
  push 12
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.569
  mov rax, 0
  call consume_kind
  jmp .L.end.569
.L.call.569:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.569:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  cmp rax, 0
  je .Lelse568
  mov rax, rbp
  sub rax, 52
  push rax
  push 1
  push 24
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.570
  mov rax, 0
  call calloc
  jmp .L.end.570
.L.call.570:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.570:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  push 100
  push 1
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.571
  mov rax, 0
  call calloc
  jmp .L.end.571
.L.call.571:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.571:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.572
  mov rax, 0
  call memcpy
  jmp .L.end.572
.L.call.572:
  sub rsp, 8
  mov rax, 0
  call memcpy
  add rsp, 8
.L.end.572:
  push rax
  push offset strings
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse573
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  push offset strings
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend573
.Lelse573:
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend573
.Lend573:
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  push offset strings
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 60
  push rax
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.574
  mov rax, 0
  call new_string
  jmp .L.end.574
.L.call.574:
  sub rsp, 8
  mov rax, 0
  call new_string
  add rsp, 8
.L.end.574:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset strings
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 60
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend568
.Lelse568:
.Lend568:
  mov rax, rsp
  and rax, 15
  jnz .L.call.576
  mov rax, 0
  call expect_number
  jmp .L.end.576
.L.call.576:
  sub rsp, 8
  mov rax, 0
  call expect_number
  add rsp, 8
.L.end.576:
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.575
  mov rax, 0
  call new_num
  jmp .L.end.575
.L.call.575:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.575:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global variable
variable:
  push rbp
  mov rbp, rsp
  sub rsp, 884
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  push 11
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.577
  mov rax, 0
  call new_node
  jmp .L.end.577
.L.call.577:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.577:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.578
  mov rax, 0
  call find_variable
  jmp .L.end.578
.L.call.578:
  sub rsp, 8
  mov rax, 0
  call find_variable
  add rsp, 8
.L.end.578:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse579
  mov rax, rbp
  sub rax, 824
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 40
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 48
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 56
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 64
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 72
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 80
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 88
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 96
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 104
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 112
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 120
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 128
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 136
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 144
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 152
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 160
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 168
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 176
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 184
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 192
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 200
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 208
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 216
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 224
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 232
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 240
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 248
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 256
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 264
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 272
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 280
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 288
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 296
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 304
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 312
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 320
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 328
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 336
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 344
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 352
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 360
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 368
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 376
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 384
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 392
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 400
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 408
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 416
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 424
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 432
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 440
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 448
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 456
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 464
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 472
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 480
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 488
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 496
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 504
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 512
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 520
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 528
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 536
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 544
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 552
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 560
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 568
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 576
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 584
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 592
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 600
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 608
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 616
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 624
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 632
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 640
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 648
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 656
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 664
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 672
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 680
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 688
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 696
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 704
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 712
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 720
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 728
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 736
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 744
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 752
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 760
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 768
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 776
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 784
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  push 792
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 824
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.580
  mov rax, 0
  call memcpy
  jmp .L.end.580
.L.call.580:
  sub rsp, 8
  mov rax, 0
  call memcpy
  add rsp, 8
.L.end.580:
  push rax
  push offset .LC158
  mov rax, rbp
  sub rax, 824
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.581
  mov rax, 0
  call printf
  jmp .L.end.581
.L.call.581:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.581:
  push rax
  push offset .LC159
  mov rax, rbp
  sub rax, 824
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.582
  mov rax, 0
  call error1
  jmp .L.end.582
.L.call.582:
  sub rsp, 8
  mov rax, 0
  call error1
  add rsp, 8
.L.end.582:
  push rax
  jmp .Lend579
.Lelse579:
.Lend579:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 36
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse583
  push 11
  jmp .Lend583
.Lelse583:
  push 13
  jmp .Lend583
.Lend583:
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 64
  push rax
  push 100
  push 1
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.584
  mov rax, 0
  call calloc
  jmp .L.end.584
.L.call.584:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.584:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.585
  mov rax, 0
  call memcpy
  jmp .L.end.585
.L.call.585:
  sub rsp, 8
  mov rax, 0
  call memcpy
  add rsp, 8
.L.end.585:
  push rax
  mov rax, rbp
  sub rax, 832
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 840
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin586:
.Lcontinue586:
  push 1
  pop rax
  cmp rax, 0
  je .Lend586
  mov rax, rbp
  sub rax, 844
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin587:
.Lcontinue587:
  push offset .LC160
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.588
  mov rax, 0
  call consume
  jmp .L.end.588
.L.call.588:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.588:
  push rax
  pop rax
  cmp rax, 0
  je .Lend587
  mov rax, rbp
  sub rax, 844
  push rax
  push 1
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 852
  push rax
  push 1
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.589
  mov rax, 0
  call new_node
  jmp .L.end.589
.L.call.589:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.589:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 852
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 860
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.590
  mov rax, 0
  call expr
  jmp .L.end.590
.L.call.590:
  sub rsp, 8
  mov rax, 0
  call expr
  add rsp, 8
.L.end.590:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 852
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  push 3
  mov rax, rbp
  sub rax, 860
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 840
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.593
  mov rax, 0
  call get_type_size
  jmp .L.end.593
.L.call.593:
  sub rsp, 8
  mov rax, 0
  call get_type_size
  add rsp, 8
.L.end.593:
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.592
  mov rax, 0
  call new_num
  jmp .L.end.592
.L.call.592:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.592:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.591
  mov rax, 0
  call new_binary
  jmp .L.end.591
.L.call.591:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.591:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 840
  push rax
  mov rax, rbp
  sub rax, 840
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 852
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  mov rax, rbp
  sub rax, 840
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 64
  push rax
  mov rax, rbp
  sub rax, 832
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC161
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.594
  mov rax, 0
  call expect
  jmp .L.end.594
.L.call.594:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.594:
  push rax
  jmp .Lbegin587
.Lend587:
  mov rax, rbp
  sub rax, 844
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse595
  mov rax, rbp
  sub rax, 868
  push rax
  push 25
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.596
  mov rax, 0
  call new_node
  jmp .L.end.596
.L.call.596:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.596:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 868
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 868
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  mov rax, rbp
  sub rax, 840
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lcontinue586
  jmp .Lend595
.Lelse595:
.Lend595:
  push offset .LC162
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.598
  mov rax, 0
  call consume
  jmp .L.end.598
.L.call.598:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.598:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse597
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.599
  mov rax, 0
  call struct_ref
  jmp .L.end.599
.L.call.599:
  sub rsp, 8
  mov rax, 0
  call struct_ref
  add rsp, 8
.L.end.599:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 840
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 64
  push rax
  mov rax, rbp
  sub rax, 832
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lcontinue586
  jmp .Lend597
.Lelse597:
.Lend597:
  push offset .LC163
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.601
  mov rax, 0
  call consume
  jmp .L.end.601
.L.call.601:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.601:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse600
  mov rax, rbp
  sub rax, 876
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 884
  push rax
  push 25
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.602
  mov rax, 0
  call new_node
  jmp .L.end.602
.L.call.602:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.602:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 884
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 884
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  mov rax, rbp
  sub rax, 876
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 884
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.603
  mov rax, 0
  call struct_ref
  jmp .L.end.603
.L.call.603:
  sub rsp, 8
  mov rax, 0
  call struct_ref
  add rsp, 8
.L.end.603:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 840
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 64
  push rax
  mov rax, rbp
  sub rax, 832
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lcontinue586
  jmp .Lend600
.Lelse600:
.Lend600:
  push offset .LC164
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.605
  mov rax, 0
  call consume
  jmp .L.end.605
.L.call.605:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.605:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse604
  mov rax, rbp
  sub rax, 16
  push rax
  push 32
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.606
  mov rax, 0
  call new_binary
  jmp .L.end.606
.L.call.606:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.606:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 64
  push rax
  mov rax, rbp
  sub rax, 832
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lcontinue586
  jmp .Lend604
.Lelse604:
.Lend604:
  push offset .LC165
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.608
  mov rax, 0
  call consume
  jmp .L.end.608
.L.call.608:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.608:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse607
  mov rax, rbp
  sub rax, 16
  push rax
  push 33
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.609
  mov rax, 0
  call new_binary
  jmp .L.end.609
.L.call.609:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.609:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 64
  push rax
  mov rax, rbp
  sub rax, 832
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lcontinue586
  jmp .Lend607
.Lelse607:
.Lend607:
  jmp .Lend586
  jmp .Lbegin586
.Lend586:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global define_variable
define_variable:
  push rbp
  mov rbp, rsp
  sub rsp, 72
  mov [rbp-8], rdi
  mov [rbp-16], rsi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse610
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.611
  mov rax, 0
  call print_token
  jmp .L.end.611
.L.call.611:
  sub rsp, 8
  mov rax, 0
  call print_token
  add rsp, 8
.L.end.611:
  push rax
  push offset .LC166
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.612
  mov rax, 0
  call error0
  jmp .L.end.612
.L.call.612:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.612:
  push rax
  jmp .Lend610
.Lelse610:
.Lend610:
  mov rax, rbp
  sub rax, 20
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset locals
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse613
  push offset cur_scope_depth
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  jmp .Lend613
.Lelse613:
  push 0
  jmp .Lend613
.Lend613:
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 28
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.614
  mov rax, 0
  call read_define_suffix
  jmp .L.end.614
.L.call.614:
  sub rsp, 8
  mov rax, 0
  call read_define_suffix
  add rsp, 8
.L.end.614:
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 44
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 52
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC167
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.616
  mov rax, 0
  call consume
  jmp .L.end.616
.L.call.616:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.616:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse615
  mov rax, rbp
  sub rax, 52
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.617
  mov rax, 0
  call initializer
  jmp .L.end.617
.L.call.617:
  sub rsp, 8
  mov rax, 0
  call initializer
  add rsp, 8
.L.end.617:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend615
.Lelse615:
.Lend615:
  mov rax, rbp
  sub rax, 56
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.618
  mov rax, 0
  call get_type_size
  jmp .L.end.618
.L.call.618:
  sub rsp, 8
  mov rax, 0
  call get_type_size
  add rsp, 8
.L.end.618:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 64
  push rax
  push 11
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.619
  mov rax, 0
  call new_node
  jmp .L.end.619
.L.call.619:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.619:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.620
  mov rax, 0
  call find_variable
  jmp .L.end.620
.L.call.620:
  sub rsp, 8
  mov rax, 0
  call find_variable
  add rsp, 8
.L.end.620:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 64
  push rax
  push 100
  push 1
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.621
  mov rax, 0
  call calloc
  jmp .L.end.621
.L.call.621:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.621:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.622
  mov rax, 0
  call memcpy
  jmp .L.end.622
.L.call.622:
  sub rsp, 8
  mov rax, 0
  call memcpy
  add rsp, 8
.L.end.622:
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 72
  push rax
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse623
  jmp .Lend623
.Lelse623:
.Lend623:
  mov rax, rbp
  sub rax, 72
  push rax
  push 1
  push 48
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.624
  mov rax, 0
  call calloc
  jmp .L.end.624
.L.call.624:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.624:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset locals
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse625
  push 1
  jmp .Lend625
.Lelse625:
  push 2
  jmp .Lend625
.Lend625:
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse626
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 20
  push rax
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend626
.Lelse626:
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 20
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend626
.Lend626:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 36
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse627
  push 11
  jmp .Lend627
.Lelse627:
  push 13
  jmp .Lend627
.Lend627:
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global initializer
initializer:
  push rbp
  mov rbp, rsp
  sub rsp, 36
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  push 1
  push 128
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.628
  mov rax, 0
  call calloc
  jmp .L.end.628
.L.call.628:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.628:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  push 100
  push 128
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.629
  mov rax, 0
  call calloc
  jmp .L.end.629
.L.call.629:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.629:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 20
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC168
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.631
  mov rax, 0
  call check
  jmp .L.end.631
.L.call.631:
  sub rsp, 8
  mov rax, 0
  call check
  add rsp, 8
.L.end.631:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse630
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.632
  mov rax, 0
  call initializer_helper
  jmp .L.end.632
.L.call.632:
  sub rsp, 8
  mov rax, 0
  call initializer_helper
  add rsp, 8
.L.end.632:
  push rax
  jmp .Lend630
.Lelse630:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.633
  mov rax, 0
  call expr
  jmp .L.end.633
.L.call.633:
  sub rsp, 8
  mov rax, 0
  call expr
  add rsp, 8
.L.end.633:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 26
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse634
  mov rax, rbp
  sub rax, 36
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 80
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.635
  mov rax, 0
  call strlen
  jmp .L.end.635
.L.call.635:
  sub rsp, 8
  mov rax, 0
  call strlen
  add rsp, 8
.L.end.635:
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse636
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend636
.Lelse636:
.Lend636:
  jmp .Lend634
.Lelse634:
.Lend634:
  jmp .Lend630
.Lend630:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global initializer_helper
initializer_helper:
  push rbp
  mov rbp, rsp
  sub rsp, 40
  mov [rbp-8], rdi
  mov [rbp-16], rsi
  mov [rbp-24], rdx
  push offset .LC169
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.638
  mov rax, 0
  call consume
  jmp .L.end.638
.L.call.638:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.638:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse637
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 4
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse639
.Lbegin640:
  push offset .LC170
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.641
  mov rax, 0
  call consume
  jmp .L.end.641
.L.call.641:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.641:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend640
  push offset .LC171
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.643
  mov rax, 0
  call check
  jmp .L.end.643
.L.call.643:
  sub rsp, 8
  mov rax, 0
  call check
  add rsp, 8
.L.end.643:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse642
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.644
  mov rax, 0
  call initializer_helper
  jmp .L.end.644
.L.call.644:
  sub rsp, 8
  mov rax, 0
  call initializer_helper
  add rsp, 8
.L.end.644:
  push rax
  jmp .Lend642
.Lelse642:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.645
  mov rax, 0
  call expr
  jmp .L.end.645
.L.call.645:
  sub rsp, 8
  mov rax, 0
  call expr
  add rsp, 8
.L.end.645:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend642
.Lend642:
  push offset .LC172
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.646
  mov rax, 0
  call consume
  jmp .L.end.646
.L.call.646:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.646:
  push rax
.Lcontinue640:
  jmp .Lbegin640
.Lend640:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse647
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend647
.Lelse647:
.Lend647:
.Lbegin648:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend648
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.649
  mov rax, 0
  call new_num
  jmp .L.end.649
.L.call.649:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.649:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lcontinue648:
  jmp .Lbegin648
.Lend648:
  jmp .Lend639
.Lelse639:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 5
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse650
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 36
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin651:
  push offset .LC173
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.652
  mov rax, 0
  call consume
  jmp .L.end.652
.L.call.652:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.652:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend651
  push offset .LC174
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.654
  mov rax, 0
  call check
  jmp .L.end.654
.L.call.654:
  sub rsp, 8
  mov rax, 0
  call check
  add rsp, 8
.L.end.654:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse653
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.655
  mov rax, 0
  call initializer_helper
  jmp .L.end.655
.L.call.655:
  sub rsp, 8
  mov rax, 0
  call initializer_helper
  add rsp, 8
.L.end.655:
  push rax
  jmp .Lend653
.Lelse653:
  mov rax, rbp
  sub rax, 40
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.657
  mov rax, 0
  call get_type_size
  jmp .L.end.657
.L.call.657:
  sub rsp, 8
  mov rax, 0
  call get_type_size
  add rsp, 8
.L.end.657:
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.656
  mov rax, 0
  call align_to
  jmp .L.end.656
.L.call.656:
  sub rsp, 8
  mov rax, 0
  call align_to
  add rsp, 8
.L.end.656:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse658
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 44
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.659
  mov rax, 0
  call new_node
  jmp .L.end.659
.L.call.659:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.659:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 124
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 36
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend658
.Lelse658:
.Lend658:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.660
  mov rax, 0
  call expr
  jmp .L.end.660
.L.call.660:
  sub rsp, 8
  mov rax, 0
  call expr
  add rsp, 8
.L.end.660:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 36
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.661
  mov rax, 0
  call get_type_size
  jmp .L.end.661
.L.call.661:
  sub rsp, 8
  mov rax, 0
  call get_type_size
  add rsp, 8
.L.end.661:
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend653
.Lend653:
  push offset .LC175
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.662
  mov rax, 0
  call consume
  jmp .L.end.662
.L.call.662:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.662:
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lcontinue651:
  jmp .Lbegin651
.Lend651:
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse663
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 44
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.664
  mov rax, 0
  call new_node
  jmp .L.end.664
.L.call.664:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.664:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 124
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend663
.Lelse663:
.Lend663:
  jmp .Lend650
.Lelse650:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.665
  mov rax, 0
  call print_type
  jmp .L.end.665
.L.call.665:
  sub rsp, 8
  mov rax, 0
  call print_type
  add rsp, 8
.L.end.665:
  push rax
  push offset .LC176
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.666
  mov rax, 0
  call error0
  jmp .L.end.666
.L.call.666:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.666:
  push rax
  jmp .Lend650
.Lend650:
  jmp .Lend639
.Lend639:
  jmp .Lend637
.Lelse637:
.Lend637:
  mov rsp, rbp
  pop rbp
  ret
.global struct_ref
struct_ref:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  push 27
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.667
  mov rax, 0
  call new_node
  jmp .L.end.667
.L.call.667:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.667:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 96
  push rax
  push 4
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.669
  mov rax, 0
  call consume_kind
  jmp .L.end.669
.L.call.669:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.669:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.668
  mov rax, 0
  call find_member
  jmp .L.end.668
.L.call.668:
  sub rsp, 8
  mov rax, 0
  call find_member
  add rsp, 8
.L.end.668:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 96
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global find_member
find_member:
  push rbp
  mov rbp, rsp
  sub rsp, 824
  mov [rbp-8], rdi
  mov [rbp-16], rsi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse670
  push offset .LC177
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.671
  mov rax, 0
  call error0
  jmp .L.end.671
.L.call.671:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.671:
  push rax
  jmp .Lend670
.Lelse670:
.Lend670:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse672
  push offset .LC178
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.673
  mov rax, 0
  call error0
  jmp .L.end.673
.L.call.673:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.673:
  push rax
  jmp .Lend672
.Lelse672:
.Lend672:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 5
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse674
  push offset .LC179
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.675
  mov rax, 0
  call error0
  jmp .L.end.675
.L.call.675:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.675:
  push rax
  jmp .Lend674
.Lelse674:
.Lend674:
  mov rax, rbp
  sub rax, 816
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 40
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 48
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 56
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 64
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 72
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 80
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 88
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 96
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 104
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 112
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 120
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 128
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 136
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 144
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 152
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 160
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 168
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 176
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 184
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 192
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 200
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 208
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 216
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 224
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 232
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 240
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 248
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 256
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 264
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 272
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 280
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 288
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 296
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 304
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 312
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 320
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 328
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 336
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 344
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 352
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 360
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 368
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 376
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 384
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 392
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 400
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 408
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 416
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 424
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 432
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 440
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 448
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 456
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 464
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 472
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 480
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 488
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 496
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 504
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 512
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 520
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 528
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 536
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 544
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 552
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 560
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 568
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 576
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 584
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 592
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 600
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 608
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 616
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 624
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 632
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 640
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 648
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 656
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 664
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 672
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 680
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 688
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 696
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 704
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 712
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 720
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 728
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 736
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 744
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 752
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 760
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 768
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 776
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 784
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  push 792
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 816
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.676
  mov rax, 0
  call memcpy
  jmp .L.end.676
.L.call.676:
  sub rsp, 8
  mov rax, 0
  call memcpy
  add rsp, 8
.L.end.676:
  push rax
  mov rax, rbp
  sub rax, 824
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin677:
  mov rax, rbp
  sub rax, 824
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend677
  mov rax, rbp
  sub rax, 816
  push rax
  mov rax, rbp
  sub rax, 824
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.679
  mov rax, 0
  call strcmp
  jmp .L.end.679
.L.call.679:
  sub rsp, 8
  mov rax, 0
  call strcmp
  add rsp, 8
.L.end.679:
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse678
  mov rax, rbp
  sub rax, 824
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend678
.Lelse678:
.Lend678:
.Lcontinue677:
  mov rax, rbp
  sub rax, 824
  push rax
  mov rax, rbp
  sub rax, 824
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lbegin677
.Lend677:
  push offset .LC180
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.680
  mov rax, 0
  call error0
  jmp .L.end.680
.L.call.680:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.680:
  push rax
  mov rsp, rbp
  pop rbp
  ret
.global find_variable
find_variable:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  push offset locals
  push offset cur_scope_depth
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin681:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend681
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse683
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.684
  mov rax, 0
  call memcmp
  jmp .L.end.684
.L.call.684:
  sub rsp, 8
  mov rax, 0
  call memcmp
  add rsp, 8
.L.end.684:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse683
  push 1
  jmp .Lend683
.Lfalse683:
  push 0
.Lend683:
  pop rax
  cmp rax, 0
  je .Lelse682
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  push 1
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend682
.Lelse682:
.Lend682:
.Lcontinue681:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lbegin681
.Lend681:
  mov rax, rbp
  sub rax, 24
  push rax
  push offset globals
  push 0
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin685:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend685
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse687
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.688
  mov rax, 0
  call memcmp
  jmp .L.end.688
.L.call.688:
  sub rsp, 8
  mov rax, 0
  call memcmp
  add rsp, 8
.L.end.688:
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse687
  push 1
  jmp .Lend687
.Lfalse687:
  push 0
.Lend687:
  pop rax
  cmp rax, 0
  je .Lelse686
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  push 2
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend686
.Lelse686:
.Lend686:
.Lcontinue685:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lbegin685
.Lend685:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global local_variable_init
local_variable_init:
  push rbp
  mov rbp, rsp
  sub rsp, 144
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse689
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend689
.Lelse689:
.Lend689:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 26
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse690
  mov rax, rbp
  sub rax, 16
  push rax
  push 21
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.691
  mov rax, 0
  call new_node
  jmp .L.end.691
.L.call.691:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.691:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 128
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.692
  mov rax, 0
  call calloc
  jmp .L.end.692
.L.call.692:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.692:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 20
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 80
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.693
  mov rax, 0
  call strlen
  jmp .L.end.693
.L.call.693:
  sub rsp, 8
  mov rax, 0
  call strlen
  add rsp, 8
.L.end.693:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin694:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend694
  mov rax, rbp
  sub rax, 32
  push rax
  push 1
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.695
  mov rax, 0
  call new_node
  jmp .L.end.695
.L.call.695:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.695:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse697
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse697
  push 1
  jmp .Lend697
.Lfalse697:
  push 0
.Lend697:
  pop rax
  cmp rax, 0
  je .Lelse696
  mov rax, rbp
  sub rax, 36
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.698
  mov rax, 0
  call get_type_size
  jmp .L.end.698
.L.call.698:
  sub rsp, 8
  mov rax, 0
  call get_type_size
  add rsp, 8
.L.end.698:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.699
  mov rax, 0
  call new_num
  jmp .L.end.699
.L.call.699:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.699:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend696
.Lelse696:
.Lend696:
  mov rax, rbp
  sub rax, 44
  push rax
  push 25
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.700
  mov rax, 0
  call new_node
  jmp .L.end.700
.L.call.700:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.700:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 52
  push rax
  push 10
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.701
  mov rax, 0
  call new_node
  jmp .L.end.701
.L.call.701:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.701:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse702
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 80
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.703
  mov rax, 0
  call new_num
  jmp .L.end.703
.L.call.703:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.703:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend702
.Lelse702:
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  push 0
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.704
  mov rax, 0
  call new_num
  jmp .L.end.704
.L.call.704:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.704:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend702
.Lend702:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lcontinue694:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin694
.Lend694:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend690
.Lelse690:
.Lend690:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 4
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse706
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse706
  push 1
  jmp .Lend706
.Lfalse706:
  push 0
.Lend706:
  pop rax
  cmp rax, 0
  je .Lelse705
  mov rax, rbp
  sub rax, 60
  push rax
  push 21
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.707
  mov rax, 0
  call new_node
  jmp .L.end.707
.L.call.707:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.707:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 60
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 128
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.708
  mov rax, 0
  call calloc
  jmp .L.end.708
.L.call.708:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.708:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 64
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin709:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend709
  mov rax, rbp
  sub rax, 72
  push rax
  push 1
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.710
  mov rax, 0
  call new_node
  jmp .L.end.710
.L.call.710:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.710:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse712
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse712
  push 1
  jmp .Lend712
.Lfalse712:
  push 0
.Lend712:
  pop rax
  cmp rax, 0
  je .Lelse711
  mov rax, rbp
  sub rax, 76
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.713
  mov rax, 0
  call get_type_size
  jmp .L.end.713
.L.call.713:
  sub rsp, 8
  mov rax, 0
  call get_type_size
  add rsp, 8
.L.end.713:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 76
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.714
  mov rax, 0
  call new_num
  jmp .L.end.714
.L.call.714:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.714:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend711
.Lelse711:
.Lend711:
  mov rax, rbp
  sub rax, 84
  push rax
  push 25
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.715
  mov rax, 0
  call new_node
  jmp .L.end.715
.L.call.715:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.715:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 84
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 92
  push rax
  push 10
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.716
  mov rax, 0
  call new_node
  jmp .L.end.716
.L.call.716:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.716:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 92
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 84
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 92
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 60
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 92
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lcontinue709:
  mov rax, rbp
  sub rax, 64
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin709
.Lend709:
  mov rax, rbp
  sub rax, 60
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend705
.Lelse705:
.Lend705:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 5
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse718
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse718
  push 1
  jmp .Lend718
.Lfalse718:
  push 0
.Lend718:
  pop rax
  cmp rax, 0
  je .Lelse717
  mov rax, rbp
  sub rax, 100
  push rax
  push 21
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.719
  mov rax, 0
  call new_node
  jmp .L.end.719
.L.call.719:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.719:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 100
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 128
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.720
  mov rax, 0
  call calloc
  jmp .L.end.720
.L.call.720:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.720:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 112
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin721:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 112
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend721
  mov rax, rbp
  sub rax, 120
  push rax
  push 1
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.722
  mov rax, 0
  call new_node
  jmp .L.end.722
.L.call.722:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.722:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 120
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 120
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 108
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.723
  mov rax, 0
  call new_num
  jmp .L.end.723
.L.call.723:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.723:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 128
  push rax
  push 25
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.724
  mov rax, 0
  call new_node
  jmp .L.end.724
.L.call.724:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.724:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 128
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 120
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 136
  push rax
  push 10
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.725
  mov rax, 0
  call new_node
  jmp .L.end.725
.L.call.725:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.725:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 136
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 128
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 136
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 112
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 100
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 112
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  mov rax, rbp
  sub rax, 136
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  mov rax, rbp
  sub rax, 108
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lcontinue721:
  mov rax, rbp
  sub rax, 112
  push rax
  mov rax, rbp
  sub rax, 112
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin721
.Lend721:
  mov rax, rbp
  sub rax, 100
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend717
.Lelse717:
.Lend717:
  mov rax, rbp
  sub rax, 144
  push rax
  push 10
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.726
  mov rax, 0
  call new_node
  jmp .L.end.726
.L.call.726:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.726:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 144
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 144
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 144
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global get_type
get_type:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse727
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend727
.Lelse727:
.Lend727:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse728
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend728
.Lelse728:
.Lend728:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.729
  mov rax, 0
  call get_type
  jmp .L.end.729
.L.call.729:
  sub rsp, 8
  mov rax, 0
  call get_type
  add rsp, 8
.L.end.729:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse730
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.731
  mov rax, 0
  call get_type
  jmp .L.end.731
.L.call.731:
  sub rsp, 8
  mov rax, 0
  call get_type
  add rsp, 8
.L.end.731:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend730
.Lelse730:
.Lend730:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse733
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 25
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse733
  push 1
  jmp .Lend733
.Lfalse733:
  push 0
.Lend733:
  pop rax
  cmp rax, 0
  je .Lelse732
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse734
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse735
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.736
  mov rax, 0
  call print_node
  jmp .L.end.736
.L.call.736:
  sub rsp, 8
  mov rax, 0
  call print_node
  add rsp, 8
.L.end.736:
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.737
  mov rax, 0
  call print_type
  jmp .L.end.737
.L.call.737:
  sub rsp, 8
  mov rax, 0
  call print_type
  add rsp, 8
.L.end.737:
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.738
  mov rax, 0
  call print_type
  jmp .L.end.738
.L.call.738:
  sub rsp, 8
  mov rax, 0
  call print_type
  add rsp, 8
.L.end.738:
  push rax
  push offset .LC181
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.739
  mov rax, 0
  call error0
  jmp .L.end.739
.L.call.739:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.739:
  push rax
  jmp .Lend735
.Lelse735:
.Lend735:
  jmp .Lend734
.Lelse734:
.Lend734:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend732
.Lelse732:
.Lend732:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global get_type_size
get_type_size:
  push rbp
  mov rbp, rsp
  sub rsp, 12
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse740
  push offset .LC182
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.741
  mov rax, 0
  call error0
  jmp .L.end.741
.L.call.741:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.741:
  push rax
  jmp .Lend740
.Lelse740:
.Lend740:
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 5
  je .Lcase743
  cmp rax, 4
  je .Lcase744
  cmp rax, 2
  je .Lcase745
  cmp rax, 3
  je .Lcase746
  cmp rax, 1
  je .Lcase747
  jmp .Lcase748
  jmp .Lend742
.Lcase747:
  push 4
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase746:
  push 8
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase745:
  push 1
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase744:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse748
  push offset .LC183
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.749
  mov rax, 0
  call error0
  jmp .L.end.749
.L.call.749:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.749:
  push rax
  jmp .Lend748
.Lelse748:
.Lend748:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.750
  mov rax, 0
  call get_type_size
  jmp .L.end.750
.L.call.750:
  sub rsp, 8
  mov rax, 0
  call get_type_size
  add rsp, 8
.L.end.750:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase743:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase748:
  push offset .LC184
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.751
  mov rax, 0
  call error0
  jmp .L.end.751
.L.call.751:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.751:
  push rax
.Lend742:
  mov rsp, rbp
  pop rbp
  ret
.global read_define
read_define:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.752
  mov rax, 0
  call read_define_head
  jmp .L.end.752
.L.call.752:
  sub rsp, 8
  mov rax, 0
  call read_define_head
  add rsp, 8
.L.end.752:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.753
  mov rax, 0
  call read_define_suffix
  jmp .L.end.753
.L.call.753:
  sub rsp, 8
  mov rax, 0
  call read_define_suffix
  add rsp, 8
.L.end.753:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global read_define_head
read_define_head:
  push rbp
  mov rbp, rsp
  sub rsp, 80
  mov rax, rbp
  sub rax, 8
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  push 4
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.754
  mov rax, 0
  call consume_kind
  jmp .L.end.754
.L.call.754:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.754:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse755
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.756
  mov rax, 0
  call find_tag
  jmp .L.end.756
.L.call.756:
  sub rsp, 8
  mov rax, 0
  call find_tag
  add rsp, 8
.L.end.756:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse757
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend757
.Lelse757:
  push offset token
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend757
.Lend757:
  jmp .Lend755
.Lelse755:
.Lend755:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse758
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.759
  mov rax, 0
  call define_struct
  jmp .L.end.759
.L.call.759:
  sub rsp, 8
  mov rax, 0
  call define_struct
  add rsp, 8
.L.end.759:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend758
.Lelse758:
.Lend758:
  mov rax, rbp
  sub rax, 36
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse760
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.761
  mov rax, 0
  call define_enum
  jmp .L.end.761
.L.call.761:
  sub rsp, 8
  mov rax, 0
  call define_enum
  add rsp, 8
.L.end.761:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse762
  mov rax, rbp
  sub rax, 36
  push rax
  push 1
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend762
.Lelse762:
.Lend762:
  jmp .Lend760
.Lelse760:
.Lend760:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse763
  mov rax, rbp
  sub rax, 44
  push rax
  push 10
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.764
  mov rax, 0
  call consume_kind
  jmp .L.end.764
.L.call.764:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.764:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse765
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend765
.Lelse765:
.Lend765:
  mov rax, rbp
  sub rax, 8
  push rax
  push 1
  push 40
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.766
  mov rax, 0
  call calloc
  jmp .L.end.766
.L.call.766:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.766:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 48
  push rax
  push offset .LC185
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.767
  mov rax, 0
  call memcmp
  jmp .L.end.767
.L.call.767:
  sub rsp, 8
  mov rax, 0
  call memcmp
  add rsp, 8
.L.end.767:
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  push 1
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse768
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  push 2
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend768
.Lelse768:
.Lend768:
  jmp .Lend763
.Lelse763:
.Lend763:
.Lbegin769:
.Lcontinue769:
  push offset .LC186
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.770
  mov rax, 0
  call consume
  jmp .L.end.770
.L.call.770:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.770:
  push rax
  pop rax
  cmp rax, 0
  je .Lend769
  mov rax, rbp
  sub rax, 56
  push rax
  push 1
  push 40
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.771
  mov rax, 0
  call calloc
  jmp .L.end.771
.L.call.771:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.771:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  push 3
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lbegin769
.Lend769:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse772
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend772
.Lelse772:
.Lend772:
  mov rax, rbp
  sub rax, 64
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  push 4
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.773
  mov rax, 0
  call consume_kind
  jmp .L.end.773
.L.call.773:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.773:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse775
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 5
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse776
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse776
  push 1
  jmp .Lend776
.Lfalse776:
  push 0
.Lend776:
  pop rax
  cmp rax, 0
  je .Lfalse775
  push 1
  jmp .Lend775
.Lfalse775:
  push 0
.Lend775:
  pop rax
  cmp rax, 0
  je .Lelse774
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.777
  mov rax, 0
  call print_type
  jmp .L.end.777
.L.call.777:
  sub rsp, 8
  mov rax, 0
  call print_type
  add rsp, 8
.L.end.777:
  push rax
  push offset .LC187
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.778
  mov rax, 0
  call error0
  jmp .L.end.778
.L.call.778:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.778:
  push rax
  jmp .Lend774
.Lelse774:
.Lend774:
  mov rax, rbp
  sub rax, 80
  push rax
  push 1
  push 16
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.779
  mov rax, 0
  call calloc
  jmp .L.end.779
.L.call.779:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.779:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 80
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 80
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 80
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global read_define_suffix
read_define_suffix:
  push rbp
  mov rbp, rsp
  sub rsp, 40
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse780
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.781
  mov rax, 0
  call print_token
  jmp .L.end.781
.L.call.781:
  sub rsp, 8
  mov rax, 0
  call print_token
  add rsp, 8
.L.end.781:
  push rax
  push offset .LC188
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.782
  mov rax, 0
  call error0
  jmp .L.end.782
.L.call.782:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.782:
  push rax
  jmp .Lend780
.Lelse780:
.Lend780:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin783:
.Lcontinue783:
  push offset .LC189
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.784
  mov rax, 0
  call consume
  jmp .L.end.784
.L.call.784:
  sub rsp, 8
  mov rax, 0
  call consume
  add rsp, 8
.L.end.784:
  push rax
  pop rax
  cmp rax, 0
  je .Lend783
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  push 1
  push 40
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.785
  mov rax, 0
  call calloc
  jmp .L.end.785
.L.call.785:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.785:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  push 4
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 40
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 40
  push rax
  push 2
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.787
  mov rax, 0
  call consume_kind
  jmp .L.end.787
.L.call.787:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.787:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  pop rax
  cmp rax, 0
  je .Lelse786
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend786
.Lelse786:
.Lend786:
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC190
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.788
  mov rax, 0
  call expect
  jmp .L.end.788
.L.call.788:
  sub rsp, 8
  mov rax, 0
  call expect
  add rsp, 8
.L.end.788:
  push rax
  jmp .Lbegin783
.Lend783:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rsp, rbp
  pop rbp
  ret
.global align_to
align_to:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov [rbp-4], edi
  mov [rbp-8], esi
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rax
  not rax
  push rax
  pop rdi
  pop rax
  and rax, rdi
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global push_tag
push_tag:
  push rbp
  mov rbp, rsp
  sub rsp, 36
  mov [rbp-8], rdi
  mov [rbp-16], rsi
  mov [rbp-20], edx
  mov rax, rbp
  sub rax, 28
  push rax
  push 100
  push 1
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.789
  mov rax, 0
  call calloc
  jmp .L.end.789
.L.call.789:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.789:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.790
  mov rax, 0
  call memcpy
  jmp .L.end.790
.L.call.790:
  sub rsp, 8
  mov rax, 0
  call memcpy
  add rsp, 8
.L.end.790:
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.791
  mov rax, 0
  call find_or_register_tag
  jmp .L.end.791
.L.call.791:
  sub rsp, 8
  mov rax, 0
  call find_or_register_tag
  add rsp, 8
.L.end.791:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse793
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse793
  push 1
  jmp .Lend793
.Lfalse793:
  push 0
.Lend793:
  pop rax
  cmp rax, 0
  je .Lelse792
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 36
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  jmp .Lend792
.Lelse792:
.Lend792:
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rsp, rbp
  pop rbp
  ret
.global find_tag
find_tag:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  push 100
  push 1
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.794
  mov rax, 0
  call calloc
  jmp .L.end.794
.L.call.794:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.794:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.795
  mov rax, 0
  call memcpy
  jmp .L.end.795
.L.call.795:
  sub rsp, 8
  mov rax, 0
  call memcpy
  add rsp, 8
.L.end.795:
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  push offset tags
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin796:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend796
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.798
  mov rax, 0
  call strcmp
  jmp .L.end.798
.L.call.798:
  sub rsp, 8
  mov rax, 0
  call strcmp
  add rsp, 8
.L.end.798:
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse797
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend797
.Lelse797:
.Lend797:
.Lcontinue796:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lbegin796
.Lend796:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global find_or_register_tag
find_or_register_tag:
  push rbp
  mov rbp, rsp
  sub rsp, 32
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  push 100
  push 1
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.799
  mov rax, 0
  call calloc
  jmp .L.end.799
.L.call.799:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.799:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.800
  mov rax, 0
  call memcpy
  jmp .L.end.800
.L.call.800:
  sub rsp, 8
  mov rax, 0
  call memcpy
  add rsp, 8
.L.end.800:
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  push offset tags
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin801:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend801
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.803
  mov rax, 0
  call strcmp
  jmp .L.end.803
.L.call.803:
  sub rsp, 8
  mov rax, 0
  call strcmp
  add rsp, 8
.L.end.803:
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse802
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend802
.Lelse802:
.Lend802:
.Lcontinue801:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lbegin801
.Lend801:
  mov rax, rbp
  sub rax, 32
  push rax
  push 1
  push 24
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.804
  mov rax, 0
  call calloc
  jmp .L.end.804
.L.call.804:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.804:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  push 1
  push 40
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.805
  mov rax, 0
  call calloc
  jmp .L.end.805
.L.call.805:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.805:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 36
  push rax
  push 1
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push offset tags
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse806
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  push offset tags
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend806
.Lelse806:
.Lend806:
  push offset tags
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global find_enum_var
find_enum_var:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  push 100
  push 1
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.807
  mov rax, 0
  call calloc
  jmp .L.end.807
.L.call.807:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.807:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.808
  mov rax, 0
  call memcpy
  jmp .L.end.808
.L.call.808:
  sub rsp, 8
  mov rax, 0
  call memcpy
  add rsp, 8
.L.end.808:
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  push offset enum_vars
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin809:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend809
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.811
  mov rax, 0
  call strcmp
  jmp .L.end.811
.L.call.811:
  sub rsp, 8
  mov rax, 0
  call strcmp
  add rsp, 8
.L.end.811:
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse810
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.812
  mov rax, 0
  call new_num
  jmp .L.end.812
.L.call.812:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.812:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend810
.Lelse810:
.Lend810:
.Lcontinue809:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lbegin809
.Lend809:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global convert_predefined_keyword_to_num
convert_predefined_keyword_to_num:
  push rbp
  mov rbp, rsp
  push 21
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.814
  mov rax, 0
  call consume_kind
  jmp .L.end.814
.L.call.814:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.814:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse813
  push 1
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.815
  mov rax, 0
  call new_num
  jmp .L.end.815
.L.call.815:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.815:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend813
.Lelse813:
.Lend813:
  push 22
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.817
  mov rax, 0
  call consume_kind
  jmp .L.end.817
.L.call.817:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.817:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse816
  push 0
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.818
  mov rax, 0
  call new_num
  jmp .L.end.818
.L.call.818:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.818:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend816
.Lelse816:
.Lend816:
  push 23
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.820
  mov rax, 0
  call consume_kind
  jmp .L.end.820
.L.call.820:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.820:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse819
  push 0
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.821
  mov rax, 0
  call new_num
  jmp .L.end.821
.L.call.821:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.821:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend819
.Lelse819:
.Lend819:
  push 26
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.823
  mov rax, 0
  call consume_kind
  jmp .L.end.823
.L.call.823:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.823:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse822
  push 2
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.824
  mov rax, 0
  call new_num
  jmp .L.end.824
.L.call.824:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.824:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend822
.Lelse822:
.Lend822:
  push 25
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.826
  mov rax, 0
  call consume_kind
  jmp .L.end.826
.L.call.826:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.826:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse825
  push 1
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.827
  mov rax, 0
  call new_num
  jmp .L.end.827
.L.call.827:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.827:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend825
.Lelse825:
.Lend825:
  push 24
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.829
  mov rax, 0
  call consume_kind
  jmp .L.end.829
.L.call.829:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.829:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse828
  push 0
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.830
  mov rax, 0
  call new_num
  jmp .L.end.830
.L.call.830:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.830:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend828
.Lelse828:
.Lend828:
  push 27
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.832
  mov rax, 0
  call consume_kind
  jmp .L.end.832
.L.call.832:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.832:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse831
  push 0
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.833
  mov rax, 0
  call new_num
  jmp .L.end.833
.L.call.833:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.833:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend831
.Lelse831:
.Lend831:
  push 28
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.835
  mov rax, 0
  call consume_kind
  jmp .L.end.835
.L.call.835:
  sub rsp, 8
  mov rax, 0
  call consume_kind
  add rsp, 8
.L.end.835:
  push rax
  pop rax
  cmp rax, 0
  je .Lelse834
  push 0
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.836
  mov rax, 0
  call new_num
  jmp .L.end.836
.L.call.836:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.836:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend834
.Lelse834:
.Lend834:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global ptr_conversion
ptr_conversion:
  push rbp
  mov rbp, rsp
  sub rsp, 20
  mov [rbp-8], rdi
  mov [rbp-16], rsi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse838
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse838
  push 1
  jmp .Lend838
.Lfalse838:
  push 0
.Lend838:
  pop rax
  cmp rax, 0
  je .Lelse837
  mov rax, rbp
  sub rax, 20
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.839
  mov rax, 0
  call get_type_size
  jmp .L.end.839
.L.call.839:
  sub rsp, 8
  mov rax, 0
  call get_type_size
  add rsp, 8
.L.end.839:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 3
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.841
  mov rax, 0
  call new_num
  jmp .L.end.841
.L.call.841:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.841:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.840
  mov rax, 0
  call new_binary
  jmp .L.end.840
.L.call.840:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.840:
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend837
.Lelse837:
.Lend837:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global gen
gen:
  push rbp
  mov rbp, rsp
  sub rsp, 88
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse842
  push offset .LC215
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.843
  mov rax, 0
  call error0
  jmp .L.end.843
.L.call.843:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.843:
  push rax
  jmp .Lend842
.Lelse842:
.Lend842:
  mov rax, rbp
  sub rax, 12
  push rax
  push offset if_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 20
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 25
  je .Lcase845
  cmp rax, 24
  je .Lcase846
  cmp rax, 23
  je .Lcase847
  cmp rax, 22
  je .Lcase848
  cmp rax, 43
  je .Lcase849
  cmp rax, 42
  je .Lcase850
  cmp rax, 21
  je .Lcase851
  cmp rax, 18
  je .Lcase852
  cmp rax, 17
  je .Lcase853
  cmp rax, 41
  je .Lcase854
  cmp rax, 15
  je .Lcase855
  cmp rax, 29
  je .Lcase856
  cmp rax, 28
  je .Lcase857
  cmp rax, 14
  je .Lcase858
  cmp rax, 10
  je .Lcase859
  cmp rax, 11
  je .Lcase860
  cmp rax, 13
  je .Lcase861
  cmp rax, 27
  je .Lcase862
  cmp rax, 12
  je .Lcase863
  cmp rax, 5
  je .Lcase864
  cmp rax, 26
  je .Lcase865
  cmp rax, 33
  je .Lcase866
  cmp rax, 31
  je .Lcase867
  cmp rax, 32
  je .Lcase868
  cmp rax, 30
  je .Lcase869
  cmp rax, 34
  je .Lcase870
  cmp rax, 35
  je .Lcase871
  cmp rax, 39
  je .Lcase872
  cmp rax, 40
  je .Lcase873
  jmp .Lend844
.Lcase873:
  push offset if_sequence
  push offset if_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.873
  mov rax, 0
  call gen
  jmp .L.end.873
.L.call.873:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.873:
  push rax
  push offset .LC216
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.874
  mov rax, 0
  call printf
  jmp .L.end.874
.L.call.874:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.874:
  push rax
  push offset .LC217
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.875
  mov rax, 0
  call printf
  jmp .L.end.875
.L.call.875:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.875:
  push rax
  push offset .LC218
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.876
  mov rax, 0
  call printf
  jmp .L.end.876
.L.call.876:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.876:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.877
  mov rax, 0
  call gen
  jmp .L.end.877
.L.call.877:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.877:
  push rax
  push offset .LC219
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.878
  mov rax, 0
  call printf
  jmp .L.end.878
.L.call.878:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.878:
  push rax
  push offset .LC220
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.879
  mov rax, 0
  call printf
  jmp .L.end.879
.L.call.879:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.879:
  push rax
  push offset .LC221
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.880
  mov rax, 0
  call printf
  jmp .L.end.880
.L.call.880:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.880:
  push rax
  push offset .LC222
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.881
  mov rax, 0
  call printf
  jmp .L.end.881
.L.call.881:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.881:
  push rax
  push offset .LC223
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.882
  mov rax, 0
  call printf
  jmp .L.end.882
.L.call.882:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.882:
  push rax
  push offset .LC224
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.883
  mov rax, 0
  call printf
  jmp .L.end.883
.L.call.883:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.883:
  push rax
  push offset .LC225
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.884
  mov rax, 0
  call printf
  jmp .L.end.884
.L.call.884:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.884:
  push rax
  push offset .LC226
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.885
  mov rax, 0
  call printf
  jmp .L.end.885
.L.call.885:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.885:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase872:
  push offset if_sequence
  push offset if_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.886
  mov rax, 0
  call gen
  jmp .L.end.886
.L.call.886:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.886:
  push rax
  push offset .LC227
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.887
  mov rax, 0
  call printf
  jmp .L.end.887
.L.call.887:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.887:
  push rax
  push offset .LC228
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.888
  mov rax, 0
  call printf
  jmp .L.end.888
.L.call.888:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.888:
  push rax
  push offset .LC229
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.889
  mov rax, 0
  call printf
  jmp .L.end.889
.L.call.889:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.889:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.890
  mov rax, 0
  call gen
  jmp .L.end.890
.L.call.890:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.890:
  push rax
  push offset .LC230
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.891
  mov rax, 0
  call printf
  jmp .L.end.891
.L.call.891:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.891:
  push rax
  push offset .LC231
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.892
  mov rax, 0
  call printf
  jmp .L.end.892
.L.call.892:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.892:
  push rax
  push offset .LC232
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.893
  mov rax, 0
  call printf
  jmp .L.end.893
.L.call.893:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.893:
  push rax
  push offset .LC233
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.894
  mov rax, 0
  call printf
  jmp .L.end.894
.L.call.894:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.894:
  push rax
  push offset .LC234
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.895
  mov rax, 0
  call printf
  jmp .L.end.895
.L.call.895:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.895:
  push rax
  push offset .LC235
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.896
  mov rax, 0
  call printf
  jmp .L.end.896
.L.call.896:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.896:
  push rax
  push offset .LC236
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.897
  mov rax, 0
  call printf
  jmp .L.end.897
.L.call.897:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.897:
  push rax
  push offset .LC237
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.898
  mov rax, 0
  call printf
  jmp .L.end.898
.L.call.898:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.898:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase871:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.899
  mov rax, 0
  call gen
  jmp .L.end.899
.L.call.899:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.899:
  push rax
  push offset .LC238
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.900
  mov rax, 0
  call printf
  jmp .L.end.900
.L.call.900:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.900:
  push rax
  push offset .LC239
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.901
  mov rax, 0
  call printf
  jmp .L.end.901
.L.call.901:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.901:
  push rax
  push offset .LC240
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.902
  mov rax, 0
  call printf
  jmp .L.end.902
.L.call.902:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.902:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase870:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.903
  mov rax, 0
  call gen
  jmp .L.end.903
.L.call.903:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.903:
  push rax
  push offset .LC241
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.904
  mov rax, 0
  call printf
  jmp .L.end.904
.L.call.904:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.904:
  push rax
  push offset .LC242
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.905
  mov rax, 0
  call printf
  jmp .L.end.905
.L.call.905:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.905:
  push rax
  push offset .LC243
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.906
  mov rax, 0
  call printf
  jmp .L.end.906
.L.call.906:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.906:
  push rax
  push offset .LC244
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.907
  mov rax, 0
  call printf
  jmp .L.end.907
.L.call.907:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.907:
  push rax
  push offset .LC245
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.908
  mov rax, 0
  call printf
  jmp .L.end.908
.L.call.908:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.908:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase869:
  mov rax, rbp
  sub rax, 40
  push rax
  push 10
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.909
  mov rax, 0
  call new_node
  jmp .L.end.909
.L.call.909:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.909:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  push 1
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.911
  mov rax, 0
  call new_num
  jmp .L.end.911
.L.call.911:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.911:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.910
  mov rax, 0
  call new_binary
  jmp .L.end.910
.L.call.910:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.910:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.912
  mov rax, 0
  call gen
  jmp .L.end.912
.L.call.912:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.912:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase868:
  push 30
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.914
  mov rax, 0
  call new_binary
  jmp .L.end.914
.L.call.914:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.914:
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.913
  mov rax, 0
  call gen
  jmp .L.end.913
.L.call.913:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.913:
  push rax
  push offset .LC246
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.915
  mov rax, 0
  call printf
  jmp .L.end.915
.L.call.915:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.915:
  push rax
  push offset .LC247
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.916
  mov rax, 0
  call printf
  jmp .L.end.916
.L.call.916:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.916:
  push rax
  push offset .LC248
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.917
  mov rax, 0
  call printf
  jmp .L.end.917
.L.call.917:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.917:
  push rax
  push offset .LC249
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.918
  mov rax, 0
  call printf
  jmp .L.end.918
.L.call.918:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.918:
  push rax
  push offset .LC250
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.919
  mov rax, 0
  call printf
  jmp .L.end.919
.L.call.919:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.919:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase867:
  mov rax, rbp
  sub rax, 40
  push rax
  push 10
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.920
  mov rax, 0
  call new_node
  jmp .L.end.920
.L.call.920:
  sub rsp, 8
  mov rax, 0
  call new_node
  add rsp, 8
.L.end.920:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  push 2
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.922
  mov rax, 0
  call new_num
  jmp .L.end.922
.L.call.922:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.922:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.921
  mov rax, 0
  call new_binary
  jmp .L.end.921
.L.call.921:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.921:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.923
  mov rax, 0
  call gen
  jmp .L.end.923
.L.call.923:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.923:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase866:
  push 31
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.925
  mov rax, 0
  call new_binary
  jmp .L.end.925
.L.call.925:
  sub rsp, 8
  mov rax, 0
  call new_binary
  add rsp, 8
.L.end.925:
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.924
  mov rax, 0
  call gen
  jmp .L.end.924
.L.call.924:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.924:
  push rax
  push offset .LC251
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.926
  mov rax, 0
  call printf
  jmp .L.end.926
.L.call.926:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.926:
  push rax
  push offset .LC252
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.927
  mov rax, 0
  call printf
  jmp .L.end.927
.L.call.927:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.927:
  push rax
  push offset .LC253
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.928
  mov rax, 0
  call printf
  jmp .L.end.928
.L.call.928:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.928:
  push rax
  push offset .LC254
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.929
  mov rax, 0
  call printf
  jmp .L.end.929
.L.call.929:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.929:
  push rax
  push offset .LC255
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.930
  mov rax, 0
  call printf
  jmp .L.end.930
.L.call.930:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.930:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase865:
  push offset .LC256
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 80
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.931
  mov rax, 0
  call printf
  jmp .L.end.931
.L.call.931:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.931:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase864:
  push offset .LC257
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.932
  mov rax, 0
  call printf
  jmp .L.end.932
.L.call.932:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.932:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase863:
  push offset .LC258
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.933
  mov rax, 0
  call printf
  jmp .L.end.933
.L.call.933:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.933:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse934
  push offset .LC259
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 72
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.935
  mov rax, 0
  call printf
  jmp .L.end.935
.L.call.935:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.935:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend934
.Lelse934:
.Lend934:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 4
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse937
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse937
  push 1
  jmp .Lend937
.Lfalse937:
  push 0
.Lend937:
  pop rax
  cmp rax, 0
  je .Lelse936
  mov rax, rbp
  sub rax, 44
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin938:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend938
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 44
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse939
  push offset .LC260
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 124
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.940
  mov rax, 0
  call printf
  jmp .L.end.940
.L.call.940:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.940:
  push rax
  jmp .Lcontinue938
  jmp .Lend939
.Lelse939:
.Lend939:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 3
  je .Lcase942
  cmp rax, 2
  je .Lcase943
  cmp rax, 1
  je .Lcase944
  jmp .Lcase945
  jmp .Lend941
.Lcase944:
  push offset .LC261
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.945
  mov rax, 0
  call printf
  jmp .L.end.945
.L.call.945:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.945:
  push rax
  jmp .Lend941
.Lcase943:
  push offset .LC262
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.946
  mov rax, 0
  call printf
  jmp .L.end.946
.L.call.946:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.946:
  push rax
  jmp .Lend941
.Lcase942:
  push offset .LC263
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 80
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.947
  mov rax, 0
  call printf
  jmp .L.end.947
.L.call.947:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.947:
  push rax
  jmp .Lend941
.Lcase945:
  jmp .Lend941
.Lend941:
.Lcontinue938:
  mov rax, rbp
  sub rax, 44
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin938
.Lend938:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend936
.Lelse936:
.Lend936:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 26
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse948
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 56
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 4
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse949
  push offset .LC264
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 80
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.950
  mov rax, 0
  call printf
  jmp .L.end.950
.L.call.950:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.950:
  push rax
  jmp .Lend949
.Lelse949:
  push offset .LC265
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 80
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.951
  mov rax, 0
  call printf
  jmp .L.end.951
.L.call.951:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.951:
  push rax
  jmp .Lend949
.Lend949:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend948
.Lelse948:
.Lend948:
  push offset .LC266
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.952
  mov rax, 0
  call printf
  jmp .L.end.952
.L.call.952:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.952:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase862:
.Lcase861:
.Lcase860:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.953
  mov rax, 0
  call gen_val
  jmp .L.end.953
.L.call.953:
  sub rsp, 8
  mov rax, 0
  call gen_val
  add rsp, 8
.L.end.953:
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.954
  mov rax, 0
  call get_type
  jmp .L.end.954
.L.call.954:
  sub rsp, 8
  mov rax, 0
  call get_type
  add rsp, 8
.L.end.954:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse956
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 4
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse956
  push 1
  jmp .Lend956
.Lfalse956:
  push 0
.Lend956:
  pop rax
  cmp rax, 0
  je .Lelse955
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend955
.Lelse955:
.Lend955:
  push offset .LC267
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.957
  mov rax, 0
  call printf
  jmp .L.end.957
.L.call.957:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.957:
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse958
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 1
  je .Lcase960
  cmp rax, 2
  je .Lcase961
  jmp .Lcase962
  jmp .Lend959
.Lcase961:
  push offset .LC268
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.962
  mov rax, 0
  call printf
  jmp .L.end.962
.L.call.962:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.962:
  push rax
  jmp .Lend959
.Lcase960:
  push offset .LC269
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.963
  mov rax, 0
  call printf
  jmp .L.end.963
.L.call.963:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.963:
  push rax
  jmp .Lend959
.Lcase962:
  push offset .LC270
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.964
  mov rax, 0
  call printf
  jmp .L.end.964
.L.call.964:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.964:
  push rax
  jmp .Lend959
.Lend959:
  jmp .Lend958
.Lelse958:
  push offset .LC271
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.965
  mov rax, 0
  call printf
  jmp .L.end.965
.L.call.965:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.965:
  push rax
  jmp .Lend958
.Lend958:
  push offset .LC272
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.966
  mov rax, 0
  call printf
  jmp .L.end.966
.L.call.966:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.966:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase859:
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.967
  mov rax, 0
  call get_type
  jmp .L.end.967
.L.call.967:
  sub rsp, 8
  mov rax, 0
  call get_type
  add rsp, 8
.L.end.967:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.968
  mov rax, 0
  call gen_val
  jmp .L.end.968
.L.call.968:
  sub rsp, 8
  mov rax, 0
  call gen_val
  add rsp, 8
.L.end.968:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.969
  mov rax, 0
  call gen
  jmp .L.end.969
.L.call.969:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.969:
  push rax
  push offset .LC273
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.970
  mov rax, 0
  call printf
  jmp .L.end.970
.L.call.970:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.970:
  push rax
  push offset .LC274
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.971
  mov rax, 0
  call printf
  jmp .L.end.971
.L.call.971:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.971:
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse973
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 2
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse973
  push 1
  jmp .Lend973
.Lfalse973:
  push 0
.Lend973:
  pop rax
  cmp rax, 0
  je .Lelse972
  push offset .LC275
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.974
  mov rax, 0
  call printf
  jmp .L.end.974
.L.call.974:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.974:
  push rax
  jmp .Lend972
.Lelse972:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse976
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse976
  push 1
  jmp .Lend976
.Lfalse976:
  push 0
.Lend976:
  pop rax
  cmp rax, 0
  je .Lelse975
  push offset .LC276
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.977
  mov rax, 0
  call printf
  jmp .L.end.977
.L.call.977:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.977:
  push rax
  jmp .Lend975
.Lelse975:
  push offset .LC277
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.978
  mov rax, 0
  call printf
  jmp .L.end.978
.L.call.978:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.978:
  push rax
  jmp .Lend975
.Lend975:
  jmp .Lend972
.Lend972:
  push offset .LC278
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.979
  mov rax, 0
  call printf
  jmp .L.end.979
.L.call.979:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.979:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase858:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse980
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.981
  mov rax, 0
  call gen
  jmp .L.end.981
.L.call.981:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.981:
  push rax
  jmp .Lend980
.Lelse980:
  push 0
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.983
  mov rax, 0
  call new_num
  jmp .L.end.983
.L.call.983:
  sub rsp, 8
  mov rax, 0
  call new_num
  add rsp, 8
.L.end.983:
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.982
  mov rax, 0
  call gen
  jmp .L.end.982
.L.call.982:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.982:
  push rax
  jmp .Lend980
.Lend980:
  push offset .LC279
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.984
  mov rax, 0
  call printf
  jmp .L.end.984
.L.call.984:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.984:
  push rax
  push offset .LC280
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.985
  mov rax, 0
  call printf
  jmp .L.end.985
.L.call.985:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.985:
  push rax
  push offset .LC281
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.986
  mov rax, 0
  call printf
  jmp .L.end.986
.L.call.986:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.986:
  push rax
  push offset .LC282
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.987
  mov rax, 0
  call printf
  jmp .L.end.987
.L.call.987:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.987:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase857:
  push offset break_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse988
  push offset .LC283
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.989
  mov rax, 0
  call error0
  jmp .L.end.989
.L.call.989:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.989:
  push rax
  jmp .Lend988
.Lelse988:
.Lend988:
  push offset .LC284
  push offset break_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.990
  mov rax, 0
  call printf
  jmp .L.end.990
.L.call.990:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.990:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase856:
  push offset continue_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse991
  push offset .LC285
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.992
  mov rax, 0
  call error0
  jmp .L.end.992
.L.call.992:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.992:
  push rax
  jmp .Lend991
.Lelse991:
.Lend991:
  push offset .LC286
  push offset continue_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.993
  mov rax, 0
  call printf
  jmp .L.end.993
.L.call.993:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.993:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase855:
  push offset if_sequence
  push offset if_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.994
  mov rax, 0
  call gen
  jmp .L.end.994
.L.call.994:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.994:
  push rax
  push offset .LC287
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.995
  mov rax, 0
  call printf
  jmp .L.end.995
.L.call.995:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.995:
  push rax
  push offset .LC288
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.996
  mov rax, 0
  call printf
  jmp .L.end.996
.L.call.996:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.996:
  push rax
  push offset .LC289
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.997
  mov rax, 0
  call printf
  jmp .L.end.997
.L.call.997:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.997:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 16
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse998
  mov rax, rbp
  sub rax, 52
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.999
  mov rax, 0
  call gen
  jmp .L.end.999
.L.call.999:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.999:
  push rax
  push offset .LC290
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1000
  mov rax, 0
  call printf
  jmp .L.end.1000
.L.call.1000:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1000:
  push rax
  push offset .LC291
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1001
  mov rax, 0
  call printf
  jmp .L.end.1001
.L.call.1001:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1001:
  push rax
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1002
  mov rax, 0
  call gen
  jmp .L.end.1002
.L.call.1002:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1002:
  push rax
  push offset .LC292
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1003
  mov rax, 0
  call printf
  jmp .L.end.1003
.L.call.1003:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1003:
  push rax
  jmp .Lend998
.Lelse998:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1004
  mov rax, 0
  call gen
  jmp .L.end.1004
.L.call.1004:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1004:
  push rax
  push offset .LC293
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1005
  mov rax, 0
  call printf
  jmp .L.end.1005
.L.call.1005:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1005:
  push rax
  push offset .LC294
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1006
  mov rax, 0
  call printf
  jmp .L.end.1006
.L.call.1006:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1006:
  push rax
  jmp .Lend998
.Lend998:
  push offset .LC295
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1007
  mov rax, 0
  call printf
  jmp .L.end.1007
.L.call.1007:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1007:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase854:
  push offset if_sequence
  push offset if_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1008
  mov rax, 0
  call gen
  jmp .L.end.1008
.L.call.1008:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1008:
  push rax
  push offset .LC296
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1009
  mov rax, 0
  call printf
  jmp .L.end.1009
.L.call.1009:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1009:
  push rax
  push offset .LC297
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1010
  mov rax, 0
  call printf
  jmp .L.end.1010
.L.call.1010:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1010:
  push rax
  push offset .LC298
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1011
  mov rax, 0
  call printf
  jmp .L.end.1011
.L.call.1011:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1011:
  push rax
  mov rax, rbp
  sub rax, 60
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 60
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1012
  mov rax, 0
  call gen
  jmp .L.end.1012
.L.call.1012:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1012:
  push rax
  push offset .LC299
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1013
  mov rax, 0
  call printf
  jmp .L.end.1013
.L.call.1013:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1013:
  push rax
  push offset .LC300
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1014
  mov rax, 0
  call printf
  jmp .L.end.1014
.L.call.1014:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1014:
  push rax
  mov rax, rbp
  sub rax, 60
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1015
  mov rax, 0
  call gen
  jmp .L.end.1015
.L.call.1015:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1015:
  push rax
  push offset .LC301
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1016
  mov rax, 0
  call printf
  jmp .L.end.1016
.L.call.1016:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1016:
  push rax
  push offset .LC302
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1017
  mov rax, 0
  call printf
  jmp .L.end.1017
.L.call.1017:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1017:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase853:
  push offset if_sequence
  push offset if_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  push offset break_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push offset break_sequence
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 20
  push rax
  push offset continue_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push offset continue_sequence
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push offset .LC303
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1018
  mov rax, 0
  call printf
  jmp .L.end.1018
.L.call.1018:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1018:
  push rax
  push offset .LC304
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1019
  mov rax, 0
  call printf
  jmp .L.end.1019
.L.call.1019:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1019:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1020
  mov rax, 0
  call gen
  jmp .L.end.1020
.L.call.1020:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1020:
  push rax
  push offset .LC305
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1021
  mov rax, 0
  call printf
  jmp .L.end.1021
.L.call.1021:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1021:
  push rax
  push offset .LC306
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1022
  mov rax, 0
  call printf
  jmp .L.end.1022
.L.call.1022:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1022:
  push rax
  push offset .LC307
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1023
  mov rax, 0
  call printf
  jmp .L.end.1023
.L.call.1023:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1023:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1024
  mov rax, 0
  call gen
  jmp .L.end.1024
.L.call.1024:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1024:
  push rax
  push offset .LC308
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1025
  mov rax, 0
  call printf
  jmp .L.end.1025
.L.call.1025:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1025:
  push rax
  push offset .LC309
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1026
  mov rax, 0
  call printf
  jmp .L.end.1026
.L.call.1026:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1026:
  push rax
  push offset break_sequence
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push offset continue_sequence
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase852:
  push offset if_sequence
  push offset if_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  push offset break_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push offset break_sequence
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 20
  push rax
  push offset continue_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push offset continue_sequence
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1027
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1028
  mov rax, 0
  call gen
  jmp .L.end.1028
.L.call.1028:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1028:
  push rax
  jmp .Lend1027
.Lelse1027:
.Lend1027:
  push offset .LC310
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1029
  mov rax, 0
  call printf
  jmp .L.end.1029
.L.call.1029:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1029:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1030
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1031
  mov rax, 0
  call gen
  jmp .L.end.1031
.L.call.1031:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1031:
  push rax
  jmp .Lend1030
.Lelse1030:
  push offset .LC311
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1032
  mov rax, 0
  call printf
  jmp .L.end.1032
.L.call.1032:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1032:
  push rax
  jmp .Lend1030
.Lend1030:
  push offset .LC312
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1033
  mov rax, 0
  call printf
  jmp .L.end.1033
.L.call.1033:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1033:
  push rax
  push offset .LC313
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1034
  mov rax, 0
  call printf
  jmp .L.end.1034
.L.call.1034:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1034:
  push rax
  push offset .LC314
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1035
  mov rax, 0
  call printf
  jmp .L.end.1035
.L.call.1035:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1035:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1036
  mov rax, 0
  call gen
  jmp .L.end.1036
.L.call.1036:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1036:
  push rax
  push offset .LC315
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1037
  mov rax, 0
  call printf
  jmp .L.end.1037
.L.call.1037:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1037:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1038
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1039
  mov rax, 0
  call gen
  jmp .L.end.1039
.L.call.1039:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1039:
  push rax
  jmp .Lend1038
.Lelse1038:
.Lend1038:
  push offset .LC316
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1040
  mov rax, 0
  call printf
  jmp .L.end.1040
.L.call.1040:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1040:
  push rax
  push offset .LC317
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1041
  mov rax, 0
  call printf
  jmp .L.end.1041
.L.call.1041:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1041:
  push rax
  push offset break_sequence
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push offset continue_sequence
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase851:
  mov rax, rbp
  sub rax, 64
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin1042:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend1042
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1043
  mov rax, 0
  call gen
  jmp .L.end.1043
.L.call.1043:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1043:
  push rax
.Lcontinue1042:
  mov rax, rbp
  sub rax, 64
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin1042
.Lend1042:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase850:
  mov rax, rbp
  sub rax, 16
  push rax
  push offset break_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push offset break_sequence
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1044
  mov rax, 0
  call gen
  jmp .L.end.1044
.L.call.1044:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1044:
  push rax
  push offset .LC318
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1045
  mov rax, 0
  call printf
  jmp .L.end.1045
.L.call.1045:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1045:
  push rax
  mov rax, rbp
  sub rax, 72
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 104
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin1046:
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend1046
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 120
  push rax
  push offset if_sequence
  push offset if_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push offset .LC319
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1047
  mov rax, 0
  call printf
  jmp .L.end.1047
.L.call.1047:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1047:
  push rax
  push offset .LC320
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 120
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1048
  mov rax, 0
  call printf
  jmp .L.end.1048
.L.call.1048:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1048:
  push rax
.Lcontinue1046:
  mov rax, rbp
  sub rax, 72
  push rax
  mov rax, rbp
  sub rax, 72
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 104
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lbegin1046
.Lend1046:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 112
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1049
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 112
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 120
  push rax
  push offset if_sequence
  push offset if_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push offset .LC321
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 112
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 120
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1050
  mov rax, 0
  call printf
  jmp .L.end.1050
.L.call.1050:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1050:
  push rax
  jmp .Lend1049
.Lelse1049:
.Lend1049:
  push offset .LC322
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1051
  mov rax, 0
  call printf
  jmp .L.end.1051
.L.call.1051:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1051:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1052
  mov rax, 0
  call gen
  jmp .L.end.1052
.L.call.1052:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1052:
  push rax
  push offset .LC323
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1053
  mov rax, 0
  call printf
  jmp .L.end.1053
.L.call.1053:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1053:
  push rax
  push offset break_sequence
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase849:
  push offset .LC324
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 120
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1054
  mov rax, 0
  call printf
  jmp .L.end.1054
.L.call.1054:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1054:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase848:
  push offset if_sequence
  push offset if_sequence
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 76
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin1055:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 76
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend1055
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 76
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1056
  mov rax, 0
  call gen
  jmp .L.end.1056
.L.call.1056:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1056:
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
.Lcontinue1055:
  mov rax, rbp
  sub rax, 76
  push rax
  mov rax, rbp
  sub rax, 76
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin1055
.Lend1055:
  push 6
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1057
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC325
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1058
  mov rax, 0
  call error_at0
  jmp .L.end.1058
.L.call.1058:
  sub rsp, 8
  mov rax, 0
  call error_at0
  add rsp, 8
.L.end.1058:
  push rax
  jmp .Lend1057
.Lelse1057:
.Lend1057:
  mov rax, rbp
  sub rax, 80
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin1059:
  push 0
  mov rax, rbp
  sub rax, 80
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setle al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend1059
  push offset .LC326
  push offset argreg8
  mov rax, rbp
  sub rax, 80
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1060
  mov rax, 0
  call printf
  jmp .L.end.1060
.L.call.1060:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1060:
  push rax
.Lcontinue1059:
  mov rax, rbp
  sub rax, 80
  push rax
  mov rax, rbp
  sub rax, 80
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  jmp .Lbegin1059
.Lend1059:
  push offset .LC327
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1061
  mov rax, 0
  call printf
  jmp .L.end.1061
.L.call.1061:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1061:
  push rax
  push offset .LC328
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1062
  mov rax, 0
  call printf
  jmp .L.end.1062
.L.call.1062:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1062:
  push rax
  push offset .LC329
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1063
  mov rax, 0
  call printf
  jmp .L.end.1063
.L.call.1063:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1063:
  push rax
  push offset .LC330
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1064
  mov rax, 0
  call printf
  jmp .L.end.1064
.L.call.1064:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1064:
  push rax
  push offset .LC331
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1065
  mov rax, 0
  call printf
  jmp .L.end.1065
.L.call.1065:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1065:
  push rax
  push offset .LC332
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1066
  mov rax, 0
  call printf
  jmp .L.end.1066
.L.call.1066:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1066:
  push rax
  push offset .LC333
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1067
  mov rax, 0
  call printf
  jmp .L.end.1067
.L.call.1067:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1067:
  push rax
  push offset .LC334
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1068
  mov rax, 0
  call printf
  jmp .L.end.1068
.L.call.1068:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1068:
  push rax
  push offset .LC335
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1069
  mov rax, 0
  call printf
  jmp .L.end.1069
.L.call.1069:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1069:
  push rax
  push offset .LC336
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1070
  mov rax, 0
  call printf
  jmp .L.end.1070
.L.call.1070:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1070:
  push rax
  push offset .LC337
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1071
  mov rax, 0
  call printf
  jmp .L.end.1071
.L.call.1071:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1071:
  push rax
  push offset .LC338
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1072
  mov rax, 0
  call printf
  jmp .L.end.1072
.L.call.1072:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1072:
  push rax
  push offset .LC339
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1073
  mov rax, 0
  call printf
  jmp .L.end.1073
.L.call.1073:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1073:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase847:
  push offset .LC340
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1074
  mov rax, 0
  call printf
  jmp .L.end.1074
.L.call.1074:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1074:
  push rax
  push offset .LC341
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1075
  mov rax, 0
  call printf
  jmp .L.end.1075
.L.call.1075:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1075:
  push rax
  push offset .LC342
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1076
  mov rax, 0
  call printf
  jmp .L.end.1076
.L.call.1076:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1076:
  push rax
  push offset .LC343
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1077
  mov rax, 0
  call printf
  jmp .L.end.1077
.L.call.1077:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1077:
  push rax
  push offset locals
  push offset cur_scope_depth
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1078
  mov rax, rbp
  sub rax, 84
  push rax
  push offset locals
  push offset cur_scope_depth
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push offset .LC344
  mov rax, rbp
  sub rax, 84
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1079
  mov rax, 0
  call printf
  jmp .L.end.1079
.L.call.1079:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1079:
  push rax
  jmp .Lend1078
.Lelse1078:
.Lend1078:
  mov rax, rbp
  sub rax, 88
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin1080:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 48
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend1080
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 48
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 72
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1081
  push offset .LC345
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 48
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push offset argreg1
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1082
  mov rax, 0
  call printf
  jmp .L.end.1082
.L.call.1082:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1082:
  push rax
  jmp .Lend1081
.Lelse1081:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 48
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 72
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 4
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1083
  push offset .LC346
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 48
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push offset argreg4
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1084
  mov rax, 0
  call printf
  jmp .L.end.1084
.L.call.1084:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1084:
  push rax
  jmp .Lend1083
.Lelse1083:
  push offset .LC347
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 48
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push offset argreg8
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1085
  mov rax, 0
  call printf
  jmp .L.end.1085
.L.call.1085:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1085:
  push rax
  jmp .Lend1083
.Lend1083:
  jmp .Lend1081
.Lend1081:
.Lcontinue1080:
  mov rax, rbp
  sub rax, 88
  push rax
  mov rax, rbp
  sub rax, 88
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin1080
.Lend1080:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1086
  mov rax, 0
  call gen
  jmp .L.end.1086
.L.call.1086:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1086:
  push rax
  push offset .LC348
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1087
  mov rax, 0
  call printf
  jmp .L.end.1087
.L.call.1087:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1087:
  push rax
  push offset .LC349
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1088
  mov rax, 0
  call printf
  jmp .L.end.1088
.L.call.1088:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1088:
  push rax
  push offset .LC350
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1089
  mov rax, 0
  call printf
  jmp .L.end.1089
.L.call.1089:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1089:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase846:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1090
  mov rax, 0
  call gen_val
  jmp .L.end.1090
.L.call.1090:
  sub rsp, 8
  mov rax, 0
  call gen_val
  add rsp, 8
.L.end.1090:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase845:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1091
  mov rax, 0
  call gen
  jmp .L.end.1091
.L.call.1091:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1091:
  push rax
  push offset .LC351
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1092
  mov rax, 0
  call printf
  jmp .L.end.1092
.L.call.1092:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1092:
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1093
  mov rax, 0
  call get_type
  jmp .L.end.1093
.L.call.1093:
  sub rsp, 8
  mov rax, 0
  call get_type
  add rsp, 8
.L.end.1093:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse1095
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 2
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse1095
  push 1
  jmp .Lend1095
.Lfalse1095:
  push 0
.Lend1095:
  pop rax
  cmp rax, 0
  je .Lelse1094
  push offset .LC352
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1096
  mov rax, 0
  call printf
  jmp .L.end.1096
.L.call.1096:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1096:
  push rax
  jmp .Lend1094
.Lelse1094:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse1098
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse1098
  push 1
  jmp .Lend1098
.Lfalse1098:
  push 0
.Lend1098:
  pop rax
  cmp rax, 0
  je .Lelse1097
  push offset .LC353
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1099
  mov rax, 0
  call printf
  jmp .L.end.1099
.L.call.1099:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1099:
  push rax
  jmp .Lend1097
.Lelse1097:
  push offset .LC354
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1100
  mov rax, 0
  call printf
  jmp .L.end.1100
.L.call.1100:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1100:
  push rax
  jmp .Lend1097
.Lend1097:
  jmp .Lend1094
.Lend1094:
  push offset .LC355
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1101
  mov rax, 0
  call printf
  jmp .L.end.1101
.L.call.1101:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1101:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lend844:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1102
  mov rax, 0
  call gen
  jmp .L.end.1102
.L.call.1102:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1102:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1103
  mov rax, 0
  call gen
  jmp .L.end.1103
.L.call.1103:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1103:
  push rax
  push offset .LC356
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1104
  mov rax, 0
  call printf
  jmp .L.end.1104
.L.call.1104:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1104:
  push rax
  push offset .LC357
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1105
  mov rax, 0
  call printf
  jmp .L.end.1105
.L.call.1105:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1105:
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 38
  je .Lcase1107
  cmp rax, 36
  je .Lcase1108
  cmp rax, 37
  je .Lcase1109
  cmp rax, 9
  je .Lcase1110
  cmp rax, 8
  je .Lcase1111
  cmp rax, 7
  je .Lcase1112
  cmp rax, 6
  je .Lcase1113
  cmp rax, 4
  je .Lcase1114
  cmp rax, 3
  je .Lcase1115
  cmp rax, 2
  je .Lcase1116
  cmp rax, 1
  je .Lcase1117
  jmp .Lend1106
.Lcase1117:
  push offset .LC358
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1117
  mov rax, 0
  call printf
  jmp .L.end.1117
.L.call.1117:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1117:
  push rax
  jmp .Lend1106
.Lcase1116:
  push offset .LC359
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1118
  mov rax, 0
  call printf
  jmp .L.end.1118
.L.call.1118:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1118:
  push rax
  jmp .Lend1106
.Lcase1115:
  push offset .LC360
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1119
  mov rax, 0
  call printf
  jmp .L.end.1119
.L.call.1119:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1119:
  push rax
  jmp .Lend1106
.Lcase1114:
  push offset .LC361
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1120
  mov rax, 0
  call printf
  jmp .L.end.1120
.L.call.1120:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1120:
  push rax
  push offset .LC362
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1121
  mov rax, 0
  call printf
  jmp .L.end.1121
.L.call.1121:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1121:
  push rax
  jmp .Lend1106
.Lcase1113:
  push offset .LC363
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1122
  mov rax, 0
  call printf
  jmp .L.end.1122
.L.call.1122:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1122:
  push rax
  push offset .LC364
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1123
  mov rax, 0
  call printf
  jmp .L.end.1123
.L.call.1123:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1123:
  push rax
  push offset .LC365
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1124
  mov rax, 0
  call printf
  jmp .L.end.1124
.L.call.1124:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1124:
  push rax
  jmp .Lend1106
.Lcase1112:
  push offset .LC366
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1125
  mov rax, 0
  call printf
  jmp .L.end.1125
.L.call.1125:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1125:
  push rax
  push offset .LC367
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1126
  mov rax, 0
  call printf
  jmp .L.end.1126
.L.call.1126:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1126:
  push rax
  push offset .LC368
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1127
  mov rax, 0
  call printf
  jmp .L.end.1127
.L.call.1127:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1127:
  push rax
  jmp .Lend1106
.Lcase1111:
  push offset .LC369
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1128
  mov rax, 0
  call printf
  jmp .L.end.1128
.L.call.1128:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1128:
  push rax
  push offset .LC370
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1129
  mov rax, 0
  call printf
  jmp .L.end.1129
.L.call.1129:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1129:
  push rax
  push offset .LC371
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1130
  mov rax, 0
  call printf
  jmp .L.end.1130
.L.call.1130:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1130:
  push rax
  jmp .Lend1106
.Lcase1110:
  push offset .LC372
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1131
  mov rax, 0
  call printf
  jmp .L.end.1131
.L.call.1131:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1131:
  push rax
  push offset .LC373
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1132
  mov rax, 0
  call printf
  jmp .L.end.1132
.L.call.1132:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1132:
  push rax
  push offset .LC374
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1133
  mov rax, 0
  call printf
  jmp .L.end.1133
.L.call.1133:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1133:
  push rax
  jmp .Lend1106
.Lcase1109:
  push offset .LC375
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1134
  mov rax, 0
  call printf
  jmp .L.end.1134
.L.call.1134:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1134:
  push rax
  jmp .Lend1106
.Lcase1108:
  push offset .LC376
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1135
  mov rax, 0
  call printf
  jmp .L.end.1135
.L.call.1135:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1135:
  push rax
  jmp .Lend1106
.Lcase1107:
  push offset .LC377
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1136
  mov rax, 0
  call printf
  jmp .L.end.1136
.L.call.1136:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1136:
  push rax
  jmp .Lend1106
.Lend1106:
  push offset .LC378
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1137
  mov rax, 0
  call printf
  jmp .L.end.1137
.L.call.1137:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1137:
  push rax
  mov rsp, rbp
  pop rbp
  ret
.global gen_val
gen_val:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 25
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1138
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1139
  mov rax, 0
  call gen
  jmp .L.end.1139
.L.call.1139:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1139:
  push rax
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend1138
.Lelse1138:
.Lend1138:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 11
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1140
  push offset .LC379
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1141
  mov rax, 0
  call printf
  jmp .L.end.1141
.L.call.1141:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1141:
  push rax
  push offset .LC380
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1142
  mov rax, 0
  call printf
  jmp .L.end.1142
.L.call.1142:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1142:
  push rax
  push offset .LC381
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1143
  mov rax, 0
  call printf
  jmp .L.end.1143
.L.call.1143:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1143:
  push rax
  jmp .Lend1140
.Lelse1140:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 13
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1144
  push offset .LC382
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1145
  mov rax, 0
  call printf
  jmp .L.end.1145
.L.call.1145:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1145:
  push rax
  jmp .Lend1144
.Lelse1144:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 27
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1146
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1147
  mov rax, 0
  call gen_val
  jmp .L.end.1147
.L.call.1147:
  sub rsp, 8
  mov rax, 0
  call gen_val
  add rsp, 8
.L.end.1147:
  push rax
  push offset .LC383
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1148
  mov rax, 0
  call printf
  jmp .L.end.1148
.L.call.1148:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1148:
  push rax
  push offset .LC384
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 96
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1149
  mov rax, 0
  call printf
  jmp .L.end.1149
.L.call.1149:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1149:
  push rax
  push offset .LC385
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1150
  mov rax, 0
  call printf
  jmp .L.end.1150
.L.call.1150:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1150:
  push rax
  jmp .Lend1146
.Lelse1146:
  push offset .LC386
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1151
  mov rax, 0
  call error0
  jmp .L.end.1151
.L.call.1151:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.1151:
  push rax
  jmp .Lend1146
.Lend1146:
  jmp .Lend1144
.Lend1144:
  jmp .Lend1140
.Lend1140:
  mov rsp, rbp
  pop rbp
  ret
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 52
  mov [rbp-4], edi
  mov [rbp-12], rsi
  mov rax, rbp
  sub rax, 20
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 24
  push rax
  push 1
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin1152:
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend1152
  push offset filename
  mov rax, rbp
  sub rax, 12
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset .LC387
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push offset filename
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1153
  mov rax, 0
  call printf
  jmp .L.end.1153
.L.call.1153:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1153:
  push rax
  push offset user_input
  push offset filename
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1154
  mov rax, 0
  call read_file
  jmp .L.end.1154
.L.call.1154:
  sub rsp, 8
  mov rax, 0
  call read_file
  add rsp, 8
.L.end.1154:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 20
  push rax
  mov rax, rsp
  and rax, 15
  jnz .L.call.1155
  mov rax, 0
  call tokenize
  jmp .L.end.1155
.L.call.1155:
  sub rsp, 8
  mov rax, 0
  call tokenize
  add rsp, 8
.L.end.1155:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1156
  push offset token
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend1156
.Lelse1156:
  mov rax, rbp
  sub rax, 32
  push rax
  push offset token
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin1157:
.Lcontinue1157:
  push 1
  pop rax
  cmp rax, 0
  je .Lend1157
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 3
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1158
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend1157
  jmp .Lend1158
.Lelse1158:
.Lend1158:
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lbegin1157
.Lend1157:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend1156
.Lend1156:
.Lcontinue1152:
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin1152
.Lend1152:
  push offset cur_scope_depth
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1159
  mov rax, 0
  call program
  jmp .L.end.1159
.L.call.1159:
  sub rsp, 8
  mov rax, 0
  call program
  add rsp, 8
.L.end.1159:
  push rax
  push offset .LC388
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1160
  mov rax, 0
  call printf
  jmp .L.end.1160
.L.call.1160:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1160:
  push rax
  push offset .LC389
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1161
  mov rax, 0
  call printf
  jmp .L.end.1161
.L.call.1161:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1161:
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin1162:
  push offset code
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend1162
  push offset code
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 12
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1163
  push offset code
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1164
  push offset code
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1165
  mov rax, 0
  call gen
  jmp .L.end.1165
.L.call.1165:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1165:
  push rax
  jmp .Lend1164
.Lelse1164:
.Lend1164:
  jmp .Lend1163
.Lelse1163:
.Lend1163:
.Lcontinue1162:
  mov rax, rbp
  sub rax, 36
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin1162
.Lend1162:
  push offset .LC390
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1166
  mov rax, 0
  call printf
  jmp .L.end.1166
.L.call.1166:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1166:
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  push offset strings
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin1167:
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend1167
  push offset .LC391
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 8
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1168
  mov rax, 0
  call printf
  jmp .L.end.1168
.L.call.1168:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1168:
  push rax
  push offset .LC392
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1169
  mov rax, 0
  call printf
  jmp .L.end.1169
.L.call.1169:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1169:
  push rax
.Lcontinue1167:
  mov rax, rbp
  sub rax, 44
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lbegin1167
.Lend1167:
  mov rax, rbp
  sub rax, 48
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin1170:
  push offset code
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend1170
  push offset code
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 12
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1171
  push offset code
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 88
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1172
  push offset code
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1173
  mov rax, 0
  call gen
  jmp .L.end.1173
.L.call.1173:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1173:
  push rax
  jmp .Lend1172
.Lelse1172:
.Lend1172:
  jmp .Lend1171
.Lelse1171:
.Lend1171:
.Lcontinue1170:
  mov rax, rbp
  sub rax, 48
  push rax
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin1170
.Lend1170:
  push offset .LC393
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1174
  mov rax, 0
  call printf
  jmp .L.end.1174
.L.call.1174:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1174:
  push rax
  push offset .LC394
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1175
  mov rax, 0
  call printf
  jmp .L.end.1175
.L.call.1175:
  sub rsp, 8
  mov rax, 0
  call printf
  add rsp, 8
.L.end.1175:
  push rax
  push offset cur_scope_depth
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 52
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
.Lbegin1176:
  push offset code
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lend1176
  push offset code
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 23
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1177
  push offset cur_scope_depth
  push offset cur_scope_depth
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  push offset code
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 8
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1178
  mov rax, 0
  call gen
  jmp .L.end.1178
.L.call.1178:
  sub rsp, 8
  mov rax, 0
  call gen
  add rsp, 8
.L.end.1178:
  push rax
  jmp .Lend1177
.Lelse1177:
.Lend1177:
.Lcontinue1176:
  mov rax, rbp
  sub rax, 52
  push rax
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin1176
.Lend1176:
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global read_file
read_file:
  push rbp
  mov rbp, rsp
  sub rsp, 28
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 16
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push offset .LC395
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1179
  mov rax, 0
  call fopen
  jmp .L.end.1179
.L.call.1179:
  sub rsp, 8
  mov rax, 0
  call fopen
  add rsp, 8
.L.end.1179:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1180
  push offset .LC396
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1182
  mov rax, 0
  call strerror
  jmp .L.end.1182
.L.call.1182:
  sub rsp, 8
  mov rax, 0
  call strerror
  add rsp, 8
.L.end.1182:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1181
  mov rax, 0
  call error2
  jmp .L.end.1181
.L.call.1181:
  sub rsp, 8
  mov rax, 0
  call error2
  add rsp, 8
.L.end.1181:
  push rax
  jmp .Lend1180
.Lelse1180:
.Lend1180:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  push 2
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1184
  mov rax, 0
  call fseek
  jmp .L.end.1184
.L.call.1184:
  sub rsp, 8
  mov rax, 0
  call fseek
  add rsp, 8
.L.end.1184:
  push rax
  push 0
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1183
  push offset .LC397
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1186
  mov rax, 0
  call strerror
  jmp .L.end.1186
.L.call.1186:
  sub rsp, 8
  mov rax, 0
  call strerror
  add rsp, 8
.L.end.1186:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1185
  mov rax, 0
  call error2
  jmp .L.end.1185
.L.call.1185:
  sub rsp, 8
  mov rax, 0
  call error2
  add rsp, 8
.L.end.1185:
  push rax
  jmp .Lend1183
.Lelse1183:
.Lend1183:
  mov rax, rbp
  sub rax, 20
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1187
  mov rax, 0
  call ftell
  jmp .L.end.1187
.L.call.1187:
  sub rsp, 8
  mov rax, 0
  call ftell
  add rsp, 8
.L.end.1187:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  push 0
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1189
  mov rax, 0
  call fseek
  jmp .L.end.1189
.L.call.1189:
  sub rsp, 8
  mov rax, 0
  call fseek
  add rsp, 8
.L.end.1189:
  push rax
  push 0
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1188
  push offset .LC398
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1191
  mov rax, 0
  call strerror
  jmp .L.end.1191
.L.call.1191:
  sub rsp, 8
  mov rax, 0
  call strerror
  add rsp, 8
.L.end.1191:
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1190
  mov rax, 0
  call error2
  jmp .L.end.1190
.L.call.1190:
  sub rsp, 8
  mov rax, 0
  call error2
  add rsp, 8
.L.end.1190:
  push rax
  jmp .Lend1188
.Lelse1188:
.Lend1188:
  mov rax, rbp
  sub rax, 28
  push rax
  push 1
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 2
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1192
  mov rax, 0
  call calloc
  jmp .L.end.1192
.L.call.1192:
  sub rsp, 8
  mov rax, 0
  call calloc
  add rsp, 8
.L.end.1192:
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1193
  mov rax, 0
  call fread
  jmp .L.end.1193
.L.call.1193:
  sub rsp, 8
  mov rax, 0
  call fread
  add rsp, 8
.L.end.1193:
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue1195
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue1195
  push 0
  jmp .Lend1195
.Ltrue1195:
  push 1
.Lend1195:
  pop rax
  cmp rax, 0
  je .Lelse1194
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 10
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  jmp .Lend1194
.Lelse1194:
.Lend1194:
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 20
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 10
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1196
  mov rax, 0
  call fclose
  jmp .L.end.1196
.L.call.1196:
  sub rsp, 8
  mov rax, 0
  call fclose
  add rsp, 8
.L.end.1196:
  push rax
  mov rax, rbp
  sub rax, 28
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  mov rsp, rbp
  pop rbp
  ret
.global print_token
print_token:
  push rbp
  mov rbp, rsp
  sub rsp, 108
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1197
  push 0
  push offset .LC399
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1198
  mov rax, 0
  call fprintf
  jmp .L.end.1198
.L.call.1198:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1198:
  push rax
  jmp .Lend1197
.Lelse1197:
  mov rax, rbp
  sub rax, 108
  push rax
  push 0
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 2
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 3
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 4
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 5
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 6
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 7
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 8
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 9
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 10
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 11
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 12
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 13
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 14
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 15
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 16
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 17
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 18
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 19
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 20
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 21
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 22
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 23
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 24
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 25
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 26
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 27
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 28
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 29
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 30
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 31
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 32
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 33
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 34
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 35
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 36
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 37
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 38
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 39
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 40
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 41
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 42
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 43
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 44
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 45
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 46
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 47
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 48
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 49
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 50
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 51
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 52
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 53
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 54
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 55
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 56
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 57
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 58
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 59
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 60
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 61
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 62
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 63
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 64
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 65
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 66
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 67
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 68
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 69
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 70
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 71
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 72
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 73
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 74
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 75
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 76
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 77
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 78
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 79
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 80
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 81
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 82
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 83
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 84
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 85
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 86
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 87
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 88
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 89
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 90
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 91
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 92
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 93
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 94
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 95
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 96
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 97
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 98
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  push 99
  pop rdi
  pop rax
  add rax, rdi
  push rax
  push 0
  pop rdi
  pop rax
  mov [rax], dil
  push rdi
  mov rax, rbp
  sub rax, 108
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 32
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1199
  mov rax, 0
  call memcpy
  jmp .L.end.1199
.L.call.1199:
  sub rsp, 8
  mov rax, 0
  call memcpy
  add rsp, 8
.L.end.1199:
  push rax
  push 0
  push offset .LC400
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  mov rax, rbp
  sub rax, 108
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1200
  mov rax, 0
  call fprintf
  jmp .L.end.1200
.L.call.1200:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1200:
  push rax
  jmp .Lend1197
.Lend1197:
  mov rsp, rbp
  pop rbp
  ret
.global print_type
print_type:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1201
  push 0
  push offset .LC401
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1202
  mov rax, 0
  call fprintf
  jmp .L.end.1202
.L.call.1202:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1202:
  push rax
  jmp .Lend1201
.Lelse1201:
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 5
  je .Lcase1204
  cmp rax, 4
  je .Lcase1205
  cmp rax, 3
  je .Lcase1206
  cmp rax, 2
  je .Lcase1207
  cmp rax, 1
  je .Lcase1208
  jmp .Lcase1209
  jmp .Lend1203
.Lcase1208:
  mov rax, rbp
  sub rax, 16
  push rax
  push offset .LC402
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend1203
.Lcase1207:
  mov rax, rbp
  sub rax, 16
  push rax
  push offset .LC403
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend1203
.Lcase1206:
  mov rax, rbp
  sub rax, 16
  push rax
  push offset .LC404
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend1203
.Lcase1205:
  mov rax, rbp
  sub rax, 16
  push rax
  push offset .LC405
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  jmp .Lend1203
.Lcase1204:
  mov rax, rbp
  sub rax, 16
  push rax
  push offset .LC406
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lcase1209:
  jmp .Lend1203
.Lend1203:
  push 0
  push offset .LC407
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1209
  mov rax, 0
  call fprintf
  jmp .L.end.1209
.L.call.1209:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1209:
  push rax
  jmp .Lend1201
.Lend1201:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse1211
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 4
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue1212
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 3
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  jne .Ltrue1212
  push 0
  jmp .Lend1212
.Ltrue1212:
  push 1
.Lend1212:
  pop rax
  cmp rax, 0
  je .Lfalse1211
  push 1
  jmp .Lend1211
.Lfalse1211:
  push 0
.Lend1211:
  pop rax
  cmp rax, 0
  je .Lelse1210
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .Lend1210
.Lelse1210:
.Lend1210:
  push 0
  push offset .LC408
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1213
  mov rax, 0
  call fprintf
  jmp .L.end.1213
.L.call.1213:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1213:
  push rax
  mov rsp, rbp
  pop rbp
  ret
.global print_node
print_node:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov [rbp-8], rdi
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1214
  push 0
  push offset .LC409
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1215
  mov rax, 0
  call fprintf
  jmp .L.end.1215
.L.call.1215:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1215:
  push rax
  jmp .Lend1214
.Lelse1214:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1216
  push 0
  push offset .LC410
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1217
  mov rax, 0
  call fprintf
  jmp .L.end.1217
.L.call.1217:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1217:
  push rax
  jmp .Lend1216
.Lelse1216:
.Lend1216:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1218
  push 0
  push offset .LC411
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 64
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1219
  mov rax, 0
  call fprintf
  jmp .L.end.1219
.L.call.1219:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1219:
  push rax
  jmp .Lend1218
.Lelse1218:
.Lend1218:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 72
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1220
  push 0
  push offset .LC412
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 72
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1221
  mov rax, 0
  call fprintf
  jmp .L.end.1221
.L.call.1221:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1221:
  push rax
  jmp .Lend1220
.Lelse1220:
.Lend1220:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1222
  push 0
  push offset .LC413
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  add rax, 0
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1223
  mov rax, 0
  call fprintf
  jmp .L.end.1223
.L.call.1223:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1223:
  push rax
  jmp .Lend1222
.Lelse1222:
.Lend1222:
  jmp .Lend1214
.Lend1214:
  mov rsp, rbp
  pop rbp
  ret
.global error0
error0:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov [rbp-8], rdi
  push 0
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1224
  mov rax, 0
  call fprintf
  jmp .L.end.1224
.L.call.1224:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1224:
  push rax
  push 0
  push offset .LC414
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1225
  mov rax, 0
  call fprintf
  jmp .L.end.1225
.L.call.1225:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1225:
  push rax
  push 1
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1226
  mov rax, 0
  call exit
  jmp .L.end.1226
.L.call.1226:
  sub rsp, 8
  mov rax, 0
  call exit
  add rsp, 8
.L.end.1226:
  push rax
  mov rsp, rbp
  pop rbp
  ret
.global error1
error1:
  push rbp
  mov rbp, rsp
  sub rsp, 16
  mov [rbp-8], rdi
  mov [rbp-16], rsi
  push 0
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1227
  mov rax, 0
  call fprintf
  jmp .L.end.1227
.L.call.1227:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1227:
  push rax
  push 0
  push offset .LC415
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1228
  mov rax, 0
  call fprintf
  jmp .L.end.1228
.L.call.1228:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1228:
  push rax
  push 1
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1229
  mov rax, 0
  call exit
  jmp .L.end.1229
.L.call.1229:
  sub rsp, 8
  mov rax, 0
  call exit
  add rsp, 8
.L.end.1229:
  push rax
  mov rsp, rbp
  pop rbp
  ret
.global error2
error2:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov [rbp-8], rdi
  mov [rbp-16], rsi
  mov [rbp-24], rdx
  push 0
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1230
  mov rax, 0
  call fprintf
  jmp .L.end.1230
.L.call.1230:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1230:
  push rax
  push 0
  push offset .LC416
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1231
  mov rax, 0
  call fprintf
  jmp .L.end.1231
.L.call.1231:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1231:
  push rax
  push 1
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1232
  mov rax, 0
  call exit
  jmp .L.end.1232
.L.call.1232:
  sub rsp, 8
  mov rax, 0
  call exit
  add rsp, 8
.L.end.1232:
  push rax
  mov rsp, rbp
  pop rbp
  ret
.global error_at0
error_at0:
  push rbp
  mov rbp, rsp
  sub rsp, 52
  mov [rbp-8], rdi
  mov [rbp-16], rsi
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin1233:
.Lcontinue1233:
  push offset user_input
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse1234
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse1234
  push 1
  jmp .Lend1234
.Lfalse1234:
  push 0
.Lend1234:
  pop rax
  cmp rax, 0
  je .Lend1233
  mov rax, rbp
  sub rax, 24
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  jmp .Lbegin1233
.Lend1233:
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin1235:
.Lcontinue1235:
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend1235
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin1235
.Lend1235:
  mov rax, rbp
  sub rax, 36
  push rax
  push 1
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  push offset user_input
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin1236:
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend1236
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1237
  mov rax, rbp
  sub rax, 36
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lend1237
.Lelse1237:
.Lend1237:
.Lcontinue1236:
  mov rax, rbp
  sub rax, 44
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin1236
.Lend1236:
  mov rax, rbp
  sub rax, 48
  push rax
  push 0
  push offset .LC417
  push offset filename
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 36
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1238
  mov rax, 0
  call fprintf
  jmp .L.end.1238
.L.call.1238:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1238:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 0
  push offset .LC418
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1239
  mov rax, 0
  call fprintf
  jmp .L.end.1239
.L.call.1239:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1239:
  push rax
  mov rax, rbp
  sub rax, 52
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 0
  push offset .LC419
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push offset .LC420
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1240
  mov rax, 0
  call fprintf
  jmp .L.end.1240
.L.call.1240:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1240:
  push rax
  push 0
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1241
  mov rax, 0
  call fprintf
  jmp .L.end.1241
.L.call.1241:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1241:
  push rax
  push 1
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1242
  mov rax, 0
  call exit
  jmp .L.end.1242
.L.call.1242:
  sub rsp, 8
  mov rax, 0
  call exit
  add rsp, 8
.L.end.1242:
  push rax
  mov rsp, rbp
  pop rbp
  ret
.global error_at1
error_at1:
  push rbp
  mov rbp, rsp
  sub rsp, 60
  mov [rbp-8], rdi
  mov [rbp-16], rsi
  mov [rbp-24], rdx
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin1243:
.Lcontinue1243:
  push offset user_input
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse1244
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse1244
  push 1
  jmp .Lend1244
.Lfalse1244:
  push 0
.Lend1244:
  pop rax
  cmp rax, 0
  je .Lend1243
  mov rax, rbp
  sub rax, 32
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  jmp .Lbegin1243
.Lend1243:
  mov rax, rbp
  sub rax, 40
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin1245:
.Lcontinue1245:
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend1245
  mov rax, rbp
  sub rax, 40
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin1245
.Lend1245:
  mov rax, rbp
  sub rax, 44
  push rax
  push 1
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 52
  push rax
  push offset user_input
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin1246:
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend1246
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1247
  mov rax, rbp
  sub rax, 44
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lend1247
.Lelse1247:
.Lend1247:
.Lcontinue1246:
  mov rax, rbp
  sub rax, 52
  push rax
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin1246
.Lend1246:
  mov rax, rbp
  sub rax, 56
  push rax
  push 0
  push offset .LC421
  push offset filename
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 44
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1248
  mov rax, 0
  call fprintf
  jmp .L.end.1248
.L.call.1248:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1248:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 0
  push offset .LC422
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1249
  mov rax, 0
  call fprintf
  jmp .L.end.1249
.L.call.1249:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1249:
  push rax
  mov rax, rbp
  sub rax, 60
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 56
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 0
  push offset .LC423
  mov rax, rbp
  sub rax, 60
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push offset .LC424
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1250
  mov rax, 0
  call fprintf
  jmp .L.end.1250
.L.call.1250:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1250:
  push rax
  push 0
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1251
  mov rax, 0
  call fprintf
  jmp .L.end.1251
.L.call.1251:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1251:
  push rax
  push 1
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1252
  mov rax, 0
  call exit
  jmp .L.end.1252
.L.call.1252:
  sub rsp, 8
  mov rax, 0
  call exit
  add rsp, 8
.L.end.1252:
  push rax
  mov rsp, rbp
  pop rbp
  ret
.global error_at2
error_at2:
  push rbp
  mov rbp, rsp
  sub rsp, 68
  mov [rbp-8], rdi
  mov [rbp-16], rsi
  mov [rbp-24], rdx
  mov [rbp-32], rcx
  mov rax, rbp
  sub rax, 40
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin1253:
.Lcontinue1253:
  push offset user_input
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse1254
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 0
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lfalse1254
  push 1
  jmp .Lend1254
.Lfalse1254:
  push 0
.Lend1254:
  pop rax
  cmp rax, 0
  je .Lend1253
  mov rax, rbp
  sub rax, 40
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  jmp .Lbegin1253
.Lend1253:
  mov rax, rbp
  sub rax, 48
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin1255:
.Lcontinue1255:
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  setne al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend1255
  mov rax, rbp
  sub rax, 48
  push rax
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin1255
.Lend1255:
  mov rax, rbp
  sub rax, 52
  push rax
  push 1
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  mov rax, rbp
  sub rax, 60
  push rax
  push offset user_input
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.Lbegin1256:
  mov rax, rbp
  sub rax, 60
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  setl al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lend1256
  mov rax, rbp
  sub rax, 60
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  movsx rax, BYTE PTR [rax]
  push rax
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .Lelse1257
  mov rax, rbp
  sub rax, 52
  push rax
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lend1257
.Lelse1257:
.Lend1257:
.Lcontinue1256:
  mov rax, rbp
  sub rax, 60
  push rax
  mov rax, rbp
  sub rax, 60
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
  push 1
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  jmp .Lbegin1256
.Lend1256:
  mov rax, rbp
  sub rax, 64
  push rax
  push 0
  push offset .LC425
  push offset filename
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 52
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1258
  mov rax, 0
  call fprintf
  jmp .L.end.1258
.L.call.1258:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1258:
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 0
  push offset .LC426
  mov rax, rbp
  sub rax, 48
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1259
  mov rax, 0
  call fprintf
  jmp .L.end.1259
.L.call.1259:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1259:
  push rax
  mov rax, rbp
  sub rax, 68
  push rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 40
  push rax
  pop rax
  mov rax, [rax]
  push rax
  push 1
  pop rdi
  pop rax
  imul rax, rdi
  push rax
  pop rdi
  pop rax
  sub rax, rdi
  push rax
  mov rax, rbp
  sub rax, 64
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rdi
  pop rax
  add rax, rdi
  push rax
  pop rdi
  pop rax
  mov [rax], edi
  push rdi
  push 0
  push offset .LC427
  mov rax, rbp
  sub rax, 68
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  push offset .LC428
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1260
  mov rax, 0
  call fprintf
  jmp .L.end.1260
.L.call.1260:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1260:
  push rax
  push 0
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 32
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1261
  mov rax, 0
  call fprintf
  jmp .L.end.1261
.L.call.1261:
  sub rsp, 8
  mov rax, 0
  call fprintf
  add rsp, 8
.L.end.1261:
  push rax
  push 1
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1262
  mov rax, 0
  call exit
  jmp .L.end.1262
.L.call.1262:
  sub rsp, 8
  mov rax, 0
  call exit
  add rsp, 8
.L.end.1262:
  push rax
  mov rsp, rbp
  pop rbp
  ret
.global get_token_kind_name
get_token_kind_name:
  push rbp
  mov rbp, rsp
  sub rsp, 4
  mov [rbp-4], edi
  mov rax, rbp
  sub rax, 4
  push rax
  pop rax
  movsxd rax, DWORD PTR [rax]
  push rax
  pop rax
  cmp rax, 28
  je .Lcase1264
  cmp rax, 27
  je .Lcase1265
  cmp rax, 26
  je .Lcase1266
  cmp rax, 25
  je .Lcase1267
  cmp rax, 24
  je .Lcase1268
  cmp rax, 23
  je .Lcase1269
  cmp rax, 22
  je .Lcase1270
  cmp rax, 21
  je .Lcase1271
  cmp rax, 20
  je .Lcase1272
  cmp rax, 19
  je .Lcase1273
  cmp rax, 18
  je .Lcase1274
  cmp rax, 17
  je .Lcase1275
  cmp rax, 16
  je .Lcase1276
  cmp rax, 15
  je .Lcase1277
  cmp rax, 14
  je .Lcase1278
  cmp rax, 13
  je .Lcase1279
  cmp rax, 12
  je .Lcase1280
  cmp rax, 11
  je .Lcase1281
  cmp rax, 10
  je .Lcase1282
  cmp rax, 9
  je .Lcase1283
  cmp rax, 8
  je .Lcase1284
  cmp rax, 7
  je .Lcase1285
  cmp rax, 6
  je .Lcase1286
  cmp rax, 5
  je .Lcase1287
  cmp rax, 4
  je .Lcase1288
  cmp rax, 3
  je .Lcase1289
  cmp rax, 2
  je .Lcase1290
  cmp rax, 1
  je .Lcase1291
  jmp .Lcase1292
  jmp .Lend1263
.Lcase1291:
  push offset .LC429
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1290:
  push offset .LC430
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1289:
  push offset .LC431
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1288:
  push offset .LC432
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1287:
  push offset .LC433
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1286:
  push offset .LC434
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1285:
  push offset .LC435
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1284:
  push offset .LC436
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1283:
  push offset .LC437
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1282:
  push offset .LC438
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1281:
  push offset .LC439
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1280:
  push offset .LC440
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1279:
  push offset .LC441
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1278:
  push offset .LC442
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1277:
  push offset .LC443
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1276:
  push offset .LC444
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1275:
  push offset .LC445
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1274:
  push offset .LC446
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1273:
  push offset .LC447
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1272:
  push offset .LC448
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1271:
  push offset .LC449
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1270:
  push offset .LC450
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1269:
  push offset .LC451
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1268:
  push offset .LC452
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1267:
  push offset .LC453
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1266:
  push offset .LC454
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1265:
  push offset .LC455
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1264:
  push offset .LC456
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.Lcase1292:
  push offset .LC457
  pop rdi
  mov rax, rsp
  and rax, 15
  jnz .L.call.1292
  mov rax, 0
  call error0
  jmp .L.end.1292
.L.call.1292:
  sub rsp, 8
  mov rax, 0
  call error0
  add rsp, 8
.L.end.1292:
  push rax
.Lend1263:
  mov rsp, rbp
  pop rbp
  ret
