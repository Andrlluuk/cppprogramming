#include<iostream>
#include<vector>
#include<cassert>
#include<cstdio>
#include<fstream>
using std::vector;
class String
{
private:
    int size;
    char* str;
    
    //Memory allocation
    void allocation(int n)
    {
        size = n;
        str = new char[n];
    }
    
    //Private constructor
    String(int n)
    {
        allocation(n);
    }
    
    vector<int> find_substrings(const String &s)
    {
        vector<int> indexes;
        int n = size;
        int count = 0;
        int m = s.size;
        for(int i = 0; i <= n - m ;i++)
        {
            for(int j = 0; j < m; j++)
            {
                if(str[i+j] == s.str[j])
                    count++;
                else
                    break;
            }
            if(count == s.size)
            {
                indexes.push_back(i);
                i += s.size - 1;
            }
            count = 0;
        }
        return indexes;
    }
    
public:
    friend std::ostream & operator << (std::ostream &out, const String &s);
    
    friend std::istream & operator >> (std::istream &in,  String &s);
    
    friend String substitution(String& s1, const String &s2, const String &s3);
    
    //default constructor
    String()
    {
        size = 0;
        str = new char [0];
    }
    
    //constructor from char
    String(char t)
    {
        allocation(1);
        str[0] = t;
    }
    
    //constructor from null-terminated string
    String(const char *t)
    {
        allocation(strlen(t));
        for(int i = 0; i < size; i++)
            str[i] = t[i];
    }
    
    //copy-constructor
    String(const String& s)
    {
        allocation(s.size);
        for(int i = 0; i < size; i++)
            str[i] = s[i];
    }
    
    //destructor
    ~String()
    {
        delete[] str;
    }
    
    //overloaded []
    char operator[](int idx)
    {
        if((idx < 0) && (idx >= size))
            throw "Index out of bounds";
        return *(str + idx);
    }
    
    char operator[](int idx) const
    {
        if((idx < 0) && (idx >= size))
            throw "Index out of bounds";
        return *(str + idx);
    }
    
    //overloaded +=
    String& operator +=(const String &s)
    {
        String s1(str);
        String s2(s);
        delete[] str;
        str = new char[size + s.size];
        for(int i = 0; i < size; i++)
            str[i] = s1[i];
        for(int i = size; i < size + s.size; i++)
            str[i] = s2[i-size];
        size += s.size;
        return *this;
    }
    
    //overloaded =
    String& operator =(const String &s)
    {
        delete[] str;
        str = new char[s.size + 1];
        for(int i = 0; i < s.size; i++)
            str[i] = s[i];
        size = s.size;
        return *this;
    }
    
    //overloaded + for strings
    String operator +(const String &s)
    {
        String ss = *this;
        return ss += s;
    }
    
    //overloaded + for constant strings
    String operator +(const char *s)
    {
        String ss = *this;
        String ss2(s);
        return ss += ss2;
    }
    
    //overloaded () (to get substring)
    String operator()(int begin, int end) const
    {
        if((begin < 0) || (end > size) || (begin > end))
            throw "Invalid substring request";
        String news(end - begin);
        for(int i = begin; i < end; i++)
            news.str[i - begin] = str[i];
        return news;
    }
    
    //overloaded *(to duplication)
    String operator*(int x)
    {
        if(x < 0)
            throw "Can't multiply string by negative number";
        String news;
        for(int i = 0; i < x - 1; i++)
            news += str;
        return news;
    }
    
    
    //overloaded - (to delete all substrings)
    String operator-(const String &s)
    {
        vector<int> indexes = find_substrings(s);
        int n = size;
        int count = 0;
        int idx = 0;
        int m = s.size;
        for(int i = 0; i < n; i++)
        {
            if(i == indexes[idx])
            {
                i += m - 1;
                idx++;
                while(i < indexes[idx])
                    idx++;
            }
            else
                count++;
        }
        String newstr(count);
        int counter = 0;
        idx = 0;
        for(int i = 0; i < n; i++)
        {
            if(i == indexes[idx])
            {
                i += m;
                idx++;
                while(i < indexes[idx])
                    idx++;
            }
            newstr.str[i] = str[i];
        }
        return newstr;
    }
    
    //string-length getter
    int length() const
    {
        return size;
    }
    
    //Substitution substring by other string
    String substitution(String& s1, const String &s2, const String &s3)
    {
        vector<int> idxs = s1.find_substrings(s2);
        if(idxs.size())
        {
            String final_string = s1(0, idxs[0]);
            for(int i = 0; i < idxs.size() - 1; i++)
            {
                final_string += s3;
                final_string += s1(idxs[i] + s2.length(), idxs[i + 1]);
                
            }
            final_string += s3;
            
            final_string += s1(idxs[idxs.size() - 1] + s2.length(), s1.length());
            return final_string;
        }
        else
            return s1;
    }
};

std::ostream& operator << (std::ostream &out, const String &s)
{
    for(int i = 0; i < s.size; i++)
        out << s[i];
    return out;
}
std::istream& operator >> (std::istream &in,  String &s)
{
    char ss[16];
    in.getline(ss, 16);
    s += ss;
    in.clear();
    while(strlen(ss) == 15)
    {
        in.getline(ss, 16);
        in.clear();
        s += ss;
    }
    return in;
}

String substitution(String& s1, const String &s2, const String &s3)
{
    vector<int> idxs = s1.find_substrings(s2);
    if(idxs.size())
    {
        String final_string = s1(0, idxs[0]);
        for(int i = 0; i < idxs.size() - 1; i++)
        {
            final_string += s3;
            final_string += s1(idxs[i] + s2.length(), idxs[i + 1]);
            
        }
        final_string += s3;
        
        final_string += s1(idxs[idxs.size() - 1] + s2.length(), s1.length());
        return final_string;
    }
    else
        return s1;
}

int main()
{
    using std::cin;
    using std::cout;
    using std::endl;
    String s1;
    String s2;
    String s3;
    try
    {
        cin >> s1;
        cin >> s2;
        cin >> s3;
        String s4 = substitution(s1,s2,s3);
        cout << s4;
    }
    catch(const char* exception)
    {
        cout << "Error : " << exception << endl;
    }
    return 0;
}
