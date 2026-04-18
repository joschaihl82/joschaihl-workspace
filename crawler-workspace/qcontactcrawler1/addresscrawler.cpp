// addresscrawler.cpp
//
// features:
// - state persistence using "history.crl" (binary file)
// - restartable crawl (loads history on start)
// - shutdown persistence (saves state on quit)
// - extracts emails ONLY from <a href="mailto:..."> tags
// - depth limit of 16
// - scrolling log window
// - multithreaded (8 workers) with libcurl for fast fetching
//
// build requirements:
// - libcurl development headers
// - libxml2 development headers
// - qt5 or qt6
// - qmake: libs += -lcurl -lxml2

#include <qapplication>
#include <qmainwindow>
#include <qtablewidget>
#include <qlineedit>
#include <qpushbutton>
#include <qprogressbar>
#include <qhboxlayout>
#include <qvboxlayout>
#include <qheaderview>
#include <qset>
#include <qmutex>
#include <qmutexlocker>
#include <qwaitcondition>
#include <qurl>
#include <qthread>
#include <qmetatype>
#include <qlabel>
#include <qtextedit>
#include <qdatetime>
#include <qfile>
#include <qdatastream>
#include <qlist>

#include <vector>
#include <queue>
#include <atomic>

// external libraries
#include <curl/curl.h>
#include <libxml/htmlparser.h>
#include <libxml/xpath.h>

// ----------------------------- persistence configuration -----------------------------

const quint32 crawl_magic = 0x51744352; // "qtcr"
const quint32 crawl_version = 1;
const char *history_file = "history.crl";

// ----------------------------- types -----------------------------

struct crawlitem {
    qstring url;
    qstring norm;
    int depth;
    double score;

    bool operator<(const crawlitem &other) const {
        return score < other.score;
    }
};

q_declare_metatype(crawlitem)

// correct serialization/deserialization for crawlitem
qdatastream &operator<<(qdatastream &out, const crawlitem &item) {
    out << item.url << item.norm << item.depth << item.score;
    return out;
}

qdatastream &operator>>(qdatastream &in, crawlitem &item) {
    in >> item.url >> item.norm >> item.depth >> item.score;
    return in;
}

// ----------------------------- shared state -----------------------------

class crawlsharedstate {
public:
    qmutex mutex;
    qwaitcondition cond;
    std::priority_queue<crawlitem> frontier;
    qset<qstring> seenurls;
    qset<qstring> seenemails;

    std::atomic<bool> stopflag{false};
    std::atomic<int> pagescrawled{0};
    
    int maxpages = 100000; 
    int maxdepth = 16;

    // --- persistence methods ---

    bool save(const qstring &filename) {
        qfile file(filename);
        if (!file.open(qiodevice::writeonly)) return false;

        qdatastream out(&file);
        out.setversion(qdatastream::qt_5_12); 

        out << crawl_magic << crawl_version;
        out << (quint32)pagescrawled.load() << (quint32)maxpages << (quint32)maxdepth;
        out << seenurls << seenemails;

        qlist<crawlitem> frontierlist;
        {
            qmutexlocker locker(&mutex); 
            // copy and then iterate to save
            std::priority_queue<crawlitem> tempfrontier = frontier; 
            while (!tempfrontier.empty()) {
                frontierlist.append(tempfrontier.top());
                tempfrontier.pop();
            }
        }
        out << frontierlist;

        return true;
    }

    bool load(const qstring &filename) {
        qfile file(filename);
        if (!file.open(qiodevice::readonly)) return false;

        qdatastream in(&file);
        in.setversion(qdatastream::qt_5_12);

        quint32 magic, version;
        if (in.atend()) return false;
        in >> magic;
        if (magic != crawl_magic) return false;
        if (in.atend()) return false;
        in >> version;
        if (version != crawl_version) return false;

        quint32 pcount, pmax, dmax;
        if (in.atend()) return false;
        in >> pcount;
        if (in.atend()) return false;
        in >> pmax;
        if (in.atend()) return false;
        in >> dmax;
        
        pagescrawled = (int)pcount;
        maxpages = (int)pmax;
        maxdepth = (int)dmax;

        if (in.atend()) return false;
        in >> seenurls;
        if (in.atend()) return false;
        in >> seenemails;

        qlist<crawlitem> frontierlist;
        if (in.atend()) return false;
        in >> frontierlist;
        
        // rebuild priority queue
        for (const auto &item : frontierlist) {
            frontier.push(item);
        }

        return true;
    }

