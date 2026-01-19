/*
  VireonOS Beta - Fixed (second pass)
  Naprawione:
   - dodane prototypy drawWSM() i drawDesktop()
   - usunieta zdublowana definicja smallLogo()
   - dodana implementacja drawWSM()
  Kompatybilne z Dev-C++ 5.11 (C++98)
*/

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <algorithm>

using namespace std;

/* ===============================
   KONSOLE / KOLORY / SLEEP
=============================== */
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
void setColor(int color) { SetConsoleTextAttribute(hConsole, color); }
void msleep(int ms) { Sleep(ms); }

/* ===============================
   GLOBAL DATA
=============================== */
string OS_NAME = "VireonOS Beta";
string KERNEL_VERSION = "Vireon Kernel Beta";
string CURRENT_USER = "admin";
string currentEnvironment = "GUI_Basic";
time_t bootTime;

map<string,string> fileSystem;
vector<string> processList;
vector<string> systemLog;
vector<string> installedPrograms;
vector<string> installedEnvironments;

vector<string> browserHistory;
vector<string> browserBookmarks;
struct Tab { string title; string url; };
vector<Tab> browserTabs;
int activeTabIndex = -1;

/* ===============================
   PROTOTYPES (naprawa: brakujace deklaracje)
=============================== */
string getUptime();
void addLog(const string &msg);
void showLogo();
void boot();

void initFS();
void ls();
void cat(const string &f);
void touch(const string &f);
void writeFile(const string &f);

void initProcesses();
void htop(bool verbose=false);
void showLogs();

void guessGame();
void calculator();
void notesApp();
void paint();
void musicPlayer();

void installer();
void changeEnvironment();

void wsmApps();
void wsmCmds();
void drawCommandsTable();

/* Now add prototypes for functions that were referenced before their definitions */
void drawWSM();
void drawDesktop();

void fastfetch();
void extendedFastfetch();

void browserShell();
void browserOpen(const string &url);
void browserShowBookmarks();
void browserShowHistory();
void browserNewTab(const string &url);
void browserCloseTab(int idx);
void browserSwitchTab(int idx);
void browserViewSource(const string &url);
void browserDownload(const string &url);

void asciiBorder(const string &title, int width=64, int color=11);
void loadingBar(const string &label, int length=30, int color=10);
void smallLogo(const string &id);

string promptLine(const string &prompt);
string toLowerStr(const string &s);
void pressAnyKey();

/* ===============================
   UTILITIES
=============================== */
string getUptime(){
    time_t now = time(0);
    int sec = (int)difftime(now, bootTime);
    int min = sec/60; sec%=60;
    char buffer[64]; sprintf(buffer,"%dm %ds",min,sec);
    return string(buffer);
}

void addLog(const string &msg){
    time_t now=time(0);
    char tbuf[32]; struct tm *lt=localtime(&now);
    sprintf(tbuf,"%02d:%02d:%02d",lt->tm_hour,lt->tm_min,lt->tm_sec);
    systemLog.push_back(string("[")+tbuf+"] "+msg);
}

/* ===============================
   ASCII LOGO / BETA BANNER
=============================== */
void showLogo(){
    setColor(9);
    cout<<"   /.';;[-_()((  VIREON   ))()_-];';.\\\n";
    cout<<"  /'.,-__..--..  OS BETA  ..--..__-,.'\\\n";
    setColor(11);
    cout<<"   __      __.__                         ____  _____\n";
    cout<<"  /  \\    /  \\  |__ _____    ____       /  _ \\/  _  \\\n";
    cout<<"  \\   \\/\\/   /  |  \\\\__  \\  /    \\     /  /_\\  \\  /_\\  \\\n";
    cout<<"   \\        /|   Y  \\/ __ \\|   |  \\   /    |    \\  |    \\\n";
    cout<<"    \\__/\\  / |___|  (____  /___|  /   \\____|__  /__|__  /\n";
    cout<<"         \\/       \\/     \\/     \\/            \\/      \\/\n\n";
    setColor(12);
    cout<<"    ###########################################\n";
    cout<<"    #   VireonOS Beta - System Simulation    #\n";
    cout<<"    #      Not a real OS - for demo only     #\n    ";
    cout<<"    ###########################################\n\n";
    setColor(7);
}

