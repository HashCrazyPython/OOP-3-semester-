#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <math.h>
using namespace std;
ifstream inf("input.txt");
ofstream ouf("output.txt");

class matrix {
    int** arr;
    int trans;
    int size;
public:

    matrix()
    {
        this->arr = NULL;
        this->trans = 0;
        this->size = 0;
    }

    matrix(int n, int k = 1)
    {
        this->size = n;
        this->trans = 0;
        this->arr = new int*[n];
        for(int i = 0; i<n; i++)
        {
            this->arr[i] = new int[n]();
            this->arr[i][i] = k;
        }
    }

    matrix(int n, int* temp)
    {
        this->size = n;
        this->trans = 0;
        this->arr = new int*[n];
        for(int i = 0; i<n; i++)
        {
            this->arr[i] = new int[n]();
            this->arr[i][i] = temp[i];
        }
    }

    ~matrix()
    {
        for(int i = 0; i<this->size; i++)
            delete[] this->arr[i];
        delete[] this->arr;
    }

    matrix* operator=(const matrix& A)
    {
         if (*this != A)
         {
            for(int i = 0; i<this->size; i++)  // cleaning previous used memory
                delete[] this->arr[i];
            delete[] this->arr;

            this->trans = A.trans;
            this->size = A.size;
            this->arr = new int*[A.size];      // rewriting data
            for(int i = 0; i < A.size; i++)
            {
                this->arr[i] = new int[A.size];
                for(int j = 0; j < A.size; j++)
                    this->arr[i][j] = A.arr[i][j];
             }
            if(A.trans == 1)
            {
                this->trans = 0;
                ~(*this);
            }
         }
         return this;
    }

    matrix(const matrix &A) // copy constructor
    {
        this->arr = NULL;
        this->size = 0;
        this->trans = 0;
        *this = A;
    }

    matrix operator+(const matrix& A) const
    {
        if (this->size != A.size)
        {
            cerr << "Error. You've tried to work with not equal matrix sizes (addition)" << endl;\
            exit(1);
        }
        else
        {
            matrix temp(*this);
            for (int i = 0; i<A.size; i++)
                for(int j = 0; j<A.size; j++)
                    if (temp.trans == A.trans)
                        temp.arr[i][j] += A.get_elem(i, j);
                    else
                        temp.arr[i][j] += A.get_elem(j,i);
            return temp;
       }
    }

    matrix* operator+=(const matrix& A)
    {
        if (this->size != A.size)
            cerr << "Error. You've tried to work with not equal matrix sizes (self addition)" << endl;
        else
            for (int i = 0; i<A.size; i++)
                for(int j = 0; j<A.size; j++)
                    if (this->trans == A.trans)
                        this->arr[i][j] += A.get_elem(i, j);
                    else
                        this->arr[i][j] += A.get_elem(j,i);
        return this;
    }

    matrix operator-(const matrix& A) const
    {
        if (this->size != A.size)
        {
            cerr << "Error. You've tried to work with not equal matrix sizes (subtraction)" << endl;
            exit(0);
        }
        else
        {
            matrix temp(*this);
            for (int i = 0; i<A.size; i++)
                for(int j = 0; j<A.size; j++)
                    if (temp.trans == A.trans)
                        temp.arr[i][j] -= A.get_elem(i, j);
                    else
                        temp.arr[i][j] -= A.get_elem(j,i);
            return temp;
       }
    }

    matrix* operator-=(const matrix& A)
    {
        if (this->size != A.size)
        {
            cerr << "Error. You've tried to work with not equal matrix sizes (self subtraction)";
            exit(1);
        }
        else
            for (int i = 0; i<A.size; i++)
                for(int j = 0; j<A.size; j++)
                    if (this->trans == A.trans)
                        this->arr[i][j] -= A.get_elem(i, j);
                    else
                        if (this->trans == 1)
                            this->arr[i][j] = this->arr[j][i] - A.get_elem(i,j);
                        else
                            this->arr[i][j] -= A.get_elem(j,i);

        if (!(A.trans == 1 && this->trans == 1))
            this->trans = 0;
        return this;
    }

    matrix operator*(const matrix& A) const
    {
        if (this->size != A.size)
        {
            cerr << "Error. You've tried to work with not equal matrix sizes (multiplication)" << endl;
            exit(1);
        }
        else
        {
            matrix out(*this);
            for(int i = 0; i<A.size; i++)
            {
                int *temp = new int[A.size]();
                for(int j = 0; j<A.size; j++)
                {
                    for(int k = 0; k <A.size; k++)
                    {
                        if (out.trans == A.trans)
                            temp[j] += this->arr[i][k]*(A.get_elem(k, j));
                        else
                            temp[j] += this->arr[i][k]*(A.get_elem(j, k));
                    }
                }
                //write result to received matrix string
                int* t = out.arr[i];
                out.arr[i] = temp;
                delete[] t;
            }
            return out;
        }
    }

