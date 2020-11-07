#include<iostream>
#include<string>
#include "HashMap.h"
#include<vector>
using namespace std;


template<typename K, typename V>
class MultiHashMap : public HashMap<K,V>
{
public:
    MultiHashMap(int size,double max_occupancy = 0.75): HashMap<K,V>(size, max_occupancy){};
    
    ~MultiHashMap()
    {}
    
    void insert(K& k, V& v)
    {
        unsigned int key = division_hash(this->hsh(k), this->size);
        this->arr[key].push(k,v);
        this->capacity++;
        if ((double) this->capacity / (double) this->size >= 0.75)
            this->rehash();
    }
    
    void remove(K& k)
    {
        unsigned int key = division_hash(this->hsh(k), this->size);
        auto it = this->arr[key].begin();
        if(it.isNullptr())
            return;
        while ((!it.isNullptr()) && ((*it).first == k))
        {
            this->arr[key].set_head(it.curr->next);
            delete (it.curr);
            it = this->arr[key].begin();
            this->capacity--;
        }
        auto prev = it;
        it++;
        while (!it.isNullptr())
        {
            if ((*it).first == k)
            {
                prev.curr->next = it.curr->next;
                auto ptr = it.curr;
                it++;
                prev = it;
                it++;
                delete ptr;
                this->capacity--;
            }
            else
            {
                it++;
                prev++;
            }
            
        }
    }
    
    int getNumberOfElements(const K &k) {
       unsigned int key = division_hash(this->hsh(k), this->size);
        int count = 0;
        for (auto it = this->arr[key].begin(); it != this->arr[key].end(); it++)
        {
            if ((*it).first == k)
                count++;
        }
        return count;
    }
    
    vector<V> getAllValuesByKey(const K &k)
    {
            unsigned int key = division_hash(this->hsh(k), this->size);
            int count = getNumberOfElements(k);
            vector<V> values(count);
            int id = 0;
            for (auto it = this->arr[key].begin(); it != this->arr[key].end(); it++)
                if ((*it).first == k)
                {
                    values[id] = (*it).second;
                    id++;
                }
            return values;
        }
};


template<typename Key, typename Value>
void Multibuild()
{
    int n;
    Key key;
    Value value;
    cin >> n;
    char c;
    MultiHashMap<Key, Value> hashtable(n, 0.75);
    while(n--)
    {
        cin >> c;
        if(c == 'A')
        {
            cin >> key >> value;
            hashtable.insert(key,value);
        }
        else
        {
            cin >> key;
            hashtable.remove(key);
        }
    }
    cout << hashtable.countValues() << " " << hashtable.countUniqueValues() << endl;
    cout << hashtable.getNumberOfElements(key) << endl;
    vector<Value> values = hashtable.getAllValuesByKey(key);
    int x = hashtable.getNumberOfElements(key);
    for(int i = 0; i < x;i++)
        cout << values[i] << " ";
}


int main()
{
    char k, v;
    cin >> k >> v;
    if((k == 'I') && (v == 'S'))
        Multibuild<int,string>();
    if((k == 'S') && (v == 'S'))
        Multibuild<string,string>();
    if((k == 'D') && (v == 'S'))
        Multibuild<double,string>();
    if((k == 'I') && (v == 'I'))
        Multibuild<int,int>();
    if((k == 'I') && (v == 'D'))
        Multibuild<int,double>();
    if((k == 'D') && (v == 'I'))
        Multibuild<double,int>();
    if((k == 'S') && (v == 'I'))
        Multibuild<string,int>();
    if((k == 'D') && (v == 'D'))
        Multibuild<double,double>();
    if((k == 'S') && (v == 'D'))
        Multibuild<string,double>();
    
    return 0;
}