    void addurl(const qstring &url, const qstring &norm, int depth, double score) {
        qmutexlocker locker(&mutex);
        if (seenurls.contains(norm)) return;
        seenurls.insert(norm);

        crawlitem item;
        item.url = url;
        item.norm = norm;
        item.depth = depth;
        item.score = score;
        frontier.push(item);
        cond.wakeone();
    }

    bool popitem(crawlitem &item) {
        qmutexlocker locker(&mutex);
        while (frontier.empty() && !stopflag.load()) {
            cond.wait(&mutex);
        }
        if (stopflag.load() || frontier.empty()) return false;

        item = frontier.top();
        frontier.pop();
        return true;
    }
};

// ----------------------------- worker -----------------------------

class crawlerworker : public qthread {
    q_object

public:
    crawlsharedstate *state;

    explicit crawlerworker(crawlsharedstate *s, qobject *parent = nullptr)
        : qthread(parent), state(s) {}

protected:
    void run() override {
        // init curl per thread (required for multi-threading)
        curl *curl = curl_easy_init();
        if (!curl) return;

        // configure curl options
        curl_easy_setopt(curl, curlope_followlocation, 1l);
        curl_easy_setopt(curl, curlope_maxredirs, 5l);
        curl_easy_setopt(curl, curlope_timeout, 10l);
        curl_easy_setopt(curl, curlope_connecttimeout, 5l);
        curl_easy_setopt(curl, curlope_useragent, "addresscrawlerqt/mailto-only");
        curl_easy_setopt(curl, curlope_accept_encoding, ""); 
        curl_easy_setopt(curl, curlope_ssl_verifypeer, 0l);
        curl_easy_setopt(curl, curlope_ssl_verifyhost, 0l);
        curl_easy_setopt(curl, curlope_writefunction, writecallback);
        curl_easy_setopt(curl, curlope_writedata, this);
        curl_easy_setopt(curl, curlope_nosignal, 1l);

        while (!state->stopflag.load()) {
            if (state->pagescrawled.load() >= state->maxpages) {
                state->stopflag = true;
                break;
            }

            crawlitem item;
            if (!state->popitem(item)) break; // checks stopflag and frontier status

            emit logmessage(qstring("fetching: %1 (depth %2, frontier: %3)").arg(item.url).arg(item.depth).arg(state->frontier.size()));

            currentbuffer.clear();
            curl_easy_setopt(curl, curlope_url, item.url.toUtf8().constdata());

            curlcode res = curl_easy_perform(curl);
            if (res == curle_ok && !currentbuffer.isempty()) {
                parseandextract(currentbuffer, item);
                state->pagescrawled++;
                emit progressupdated(state->pagescrawled);
            } else {
                emit logmessage(qstring("curl error: %1, url: %2").arg(curl_easy_strerror(res)).arg(item.url));
            }
            
            // small sleep to yield cpu time, might not be necessary with a good frontier/pop logic
            qthread::usleep(50); 
        }

        curl_easy_cleanup(curl);
    }

signals:
    void foundlink(double score, int depth, qstring url);
    void foundemail(qstring email, qstring source);
    void progressupdated(int count);
    void logmessage(qstring msg);

private:
    qbytearray currentbuffer;

    static size_t writecallback(void *data, size_t size, size_t nmemb, void *userp) {
        size_t realsize = size * nmemb;
        crawlerworker *worker = static_cast<crawlerworker*>(userp);
        worker->currentbuffer.append((const char*)data, realsize);
        return realsize;
    }

