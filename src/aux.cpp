#include "aux.h"
#include <cstdio>
#include <cmath>


#define Di 6.0   // Diametro interior del tambor en cm
#define M 6.0    // Longitud del carrete (cambiado para que cuadre) 
#define e 0.4

#define L_MAX 21.0        // Longitud del cable
#define N_MAX 6.62         // Numero maximo de espiras (REVISAR)

int calcularVueltas(float prr){

    int V = (int)(M/e); // Numero de vueltas por espira

    if (prr >= L_MAX){
        prr = 10.0;
        printf("Profundidad excesiva. Estableciendo a 10 m");
    }

    float a = M_PI * M / 2.0;
    float b = (M_PI * Di * M / e) - (M_PI * M / 2.0);
    float c = (L_MAX - prr)*100.0;  // Convertir a cm

    float d = b * b + 4.0 * a * c;
    if(d < 0){
        printf("Error en el calculo de vueltas: Determinante Negativo\n");
        return 0;  // Si por lo que sea esto da negativo se aborta 
    }

    float N = (-b + sqrt(d))/(2*a);
    N = N_MAX - N;  // Ajuste para dar la vuelta a la ecuacion
    if (N<0){
        N = 0;
    }

    int K_e = (int)N*V;
    int K_r = (int)round(N*V - K_e);

    if ((K_e + K_r) <= V*N_MAX)  // Maximo de vueltas aproximado
        return K_e + K_r;
    else
        return V*N_MAX;
}