    matrix* operator*=(const matrix& A)
    {
        if (this->size != A.size)
            cerr << "Error. You've tried to work with not equal matrix sizes (self multiplication)" << endl;
        else
        {
            if (this->trans == 1)
            {
                *this = ~(*this);
                this->trans = 0;
            }

            for(int i = 0; i<A.size; i++)
            {
                int *temp = new int[A.size]();
                for(int j = 0; j<A.size; j++)
                {
                    for(int k = 0; k <A.size; k++)
                    {
                        if (this->trans == A.trans)
                            temp[j] += this->arr[i][k]*(A.get_elem(k, j));
                        else
                            temp[j] += this->arr[i][k]*(A.get_elem(j, k));
                    }
                }
                //write result to received matrix string and cleanitng memory
                int* t = this->arr[i];
                this->arr[i] = temp;
                delete[] t;
            }
        }
        return this;
    }

    //operator for transposing matrix
    matrix operator~ ()
    {
        for(int i = 0; i<this->size-1; i++)
            for(int j = i + 1; j<this->size; j++)
            {
                int temp = this->arr[j][i];
                this->arr[j][i] = this->arr[i][j];
                this->arr[i][j] = temp;
            }
        return *this;
    }

    bool operator== (const matrix& A) const
    {
        if (this->size != A.size)
            return false;
        else
        {
            for(int i = 0; i < A.size; i++)
                for(int j = 0; j < A.size; j++)
                    if (this->trans == A.trans)
                    {
                        if (this->arr[i][j] != A.arr[i][j])
                            return false;
                    }
                    else
                        if (this->arr[i][j] != A.arr[j][i])
                            return false;
            return true;
        }
    }

    bool operator!= (const matrix& A) const
    {
        return !(*this == A);
    }

    matrix operator() (int s, int k)
    {
        if (this->trans == 1)
        {
            this->trans = 0;
            *this = ~(*this);
        }

        matrix temp(this->size-1);
            for(int i = 0; i<this->size-1; i++)
            {
                for(int j = 0; j<this->size-1; j++)
                {
                    if (i < s-1)
                    {
                        if (j < k-1)
                            temp.arr[i][j] = this->arr[i][j];
                        if (j >= k-1)
                            temp.arr[i][j] = this->arr[i][j+1];
                    }
                    else
                    {
                        if (j < k-1)
                            temp.arr[i][j] = this->arr[i+1][j];
                        if (j >= k-1)
                            temp.arr[i][j] = this->arr[i+1][j+1];
                    }
                }
            }
       return temp;
    }

    int* operator[] (int k)
    {
        if (this->trans == 0)
            return this->arr[k];
        else
        {
            ~(*this);
            this->trans = 0;
            return this->arr[k];
        }
    }

    int* operator() (int k)
    {
        if (this->trans == 1)
            return this->arr[k];
        else
        {
            ~(*this);
            this->trans = 1;
            return this->arr[k];
        }
    }

    int get_elem(int i, int j) const
    {
        return this->arr[i][j];
    }

    int get_size() const
    {
        return this->size;
    }

    int get_trans() const
    {
        return this->trans;
    }

    void input_matrix()
    {
        for(int i = 0; i< this->size; i++)
            for(int j = 0; j<this->size; j++)
                inf >> this->arr[i][j];
    }

    void output_matrix()
    {
        for(int i = 0; i< this->size; i++)
        {
            for(int k = 0; k<this->size; k++)
                if (this->trans == 0)
                    ouf << this->arr[i][k]<< " ";
                else
                    ouf << this->arr[k][i]<< " ";
            ouf << endl;
        }
    }

};


// version of hash for matrices
template<>
struct std::hash<matrix>
{
    size_t operator() (matrix& A) const noexcept
    {
        size_t val = 0;
        for(int i = 0; i<A.get_size(); i++)
            val += (i+1)*(i+2)*(abs(A[i][i]) + 1 + i);
        return val;
    }
};

template <typename K, typename V>
class Hashmap {
protected:
    struct Pair {
        K key;
        V val;
        Pair* next = nullptr;

        Pair(K key, V val) {
            this->key = key;
            this->val = val;
        }
    };

    struct my_list {
        Pair* head = nullptr;
        Pair* tail = nullptr;

        my_list(Pair* temp) {
            head = temp;
            tail = head;
        }

        my_list(K key, V val) {
            head = new Pair(key, val);
            tail = head;
        }

        void clean(const my_list& temp) {
            Pair* buff = temp.head;
            Pair* prev = temp.head;
            while(buff != nullptr) {
                prev = buff;
                buff = buff->next;
                delete prev;
            }
        }

