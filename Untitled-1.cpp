//Multi Threading
//Parallelism
#include <iostream>
#include <thread>
#include <future>
using namespace std;
long add(long j, long k){
    for(long i=1; i<=j; i++){
        k+=j;
    }
    return k;
 }
int main(){
    future<long> t1=async(add,211239,30);
    future<long> t2=async(add,21162339,40);
    future<long> t3=async(add,21124339,70);
    cout<<"Thread 1"<<t1.get()<<endl;
    cout<<"Thread 2"<<t2.get()<<endl;
    cout<<"Thread 3"<<t3.get()<<endl;
    return 0;
 }