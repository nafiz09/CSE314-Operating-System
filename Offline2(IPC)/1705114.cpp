#include<iostream>
#include<pthread.h>
#include<semaphore.h>
#include<queue>
#include <unistd.h>
#include <random>
#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>

using namespace std;

int M,N,P,w,x,y,z;
int belt_num;
string msg;
default_random_engine generator;
poisson_distribution<int> distribution(4.1);

int getElapsedTImefromstart(){
    static int start = time(NULL);
    return time(NULL) - start;
}

sem_t pass_full;
sem_t *kiosk_empty;
//sem_t kiosk_full;
sem_t *security_empty;
sem_t security_full;
sem_t boarding_lock;
sem_t boarding_full;
sem_t c_lock;


void lockprint(string str)
{
    sem_wait(&c_lock);
    cout<<str<<endl;
    sem_post(&c_lock);
}

class Passenger{
    public:
    int id;
    bool is_VIP;
    bool has_boardingpass;
    Passenger(int id, bool is_VIP,bool has_boardingpass){
        this->id = id;
        this->is_VIP = is_VIP;
        this->has_boardingpass = has_boardingpass;
    }
};

queue<Passenger> passenger_q;
queue<Passenger> security_q;
queue<Passenger> boarding_q;

void *Passenger_t(void *arg)
{
    int i=1;
    while(1){
    int sleep_time = distribution(generator);
        sleep(sleep_time);
        Passenger p(i,false,false);
        if(i%5 == 0)
            p.is_VIP = true;
        if(p.is_VIP == false)
        {
            msg = "Passenger "+to_string(i)+" has arrived at the airport at time "+to_string(getElapsedTImefromstart());
            lockprint(msg);

        }
        else
        {
            msg = "Passenger " + to_string(i) + " (VIP) has arrived at the airport at time " + to_string(getElapsedTImefromstart());
            lockprint(msg);
        }
        passenger_q.push(p);
        sem_post(&pass_full);
        i++;
    }
}

void *Kiosk(void *arg)
{
    //cout<<"Kiosk thread started"<<endl;
    while(1)
    {
        sem_wait(&pass_full);
        Passenger psg = passenger_q.front();
        passenger_q.pop();
        int kiosk_count = rand();
        kiosk_count = kiosk_count%M;
        sem_wait(&kiosk_empty[kiosk_count]);
        if(psg.is_VIP == false)
        {
            msg = "Passenger "+ to_string(psg.id) + " has started self-check in at kiosk " + to_string(kiosk_count) + " at time " + to_string(getElapsedTImefromstart());
            lockprint(msg);
        }
        else
        {
            msg = "Passenger " + to_string(psg.id) + " (VIP) has started self-check in at kiosk " + to_string(kiosk_count) + " at time " + to_string(getElapsedTImefromstart());
            lockprint(msg);
        }
        sleep(w);
        if(psg.is_VIP == false)
        {
            msg = "Passenger " + to_string(psg.id) + " has finished check in at time " + to_string(getElapsedTImefromstart());
            lockprint(msg);
        }
        else
        {
            msg = "Passenger " + to_string(psg.id) + " (VIP) has finished check in at time " + to_string(getElapsedTImefromstart());
            lockprint(msg);
        }
        sem_post(&kiosk_empty[kiosk_count]);
        sleep(1);
        belt_num = rand();
        belt_num = belt_num%N;
        msg = "Passenger " + to_string(psg.id) + " has started waiting for security check in belt " + to_string(belt_num) + " from time " + to_string(getElapsedTImefromstart());
        lockprint(msg);
        if(psg.is_VIP == false){
            security_q.push(psg);
            sem_post(&security_full);
        }
        else
        {
            msg = "Passenger " + to_string(psg.id) + " (VIP) has started waiting to be boarded at time " + to_string(getElapsedTImefromstart());
            lockprint(msg);
            boarding_q.push(psg);
            sem_post(&boarding_full);
        }
    }
}

void *Security(void *arg)
{
    //cout<<"Security thread started"<<endl;
    while(1)
    {
        sem_wait(&security_full);
        Passenger psg = security_q.front();
        security_q.pop();
        sem_wait(&security_empty[belt_num]);
        sleep(1);
        msg = "Passenger " + to_string(psg.id) + " has started the security check at time " + to_string(getElapsedTImefromstart());
        lockprint(msg);
        sleep(x);
        msg = "Passenger " + to_string(psg.id) + " has crossed the security check at time " + to_string(getElapsedTImefromstart());
        lockprint(msg);
        sem_post(&security_empty[belt_num]);
        boarding_q.push(psg);
        sleep(1);
        msg = "Passenger " + to_string(psg.id) + " has started waiting to be boarded at time " + to_string(getElapsedTImefromstart());
        lockprint(msg);
        sem_post(&boarding_full);
        
    }
}

void *Boarding(void *arg)
{
    //cout<<"Boarding thread started"<<endl;
    while(1)
    {
        sem_wait(&boarding_full);
        sem_wait(&boarding_lock);
        Passenger psg = boarding_q.front();
        boarding_q.pop();
        sleep(1);
        if(psg.is_VIP == false)
        {
            msg = "Passenger " + to_string(psg.id) + " has started boarding the plane at time " + to_string(getElapsedTImefromstart());
            lockprint(msg);
        }
        else
        {
            msg = "Passenger " + to_string(psg.id) + " (VIP) has started boarding the plane at time " + to_string(getElapsedTImefromstart());
            lockprint(msg);
        }
        sleep(y);
        if(psg.is_VIP == false)
        {
            msg = "Passenger " + to_string(psg.id) + " has boarded the plane at time " + to_string(getElapsedTImefromstart());
            lockprint(msg);
        }
        else
        {
            msg = "Passenger " + to_string(psg.id) + " (VIP) has boarded the plane at time " + to_string(getElapsedTImefromstart());
            lockprint(msg);
        }
        sem_post(&boarding_lock);
    }
}

int main()
{
    freopen("output.txt","w",stdout);
    string myText;
    string str;
    vector<int> vals;
    ifstream MyReadFile("input.txt");
    while (getline (MyReadFile, myText)) {
        //cout<<myText<<endl;
        stringstream ss(myText);
        while(getline(ss,str,' '))
            vals.push_back(stoi(str));
    }
    MyReadFile.close();
    M=vals[0];
    N=vals[1];
    P=vals[2];
    w=vals[3];
    x=vals[4];
    y=vals[5];
    z=vals[6];
    sem_init(&c_lock,0,1);
    kiosk_empty = new sem_t[M];
    for(int i = 0;i<M;i++)
        sem_init(&kiosk_empty[i],0,1);
    
     //sem_init(&kiosk_full,0,0);
    security_empty = new sem_t[N];
    for(int i = 0;i<N;i++)
        sem_init(&security_empty[i],0,P);
    sem_init(&pass_full,0,0);
    sem_init(&security_full,0,0);
    sem_init(&boarding_lock,0,1);
    sem_init(&boarding_full,0,0);
    pthread_t pass_thread;
    pthread_create(&pass_thread,NULL,Passenger_t,NULL);
    pthread_t kiosk_thread;
    pthread_create(&kiosk_thread,NULL,Kiosk,NULL); 
    pthread_t security_thread;
    pthread_create(&security_thread,NULL,Security,NULL);
    pthread_t boarding_thread;
    pthread_create(&boarding_thread,NULL,Boarding,NULL);
    pthread_join(kiosk_thread,NULL);
    pthread_join(security_thread,NULL);
    pthread_join(boarding_thread,NULL);
    return 0;
}
