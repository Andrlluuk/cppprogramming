#include<iostream>
#include<vector>
#include<cassert>
#include<cmath>
using std::vector;
class String
{
private:
    int size;
    int capacity;
public:
    char* str;
public:
    friend std::ostream & operator << (std::ostream &out, const String &s);
     
    friend std::istream & operator >> (std::istream &in,  String &s);
    
    //default constructor
    String()
    {
        size = 0;
        str = new char [1];
        capacity = 1;
        str[0] = '\0';
    }
    
    //constructor from char
    String(char t)
    {
        str = new char [2];
        str[0] = t;
        str[1] = '\0';
        capacity = 2;
        size = 1;
    }
    
    //constructor from null-terminated string
    String(const char *t)
    {
        size =(int) strlen(t);
        capacity = ceil((double)log10(size+1)/log10(2));
        str = new char [capacity];
        for(int i = 0; i < size; i++)
            str[i] = t[i];
        str[size] = '\0';
    }
    
    //copy-constructor
    String(const String& s)
    {
        capacity = ceil((double)log10(s.size+1)/log10(2));
        str = new char [capacity];
        size = s.size;
        for(int i = 0; i < size; i++)
            str[i] = s[i];
        str[size] = '\0';
    }
    
    //destructor
    ~String()
    {
        delete[] str;
        size = 0;
        capacity = 0;
    }
    
    //overloaded []
    char operator[](int idx) const
    {
        assert(idx < size);
        return *(str + idx);
    }
    
    //overloaded +=
    String& operator +=(const String &s)
    {
        if(size + s.size + 1 < capacity)
        {
            for(int i = size; i < size + s.size; i++)
            {
                str[i] = s.str[i];
            }
            str[size + s.size] = '\0';
        }
        else
        {
        String s1(str);
        String s2(s);
        delete[] str;
        capacity = ceil((double)log10(size+ s.size + 1)/log10(2));
        str = new char[capacity];
        for(int i = 0; i < size; i++)
            str[i] = s1[i];
        for(int i = size; i < size + s.size; i++)
            str[i] = s2[i-size];
        size += s.size;
        str[size] = '\0';
        }
        return *this;
    }
    
    //overloaded =
    String& operator =(const String &s)
    {
        if(s.size + 1 < capacity)
            for(int i = 0; i < s.size; i++)
                str[i] = s[i];
        else
        {
        delete[] str;
        capacity = ceil((double)log10(s.size + 1)/log10(2));
        str = new char[capacity];
        for(int i = 0; i < s.size; i++)
            str[i] = s[i];
        }
        str[s.size] = '\0';
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
        assert((begin >=0) && (end <= size) && (begin <= end));
        char* part_of_string = new char [end - begin + 1];
        for(int i = begin; i < end; i++)
            part_of_string[i - begin] = str[i];
        part_of_string[end - begin] = '\0';
        String news(part_of_string);
        delete[] part_of_string;
        return news;
    }
    
    //overloaded *(to duplication)
    String operator*(int x)
    {
        assert(x >= 0);
        String news(str);
        if(x == 0)
        {
           String news;
           return news;
        }
        for(int i = 0; i < x - 1; i++)
            news += str;
        return news;
    }
    vector<int> entry(const String &s)
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
    //overloaded - (to delete all substrings)
    String operator-(const String &s)
    {
        vector<int> indexes = entry(s);
        int n = size;
        int count = 0;
        int m = s.size;
        for(auto x: indexes)
            for(int i = x; i < x+m; i++)
                str[i] = '\0';
        for(int i = 0; i < n; i++)
            if(str[i] != '\0')
                count++;
        char* ss = new char[count];
        int counter = 0;
        for(int i = 0; i < n; i++)
        if(str[i] != '\0')
        {
            ss[counter] = str[i];
            counter++;
        }
        String newstr(ss);
        *this = newstr;
        delete[] ss;
        return newstr;
    }
    
    //string-length getter
    int length() const
    {
        return size;
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
    char ss[250];
    in.getline(ss, sizeof(ss));
    s = ss;
    return in;
}
String substitution(String& s1, const String &s2, const String &s3)
{
    vector<int> idxs = s1.entry(s2);
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
    String s1;
    String s2;
    String s3;
    cin >> s1;
    cin >> s2;
    cin >> s3;
    String s4 = substitution(s1,s2,s3);
    cout << s4;
}
 