/* ===============================
   FILE SYSTEM
=============================== */
void initFS(){
    fileSystem.clear();
    fileSystem["readme.txt"] =
        "VireonOS Beta\n"
        "This is a simulated OS environment.\n";
    fileSystem["about.txt"] =
        "VireonOS Beta - Combined demo executable for Dev-C++ 5.11.\nContact: fake@vireonos.dev\n";
    fileSystem["notes.txt"] = "Initial notes...\n";
    addLog("Filesystem initialized");
}

void ls(){ 
    setColor(11); 
    cout<<"\nFiles:\n";
    for(map<string,string>::iterator it=fileSystem.begin(); it!=fileSystem.end(); ++it) 
        cout<<" - "<<it->first<<"\n"; 
    setColor(7); 
    cout<<"\n"; 
}

void cat(const string &f){ 
    if(fileSystem.count(f)) cout<<"\n"<<fileSystem[f]<<"\n";
    else cout<<"\nFile not found: "<<f<<"\n";
    cout<<"\n";
}

void touch(const string &f){ 
    if(!fileSystem.count(f)){
        fileSystem[f]="";
        addLog("File created: "+f);
        cout<<"\nCreated file: "<<f<<"\n\n";
    } else {
        cout<<"\nFile already exists: "<<f<<"\n\n";
    }
}

void writeFile(const string &f){
    cin.ignore();
    string t;
    cout<<"Enter text (single line will be saved): ";
    getline(cin,t);
    fileSystem[f]=t;
    addLog("File written: "+f);
    cout<<"Saved.\n\n";
}

/* ===============================
   PROCESS MANAGER (HTOP-like)
=============================== */
void initProcesses(){
    processList.clear();
    processList.push_back("kernel");
    processList.push_back("vireonshell");
    processList.push_back("logger");
    processList.push_back("installer");
    processList.push_back("netif");
    processList.push_back("audio");
    processList.push_back("gfx");
    addLog("Processes initialized");
}

void htop(bool verbose){
    setColor(10);
    cout<<"\n+------------------------------------------------------------+\n";
    cout<<"|               VireonOS Process Monitor (htop)             |\n";
    cout<<"+------------------------------------------------------------+\n";
    cout<<"| PID  | NAME         | CPU% | MEM% | THREADS | STATE       |\n";
    cout<<"+------------------------------------------------------------+\n";
    for(size_t i=0;i<processList.size();i++){
        int pid = 1000 + (int)i*3 + rand()%50;
        int cpu = rand()%100;
        int mem = rand()%100;
        int thr = rand()%12 + 1;
        string state = (rand()%2==0?"running":"sleeping");
        char buf[256];
        sprintf(buf,"| %-4d | %-12s | %3d%% | %3d%% | %6d | %-10s |",
                pid, processList[i].c_str(), cpu, mem, thr, state.c_str());
        cout<<buf<<"\n";
        if(verbose){
            cout<<"    CMD: /bin/"<<processList[i]<<" --service\n";
            cout<<"    Uptime: "<<(rand()%360)<<"s  Started by: root\n";
        }
    }
    cout<<"+------------------------------------------------------------+\n\n";
    setColor(7);
}

/* ===============================
   SYSTEM LOGS
=============================== */
void showLogs(){
    setColor(14);
    cout<<"\n--- System Logs ---\n";
    for(size_t i=0;i<systemLog.size();i++){
        cout<<systemLog[i]<<"\n";
    }
    cout<<"\n";
    setColor(7);
}

/* ===============================
   MINI GAMES
=============================== */
void guessGame(){
    srand((unsigned)time(0));
    int secret = rand()%20+1, g;
    cout<<"Guess number (1-20): ";
    cin>>g;
    if(g==secret){
        setColor(10); cout<<"Correct! You win!\n\n";
        addLog("guessGame: user guessed correctly");
    } else {
        setColor(12); cout<<"Wrong! The number was "<<secret<<"\n\n";
        addLog("guessGame: user guessed wrong");
    }
    setColor(7);
}

/* ===============================
   CALCULATOR
=============================== */
void calculator(){
    double a,b; char op;
    cout<<"Calculator - enter: num1 operator num2\n> ";
    cin >> a >> op >> b;
    double res=0;
    switch(op){
        case '+': res=a+b; break;
        case '-': res=a-b; break;
        case '*': res=a*b; break;
        case '/': if(b!=0) res=a/b; else { cout<<"Error: division by zero\n"; return; } break;
        default: cout<<"Unknown operator\n"; return;
    }
    cout<<"Result: "<<res<<"\n\n";
}

