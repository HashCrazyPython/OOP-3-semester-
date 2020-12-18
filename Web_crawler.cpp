#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <queue>
#include <set>
#include <thread>
#include <vector>
#include <atomic>
using namespace std;
// WARNING!!!
// before start program indicate path to database of visiting pages
const string input_directory = "Your input directory";
const string output_directory = "Your output directory";

class Web_Crawler {
    queue<string> active_pages;
    set<string> working_pages;
    set<string> database;
    mutex door;
    vector<thread> threads;
public:
    Web_Crawler(string& FirstFIle) {
        parse_string(FirstFIle);
    }

    void parse_string(string& temp) {
        string def;
        string address;
        string protocol;
        // we should split our strings to three and compare two of them with conditions
        // 1) check definition of link
        // There we compare def length with len("<a href=") as stop condition (according to task condition)
        auto t = temp.begin();
        for(; def.length() < 8; t++)
            def += *t;
        t++;
        // 2) check correctness of the protocol (must be equal "file://")
        for(; protocol.length() < 7; t++)
            protocol += *t;
        // 3) if all of those two string satisfy initial condition, then compose third string
        if(def + protocol == "<a href=file://") {
            for(; *t != '"'; t++)
                address += *t;
            lock_guard<mutex> lg(door);
            if(database.find(address) == database.end()) {
                active_pages.push(address);
                database.insert(address);
            }
        }
    }

    void analysis_file(string& FileName) {
        string s;
        ifstream inf(input_directory + FileName);
        // while we don't achieve end of file
        while(getline(inf, s)) {
            for(auto it = s.begin(); it != s.end(); it++) {
                if(*it == '<') {
                    string temp = "<";
                    it++;
                    while(*it != '>' && *it != '<' && it != s.end()) {
                        temp += *it;
                        it++;
                    }
                    if(*it == '>')
                        parse_string(temp);
                }
            }
        }
    }

    void download_file(string& FileName) {
        bool control = false;
        {
            lock_guard<mutex> lg(door);
            if(database.find(FileName) != database.end())
                control = true;
        }
        if(control) {
            ifstream inf(input_directory + FileName);
            if(inf.is_open()) {
                ofstream ouf(output_directory + FileName);
                ouf << inf.rdbuf();
                inf.close();
            }
        }
    }

    // secure getting file from queue
    string sec_get() {
        lock_guard<mutex> lg(door);
        if(!active_pages.empty()) {
            string temp = active_pages.front();
            working_pages.insert(temp);
            active_pages.pop();
            return temp;
        }
        return "";
    }

    void start_thread(int i) {
        while(true) {
            {
                lock_guard<mutex> lg(door);
                if(active_pages.empty() && working_pages.empty())
                    break;
            }
            auto it = sec_get();
            if(!it.empty()) {
                download_file(it);
                analysis_file(it);
                {
                    lock_guard<mutex> lg(door);
                    working_pages.erase(it);
                }
            }
        }
    }

    void begin_crawl(int number_threads) {
        auto begin = chrono::steady_clock::now();
        for(int i = 0; i< number_threads; i++)
            threads.emplace_back(&Web_Crawler::start_thread, ref(*this), i);
        for(auto & it : threads)
            it.join();
        auto end = chrono::steady_clock::now();
        auto diff = chrono::duration_cast<chrono::milliseconds>(end - begin);
        // results of work
        cout << database.size() << ' ' << diff.count() << endl;
    }
};

int main() {
    ifstream t(input_directory + "input.html");
    string first_page;
    int n;
    getline(t, first_page);
    t >> n;
    Web_Crawler temp(first_page);
    temp.begin_crawl(n);
    return 0;
}
