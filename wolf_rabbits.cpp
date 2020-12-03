#include <iostream>
#include <fstream>
#include <list>
using namespace std;
ifstream inf("input.txt");
ofstream ouf("output.txt");

class Animals
{
protected:
    int x, y;
    int age = 0;
    char specie = 'u'; // separate wolfs from rabbits
    int dir;
    int state;
public:
    Animals()
    {
        this->x = 0;
        this->y = 0;
        this->dir = 0;
        this->state = 0;
    }

    Animals(int x, int y, int dir, int state, char specie)
    {
        this->x = x;
        this->specie = specie;
        this->dir = dir;
        this->y = y;
        this->state = state;
    }

    void movement(int n, int m)
    {
        int speed;
        if (this->specie == 'w' || this->specie == 'h')
            speed = 2;
        else if (this->specie == 'r')
            speed = 1;

        if (this->dir == 0)
            this->y -= speed;
        else if (this->dir ==  1)
            this->x += speed;
        else if (this->dir == 2)
            this->y += speed;
        else if (this->dir == 3)
            this->x -= speed;

            // check borders:
            if (this->x < 0)
                this->x += m;
            else if (this->x > m-1)
                this->x -= m;

            if (this->y < 0)
                this->y += n;
            else if (this->y > n-1)
                this->y -= n;

            // check statement
            if ((this->age+1) % this->state == 0 && (this->age != 0 || this->state == 1))
            {
                this->dir = (this->dir + 1) % 4;
            }
    }

    void aging() {this->age++;}

    int get_x() {return this->x;}
    int get_dir() {return this->dir;}
    int get_state() {return this->state;}
    char get_specie() {return this->specie;}
    int get_y() {return this->y;}
    int get_age() {return this->age;}
};

class Wolf: public Animals
{
protected:
    int hung = 0;
public:
    Wolf() : Animals() {}
    Wolf(int x, int y, int d, int k) : Animals(x, y, d, k, 'w') {}
    int get_hung() {return this->hung; }
    void inc_hung() {this->hung++;}
    void reset() {this->hung = 0;}
};

class Rabbit: public Animals
{
public:
    Rabbit() : Animals() {}
    Rabbit(int x, int y, int d, int k) : Animals(x, y, d, k, 'r') {}
};

class Hyena: public Animals
{
protected:
    int hung = 0;
    int reprod = 0;
public:
    Hyena(): Animals() {}
    Hyena(int x, int y, int d, int k) : Animals(x,y,d,k,'h') {}
    int get_hung() {return this->hung; }
    int get_reprod() {return this->reprod;}
    void inc_hung() {this->hung++;}
    void inc_reprod() {this->reprod++;}
};

class Simulation
{
protected:
    list <Rabbit> rabbits;
    list <Wolf> wolfs;
    list <Hyena> hyenas;
    int n = 0, m = 0, t = 0;
public:
    Simulation(int n, int m, int t, int r, int w, int h = 0)
    {
        this->n = n;
        this->m = m;
        this->t = t;
        while(r-- > 0)
        {
            int x, y, d, k;
            inf >> x >> y >> d >> k;
            this->rabbits.push_back(Rabbit(x,y,d,k));
        }
        while(w-- > 0)
        {
            int x, y, d, k;
            inf >> x >> y >> d >> k;
            this->wolfs.push_back(Wolf(x,y,d,k));
        }
        while(h-- > 0)
        {
            int x,y,d,k;
            inf >> x >> y >> d >> k;
            this->hyenas.push_back(Hyena(x,y,d,k));
        }
    }

    void Simulate()
    {
        while(this->t-- != 0)
        {
            this->moving();
            this->eating();
            this->aging();
            this->breeding();
            this->extinction();
        }
    }

    void moving()
    {
        for(auto it = this->wolfs.begin(); it != this->wolfs.end(); it++)
            it->movement(n, m);
        for(auto it = this->rabbits.begin(); it != this->rabbits.end(); it++)
            it->movement(n, m);
        for(auto it = this->hyenas.begin(); it != this->hyenas.end(); it++)
            if (it->get_hung() < 2)
                it->movement(n, m);
    }

    void aging()
    {
        for(auto it = this->wolfs.begin(); it != this->wolfs.end(); it++)
            it->aging();
        for(auto it = this->rabbits.begin(); it != this->rabbits.end(); it++)
            it->aging();
        for(auto it = this->hyenas.begin(); it != this->hyenas.end(); it++)
            it->aging();
    }

