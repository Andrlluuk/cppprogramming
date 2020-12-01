#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>
#include <set>
#include <queue>
#include <regex>
#include <chrono>
#include <vector>

static int sum = 0;
using namespace std;

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


int main()
{
    auto start = std::chrono::system_clock::now();
    set<string> ind;
    queue<string> q;
    string name_of_file;
    regex rx("<a href=\"file://.*?>");
    ifstream in;
    int x;
    in.open("input.txt");
    getline (in,name_of_file);
    in >> x;
    vector<int> locks(x);
    in.close();
    ofstream out;
    q.push("<a href=\"file://" + name_of_file + ">");
    while(!q.empty())
    {
        string result;
        string s = q.front();
        q.pop();
        if(!ind.count(s))
        {
            sum++;
            ind.insert(s);
            smatch t;
            regex rg("[0-9]*.html");
            auto res = regex_search(s, t, rg);
            in.open("test_data/" + t.str());
            out.open("Result_data/readed_" + t.str());
            in >> result;
            smatch m;
            string str = result;
            while(regex_search(str, m, rx))
            {
                for(int i = 0; i < m.size(); ++i)
                {
                    q.push(m[i]);
                }
                str = m.suffix().str();
            }
            out << result;
            in.close();
            out.close();
        }
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    cout << elapsed_seconds.count() << std::endl;
    return 0;
}
