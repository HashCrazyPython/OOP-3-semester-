#include <iostream>
#include <fstream>
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

int main()
{
    int n, k;
    inf >> n >> k;
    matrix K(n, k);  //initialization of matrix K
    //initialization of other matrices
    matrix A(n);
    A.input_matrix();

    matrix B(n);
    B.input_matrix();

    matrix C(n);
    C.input_matrix();

    matrix D(n);
    D.input_matrix();
    ((A + B*(~C) + K)*(~D)).output_matrix();
    return 0;
}