    void parseandextract(const qbytearray &htmldata, const crawlitem &currentitem) {
        if (htmldata.isempty()) return;

        htmldocptr doc = htmlreadmemory(htmldata.constdata(), htmldata.size(),
                                             currentitem.url.toUtf8().constdata(),
                                             nullptr,
                                             html_parse_noerror | html_parse_nowarning | html_parse_recover);
        if (!doc) return;

        xmlxpathcontextptr xpathctx = xmlxpathnewcontext(doc);
        if (!xpathctx) {
            xmlfreedoc(doc);
            return;
        }

        // only extract links and mailto hrefs
        if (currentitem.depth < state->maxdepth) {
            xmlxpathobjectptr hrefobj = xmlxpathevalexpression((xmlchar *)"//@href", xpathctx);
            if (hrefobj && hrefobj->nodesetval) {
                for (int i = 0; i < hrefobj->nodesetval->nodenr; ++i) {
                    xmlattrptr attr = (xmlattrptr)hrefobj->nodesetval->nodetab[i];
                    if (attr && attr->children && attr->children->content) {
                        qstring href = qstring::fromUtf8((const char *)attr->children->content);
                        processhref(href, currentitem);
                    }
                }
            }
            if (hrefobj) xmlxpathfreeobject(hrefobj);
        }

        xmlxpathfreecontext(xpathctx);
        xmlfreedoc(doc);
    }

    void processhref(const qstring &href, const crawlitem &parentitem) {
        qstring cleanhref = href.trimmed();
        if (cleanhref.isempty()) return;
        
        // --- 1. handle mailto (the only source for emails now) ---
        if (cleanhref.startswith("mailto:", qt::caseinsensitive)) {
             qstring e = cleanhref.mid(7);
             if (e.contains('?')) e = e.section('?', 0, 0);
             e = e.toLower();
             
             bool isnew = false;
             {
                 qmutexlocker l(&state->mutex);
                 if(!state->seenemails.contains(e)) { 
                     state->seenemails.insert(e); 
                     isnew = true;
                 }
             }
             if(isnew) {
                 emit foundemail(e, parentitem.url);
                 emit logmessage(qstring(">>> found email: %1").arg(e));
             }
             return;
        }

        // --- 2. handle regular links for crawling ---
        if (cleanhref.startswith("javascript:", qt::caseinsensitive) || cleanhref.startswith("tel:", qt::caseinsensitive)) return; 

        qurl baseurl(parentitem.url);
        qurl resolvedurl = baseurl.resolved(qurl(cleanhref));
        
        if (!resolvedurl.isvalid()) return;
        if (resolvedurl.scheme() != "http" && resolvedurl.scheme() != "https") return;

        qurl normurl = resolvedurl;
        normurl.setfragment(qstring());
        qstring normstr = normurl.tostring(qurl::striptrailingslash | qurl::removeuserinfo);
        normstr = normstr.toLower();

        // simple score
        double score = 1.0;
        qstring host = resolvedurl.host();
        if (host.endswith(".gov")) score = 5.0;
        else if (host.endswith(".edu")) score = 4.0;
        else if (host.endswith(".org")) score = 2.0;
        else score = 1.0;
        
        int newdepth = parentitem.depth + 1;
        score -= (newdepth * 1.0); 

        {
            qmutexlocker locker(&state->mutex);
            if (!state->seenurls.contains(normstr)) {
                state->seenurls.insert(normstr);
                
                crawlitem item;
                item.url = resolvedurl.tostring();
                item.norm = normstr;
                item.depth = newdepth;
                item.score = score;
                state->frontier.push(item);
                state->cond.wakeone();

                emit foundlink(score, newdepth, item.url);
            }
        }
    }
};

// ----------------------------- main window -----------------------------

