#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include "laberinto.h"
#include "copa.h" 

#define JUEGO_EN_CURSO 0
#define JUEGO_GANADO 1
#define JUEGO_PERDIDO -1


/* PRE: Recibe un caracter y un juego_t
   POST: El caracter tiene uno de los siguientes valores: MOV_DERECHA, MOV_IZQUIERDA, MOV_ARRIBA o MOV_ABAJO.
         La posición de la matriz a la que deberia moverse el jugador según el caracter tiene un valor igual a VACIO. */
void pedir_movimiento(char* direccion, juego_t juego){
    scanf(" %c", &(*direccion));
    while(!es_movimiento_valido(&juego, (*direccion))){
        printf("Movimiento invalido. Vuelva a ingresar la dirección del movimiento.\n");
        scanf(" %c", &(*direccion));
    }
}

/* PRE: Recibe una variable de tipo juego_t inicializada
   POST: Muestra por pantalla datos de la variable */
void mostrar_interfaz(juego_t juego){
    printf("Puntos de vida: %i\n", juego.jugador.vida);
    
    printf("Ayudas obtenidas:");
    for(int i = 0; i < juego.jugador.tope_ayudas; i++){
        printf(" %c", juego.jugador.ayudas[i].codigo);
    }
    printf("\n");
    
    if(estado_juego(juego) == JUEGO_GANADO){
        printf("Felicitaciones!! Has alcanzado la copa, eres el nuevo Campeon del Torneo de los Tres Magos\n");        
    } else if(estado_juego(juego) == JUEGO_PERDIDO){
        if(juego.jugador.vida <= 0){
            printf("Te has quedado sin vida :(\n");
        } else {       
            printf("Has perdido, el rival ha alcanzado la copa\n"); 
        }               
    }
}

int main(){

    juego_t juego;
    char laberinto_mostrado[TAMANIO][TAMANIO];
    char direccion;
    srand((unsigned int) time(NULL));
    
    system("clear");
    inicializar_laberinto(&juego);    
    actualizar_laberinto(juego, laberinto_mostrado);
    mostrar_laberinto(laberinto_mostrado);
    mostrar_interfaz(juego);
    
    while(estado_juego(juego) == JUEGO_EN_CURSO){
      
        pedir_movimiento(&direccion, juego);  
        system("clear");
        
        mover_jugador(&juego.jugador, direccion);
        actualizar_juego(&juego);  
        if(estado_juego(juego) == JUEGO_EN_CURSO){
            mover_rival(&juego);
        }
        actualizar_laberinto(juego, laberinto_mostrado);
        mostrar_laberinto(laberinto_mostrado);
        mostrar_interfaz(juego);       
    }  
}