/* ===============================
   NOTES APP
=============================== */
void notesApp(){
    cin.ignore();
    string line;
    cout<<"Notes - enter lines. Single '.' on a line to finish.\n";
    while(true){
        getline(cin,line);
        if(line==".") break;
        fileSystem["notes.txt"] += line + "\n";
        addLog("Note added");
    }
    cout<<"Notes saved to notes.txt\n\n";
}

/* ===============================
   PAINT (ASCII)
=============================== */
void paint(){
    asciiBorder("PAINT - ASCII CANVAS",48,12);
    for(int y=0;y<8;y++){
        cout<<"| ";
        for(int x=0;x<40;x++){
            cout<<(rand()%3==0? '#' : ' ');
        }
        cout<<" |\n";
        msleep(60);
    }
    asciiBorder("END PAINT",48,12);
    cout<<"\n";
}

/* ===============================
   MUSIC PLAYER (FAKE)
=============================== */
void musicPlayer(){
    asciiBorder("MUSICPLAYER",48,13);
    cout<<" Now playing: 'Synthetic Waves' [demo]\n\n";
    for(int i=0;i<12;i++){
        cout<<"   ";
        for(int j=0;j<30;j++){
            if(rand()%3==0) cout<<((j%2) ? '+' : '~'); else cout<<' ';
        }
        cout<<"\n";
        msleep(120);
    }
    asciiBorder("END MUSIC",48,13);
    cout<<"\n";
    addLog("musicPlayer: played Synthetic Waves");
}

/* ===============================
   INSTALLER (expanded) - C++98-compatible
=============================== */
void installer(){
    asciiBorder("VIREON INSTALLER",60,11);
    smallLogo("installer");
    addLog("Installer launched");
    installedPrograms.clear();
    installedEnvironments.clear();

    vector<string> programs;
    programs.push_back("TextEditor");
    programs.push_back("WebBrowser");
    programs.push_back("MusicPlayer");
    programs.push_back("Calculator");
    programs.push_back("Paint");
    programs.push_back("Notes");
    programs.push_back("SystemMonitor");
    programs.push_back("MiniGames");
    programs.push_back("DevKit");

    vector<string> envs;
    envs.push_back("GUI_Basic");
    envs.push_back("GUI_Advanced");
    envs.push_back("Desktop_3D");
    envs.push_back("RetroConsole");

    cout<<"Available Programs:\n";
    for(size_t i=0;i<programs.size();i++){
        printf(" %2d) %s\n", (int)i+1, programs[i].c_str());
    }
    cout<<"  0) finish selection\n";
    int choice;
    while(true){
        cout<<"Select program number to install (0 to finish): ";
        cin>>choice;
        if(choice==0) break;
        if(choice>=1 && choice <= (int)programs.size()){
            string p = programs[choice-1];
            bool already = false;
            for(size_t k=0;k<installedPrograms.size();k++){
                if(installedPrograms[k] == p){ already = true; break; }
            }
            if(!already){
                loadingBar("Installing "+p, 28, 10);
                installedPrograms.push_back(p);
                addLog("Installed program: "+p);
                cout<<p<<" installed.\n\n";
            } else {
                cout<<p<<" already installed.\n";
            }
        } else {
            cout<<"Invalid selection\n";
        }
    }

    cout<<"\nAvailable Environments:\n";
    for(size_t i=0;i<envs.size();i++){
        printf(" %2d) %s\n", (int)i+1, envs[i].c_str());
    }
    cout<<"  0) finish selection\n";
    while(true){
        cout<<"Select environment number to install (0 to finish): ";
        cin>>choice;
        if(choice==0) break;
        if(choice>=1 && choice <= (int)envs.size()){
            string e = envs[choice-1];
            bool already = false;
            for(size_t k=0;k<installedEnvironments.size();k++){
                if(installedEnvironments[k] == e){ already = true; break; }
            }
            if(!already){
                loadingBar("Installing env "+e, 24, 9);
                installedEnvironments.push_back(e);
                addLog("Installed environment: "+e);
                cout<<e<<" installed.\n\n";
            } else {
                cout<<e<<" already installed.\n";
            }
        } else {
            cout<<"Invalid selection\n";
        }
    }

    cout<<"Auto-configure startup services? (y/n): ";
    char yn; cin>>yn;
    if(tolower(yn)=='y'){
        loadingBar("Configuring services", 20, 14);
        addLog("Services configured");
        cout<<"Services configured.\n\n";
    }

    cout<<"Installation finished. Installed items:\n";
    for(size_t i=0;i<installedPrograms.size();i++) cout<<" - "<<installedPrograms[i]<<"\n";
    for(size_t i=0;i<installedEnvironments.size();i++) cout<<" - "<<installedEnvironments[i]<<"\n";
    cout<<"\n";
    addLog("Installer finished");
}