class mainwindow : public qmainwindow
{
    q_object // <--- fix: added q_object macro

public:
    mainwindow(qobject *parent = nullptr) : qmainwindow(parent) {
        // state initialization and loading is the first step
        state = new crawlsharedstate();
        bool loaded = state->load(history_file);
        
        qwidget *central = new qwidget(this);
        setcentralwidget(central);
        qvboxlayout *layout = new qvboxlayout(central);

        // top bar
        qhboxlayout *top = new qhboxlayout();
        urlinput = new qlineedit(this);
        urlinput->setplaceholdertext("https://example.com");
        
        if (loaded && !state->frontier.empty()) {
            urlinput->settext(qstring("resuming crawl. frontier size: %1").arg(state->frontier.size()));
            urlinput->setenabled(false);
        } else {
            urlinput->settext("https://www.gnu.org");
        }

        crawlbtn = new qpushbutton("start/resume crawl", this);
        top->addwidget(urlinput);
        top->addwidget(crawlbtn);
        layout->addlayout(top);

        // progress
        progressbar = new qprogressbar(this);
        progressbar->setrange(0, state->maxpages);
        progressbar->setvalue(state->pagescrawled);
        layout->addwidget(progressbar);

        // content area
        qhboxlayout *contentlayout = new qhboxlayout();
        
        // left: links queue
        qvboxlayout *leftcol = new qvboxlayout();
        linkslabel = new qlabel("crawled links (seen: " + qstring::number(state->seenurls.size()) + ")");
        leftcol->addwidget(linkslabel);
        linkstable = createtable({"score", "depth", "url"});
        leftcol->addwidget(linkstable);
        contentlayout->addlayout(leftcol, 2);

        // middle: emails found
        qvboxlayout *midcol = new qvboxlayout();
        emailslabel = new qlabel("emails (from mailto, seen: " + qstring::number(state->seenemails.size()) + ")");
        midcol->addwidget(emailslabel);
        emailstable = createtable({"email", "source"});
        // pre-populate emails if loaded
        for(const qstring& email : state->seenemails) {
            int row = emailstable->rowcount();
            emailstable->insertrow(row);
            emailstable->setitem(row, 0, new qtablewidgetitem(email));
            emailstable->setitem(row, 1, new qtablewidgetitem("loaded from history"));
        }
        midcol->addwidget(emailstable);
        contentlayout->addlayout(midcol, 2);

        // right: scrolling log
        qvboxlayout *rightcol = new qvboxlayout();
        rightcol->addwidget(new qlabel("activity log"));
        logarea = new qtextedit(this);
        logarea->setreadonly(true);
        logarea->setstylesheet("font-family: monospace; font-size: 11px;");
        rightcol->addwidget(logarea);
        contentlayout->addlayout(rightcol, 3);

        layout->addlayout(contentlayout);

        connect(crawlbtn, &qpushbutton::clicked, this, &mainwindow::startcrawl);

        setwindowtitle("addresscrawler: persistence + mailto only (depth 16)");
        resize(1200, 700);
        
        if (loaded) log("history.crl loaded successfully. frontier size: " + qstring::number(state->frontier.size()));

        xmlinitparser();
    }

    ~mainwindow() {
        stopcrawl();
        if (state) {
            log("saving state to history.crl...");
            if (state->save(history_file)) {
                 log("save successful. goodbye.");
            } else {
                 log("error saving state.");
            }
            // must delete the shared state object
            delete state;
        }
        xmlcleanupparser();
    }

private slots:
    void startcrawl() {
        if (isrunning) {
            stopcrawl();
            crawlbtn->settext("start/resume crawl");
            return;
        }

        bool isresume = !state->frontier.empty();

        if (isresume) {
            // case 1: resume an existing crawl state
            urlinput->settext(qstring("resuming crawl. frontier size: %1").arg(state->frontier.size()));
            urlinput->setenabled(false);
            log("resuming crawl from history...");
        } else {
            // case 2: fresh start needed (no history or history was completed/empty)
            qstring seed = urlinput->text().trimmed();
            if (seed.isempty()) return;

            // delete old state and create a new one to guarantee a clean slate
            if (state) delete state;
            state = new crawlsharedstate();
            state->addurl(seed, seed.toLower(), 0, 10.0);

            // reset ui for fresh start
            logarea->clear();
            linkstable->setrowcount(0);
            emailstable->setrowcount(0);
            progressbar->setrange(0, state->maxpages);
            progressbar->setvalue(0);
            
            // update labels to reflect new, empty state
            linkslabel->settext("crawled links (seen: 0)");
            emailslabel->settext("emails (from mailto, seen: 0)");
            urlinput->setenabled(true);
            urlinput->settext(seed);

            log("starting new crawl on seed: " + seed);
        }
        
        // final prep before spawning workers
        progressbar->setrange(0, state->maxpages);
        progressbar->setvalue(state->pagescrawled);
        crawlbtn->settext("stop");
        isrunning = true;

        // spawn workers
        int threadcount = 8;
        for(int i=0; i<threadcount; ++i) {
            crawlerworker *w = new crawlerworker(state, this);
            connect(w, &crawlerworker::foundlink, this, &mainwindow::onfoundlink);
            connect(w, &crawlerworker::foundemail, this, &mainwindow::onfoundemail);
            connect(w, &crawlerworker::progressupdated, this, &mainwindow::onprogress);
            connect(w, &crawlerworker::logmessage, this, &mainwindow::log);
            connect(w, &crawlerworker::finished, w, &qobject::deletelater);
            w->start();
            workers.append(w);
        }
    }

