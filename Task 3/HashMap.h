

#ifndef HashMap_h
#define HashMap_h

using namespace std;

template<typename K, typename V>
struct Node
{
    Node<K,V>* next;
    pair<K,V> s;
    
    Node<K,V>(K& k, V& v)
    {
        s = std::make_pair(k,v);
        next = nullptr;
    }
    
};

template<typename K, typename V>
Node<K,V>* create(K& k, V& v)
{
    return new Node<K,V>(k,v);
}

template<typename K, typename V>
class LinkedList
{
    
    Node<K,V> *head;
    
public:
    LinkedList()
    {
        head = nullptr;
    }
    
    ~LinkedList()
    {
        delete_list();
    }
    
    bool isEmpty()
    {
        return head == nullptr;
    }
    
    void delete_list()
    {
        if(!isEmpty())
        {
            Node<K,V> *ptr = head;
            while(ptr->next != nullptr)
            {
                Node<K,V> *ptr1 = ptr;
                ptr = ptr->next;
                delete ptr1;
            }
        }
        head = nullptr;
    }
    
    LinkedList(K& k, V& v)
    {
        head = create(k,v);
    }
    
    
    void push(K& k, V& v)
    {
        Node<K,V>* s = head;
        head = create(k,v);
        head->next = s;
    }
    
    Node<K,V>* get_head()
    {
        return head;
    }
    
    class List_Iterator
    {
        friend class LinkedList<K,V>;
        
    public:
        Node<K,V> *curr;
        
        List_Iterator()
        {
            curr = nullptr;
        }
        
        List_Iterator(const List_Iterator &t)
        {
            this->curr = t.curr;
        }
        
        List_Iterator(LinkedList<K,V> &list)
        {
            curr = list.head;
        }
        
        List_Iterator& operator++(int)
        {
            if(curr == nullptr)
                curr = nullptr;
            else
                curr = curr->next;
            return *this;
        }
        
        pair<K,V>* operator->() const
        {
            return &((*curr).s);
        }
        
        pair<K,V>& operator*() const
        {
            return curr->s;
        }
        bool operator !=(List_Iterator other) const
        {
            return this->curr != other.curr;
        }
        
        bool operator ==(List_Iterator &other) const
        {
            return this->curr == other.curr;
        }
        
        bool isNullptr()
        {
            return curr == nullptr;
        }
    };
    
    List_Iterator begin()
    {
        List_Iterator it(*this);
        return it;
    }
    
    List_Iterator end()
    {
        List_Iterator it;
        return it;
    }
    
    Node<K,V>* searchByKey(K& k)
    {
        for(List_Iterator it = this->begin(); it != this->end(); it++)
        {
            if(it->first == k)
            {
                return it.curr;
            }
        }
        return nullptr;
    }
    
    void deleteByValue(V& v)
    {
        if(begin()->second == v)
        {
            Node<K,V> *ptr = head;
            head = head->next;
            delete ptr;
            return;
        }
        
        List_Iterator prev = begin();
        List_Iterator iter = begin();
        iter++;
        while(iter != end())
        {
            if(iter->second == v)
            {
                prev.curr->next = iter.curr->next;
                delete iter.curr;
                return;
            }
            iter++;
            prev++;
        }
        if(iter->second == v)
        {
            prev.curr->next = nullptr;
            delete iter.curr;
            return;
        }
    }
    
    void set_head(Node<K,V> *new_head)
    {
        head = new_head;
    }
};



template<typename K, typename V>
class HashMap
{
private:
    double max_occupancy;
    int capacity;
    int size;
    LinkedList<K,V>* arr;
    hash<K> hsh;
    
    void rehash()
    {
        HashMap<K,V>* newHashMap = new HashMap(size * 2, max_occupancy);
        for(auto iter = this->begin(); iter != this->end(); iter++)
        {
            newHashMap->insert(iter->first, iter->second);
        }
        auto temp = this->arr;
        this->arr = newHashMap->arr;
        newHashMap->arr = temp;
        
        this->size *= 2;
        newHashMap->size /= 2;
        delete newHashMap;
    }
    