/* ===============================
   ENVIRONMENT CHANGE
=============================== */
void changeEnvironment(){
    cout<<"Available Environments:\n";
    for(size_t i=0;i<installedEnvironments.size();i++){
        cout<<" "<<i+1<<") "<<installedEnvironments[i]<<"\n";
    }
    cout<<"Select number: ";
    int c; cin>>c;
    if(c>=1 && c <= (int)installedEnvironments.size()){
        currentEnvironment = installedEnvironments[c-1];
        addLog("Environment changed to "+currentEnvironment);
        cout<<"Environment set to "<<currentEnvironment<<"\n\n";
    } else {
        cout<<"Invalid choice or no environments installed.\n\n";
    }
}

/* ===============================
   WSM / COMMANDS TABLE (visually improved)
=============================== */
void drawCommandsTable(){
    asciiBorder("COMMANDS REFERENCE",72,14);
    setColor(14);

    vector< pair<string, vector< pair<string,string> > > > table;

    vector< pair<string,string> > sys;
    sys.push_back(make_pair(string("help"), string("Show help")));
    sys.push_back(make_pair(string("ver"), string("Version info")));
    sys.push_back(make_pair(string("whoami"), string("Current user")));
    sys.push_back(make_pair(string("uptime"), string("System uptime")));
    sys.push_back(make_pair(string("neofetch"), string("Full info")));
    table.push_back(make_pair(string("System"), sys));

    vector< pair<string,string> > files;
    files.push_back(make_pair(string("ls"), string("List files")));
    files.push_back(make_pair(string("cat FILE"), string("Show file")));
    files.push_back(make_pair(string("touch FILE"), string("Create file")));
    files.push_back(make_pair(string("write FILE"), string("Write to file")));
    table.push_back(make_pair(string("Files"), files));

    vector< pair<string,string> > procs;
    procs.push_back(make_pair(string("ps"), string("Process list (htop)")));
    procs.push_back(make_pair(string("htop"), string("Detailed htop view")));
    table.push_back(make_pair(string("Processes"), procs));

    vector< pair<string,string> > apps;
    apps.push_back(make_pair(string("browser"), string("Open browser")));
    apps.push_back(make_pair(string("youtube"), string("YouTube ascii")));
    apps.push_back(make_pair(string("paint"), string("Paint")));
    apps.push_back(make_pair(string("musicplayer"), string("Music player")));
    apps.push_back(make_pair(string("notes"), string("Notes")));
    apps.push_back(make_pair(string("calculator"), string("Calculator")));
    table.push_back(make_pair(string("Apps"), apps));

    vector< pair<string,string> > maint;
    maint.push_back(make_pair(string("installer"), string("Run installer")));
    maint.push_back(make_pair(string("envchange"), string("Change environment")));
    maint.push_back(make_pair(string("logs"), string("Show logs")));
    maint.push_back(make_pair(string("cls"), string("Clear screen")));
    maint.push_back(make_pair(string("exit"), string("Shutdown")));
    table.push_back(make_pair(string("Maintenance"), maint));

    for(size_t i=0;i<table.size();i++){
        string cat = table[i].first;
        cout<<"["<<cat<<"]\n";
        for(size_t j=0;j<table[i].second.size();j++){
            string cmd = table[i].second[j].first;
            string desc = table[i].second[j].second;
            printf("  %-16s - %s\n", cmd.c_str(), desc.c_str());
        }
        cout<<"\n";
    }
    setColor(7);
    asciiBorder("END COMMANDS",72,14);
}

/* ===============================
   FASTFETCH / EXTENDED
=============================== */
void fastfetch(){
    setColor(13);
    cout<<OS_NAME<<" | "<<KERNEL_VERSION<<" | User: "<<CURRENT_USER<<" | Env: "<<currentEnvironment<<" | Uptime: "<<getUptime()<<"\n\n";
    setColor(7);
}

