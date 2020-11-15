#include<iostream>
#include<string>
#include<vector>
#include<cassert>
#include<cstdio>
#include<fstream>
#include "String_class.h"
#include "HashMap.h"



template<typename Key,typename Value>
void build()
{
    int n;
    Key key;
    Value value;
    cin >> n;
    char c;
    
    HashMap<Key, Value> hashtable(n, 0.75);
    while(n--)
    {
        cin >> c;
        if(c == 'A')
        {
            cin >> key;
            cin >> value;
            hashtable.insert(key,value);
        }
        else
        {
            cin >> key;
            hashtable.remove(key);
        }
    }
    try
    {
        hashtable.getValueByKey(key);
    }
    catch(const char *ex)
    {
        cout << ex << endl;;
    }
    cout << hashtable.countValues() << " " << hashtable.countUniqueValues() << endl;
}

template<typename Key>
void get_value()
{
    char v;
    cin >> v;
    if(v == 'I')
        build<Key,int>();
    if(v == 'S')
        build<Key,string>();
    if(v == 'D')
        build<Key,double>();
}

int main()
{
    char k;
    cin >> k;
    if(k == 'I')
        get_value<int>();
    if(k == 'S')
        get_value<string>();
    if(k == 'D')
        get_value<double>();
    if(k == 'O')
        get_value<String>();
    return 0;
    
}