    unsigned int division_hash(size_t key, int size)
    {
        return key % size;
    }
    
public:
    
    
    HashMap(int size, double max_occupancy = 0.75): size(size), max_occupancy(max_occupancy)
    {
        arr = new LinkedList<K,V> [size];
        
        for(int i = 0; i < size; i++)
            arr[i] = LinkedList<K,V>();
        capacity = 0;
    }
    
    ~HashMap()
    {
        for(int i = 0; i < size; i++)
            arr[i].delete_list();
        delete[] arr;
    }
    virtual void insert(K& k, V& v)
    {
        unsigned int key = division_hash(hsh(k), size);
        if(arr[key].searchByKey(k) != nullptr)
        {
            arr[key].searchByKey(k)->s.second = v;
            return;
        }
        arr[key].push(k,v);
        capacity++;
        if ((double) capacity / (double) size >= max_occupancy)
            rehash();
    }
    
    virtual void remove(K& k)
    {
        unsigned int key = division_hash(hsh(k), size);
        auto it = arr[key].begin();
        if(it.isNullptr())
            return;
        if((!it.isNullptr()) && (it->first == k))
        {
            arr[key].set_head(it.curr->next);
            delete (it.curr);
            it = arr[key].begin();
            capacity--;
        }
        auto prev = it;
        it++;
        while (!it.isNullptr())
        {
            if (it->first == k)
            {
                prev.curr->next = it.curr->next;
                auto ptr = it.curr;
                it++;
                prev = it;
                it++;
                delete ptr;
                capacity--;
            }
            else
            {
                it++;
                prev++;
            }
            
        }
    }
    
   Iterator getValueByKey(K &k)
    {
        unsigned int key = division_hash(hsh(k), size);
        Iterator it;
        if(arr[key].searchByKey(k) == nullptr)
            return it.end();
        V v = arr[key].searchByKey(k)->s.second
        for (auto it = this->begin(); it != this->end(); it++)
            if(v == it->second)
                return it;
        return it.end();
    }
    
    class Iterator {
    private:
        friend class HashMap<K, V>;
        LinkedList<K, V> *arr;
        typename LinkedList<K,V>::List_Iterator List_Iterator;
        unsigned data_id;
        unsigned data_size;
    public:
        
        
        pair<K, V>& operator*() {
            return (*List_Iterator);
        }
        
        pair<K, V>* operator->() {
            return &(*List_Iterator);
        }
        
        Iterator &operator++(int) {
            List_Iterator++;
            if (List_Iterator.isNullptr()) {
                data_id++;
                while (data_id < data_size && arr[data_id].isEmpty())
                    data_id++;
                if (data_id != data_size)
                    List_Iterator = arr[data_id].begin();
            }
            return *this;
        }
        
        bool operator!=(const Iterator &other) const
        {
            if (data_id != other.data_id)
                return true;
            return (this->List_Iterator != other.List_Iterator);
        }
        
        bool operator==(const Iterator &other) const
        {
            return (this->List_Iterator == other.List_Iterator);
        }
    };
    
    Iterator begin() {
        if (capacity == 0)
            return end();
        Iterator it;
        it.arr = arr;
        it.data_size = size;
        for (unsigned id = 0; id < size; id++) {
            if (!arr[id].isEmpty()) {
                it.data_id = id;
                it.List_Iterator = arr[id].begin();
                break;
            }
        }
        return it;
    }
    
    Iterator end() {
        Iterator it;
        it.arr = nullptr;
        it.data_size = size;
        it.data_id = size;
        return it;
    }
    
    size_t countValues() {
        return capacity;
    }
    
    size_t countUniqueValues() {
        if (countValues()==0)
            return 0;
        HashMap<V, V> map(size, max_occupancy);
        for (auto it = this->begin(); it != this->end(); it++) {
            map.insert(it->second, it->second);
        }
        return map.countValues();
    }
};


#endif /* HashMap_h */