void extendedFastfetch(){
    asciiBorder("EXTENDED SYSTEM INFO",60,13);
    setColor(13);
    cout<<" OS: "<<OS_NAME<<"\n";
    cout<<" Kernel: "<<KERNEL_VERSION<<"\n";
    cout<<" User: "<<CURRENT_USER<<"\n";
    cout<<" Environment: "<<currentEnvironment<<"\n";
    cout<<" Uptime: "<<getUptime()<<"\n";
    cout<<" Installed Programs:\n   ";
    for(size_t i=0;i<installedPrograms.size();i++){
        cout<<installedPrograms[i]<<"  ";
        if((i+1)%4==0) cout<<"\n   ";
    }
    cout<<"\n Installed Environments:\n   ";
    for(size_t i=0;i<installedEnvironments.size();i++){
        cout<<installedEnvironments[i]<<"  ";
    }
    cout<<"\n";
    setColor(7);
    asciiBorder("END EXTENDED INFO",60,13);
}

/* ===============================
   BROWSER (expanded) - C++98-compatible
=============================== */
void browserShell(){
    asciiBorder("ASCII BROWSER - Session",64,9);
    smallLogo("browser");
    if(browserTabs.empty()){
        browserNewTab("home://start");
    }
    string line;
    while(true){
        if(activeTabIndex >=0 && activeTabIndex < (int)browserTabs.size()){
            setColor(11);
            cout<<"[Tab "<<activeTabIndex+1<<"/"<<browserTabs.size()<<"] "<<browserTabs[activeTabIndex].title<<" - "<<browserTabs[activeTabIndex].url<<"\n";
            setColor(7);
        }
        cout<<"browser> ";
        if(!getline(cin,line)) break;
        if(line.size()==0) continue;
        string cmd = toLowerStr(line);
        if(cmd=="exit" || cmd=="quit") break;
        else if(cmd=="help"){
            cout<<"Browser commands: open URL | newtab URL | closetab N | switch N | back | forward | refresh | bookmark | bookmarks | history | search TERM | viewsource | download | tabs | exit\n";
        }
        else if(cmd.substr(0,5)=="open "){
            string url = line.substr(5);
            browserOpen(url);
        }
        else if(cmd.substr(0,7)=="newtab "){
            string url = line.substr(7);
            browserNewTab(url);
        }
        else if(cmd.substr(0,8)=="closetab"){
            int idx = -1;
            if(sscanf(line.c_str(),"closetab %d",&idx)==1){
                browserCloseTab(idx-1);
            } else {
                cout<<"Usage: closetab N\n";
            }
        }
        else if(cmd.substr(0,6)=="switch"){
            int idx=-1;
            if(sscanf(line.c_str(),"switch %d",&idx)==1){
                browserSwitchTab(idx-1);
            } else cout<<"Usage: switch N\n";
        }
        else if(cmd=="tabs"){
            cout<<"Open Tabs:\n";
            for(size_t i=0;i<browserTabs.size();i++){
                printf(" %2d) %-30s %s\n", (int)i+1, browserTabs[i].title.c_str(), browserTabs[i].url.c_str());
            }
        }
        else if(cmd=="bookmark"){
            if(activeTabIndex>=0 && activeTabIndex < (int)browserTabs.size()){
                browserBookmarks.push_back(browserTabs[activeTabIndex].url);
                cout<<"Bookmarked: "<<browserTabs[activeTabIndex].url<<"\n";
                addLog("Browser bookmark added: "+browserTabs[activeTabIndex].url);
            } else cout<<"No active tab\n";
        }
        else if(cmd=="bookmarks"){
            browserShowBookmarks();
        }
        else if(cmd=="history"){
            browserShowHistory();
        }
        else if(cmd.substr(0,7)=="search "){
            string term = line.substr(7);
            cout<<"Search results for: "<<term<<"\n";
            for(int i=1;i<=5;i++){
                printf(" %d) https://search.fake/%s/result%d\n", i, term.c_str(), i);
            }
            cout<<"Open result number? (0 = none): ";
            int r; cin>>r; cin.ignore();
            if(r>=1 && r<=5){
                char buf[256];
                sprintf(buf, "https://search.fake/%s/result%d", term.c_str(), r);
                string url = buf;
                browserOpen(url);
            }
        }
        else if(cmd=="back"){
            if(browserHistory.size() >= 2){
                browserHistory.pop_back();
                string prev = browserHistory.back();
                browserOpen(prev);
            } else cout<<"No history\n";
        }
        else if(cmd=="viewsource"){
            if(activeTabIndex>=0 && activeTabIndex < (int)browserTabs.size()){
                browserViewSource(browserTabs[activeTabIndex].url);
            } else cout<<"No active tab\n";
        }
        else if(cmd=="download"){
            if(activeTabIndex>=0 && activeTabIndex < (int)browserTabs.size()){
                browserDownload(browserTabs[activeTabIndex].url);
            } else cout<<"No active tab\n";
        }
        else if(cmd=="refresh"){
            if(activeTabIndex>=0 && activeTabIndex < (int)browserTabs.size()){
                cout<<"Refreshing "<<browserTabs[activeTabIndex].url<<"\n";
                loadingBar("Refresh",24,9);
                browserOpen(browserTabs[activeTabIndex].url);
            } else cout<<"No active tab\n";
        }
        else cout<<"Unknown browser command. Type 'help'.\n";
    }

    asciiBorder("CLOSING BROWSER",64,9);
}