        my_list(const my_list& temp) {
            *this = temp;
        }

        my_list& operator=(const my_list& temp) {
            clean(*this);
            head = nullptr;
            tail = nullptr;
            Pair* buff = temp.head;
            while(buff != nullptr) {
                this->append(buff->key, buff->val);
                buff = buff->next;
            }
            return *this;
        }

        void append(K key, V val) {
            if(tail != nullptr) {
                tail->next = new Pair(key, val);
                tail = tail->next;
            }
            else {
                head = new Pair(key, val);
                tail = head;
            }
        }

        bool pop(K key) {
            if (head != nullptr) {
                Pair* buff = head;
                Pair* prev = buff;
                while(buff != nullptr) {
                    if(buff->key == key) {
                        if(head == buff) {
                            head = head->next;
                            delete buff;
                            return head == nullptr;
                        }
                        else {
                            prev->next = buff->next;
                            delete buff;
                            if(prev->next == nullptr)
                                tail = prev;
                            return false;
                        }
                    }
                    prev = buff;
                    buff = buff->next;
                }
            }
            return true;
        }

        Pair* find_key(K key) {
            Pair* buff = head;
            while(buff != nullptr) {
                if (buff->key == key)
                    return buff;
                buff = buff->next;
            }
            return nullptr;
        }

        void change_value(K key, V val) {
            Pair* buff = find_key(key);
            if(buff != nullptr)
                buff->val = val;
            else
                throw invalid_argument("There isn't that key in the HashMap");
        }

       ~my_list() { clean(*this); }
    };

    // clean array of my_list pointers
    void raw_clean() {
        for(int i = 0; i<size; i++)
            delete  items[i];
        delete[] items;
    }

    void raw_copy(const Hashmap& temp) {
        size = temp.size;
        quant = temp.quant;
        items = new my_list*[temp.size]();
        // copying array of my_list pointers
        for(int i = 0; i < size; i++)
            if(temp[i] != nullptr) {
                Pair* buff = temp[i]->head;
                if(items[i] == nullptr)
                    items[i] = new my_list(buff->key, buff->val);
                buff = buff->next;
                while(buff != nullptr) {
                    items[i]->append(buff->key, buff->val);
                    buff = buff->next;
                }
            }
    }

    my_list** items = nullptr;
    hash<K> hasher;
    int quant = 0;
    int size = 0;
public:
    class iterator {
        Pair* item = nullptr;
        int index = 0;
        Hashmap<K,V>* ptr = nullptr;
    public:
        explicit iterator(Hashmap<K,V>& temp) {
            ptr = &temp;
            (*this)++;
        }

        explicit iterator(Hashmap<K,V>* temp, Pair*it = nullptr, int buff = 0) {
            item = it;
            index = buff;
            ptr = temp;
            (*this)++;
        }

        V& operator*() {
            if(item != nullptr)
                return item->val;
            throw runtime_error("Attempting to dereference a null pointer");
        }

        iterator& operator++(int) {
            if (ptr == nullptr && item == nullptr)
                return *this;
            if(item == nullptr || (item->next == nullptr && index < ptr->size)) {
                if(item != nullptr)
                    index++;
                for(int i = index; i < ptr->size; i++)
                    if (ptr->items[i] == nullptr)
                        index++;
                    else {
                        item = ptr->items[i]->head;
                        return *this;
                    }
            }
            else if (index < ptr->size)
                item = item->next;

            // creating end iterator
            if(index >= ptr->size) {
                item = nullptr;
                ptr = nullptr;
                index = -1;
            }
            return *this;
        }

        bool operator!=(typename Hashmap<K,V>::iterator& temp) {
            return !(ptr == temp.ptr && item == temp.item);
        }

        K get_key() {
            if(item != nullptr)
                return item->key;
            throw runtime_error("Attempting to dereference a null pointer");
        }

        V get_val() {
            if(item != nullptr)
                return item->val;
            throw runtime_error("Attempting to dereference a null pointer");
        }
    };
    iterator end;

    explicit Hashmap(int n) : end(nullptr, nullptr, -1) {
        items = new my_list*[n]();
        size = n;
    }

    Hashmap& operator=(const Hashmap<K,V>& temp) {
        raw_clean();
        raw_copy(temp);
        return *this;
    }

    // copy constructor
    Hashmap(const Hashmap<K,V>& temp) : end(nullptr, nullptr, -1) {
        raw_copy(temp);
    }

    // insert element into hash map
    virtual void Push(K key, V val) {
        size_t index = hasher(key) % size;
        if(items[index] == nullptr) {
            items[index] = new my_list(key, val);
            quant++;
        }
        else {
            Pair* buff = items[index]->find_key(key);
            if(buff != nullptr)
                items[index]->change_value(key, val);
            else {
                quant++;
                items[index]->append(key, val);
            }
        }
         if (float(quant)/size > 0.75)
            reallocate();
    }

