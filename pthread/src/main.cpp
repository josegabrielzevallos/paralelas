#include<iostream>
#include <pthread.h>
using namespace std;

void print(int a){
  cout<<"this->is a value"<<endl;
  for(int i=0; i<3; i++){}
}

int main(){
  cout<<"hello pthread"<<endl;
  for (int i = 0; i < 10; ++i) {
   cout <<"test"<<i<<endl; 
  }
  return 0;
}