void browserOpen(const string &url){
    string u = url;
    if(u=="home://start") u = "vireonos.com/home";
    if(activeTabIndex==-1){
        browserNewTab(u);
        return;
    } else {
        browserTabs[activeTabIndex].url = u;
        browserTabs[activeTabIndex].title = u;
    }
    browserHistory.push_back(u);
    addLog("Browser opened: "+u);

    asciiBorder("PAGE: "+u,64,10);
    if(u.find("vireonos.com")!=string::npos){
        smallLogo("vireon");
        cout<<"Welcome to VireonOS official ASCII page!\n";
        cout<<"- Projects: VireonOS, VireonTools, VireonArt\n";
        cout<<"- Follow the fake dev announcements.\n";
    } else if(u.find("github.com")!=string::npos){
        cout<<"GitHub - Fake Repositories:\n";
        cout<<" - fireon/vireonos\n - guest/demo\n - tools/ascii-suite\n";
    } else if(u.find("youtube")!=string::npos || u.find("video")!=string::npos){
        cout<<"YouTube Beta - Video Player (ASCII)\n";
        cout<<"Type 'play' to play sample, or 'download' to fake-download.\n";
    } else if(u.find("example.com")!=string::npos){
        cout<<"Example Page\nLorem ipsum dolor sit amet, ascii content demo.\n";
    } else if(u.find("search.fake")!=string::npos){
        cout<<"Search engine placeholder page.\n";
    } else {
        cout<<"Generic page for: "<<u<<"\n";
        cout<<"[ASCII CONTENT FOLLOWS]\n";
        for(int i=0;i<6;i++){
            cout<<" ~~~ "<<string(40-(i*2),'~')<<"\n";
        }
    }
    cout<<"\n";
}

void browserShowBookmarks(){
    cout<<"\nBookmarks:\n";
    if(browserBookmarks.empty()) cout<<" (none)\n\n";
    else {
        for(size_t i=0;i<browserBookmarks.size();i++){
            cout<<" "<<i+1<<") "<<browserBookmarks[i]<<"\n";
        }
        cout<<"\n";
    }
}

void browserShowHistory(){
    cout<<"\nHistory (most recent last):\n";
    if(browserHistory.empty()) cout<<" (none)\n\n";
    else {
        for(size_t i=0;i<browserHistory.size();i++){
            cout<<" "<<i+1<<") "<<browserHistory[i]<<"\n";
        }
        cout<<"\n";
    }
}

void browserNewTab(const string &url){
    Tab t; t.url = url; t.title = url;
    browserTabs.push_back(t);
    activeTabIndex = (int)browserTabs.size()-1;
    cout<<"New tab opened: "<<url<<"\n";
    browserOpen(url);
}

void browserCloseTab(int idx){
    if(idx<0 || idx >= (int)browserTabs.size()){
        cout<<"Invalid tab index\n";
        return;
    }
    cout<<"Closing tab "<<idx+1<<": "<<browserTabs[idx].url<<"\n";
    browserTabs.erase(browserTabs.begin()+idx);
    if(browserTabs.empty()) activeTabIndex = -1;
    else activeTabIndex = max(0, idx-1);
}

