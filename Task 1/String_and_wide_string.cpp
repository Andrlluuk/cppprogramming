#include<iostream>
#include<vector>
#include<cassert>
#include<cstdio>
#include<fstream>

using std::vector;

enum TypeString{S, WS};
class String
{
private:
    int size;
    char* str;
    TypeString type;
    
    //Memory allocation
    void allocation(int n)
    {
        size = n;
        str = new char[n];
    }
    
    char* get_string() const
    {
        return str;
    }
    //Private constructor
    String(int n)
    {
        allocation(n);
        type = S;
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
                if(*(str + i + j) == *(s.get_string() + j))
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
    String(TypeString t = S)
    {
          size = 0;
          str = new char [0];
          type = t;
    }
    //constructor from char
    
    String(char t, TypeString tt = S)
    {
           allocation(1);
           *str = t;
           type = tt;
    }
    
    //constructor from null-terminated string
    String(const char *t, TypeString tt = S)
    {
        allocation(strlen(t));
        for(int i = 0; i < size; i++)
            *(str + i) = *(t + i);
        type = tt;
    }
    //copy-constructor
    String(const String& s)
    {
        allocation(s.size);
        for(int i = 0; i < size; i++)
            *(str + i) = *(s.get_string() + i);
        type = s.type;
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
        int bytes = 0;
        int real_index = 0;
        while(bytes != idx + 1)
        {
            if(!((((str[real_index] >> 7)&1) == 1) && (((str[real_index] >> 6)&1) == 0)))
                bytes++;
            real_index++;
        }
        return *(str + real_index - 1);
    }
    
    char operator[](int idx) const
    {
        if((idx < 0) && (idx >= size))
            throw "Index out of bounds";
        int bytes = 0;
        int real_index = 0;
        while(bytes != idx + 1)
        {
            if(!((((str[real_index] >> 7)&1) == 1) && (((str[real_index] >> 6)&1) == 0)))
            {
                bytes++;
            }
            real_index++;
        }
        return *(str + real_index - 1);
    }
    
    int memory_position(int idx) const
    {
        if((idx < 0) && (idx >= size))
            throw "Index out of bounds";
        int bytes = 0;
        int real_index = 0;
        while(bytes != idx + 1)
        {
            if(!((((str[real_index] >> 7)&1) == 1) && (((str[real_index] >> 6)&1) == 0)))
            {
                bytes++;
            }
            real_index++;
        }
        return real_index - 1;
    }
    
    //overloaded +=
    String& operator +=(const String &s)
    {
        String s1(str);
        String s2(s);
        delete[] str;
        str = new char[size + s.size];
        for(int i = 0; i < size; i++)
            *(str + i) = *(s1.get_string() + i);
        for(int i = size; i < size + s.size; i++)
            *(str + i) = *(s2.get_string() + i - size);
        size += s.size;
        return *this;
    }
    
    //overloaded =
    String& operator =(const String &s)
    {
        delete[] str;
        str = new char[s.size + 1];
        for(int i = 0; i < s.size; i++)
            *(str + i) = *(s.get_string() + i);
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
            *(news.get_string() + i - begin) = *(str + i);
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
            *(newstr.get_string() + i) = *(str + i);
        }
        return newstr;
    }
    
    //string-length getter
    int length() const
    {
        int len = 0;
        for(int i = 0; i < size; i++)
        {
            if(!((((str[i] >> 7)&1) == 1) && (((str[i] >> 6)&1) == 0)))
                len++;
        }
        return len;
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
    
    void get(int x)
    {
        if((x < 0) || (x >= size))
           throw "Tried to get symbol in uncorrect position";
        int i = memory_position(x);
        int j = memory_position(x + 1);
        char *result = (char*)malloc(sizeof(char)*(j-i));
        for(int k = i; k < j; k++)
            *(result + k - i) = *(str + k);
        std::cout << result;
    }
};

std::ostream& operator << (std::ostream &out, const String &s)
{
    for(int i = 0; i < s.size; i++)
        out << *(s.get_string() + i);
    return out;
}
std::istream& operator >> (std::istream &in,  String &s)
{
    char ss[16];
    in.clear();
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

TypeString get_type_of_string()
{
    char t;
    std::cout << "Print the type of string which you'll be working";
    std::cin >> t;
    switch(t)
    {
    case 'w':
        return WS;
            
    case 's':
        return S;
            
    default:
        {
            std::cout << "Incorrect Type ";
            return get_type_of_string();
        }
    }
}

int main()
{
    using std::cin;
    using std::cout;
    using std::endl;
    TypeString t = get_type_of_string();
    String s1(t);
    String s2(t);
    String s3(t);
    fflush(stdin);
    try
    {
        cin >> s1;
        cin >> s2;
        cin >> s3;
        String s4 = substitution(s1,s2,s3);
        //cout << s4;
        s2.get(2);
        cout << s2.length();
        //cout << s2[4];
    }
    catch(const char* exception)
    {
        cout << "Error : " << exception << endl;
    }
    return 0;
}