    // remove pair from hash map
    virtual void Remove(K key) {
        size_t index = hasher(key) % size;
        if(items[index] != nullptr)
            if(items[index]->find_key(key)) {
                quant--;
                bool control = items[index]->pop(key);
                if(control == true) {
                    delete items[index];
                    items[index] = nullptr;
                }
            }
    }

    // searching value according to key
    virtual V& operator[](const K key) {
        size_t index = hasher(key) % size;
        Pair* buff = items[index]->find_key(key);
        if(buff != nullptr)
            return buff->val;
        throw runtime_error("Attempting to dereference a null pointer");
    }

    int get_size() const {return size;}
    int get_quant() const {return quant;}

    void reallocate() {
        auto** fresh = new my_list*[size*2]();;
        for(iterator it(this); it != end; it++) {
            int index = hasher(it.get_key()) % (size*2);
            if (fresh[index] == nullptr)
                fresh[index] = new my_list(it.get_key(), it.get_val());
            else
                fresh[index]->append(it.get_key(), it.get_val());
        }
        raw_clean();
        size*= 2;
        items = fresh;
    }

    ~Hashmap() {
        raw_clean();
    }
};

template <typename K, typename V>
class MultiHashMap : public Hashmap<K,V> {
public:
    explicit MultiHashMap(int n) : Hashmap<K,V>(n) {}

    void Push(K key, V val) override {
        size_t index = this->hasher(key) % this->size;
        if(this->items[index] == nullptr) {
            this->items[index] = new typename Hashmap<K,V>::my_list(key, val);
            this->quant++;
        }
        else {
            this->quant++;
            this->items[index]->append(key, val);
        }
        if (float(this->quant)/this->size > 0.75)
            this->reallocate();
    }

    // this function is using for going through the hash map and for:
    // 1) composing set of values according to specific key
    set<V*> go_through_MultyHashMap(K key) {
        size_t index = this->hasher(key) % this->size;
        set<V*> result;
        auto buff = this->items[index]->head;
        while(buff != nullptr) {
            if(buff->key == key)
                result.insert(&buff->val);
            buff = buff->next;
        }
        return result;
    }

    V& operator[](const K key) override {
        set<V*> temp = go_through_MultyHashMap(key);
        if(!temp.empty()) {
            auto it = temp.begin();
            return **it;
        }
        throw invalid_argument("There isn't that key in MultyHashMap");
    }

    set<V*> all_elements(K key) {
        set<V*> temp = go_through_MultyHashMap(key);
        if(!temp.empty())
            return temp;
        throw invalid_argument("There isn't that key in MultyHashMap");
    }

    int compute_quantity(K key) {return all_elements(key).size();}

    void Remove(K key) override {
        size_t index = this->hasher(key) % this->size;
        if(this->items[index] != nullptr) {
            int count = compute_quantity(key);
            while(count-- > 0 && this->items[index] != nullptr) {
                this->quant--;
                bool control = this->items[index]->pop(key);
                if (control) {
                    delete this->items[index];
                    this->items[index] = nullptr;
                }
            }
        }
    }
};


template<typename K, typename V>
void set_map(int n) {
    Hashmap<K,V> map(n);
    // Hash map initialization
    int t = map.get_size();
    while(t-- > 0) {
        char buff;
        inf >> buff;
        if(buff == 'A') {
            K buff_k;
            V buff_v;
            inf >> buff_k;
            inf >> buff_v;
            map.Push(buff_k, buff_v);
        }
        else if(buff == 'R') {
            K buff_k;
            inf >> buff_k;
            map.Remove(buff_k);
        }
    }
    // Hash map otput data
    ouf << map.get_quant() << ' ';
    V* arr = new V[map.get_size()]();
    int count = 0;
    for(typename Hashmap<K,V>::iterator it(map); it != map.end; it++) {
        V value = *it;
        if(count == 0)
            arr[count++] = value;
        else {
            bool control = false;
            for(int i = 0; i<count; i++)
                if(arr[i] == value) {
                    control = true;
                    break;
                }
            if(control == false)
                arr[count++] = value;
        }
    }
    ouf << count << endl;
    delete[] arr;
}

template <typename K>
void set_value(char buff, int size) {
    if(buff == 'I')
        set_map<K,int>(size);
    else if (buff == 'D')
        set_map<K,double>(size);
    else if (buff == 'S')
        set_map<K,string>(size);
}

int main() {
    char k, v;
    int n;
    inf >> k >> v >> n;
    if(k == 'I')
        set_value<int>(v, n);
    else if(k == 'D')
        set_value<double>(v, n);
    else if(k == 'S')
        set_value<string>(v, n);
     return 0;
}