    void eating()
    {
        for(int i = 0; i < n; i++)
        {
            for(int j = 0; j< m; j++)
            {
                // hyenas have to eat first another hyenas
                int control = 0;
                auto hey = this->hyenas.begin();
                for (auto it = this->hyenas.begin(); it != this->hyenas.end(); it++)
                    if(it->get_x() == j && it->get_y() == i)
                    {
                        hey = it++; // found suitable hyena
                        for(auto p = it; p != this->hyenas.end(); p++)
                            if(p->get_x() == j && p->get_y() == i && hey->get_hung() < 2)
                            {
                                hey->inc_hung();
                                auto temp1 = p--;
                                this->hyenas.erase(temp1);
                            }
                        for(auto p = this->wolfs.begin(); p != this->wolfs.end(); p++)
                            if(p->get_x() == j && p->get_y() == i && hey->get_hung() < 2)
                            {
                                hey->inc_hung();
                                auto temp1 = p--;
                                this->wolfs.erase(temp1);
                            }
                        for(auto p = this->rabbits.begin(); p != this->rabbits.end(); p++)
                            if(p->get_x() == j && p->get_y() == i && hey->get_hung() < 2)
                            {
                                hey->inc_hung();
                                auto temp1 = p--;
                                this->rabbits.erase(temp1);
                            }
                        it--;
                    }

                auto aged = this->wolfs.begin();    // keep suitable aged wolf
                for(auto it = this->wolfs.begin(); it != this->wolfs.end(); it++)   // go through the list to find suitable wolf
                    if(it->get_x() == j && it->get_y() == i)  // searching suitable wolf
                    {
                        if(aged == this->wolfs.begin())
                        {
                            control = 1;
                            aged = it;
                        }
                    }
                if (control == 1)
                {
                    for(auto it = this->rabbits.begin(); it != this->rabbits.end(); it++)   // searching rabbits
                        if(it->get_x() == j && it->get_y() == i)
                        {
                                aged->inc_hung();
                                auto temp = it--;
                                this->rabbits.erase(temp);
                        }
                }
            }
        }

    }

    void breeding()
    {
        for(auto it = this->wolfs.begin(); it != this->wolfs.end(); it++)
            if(it->get_hung() >= 2)
            {
                it->reset();
                this->wolfs.push_back( Wolf(it->get_x(),it->get_y(),it->get_dir(),it->get_state()));
            }
        for(auto it = this->rabbits.begin(); it != this->rabbits.end(); it++)
            if(it->get_age() == 5 || it->get_age() == 10)
                this->rabbits.push_back( Rabbit(it->get_x(),it->get_y(),it->get_dir(),it->get_state()));
        for(auto it = this->hyenas.begin(); it != this->hyenas.end(); it++)
            if(it->get_hung() >= 2 && it->get_reprod() == 0)
            {
                it->inc_reprod();
                this->hyenas.push_back( Hyena(it->get_x(),it->get_y(),it->get_dir(),it->get_state()));
            }
    }

    void extinction()   // perfome deaths of animals
    {
        while (!this->wolfs.empty() && this->wolfs.front().get_age() == 15)
            this->wolfs.pop_front();
        while (!this->rabbits.empty() && this->rabbits.front().get_age() == 10)
            this->rabbits.pop_front();
        while (!this->hyenas.empty() && this->hyenas.front().get_age() == 15)
            this->hyenas.pop_front();
    }

    void print()    // print table
    {
        for(int i = 0; i < n; i++)
        {
            for(int j = 0; j< m; j++)
            {
                int sum = 0;
                for(auto it = this->wolfs.begin(); it != this->wolfs.end(); it++)   // перебор элементов списка с помощью итератора
                    if(it->get_x() == j && it->get_y() == i)  // беру элементы с нужными координатами
                        sum -= 1;
                for(auto it = this->rabbits.begin(); it != this->rabbits.end(); it++)
                    if(it->get_x() == j && it->get_y() == i)
                        sum += 1;
                for(auto it = this->hyenas.begin(); it != this->hyenas.end(); it++)
                    if(it->get_x() == j && it->get_y() == i)
                        sum += 10;
                if(sum != 0)
                    ouf << sum;
                else
                    ouf << '#';
            }
            ouf << endl;
        }
    }
};

int main()
{
    int n, m, t, r, w, h;
    inf >> n >> m >> t >> r >> w;
    Simulation rep(n,m,t,r,w);
    rep.Simulate();
    rep.print();
    return 0;
}
