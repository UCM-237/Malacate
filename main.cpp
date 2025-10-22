#include <iostream>
#include <cstdlib>
#include <thread>
#include <atomic>
#include <wiringPi.h>
#include <cmath>
#include <math.h>
#include <csignal>
#include <vector>

#include "lib/generarOndaCuadrada.h"
#include "lib/sonda.h"
#include "lib/sensor.h"
#include "lib/casa.h"
#include "lib/abajo.h"
#include "lib/arriba.h"
//#include "lib/monitorIV.h"
#include "lib/ambulanceSiren.h"
#include "lib/serial_reader.h"
//#include "lib/autoManual.h"
#include "lib/aux.h"


using namespace std;



int main(){
    
    int frecuencia = 1000;
    int duty = 90;
    double t;          // tiempo de parado
    double lc = 0;
    int nv = 0;
    int s = 1;
    
    // variables de comunicacion 
    int st=0;
    // int pr=0;
    int tm=0;
    int sp=0;
    int tem=0;
    int ft=0;
    int home=0;
    float pr1;

    // Variables de control
    bool st1 = false ;
    bool n1 = true;
    bool n2 = true;

    // Inicalizacion
    int control1;
    int out=0;
    // int out1=0;
    // int m=0;
    float prr=5;  // Profundidad de bajada (default value)
    
    wiringPiSetupGpio(); // Configuración para usar el número de pin GPIO

    //signal(SIGINT, signalHandler);

    atomic<int> control(0); // Cambiar a atomic
  
    
    // Iniciar el hilo para generar la onda cuadradaame
    thread hiloOnda(generarOndaCuadrada, frecuencia, duty, ref(control));

    // Iniciar el hilo de sonsa
    thread hiloSonda(sonda);

  


     // Iniciar el hilo para ejecutar la función sensor
    thread hiloSensor(sensor, ref(control1));
 
    //this_thread::sleep_for(chrono::milliseconds(3000));

    // Iniciamos la lectura en un hilo
    iniciarLecturaEnHilo(st, sp, tem, prr, tm, ft, home);

   
   
    
    //ambulanceSiren();
    
   
    
    int utl_conta=-1;
    lc= (2*M_PI*0.045)*100;

    

    //pr=(int)(pr*100);
    //nv= (int)(pr/lc);
    //t= (int)((1000*(tm*60)/(nv-1)/2)/60);  
    
    t=2000;
    contador=0;
    j=0;
    control1=0;
    control=0;

    
    while(true){

        // nv = (int)(prr*3.5); // Aproximacion para saber el numero de vueltas (version antigua)
        nv = calcularVueltas(prr);
        // printf("Profundidad maxima: %f,  %d \n", prr, nv);

        // Motor parado esperando datos     
        if(st==0 && sp==0 && tem==0){
            pr1=prof;         
            // cout << "Esperando Datos...  " << vm << "\t" << im << "\t" << fc << endl;                   
            stopMotor();
            //cout << "En home  esperando datos fc en:  "<< pr1 << "\t"<< "Profundidad  " <<  prof << endl;
            contador=0;
            j=0;
            ft = 0;
            control1=0;
            control=0;
            // nv=36;  // Profundidad en vueltas

        }
        
        // Este bloque se ejecuta si estamos en home (esperando datos)
       




        
        //////////////////// Modo automatico automatico en manual //////////////////////////
        while(st==1 && sp==1 && tem==0){
            if (s==1){
                pr1=prof;
                control1=1;
                control=1;
                contador=0;
                j=0;
                //cout << v
                if (fc == 1) {
                    // Simula la desactivación del fin de carrera
                    pinMode(17, OUTPUT);                // Cambia el pin a salida
                    digitalWrite(17, LOW);              // Desactiva el pin
                    delay(100);                         // Espera 100 ms
                    pinMode(17, INPUT);                 // Vuelve a poner el pin como entrada
                    // Aquí empieza el nuevo proceso
                    motorForward();                     // Reemplaza por la función correspondiente
                    delay(1000);                        // Espera 100 ms
                    
                    contador=0;
                    j=0;
                    control1=0;
                    control=0;
                }
                
                
                abajo(prr,t,nv, control1, out, st1, n1, n2, control,contador, j,fc,vm, im, prof); 
                //ft=out;
                s=out;
            }  
            if (s==0){
                pr1=prof;
                
                control1=1;
                control=1;
                contador=0;
                j=0;
                control=1;
                control1=1;
                arriba(t,nv, ft, control1, out, st1, n1, n2, control,contador, j,fc,vm,im); 
                ft=1;
                s=out;    
            }
            ft=1;
            this_thread::sleep_for(chrono::seconds(2));                      
        }
        
        


        //////////////////////////////////////////////////      

        ///// Estado 2 SW S6 manual abajo
        while(st==0 && sp==1 && tem==0){      
            control=1;
            control1=1;
            pr1=prof;
            if (fc == 1) {
                    // Simula la desactivación del fin de carrera
                    pinMode(17, OUTPUT);         // Cambia el pin a salida
                    digitalWrite(17, LOW);       // Desactiva el pin
                    delay(100);                    // Espera 100 ms
                    pinMode(17, INPUT);          // Vuelve a poner el pin como entrada
                    // Aquí empieza el nuevo proceso
                    motorForward();              // Reemplaza por la función correspondiente
                    delay(100);                    // Espera 100 ms

            }
            motorForward(); 
                  
            if (j != utl_conta){
                utl_conta=j; 
                cout << "Manual bajando " << nv << " Contador "<< contador<< "\t"<< j << "\t" << prof << endl;                                                                    
                this_thread::sleep_for(chrono::milliseconds(100));   
                control=0;
                control1=0;                  
                this_thread::sleep_for(chrono::milliseconds((int)t)); 
            }
            if(j >= 2*nv || prof >= prr){
            //if(j >= 2*nv ){
                control=0;
                control1=0;
                stopMotor();
                cout << "Max profunidad " << prof << endl;
                ft=1;              
            }          
        }
        ////////////////////////////////Termina manual abajo        

        ///////////// Manual subiendo ///////////////////////
        // Estado 3 SW S6 manual subiendo        
        while(st==1 && sp==0 && tem==0){
            int utl_conta=-1;
            j=0;
            contador=0;
            pr1=prof;
            while(fc == 0 && st==1 && sp==0 && tem==0){    

                //cout << v << "\t" << i << "\t"<< fc  << endl;
                control=1;
                control1=1;
                motorBackward();
                
                //cout << "N vueltas subiendo " << nv << "\t"<< j << "\t"<< fc  << endl; 
                this_thread::sleep_for(chrono::milliseconds(10));
                if (j != utl_conta){    
                    utl_conta=j; 
                    cout << "N vueltas subiendo " << nv << "\t"<< j << "\t"<< fc  << endl;                                                             
                    //this_thread::sleep_for(chrono::milliseconds(10));   
                    control=0;
                    control1=0;                  
                    this_thread::sleep_for(chrono::milliseconds((int)t)); 
                }   
                
            }
    
            stopMotor();
            control=0;
            control1=0;
            ft=1;
            cout << "Fin de prueba "<< endl;
            //this_thread::sleep_for(chrono::milliseconds((int)t));
            //this_thread::sleep_for(chrono::milliseconds((int)t));
            s=1;
                      
        }

        /////////////////////  Fin manual subiendo ////////////////////////////


        if(st==0 && sp==0 && tem==1){
            cout << " Navegando  " << endl;
            ft=0;

        }
        
        // Estado 5 SW SE en el punto toma de muestras

        while(st==1 && sp==0 && tem==1){
            if (s==1){
                control1=1;
                control=1;
                contador=0;
                j=0;
                //cout << v
                if (fc == 1) {
                    pr1=prof;
                   

                    // Simula la desactivación del fin de carrera
                    pinMode(17, OUTPUT);                // Cambia el pin a salida
                    digitalWrite(17, LOW);              // Desactiva el pin
                    delay(100);                         // Espera 100 ms
                    pinMode(17, INPUT);                 // Vuelve a poner el pin como entrada
                    // Aquí empieza el nuevo proceso
                    motorForward();                     // Reemplaza por la función correspondiente
                    delay(1000);                        // Espera 100 ms
                    
                    contador=0;
                    j=0;
                    control1=0;
                    control=0;
                }
                abajo(prr,t,nv, control1, out, st1, n1, n2, control,contador, j,fc,vm, im, prof); 
                //ft=out;
                s=out;
            }  


            if (s==0){
                pr1=prof;
               
                control1=1;
                control=1;
                contador=0;
                j=0;
                control=1;
                control1=1;
                arriba(t,nv, ft, control1, out, st1, n1, n2, control,contador, j,fc,vm,im); 
                ft=1;
                s=out;

            }

            ft=1;
            this_thread::sleep_for(chrono::seconds(2));                      
        }
    }

  

    // Detener el hilo de manera segura
    control = 0;                        // Asegurarse de que control se detenga antes de unirse
    hiloOnda.join();                    // Unir el hilo de onda 
    hiloSonda.join();                   // Finalizar el hilo
    hiloSensor.join();                  // Finalizar el hilo
   

    //hiloUSB.detach();
    cout << "Programa finalizado." << endl;   
    return 0;
}
       
           

           
 

   
    
    





