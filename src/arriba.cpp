#include "arriba.h"
#include "casa.h"
#include <wiringPi.h>
#include <iostream>
#include <atomic>
#include <chrono>
#include <thread>

using namespace std;


void arriba(int t,int nv, int& ft, int& control1, int &s, bool& st1, bool& n1, bool& n2, atomic<int>& control, atomic<int>& contador, atomic<int>& j, atomic<int>& fc, atomic<double>& vm,atomic<double>& im ){
    
    int utl_conta=-1;
    j=0;
    contador=0;
    
    
    while(fc == 0){    
        //cout << v << "\t" << i << "\t"<< fc  << endl;
        control=1;
        control1=1;
        motorBackward();
        
        //cout << "N vueltas subiendo " << nv << "\t"<< j << "\t"<< fc  << endl; 
        this_thread::sleep_for(chrono::milliseconds(10));
        if (j != utl_conta){    
            utl_conta=j; 
            cout << "N vueltas subiendo " << nv << "\t"<< j << "\t"<< fc  << endl;                                                             
            this_thread::sleep_for(chrono::milliseconds(10));   
            control=0;
            control1=0;                  
            this_thread::sleep_for(chrono::milliseconds((int)t)); 
        }   
        
    }
    
    stopMotor();
    control=0;
    control1=0;
    ft=1;
    cout << "Fin de prueba " << "\t" << ft <<endl;
    this_thread::sleep_for(chrono::milliseconds((int)t));
    this_thread::sleep_for(chrono::milliseconds((int)t));
    s=1;
    ft=1;
    cout << "fin de test send "<< "\t" << ft <<endl;
   
    return;
}