    void onfoundlink(double score, int depth, qstring url) {
        if (linkstable->rowcount() > 2000) return;
        int row = linkstable->rowcount();
        linkstable->insertrow(row);
        linkstable->setitem(row, 0, new qtablewidgetitem(qstring::number(score, 'f', 1)));
        linkstable->setitem(row, 1, new qtablewidgetitem(qstring::number(depth)));
        linkstable->setitem(row, 2, new qtablewidgetitem(url));
    }

    void onfoundemail(qstring email, qstring source) {
        int row = emailstable->rowcount();
        emailstable->insertrow(row);
        emailstable->setitem(row, 0, new qtablewidgetitem(email));
        emailstable->setitem(row, 1, new qtablewidgetitem(source));
        // update email count label
        emailslabel->settext("emails (from mailto, seen: " + qstring::number(state->seenemails.size()) + ")");
    }

    void onprogress(int count) {
        progressbar->setvalue(count);
        // update link count label
        linkslabel->settext("crawled links (seen: " + qstring::number(state->seenurls.size()) + ")");

        if (count >= state->maxpages && isrunning) {
            log("reached max page limit.");
            stopcrawl(); // stop workers gracefully
            crawlbtn->settext("done");
        }
    }

    void log(qstring msg) {
        qstring ts = qdatetime::currentdatetime().tostring("hh:mm:ss");
        logarea->append(qstring("[%1] %2").arg(ts, msg));
    }

private:
    qlineedit *urlinput;
    qpushbutton *crawlbtn;
    qprogressbar *progressbar;
    qtablewidget *linkstable;
    qtablewidget *emailstable;
    qtextedit *logarea;
    qlabel *linkslabel; 
    qlabel *emailslabel; 

    crawlsharedstate *state = nullptr;
    qlist<crawlerworker*> workers;
    bool isrunning = false;

    void stopcrawl() {
        if (!isrunning) return; 

        log("stopping workers...");
        if (state) {
            state->stopflag = true;
            state->cond.wakeall();
        }
        for (auto w : workers) {
            w->wait();
        }
        workers.clear();
        isrunning = false;
        log("stopped.");
    }

    qtablewidget* createtable(const qstringlist &headers) {
        qtablewidget *t = new qtablewidget(this);
        t->setcolumncount(headers.size());
        t->sethorizontalheaderlabels(headers);
        t->horizontalheader()->setsectionresizemode(headers.size()-1, qheaderview::stretch);
        t->setedittriggers(qabstractitemview::noedittriggers);
        return t;
    }
};

// ----------------------------- main -----------------------------

int main(int argc, char *argv[])
{
    // register metatypes for qt signals/slots
    qregistermetatype<int>("int");
    qregistermetatype<double>("double");
    qregistermetatype<qstring>("qstring");
    qregistermetatype<crawlitem>("crawlitem");

    // global init (required)
    curl_global_init(curl_global_all);

    qapplication app(argc, argv);
    mainwindow w;
    w.show();

    int ret = app.exec();

    // global cleanup (required)
    curl_global_cleanup();
    return ret;
}

#include "addresscrawler.moc"
