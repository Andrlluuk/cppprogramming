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
mutex checkmut;


vector<int> threadIndicator;


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


bool check(queue<string>& q)
{
    checkmut.lock();
    checkmut.unlock();
    return q.empty();
}


mutex threadlock;


bool threads_work()
{
    threadlock.lock();
    for(auto f: threadIndicator)
    {
        if(f)
        {
            threadlock.unlock();
            return true;
        }
    }
    threadlock.unlock();
    return false;
}


void RaW(queue<string>& q, unordered_set<string>& ind, int id)
{
    regex rx("<a href=\"file://.*?>");
    smatch m;
    string s;
    ifstream in;
    
    
    while(threads_work())
    {
        
        while(!check(q))
        {
            
            mut.lock();
            if(q.empty())
            {
                mut.unlock();
                break;
            }
            s = q.front();
                q.pop();
            mut.unlock();
            
            
            smatch t;
            regex rg("[0-9]*.html");
            auto res = regex_search(s, t, rg);
            string result = "";
            
            
            in.open("test_data/" + t.str());
            in >> result;
            in.close();
            
            
            string str = result;
            while(regex_search(str, m, rx))
            {
                mut.lock();
                if(ind.find(m[0]) == ind.end())
                {
                    q.push(m[0]);
                    ind.insert(m[0]);
                }
                mut.unlock();
                str = m.suffix().str();
            }
            
            
            ofstream out;
            out.open("Result_data/readed_" + t.str());
            out << result;
            
            
        }
        threadIndicator[id] = false;
    }
}


int main()
{
    
    unordered_set<string> ind;
    queue<string> q;
    string name_of_file;
    ifstream in;
    int x;
    
    
    in.open("input.txt");
    getline (in,name_of_file);
    in >> x;
    in.close();
    
    
    q.push("<a href=\"file://" + name_of_file + ">");
    ind.insert(name_of_file);
    
    
    vector<thread> threads;
    threads.resize(x);
    threadIndicator.resize(x+1);
    
    for(int i = 0; i < x; i++)
    {
        threads[i] = thread(RaW, std::ref(q), std::ref(ind), i);
        threadIndicator[i] = true;
    }
    
    threadIndicator[x] = true;
    auto start = std::chrono::system_clock::now();
    RaW(q, ind, x);
    
    
    for(int i = 0; i < x; i++)
    {
        threads[i].join();
    }
    
    
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    cout << elapsed_seconds.count() << std::endl;
    return 0;
}



//optimal solution: 9 threads with time of execution - 57.7 sec
//1-thread solution: ~155-160 sec

























