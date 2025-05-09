#include "abajo.h"
#include "casa.h"
//#include "power.h"
#include <wiringPi.h>
#include <iostream>
#include <atomic>
#include <chrono>
#include <thread>
#include <cmath>

using namespace std;


void abajo(int prr, int t,int nv, int& control1, int &s, bool& st1, bool& n1, bool& n2, atomic<int>& control, atomic<int>& contador, atomic<int>& j , atomic<int>& fc, atomic<double>& vm,atomic<double>& im,atomic<float>& prof){
    
    int utl_conta=-1;
    j=0;
    contador=0;

    
    
    while(j < 2*nv){

    //while(j < 2*nv && prof <= prr){ 

        //cout << prof << "\t" << prr << "\t"<< fc  << endl;
        control=1;
        control1=1;
        motorForward();
        //if (contador != utl_conta){
        if (j != utl_conta){    
            utl_conta=j; 
            cout << "N vueltas bajando " << nv << "\t"<< j << endl;                                                             
            this_thread::sleep_for(chrono::milliseconds(10));   
            control=0;
            control1=0;
            /*
            if(fc==1){
                stopMotor();
                break;
            }
            */          

            this_thread::sleep_for(chrono::milliseconds((int)t)); 
            
        }
    }
    stopMotor();
    control=0;
    control1=0;
    cout << "Muestreando abajo "<< endl;
    this_thread::sleep_for(chrono::milliseconds((int)t));
    this_thread::sleep_for(chrono::milliseconds((int)t));
    s=0;
    
    return;
}