void browserSwitchTab(int idx){
    if(idx<0 || idx >= (int)browserTabs.size()){
        cout<<"Invalid tab index\n";
        return;
    }
    activeTabIndex = idx;
    cout<<"Switched to tab "<<idx+1<<"\n";
    browserOpen(browserTabs[activeTabIndex].url);
}

void browserViewSource(const string &url){
    asciiBorder("VIEW SOURCE",64,12);
    cout<<"<!-- Fake HTML source for "<<url<<" -->\n";
    cout<<"<html>\n <head><title>Demo</title></head>\n <body>\n  <h1>Welcome</h1>\n  <p>This is an ASCII demo page</p>\n </body>\n</html>\n\n";
}

void browserDownload(const string &url){
    cout<<"Starting download for: "<<url<<"\n";
    loadingBar("Downloading",34,11);
    char fnamebuf[64];
    sprintf(fnamebuf, "dl_%d.bin", rand()%9999);
    string filename = fnamebuf;
    fileSystem[filename] = "FAKE-BINARY-DATA";
    addLog("Downloaded "+url+" -> "+filename);
    cout<<"Saved to "<<filename<<"\n\n";
}

/* ===============================
   VISUAL HELPERS & LOGOS (jedna definicja smallLogo)
=============================== */
void asciiBorder(const string &title, int width, int color){
    setColor(color);
    cout<<"\n+";
    for(int i=0;i<width;i++) cout<<"=";
    cout<<"+\n";
    cout<<"| "<<title;
    if((int)title.length() < width-1){
        for(int i=0;i<width - (int)title.length() -1; i++) cout<<" ";
    }
    cout<<"|\n";
    cout<<"+";
    for(int i=0;i<width;i++) cout<<"=";
    cout<<"+\n";
    setColor(7);
}

void loadingBar(const string &label, int length, int color){
    setColor(color);
    cout<<label<<": [";
    for(int i=0;i<length;i++) cout<<" ";
    cout<<"]\r"<<label<<": [";
    for(int i=0;i<length;i++){
        cout<<"#"; msleep(30 + rand()%60);
    }
    cout<<"]\n";
    setColor(7);
}

void smallLogo(const string &id){
    setColor(10);
    if(id=="installer"){
        cout<<" /'.,-__..-._ INSTALL _.-__-,.'\\\n";
    } else if(id=="browser"){
        cout<<"  {.} <::> BROWSER <::> {.}\n";
    } else if(id=="vireon"){
        cout<<" /\\/\\ VIREON  /\\/\\\n";
    } else {
        cout<<" ./.'; Vireon logo ;'\\. \n";
    }
    setColor(7);
}

/* ===============================
   Helper & IO Utilities
=============================== */
string promptLine(const string &prompt){
    cout<<prompt;
    string s;
    getline(cin, s);
    return s;
}

string toLowerStr(const string &s){
    string out = s;
    for(size_t i=0;i<out.size();i++) out[i] = (char)tolower((unsigned char)out[i]);
    return out;
}

void pressAnyKey(){
    cout<<"Press ENTER to continue...";
    string tmp; getline(cin,tmp);
}

/* ===============================
   BOOT
=============================== */
void boot(){
    bootTime = time(0);
    srand((unsigned)time(0));
    initFS();
    initProcesses();
    addLog("System booted");
    addLog("Kernel initialized");
}

/* ===============================
   Implementacja drawWSM (naprawa: brakujaca funkcja)
   prosta, laczy wsmApps i drawCommandsTable
=============================== */
void drawWSM(){
    asciiBorder("WSM PANEL - APPS & COMMANDS",60,10);
    // Left: apps, Right: commands (we'll reuse wsmApps and drawCommandsTable formatting)
    // For simplicity wyswietlimy najpierw aplikacje potem tabele komend
    wsmApps();
    drawCommandsTable();
}

/* ===============================
   drawDesktop implementacja (juz zadeklarowana)
=============================== */
void drawDesktop(){
    asciiBorder("VireonOS Desktop",72,11);
    cout<<"[ Desktop layout - Apps | Commands | Status ]\n";
    cout<<"  Apps: ";
    for(size_t i=0;i<installedPrograms.size();i++){
        cout<<installedPrograms[i];
        if(i+1 < installedPrograms.size()) cout<<" | ";
    }
    cout<<"\n  Status: User="<<CURRENT_USER<<" | Uptime="<<getUptime()<<" | Env="<<currentEnvironment<<"\n\n";
}

