#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>
#include <unordered_set>
#include <queue>
#include <regex>
#include <chrono>
#include <vector>

using namespace std;

mutex mut;
mutex other_mut;
atomic<int> workers(0);

istream& operator >> (istream &in,  string &s)
{
    char ss[16];
    in.getline(ss, 16);
    s += ss;
    in.clear();
    while(strlen(ss))
    {
        in.getline(ss, 16);
        in.clear();
        s += ss;
    }
    return in;
}


void RaW(queue<string>& q, unordered_set<string>& ind, int id)
{
    regex rx("<a href=\"file://.*?>");
    smatch m, t;
    string s;
    ifstream in;
    
    
    while(true)
    {
        while (q.empty())
        {
            if (workers == 0)
                return;
        }
        unique_lock<mutex> lock1(mut);
        if(q.empty())
        {
            break;
        }
        workers++;
        s = q.front();
        q.pop();
        lock1.unlock();
        regex rg("[0-9]*.html");
        auto res = regex_search(s, t, rg);
        string result = "";

        in.open("test_data/" + t.str());
        in >> result;
        in.close();
        
        string str = result;
        while(regex_search(str, m, rx))
        {
            lock_guard<mutex> guard(other_mut);
            if(ind.find(m[0]) == ind.end())
            {
                q.push(m[0]);
                ind.insert(m[0]);
            }
            str = m.suffix().str();
        }
        
        ofstream out;
        out.open("Result_data/readed_" + t.str());
        out << result;
    
        workers--;
    }
}


int main()
{
    auto start = std::chrono::system_clock::now();
    
    unordered_set<string> ind;
    queue<string> q;
    string name_of_file;
    ifstream in;
    int x;
    vector<thread> threads;
    
    in.open("input.txt");
    getline (in,name_of_file);
    in >> x;
    in.close();
    
    q.push("<a href=\"file://" + name_of_file + ">");
    ind.insert(name_of_file);
    
    for(int i = 0; i < x; i++)
    {
        threads.emplace_back(RaW, std::ref(q), std::ref(ind), i);
    }
    
    for(int i = 0; i < x; i++)
    {
        threads[i].join();
    }
    
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    cout << elapsed_seconds.count() << std::endl;
    return 0;
}



//optimal solution: 14 threads with time of execution - 12.84 sec with output, 8.21 sec without output
//1-thread solution: ~155-160 sec