/* ===============================
   MAIN SHELL
=============================== */
int main(){
    boot();
    showLogo();

    installer();
    drawCommandsTable();

    string rawcmd;
    while(true){
        setColor(14);
        cout<<CURRENT_USER<<"@vireon> ";
        setColor(7);
        if(!getline(cin, rawcmd)) break;
        if(rawcmd.size()==0) continue;
        string cmd = toLowerStr(rawcmd);
        if(cmd=="help") { drawCommandsTable(); }
        else if(cmd=="ver"){ cout<<OS_NAME<<" | "<<KERNEL_VERSION<<"\n\n"; }
        else if(cmd=="whoami"){ cout<<CURRENT_USER<<"\n\n"; }
        else if(cmd=="uptime"){ cout<<getUptime()<<"\n\n"; }
        else if(cmd=="neofetch"){ extendedFastfetch(); }
        else if(cmd=="fastfetch"){ fastfetch(); }
        else if(cmd=="ls"){ ls(); }
        else if(cmd.substr(0,4)=="cat "){ string f = rawcmd.substr(4); cat(f); }
        else if(cmd.substr(0,6)=="touch "){ string f = rawcmd.substr(6); touch(f); }
        else if(cmd.substr(0,6)=="write "){ string f = rawcmd.substr(6); writeFile(f); }
        else if(cmd=="ps"){ htop(false); }
        else if(cmd=="htop"){ htop(true); }
        else if(cmd=="logs"){ showLogs(); }
        else if(cmd=="guess"){ guessGame(); }
        else if(cmd=="calculator"){ calculator(); }
        else if(cmd=="paint"){ paint(); }
        else if(cmd=="musicplayer"){ musicPlayer(); }
        else if(cmd=="notes"){ notesApp(); }
        else if(cmd=="installer"){ installer(); }
        else if(cmd=="envchange"){ changeEnvironment(); }
        else if(cmd=="wsm_apps"){ wsmApps(); }
        else if(cmd=="wsm_cmds"){ wsmCmds(); }
        else if(cmd=="browser"){ browserShell(); }
        else if(cmd=="youtube"){ /* built-in youtube demo */ 
            asciiBorder("YOUTUBE ASCII BETA",64,9);
            cout<<"Select demo video (1-5):\n1) Funny Cats\n2) Coding Tutorial\n3) VireonOS Demo\n4) ASCII Music\n5) Retro DOS\nChoose: ";
            int ch=0; cin>>ch; cin.ignore();
            if(ch < 1 || ch > 5){ cout<<"Invalid\n"; }
            else {
                loadingBar("Preparing video",24,13);
                cout<<"Playing video...\n";
                for(int f=0; f<15; f++){
                    cout<<"Frame "<<f+1<<": [";
                    for(int b=0;b<40;b++) cout<<((rand()%3==0)?'*':' ');
                    cout<<"]\r";
                    msleep(120);
                }
                cout<<"\nVideo ended.\n\n";
            }
        }
        else if(cmd=="wsm" || cmd=="wsmpanel"){ drawCommandsTable(); drawWSM(); }
        else if(cmd=="drawwsm"){ drawWSM(); }
        else if(cmd=="drawdesktop"){ drawDesktop(); }
        else if(cmd=="bookmarks"){ browserShowBookmarks(); }
        else if(cmd=="history"){ browserShowHistory(); }
        else if(cmd=="cls"){ system("cls"); }
        else if(cmd=="exit"){ cout<<"Shutting down "<<OS_NAME<<"...\n"; addLog("Shutdown requested"); break; }
        else {
            cout<<"Unknown command: "<<rawcmd<<"\nType 'help' for list of commands.\n\n";
        }
    }

    return 0;
}

/* ===============================
   Additional Auxiliary GUIs (WSM / Desktop)
=============================== */
void wsmApps(){
    asciiBorder("WSM - Applications",60,10);
    smallLogo("vireon");
    cout<<"Installed Applications:\n";
    if(installedPrograms.empty()) cout<<" (none)\n";
    for(size_t i=0;i<installedPrograms.size();i++){
        printf("  %2d) %s\n",(int)i+1, installedPrograms[i].c_str());
    }
    cout<<"\n";
}

void wsmCmds(){
    asciiBorder("WSM - Commands",60,14);
    drawCommandsTable();
}

/* ===============================
   small logos and art extras
   (JUZ JEDNA definicja smallLogo powyzej)
=============================== */

/* KONIEC PLIKU */
