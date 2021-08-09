#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "laberinto.h"

#define TAMANIO 15
#define PARED '#'
#define VACIO ' '

//------

#define JUEGO_EN_CURSO 0
#define JUEGO_GANADO 1
#define JUEGO_PERDIDO -1

#define CODIGO_ESCREGUTO 'E'
#define DANIO_ESCREGUTO 20
#define TOTAL_ESCREGUTOS 1
#define CODIGO_ACROMANTULA 'A'
#define DANIO_ACROMANTULA 10
#define TOTAL_ACROMANTULAS 1
#define CODIGO_BOGGART 'B'
#define DANIO_BOGGART 15
#define TOTAL_BOGGARTS 1

#define CODIGO_IMPEDIMENTA 'I'
#define TOTAL_IMPEDIMENTAS 1
#define CODIGO_RIDDIKULUS 'R'
#define TOTAL_RIDDIKULUS 1
#define CODIGO_ESFINGE 'F'
#define TOTAL_ESFINGES 1
#define CODIGO_POCION 'P'
#define TOTAL_POCIONES 3
#define VIDA_POR_POCION 15

#define CODIGO_COPA 'C'
#define VIDA_PARA_MOSTRAR_COPA 15

#define CODIGO_JUGADOR 'J'
#define VIDA_INICIAL 50
#define VIDA_PERDIDA_POR_TURNO 3
#define MOV_ARRIBA 'w'
#define MOV_ABAJO 's'
#define MOV_DERECHA 'd'
#define MOV_IZQUIERDA 'a'
#define DISTANCIA_MINIMA_JUGADOR_COPA 10

#define TOTAL_AYUDAS 10
#define TOTAL_OBSTACULOS 10

#define CODIGO_RIVAL 'G'
#define PASOS_POR_DIRECCION_RIVAL 4
#define DIRECCION_INICIAL_RIVAL MOV_DERECHA
#define DISTANCIA_MINIMA_RIVAL_COPA 10

typedef struct coordenada {
    int fil;
    int col;
} coordenada_t;

typedef struct obstaculo {
    char codigo;
    coordenada_t posicion;
    int danio;
} obstaculo_t;

typedef struct ayuda {
    char codigo;
    coordenada_t posicion;
    int vida_a_recuperar;
} ayuda_t;

typedef struct jugador {
    char codigo;
    int vida;
    coordenada_t posicion;
    int tope_ayudas;
    ayuda_t ayudas[TOTAL_AYUDAS];
} jugador_t;


typedef struct rival {
    char codigo;
    coordenada_t posicion;
    char direccion;
    int cantidad_pasos;
} rival_t;

typedef struct copa {
    char codigo;
    coordenada_t posicion;
} copa_t;

typedef struct juego{
    char laberinto_original[TAMANIO][TAMANIO];
    jugador_t jugador;
    rival_t rival;
    copa_t copa;
    obstaculo_t obstaculos[TOTAL_OBSTACULOS];
    int tope_obstaculos;
    ayuda_t ayudas[TOTAL_AYUDAS];
    int tope_ayudas;
} juego_t;


//-------------FUNCIONES DE COMPARAR POSICIONES------------------

/* PRE: Recive dos variables de tipo coordenada_t.
   POST: Devuelve true si son iguales campo a campo. */
bool posiciones_iguales(coordenada_t posicion_1, coordenada_t posicion_2){
    bool son_iguales = false;
    if((posicion_1.fil == posicion_2.fil) && (posicion_1.col == posicion_2.col)){
        son_iguales = true;    
    }
    
    return son_iguales;
}

/* PRE: Recibe una matriz y una variable de tipo coordenada_t que indica una posición de la matriz.
   POST: Devuelve true si en la posicion indicada de la matriz hay un caracter igual a VACIO. */
bool hay_vacio(char laberinto[TAMANIO][TAMANIO], coordenada_t posicion){
    return laberinto[posicion.fil][posicion.col] == VACIO;
}

/* PRE: Recibe una variable de tipo coordenada_t y un vector de coodenada_t.
   POST: Devuelve true si la variable está en el vector. */
bool esta_ocupada(coordenada_t posiciones_ocupadas[], int tope_posiciones_ocupadas, coordenada_t posicion_buscada){
    bool ocupada = false;
    int indice_posiciones = 0;
    while(indice_posiciones < tope_posiciones_ocupadas && !ocupada){
        if(posiciones_iguales(posiciones_ocupadas[indice_posiciones], posicion_buscada)){
            ocupada = true;        
        } 
        indice_posiciones++;   
    }

    return ocupada;
}

//----------FUNCIONES DE INICIALIZACION----------

/*
 * Devolverá una coordenada aleatoria dentro del rango TAMANIOxTAMANIO.
 * No valida que dicha coordenada coincida con un pasillo ni que exista 
 * otro objeto en esa posición.
 */
coordenada_t posicion_aleatoria(){
    coordenada_t coordenada;
    coordenada.fil = rand()%TAMANIO;
    coordenada.col = rand()%TAMANIO;
    return coordenada;
}

/* PRE: Recibe una variable de tipo copa_t, una matriz, un vector de coordenada_t y el tope del vector.
   POST: El campo 'posicion', de la variable de tipo copa_t tendra valores que corresponden a una posición de la matriz con valor igual a VACIO.
         El campo 'codigo' de la variable de tipo copa_t estará inicializado con el valor CODIGO_COPA.
         El vector de tipo coordenada_t incluirá la posición de la copa, y su tope será incrementado. */
void inicializar_copa(copa_t* copa, char laberinto[TAMANIO][TAMANIO], coordenada_t posiciones_ocupadas[], int* tope_posiciones_ocupadas){
    (*copa).codigo = CODIGO_COPA;
    do { 
        (*copa).posicion = posicion_aleatoria();   
    } while(!hay_vacio(laberinto, (*copa).posicion));
    posiciones_ocupadas[(*tope_posiciones_ocupadas)] = (*copa).posicion;
    (*tope_posiciones_ocupadas)++;
}

/* PRE: Recibe un caracter que representa un obstaculo, un entero que representa el daño ha realizar por el obstaculo, un entero que representa la cantidad de ese obstaculos a inicializar,
        Y un vector de tipo obstaculo_t con su respectivo tope.
   POST: Los campos 'codigo', y 'danio' del vector obstaculo_t estarán inicializados según los parámetros ingresados.
         Serán inicializados tantas posiciones del vector obstaculo_t como indique el entero 'cantidad_obstaculos'.
         Si la cantidad de obstaculos ingresada es mayor a TOTAL_OBSTACULOS, se inicializarán todos los elementos posibles hasta llegar a dicho máximo. */
void agregar_obstaculo(char obstaculo_ha_agregar, int danio_obstaculo, int cantidad_obstaculos, obstaculo_t obstaculos[TOTAL_OBSTACULOS], int* tope_obstaculos){
    if(TOTAL_OBSTACULOS >= cantidad_obstaculos + (*tope_obstaculos)){
        for(int i = 0; i < cantidad_obstaculos; i++){
            obstaculos[(*tope_obstaculos)+i].codigo = obstaculo_ha_agregar;
            obstaculos[(*tope_obstaculos)+i].danio = danio_obstaculo;
        }
        (*tope_obstaculos) += cantidad_obstaculos;
    } else {
        while((*tope_obstaculos) < TOTAL_OBSTACULOS){
            obstaculos[(*tope_obstaculos)].codigo = obstaculo_ha_agregar;
            obstaculos[(*tope_obstaculos)].danio = danio_obstaculo;
            (*tope_obstaculos)++;
        }
    }
}

/* PRE: Recibe un vector de tipo obstaculo_t y su correspondiente tope, una matriz inicializada, y otro vector de tipo coordenada_t con su correspondiente tope.
   POST: El tope del vector de tipo obstaculo_t tendrá un valor igual a la cantidad de elementos del vector de tipo obstaculo_t inicializados.
         El campo 'posicion' de cada obstaculo inicializado indicara una posicion de la matriz en la que haya un valor igual a VACIO.
         El campo 'posición' de cada obstaculo inicializado es distinto a todos los elementos almacenadas por el vector de tipo coordenada_t.
         El vector de tipo coordenada_t contendrá las posiciones de todos los obstaculos inicializados, y su tope será incrementado acordemente. */
void inicializar_obstaculos(obstaculo_t obstaculos[TOTAL_OBSTACULOS], int* tope_obstaculos, char laberinto[TAMANIO][TAMANIO], coordenada_t posiciones_ocupadas[], int* tope_posiciones_ocupadas){
    bool coordenada_valida = false;

    (*tope_obstaculos) = 0;
    agregar_obstaculo(CODIGO_ESCREGUTO, DANIO_ESCREGUTO, TOTAL_ESCREGUTOS, obstaculos, &(*tope_obstaculos));
    agregar_obstaculo(CODIGO_ACROMANTULA, DANIO_ACROMANTULA, TOTAL_ACROMANTULAS, obstaculos, &(*tope_obstaculos));
    agregar_obstaculo(CODIGO_BOGGART, DANIO_BOGGART, TOTAL_BOGGARTS, obstaculos, &(*tope_obstaculos));

    for(int i = 0; i < (*tope_obstaculos); i++){
        coordenada_valida = false;
        do { 
            obstaculos[i].posicion = posicion_aleatoria();  
            if((hay_vacio(laberinto, obstaculos[i].posicion)) && (!esta_ocupada(posiciones_ocupadas, *tope_posiciones_ocupadas, obstaculos[i].posicion))){
                coordenada_valida = true;
                posiciones_ocupadas[(*tope_posiciones_ocupadas)] = obstaculos[i].posicion;
                (*tope_posiciones_ocupadas)++;           
            }
        } while(!coordenada_valida);
    }
}

/* PRE: Recibe un caracter que representa una ayuda, un entero que representa la cantidad de vida que la ayuda restaurará, un entero que representa la cantidad de esa ayuda a inicializar,
        y un vector de tipo ayuda_t con su respectivo tope.
   POST: Los campos 'codigo', y 'vida_a_recuperar' del vector ayuda_t estarán inicializados segun los parámetros ingresados.
         Serán inicializados tantas posiciones del vector ayuda_t como indique el entero 'cantidad_ayudas'.
         Si la cantidad de ayudas ingresada es mayor a TOTAL_AYUDAS, se inicializarán todos los elementos posibles hasta llegar a dicho máximo. */
void agregar_ayuda(char ayuda_ha_agregar, int vida_ayuda, int cantidad_ayudas, ayuda_t ayudas[TOTAL_AYUDAS], int* tope_ayudas){
    if(TOTAL_AYUDAS >= cantidad_ayudas + (*tope_ayudas)){
        for(int i = 0; i < cantidad_ayudas; i++){
            ayudas[(*tope_ayudas)+i].codigo = ayuda_ha_agregar;
            ayudas[(*tope_ayudas)+i].vida_a_recuperar = vida_ayuda;
        }
        (*tope_ayudas) += cantidad_ayudas;
    } else {
        while((*tope_ayudas) < TOTAL_AYUDAS){
            ayudas[(*tope_ayudas)].codigo = ayuda_ha_agregar;
            ayudas[(*tope_ayudas)].vida_a_recuperar = vida_ayuda;
            (*tope_ayudas)++;
        }
    }
}

/* PRE: Recibe un vector ayuda_t y su correspondiente tope, una matriz inicializada, y otro vector de tipo coordenada_t con su correspondiente tope.
   POST: El tope del vector de tipo ayuda_t tendrá un valor igual a la cantidad de elementos del vector de tipo ayuda_t inicializados.
         El campo "posicion" indicara una posicion de la matriz en la que haya un valor igual a VACIO.
         El campo "posicion" de cada ayuda inicializada es distinto a todos los elementos almacenados por el vector de tipo coordenada_t.
         El vector de tipo coordenada_t contendrá las posiciones de todas las ayudas inicializadas, y su tope será incrementado acordemente. */
void inicializar_ayudas(ayuda_t ayudas[TOTAL_AYUDAS], int* tope_ayudas, char laberinto[TAMANIO][TAMANIO], coordenada_t posiciones_ocupadas[], int* tope_posiciones_ocupadas){  
    bool coordenada_valida = false;
     
    (*tope_ayudas) = 0;
    agregar_ayuda(CODIGO_IMPEDIMENTA, 0, TOTAL_IMPEDIMENTAS, ayudas, &(*tope_ayudas));
    agregar_ayuda(CODIGO_RIDDIKULUS, 0, TOTAL_RIDDIKULUS, ayudas, &(*tope_ayudas));
    agregar_ayuda(CODIGO_POCION, VIDA_POR_POCION, TOTAL_POCIONES, ayudas, &(*tope_ayudas));
    agregar_ayuda(CODIGO_ESFINGE, 0, TOTAL_ESFINGES, ayudas, &(*tope_ayudas));
    
    for(int i = 0; i < (*tope_ayudas); i++){
        coordenada_valida = false;      
        do { 
            ayudas[i].posicion = posicion_aleatoria(); 
            if((hay_vacio(laberinto, ayudas[i].posicion)) && (!esta_ocupada(posiciones_ocupadas, *tope_posiciones_ocupadas, ayudas[i].posicion))){
                coordenada_valida = true;
                posiciones_ocupadas[(*tope_posiciones_ocupadas)] = ayudas[i].posicion;
                (*tope_posiciones_ocupadas)++;
            }  
        } while(!coordenada_valida);

    }
}

/* PRE: Recibe dos variables de tipo coodenada_t.
   POST: Devuelve la suma de los módulos de las restas de los campos correspondientes (la distancia entre ellos). */
int distancia_manhattan(coordenada_t posicion_1, coordenada_t posicion_2){
    return abs(posicion_1.fil - posicion_2.fil) + abs(posicion_1.col - posicion_2.col);
}

/* PRE: Recibe una variable de tipo rival_t, una matriz de caracteres, un vector de tipo coordenada_t con su tope y una variable de tipo coordenada_t.
   POST: Los campos 'codigo', 'cantidad_pasos' y 'direccion' de la variable rival_t estarán inicializados.
         El campo "posicion" del rival correspondera a una posicion dentro de la matriz donde el valor sea igual a VACIO.
         La distancia manhattan entre la posición del rival y la posición de la copa será mayor a 10.
         El vector de tipo coordenada_t contendrá la posicion del rival y su tope será incrementado.
         El campo "posicion" del rival es distinto a todos los elementos del vector de tipo coordenada_t. */
void inicializar_rival(rival_t* rival, char laberinto[TAMANIO][TAMANIO], coordenada_t posiciones_ocupadas[], int* tope_posiciones_ocupadas, coordenada_t posicion_copa){
    bool coordenada_valida = false;   
    int distancia_rival_copa;

    (*rival).codigo = CODIGO_RIVAL;    
    (*rival).cantidad_pasos = 0;
    (*rival).direccion = DIRECCION_INICIAL_RIVAL;
    do {
        coordenada_valida = false;
        (*rival).posicion = posicion_aleatoria();
        distancia_rival_copa = distancia_manhattan((*rival).posicion, posicion_copa);
        if((hay_vacio(laberinto, (*rival).posicion)) && (!esta_ocupada(posiciones_ocupadas, *tope_posiciones_ocupadas, (*rival).posicion)) && (distancia_rival_copa > 10)){
            coordenada_valida = true;
            posiciones_ocupadas[(*tope_posiciones_ocupadas)] = (*rival).posicion;
            (*tope_posiciones_ocupadas)++;       
        }
    } while(!coordenada_valida);
}

/* PRE: Recibe una variable de tipo jugador_t, una matriz de caracteres, y un vector de  tipo coordenada_t con su tope, una variable de tipo coordenada_t.
   POST: Los campos 'vida', 'codigo' y 'tope_ayudas' de la variable jugador_t estarán inicializados.
         La distancia manhattan entre la posición del rival y la posición de la copa será mayor a 10.
         El campo "posicion" del jugador correspondera a una posicion dentro de la matriz donde el valor sea igual a VACIO.
         El campo "posicion" del jugador es distinto a todos los elementos del vector de tipo coordenada_t. */
void inicializar_jugador(jugador_t* jugador, char laberinto[TAMANIO][TAMANIO], coordenada_t posiciones_ocupadas[], int tope_posiciones_ocupadas, coordenada_t posicion_copa){
    bool coordenada_valida = false;   
    int distancia_jugador_copa;

    (*jugador).vida = VIDA_INICIAL; 
    (*jugador).codigo = CODIGO_JUGADOR;
    (*jugador).tope_ayudas = 0;

    do {
        coordenada_valida = false;
        (*jugador).posicion = posicion_aleatoria();
        distancia_jugador_copa = distancia_manhattan((*jugador).posicion, posicion_copa);
        if((hay_vacio(laberinto, (*jugador).posicion)) && (!esta_ocupada(posiciones_ocupadas, tope_posiciones_ocupadas, (*jugador).posicion)) && (distancia_jugador_copa > 10)){
            coordenada_valida = true;        
        }
    } while(!coordenada_valida);
}

/*
 * Inicializará todas las estructuras con los valores correspondientes,
 * creará el laberinto, posicionará todos los elementos, etc.
 */
void inicializar_laberinto(juego_t* juego){
    coordenada_t posiciones_ocupadas[TOTAL_OBSTACULOS + TOTAL_AYUDAS + 2]; //CANTIDAD MAXIMA DE AYUDAS + CANTIDAD MAXIMA DE OBSTACULOS + RIVAL + COPA 
    int tope_posiciones_ocupadas = 0;
    inicializar_paredes_laberinto((*juego).laberinto_original);
    inicializar_copa(&(*juego).copa, (*juego).laberinto_original, posiciones_ocupadas, &tope_posiciones_ocupadas);
    inicializar_obstaculos((*juego).obstaculos, &(*juego).tope_obstaculos, (*juego).laberinto_original, posiciones_ocupadas, &tope_posiciones_ocupadas);
    inicializar_ayudas((*juego).ayudas, &(*juego).tope_ayudas, (*juego).laberinto_original, posiciones_ocupadas, &tope_posiciones_ocupadas);
    inicializar_rival(&(*juego).rival, (*juego).laberinto_original, posiciones_ocupadas, &tope_posiciones_ocupadas, (*juego).copa.posicion);
    inicializar_jugador(&(*juego).jugador, (*juego).laberinto_original, posiciones_ocupadas, tope_posiciones_ocupadas, (*juego).copa.posicion);
}

//----------FUNCIONES DE MOVIMIENTO----------

/*
 * Determinará si el caracter ingresado es válido, esto es, es el caracter ‘a’ o 
 * ‘s’ o ‘d’ o ‘w’ y además el jugador puede moverse en esa dirección, o sea, 
 * hay pasillo.
 */
bool es_movimiento_valido(juego_t* juego, char tecla){
    bool es_valido = false;
    coordenada_t posicion_auxiliar = (*juego).jugador.posicion;
    switch(tecla){       
        case MOV_ARRIBA:
            posicion_auxiliar.fil--;
            if(posicion_auxiliar.fil >= 0){
                if(hay_vacio((*juego).laberinto_original, posicion_auxiliar)){ 
                    es_valido = true;
                }
            }
        break;
        case MOV_ABAJO:
            posicion_auxiliar.fil++;
            if(posicion_auxiliar.fil < TAMANIO){
                if(hay_vacio((*juego).laberinto_original, posicion_auxiliar)){
                    es_valido = true;            
                }
            } 
        break;
        case MOV_DERECHA:
            posicion_auxiliar.col++;
            if(posicion_auxiliar.col < TAMANIO){
                if(hay_vacio((*juego).laberinto_original, posicion_auxiliar)){
                    es_valido = true;            
                }
            }
        break;
        case MOV_IZQUIERDA:
            posicion_auxiliar.col--;
            if(posicion_auxiliar.col >= 0){
                if(hay_vacio((*juego).laberinto_original, posicion_auxiliar)){
                    es_valido = true;            
                }
            }              
        break;
        default:
            es_valido = false;
        break;
    }
    
    return es_valido;
}

/*
 * Moverá el jugador hacia la dirección especificada.
 * Dicho movimiento es válido.
 */
void mover_jugador(jugador_t* jugador, char direccion){
    switch(direccion){       
        case MOV_ARRIBA:
            (*jugador).posicion.fil--;
        break;    
        case MOV_ABAJO:
            (*jugador).posicion.fil++;
        break;
        case MOV_DERECHA:
            (*jugador).posicion.col++;
        break;
        case MOV_IZQUIERDA:
            (*jugador).posicion.col--;
        break;
    }
}

/*
 * Moverá el rival a la próxima posición.
 */
void mover_rival(juego_t* juego){
    bool se_ha_movido = false;
    coordenada_t posicion_auxiliar;
    while(!se_ha_movido){   
        posicion_auxiliar = (*juego).rival.posicion;
        if((*juego).rival.cantidad_pasos == PASOS_POR_DIRECCION_RIVAL){
            switch((*juego).rival.direccion){
                case MOV_ARRIBA:
                    (*juego).rival.direccion = MOV_DERECHA;
                break;
                case MOV_DERECHA:
                    (*juego).rival.direccion = MOV_ABAJO;
                break;
                case MOV_ABAJO:
                    (*juego).rival.direccion = MOV_IZQUIERDA;
                break;
                case MOV_IZQUIERDA:
                    (*juego).rival.direccion = MOV_ARRIBA;
                break;                                
            }    
            (*juego).rival.cantidad_pasos = 0;
        }
    
        switch((*juego).rival.direccion){
            case MOV_ARRIBA:        
                posicion_auxiliar.fil--;
                if(posicion_auxiliar.fil >= 0){
                    if(hay_vacio((*juego).laberinto_original, posicion_auxiliar)){
                        (*juego).rival.posicion.fil--;
                        se_ha_movido = true;                
                    }
                }
            break;
            case MOV_DERECHA:
                posicion_auxiliar.col++;
                if(posicion_auxiliar.col < TAMANIO){
                    if(hay_vacio((*juego).laberinto_original, posicion_auxiliar)){
                        (*juego).rival.posicion.col++;
                        se_ha_movido = true;                
                    }
                }
            break;
            case MOV_ABAJO:
                posicion_auxiliar.fil++;
                if(posicion_auxiliar.fil < TAMANIO){
                    if(hay_vacio((*juego).laberinto_original, posicion_auxiliar)){
                        (*juego).rival.posicion.fil++;
                        se_ha_movido = true;                
                    }
                }
            break;
            case MOV_IZQUIERDA:
                posicion_auxiliar.col--;
                if(posicion_auxiliar.col >= 0){
                    if(hay_vacio((*juego).laberinto_original, posicion_auxiliar)){
                        (*juego).rival.posicion.col--;
                        se_ha_movido = true;                
                    }
                }
            break;                                
        }   

        (*juego).rival.cantidad_pasos++; 
    }
}

//---------FUNCIONES DE ACTUALIZAR EL JUEGO----------

/* PRE: Recibe un vector de tipo ayuda_t, su correspondiente tope, una variable de tipo coordenada_t y un entero.
   POST: Devuelve true si alguno de los elementos del vector ayuda_t tiene el campo "posicion" igual a la variable coordenada_t.
         El entero contendra el número del elemento con igual posición a la variable coordenada_t, -1 en caso de que no se encuentre una igualdad. */
int hay_ayuda_en_posicion_jugador(coordenada_t posicion_buscada, ayuda_t ayudas[TOTAL_AYUDAS], int tope_ayudas, int* indice_posicion_buscada){
    bool hay_un_elemento = false;
    (*indice_posicion_buscada) = -1;
    int i = 0;
    while((i < tope_ayudas) && (!hay_un_elemento)){
        if(posiciones_iguales(posicion_buscada, ayudas[i].posicion)){
            hay_un_elemento = true;
            (*indice_posicion_buscada) = i;     
        } 
        i++;   
    }

    return hay_un_elemento;
}

/* PRE: Recibe un vector de tipo ayuda_t y su correspondiente tope, un segundo vector de tipo ayuda_t y un entero mayor o igual a 0 que representa una posicion del segundo vector.
   POST: El elemento indicado del segundo vector es agregado al primer vector, y el tope es ajustado. */
void agregar_ayuda_al_jugador(ayuda_t ayudas_jugador[TOTAL_AYUDAS], int* tope_ayudas_jugador, ayuda_t ayudas_juego[TOTAL_AYUDAS], int indice_posicion_buscada){
    ayudas_jugador[(*tope_ayudas_jugador)] = ayudas_juego[indice_posicion_buscada];
    (*tope_ayudas_jugador)++;
}

/* PRE: Recibe un vector de tipo ayuda_t con su tope, y un caracter
   POST: Devuelve true si el campo 'codigo' de uno de los elementos del vector es igual al caracter recibido. */
bool conoce_el_hechizo(ayuda_t ayudas_jugador[TOTAL_AYUDAS], int tope_ayudas_jugador, char ayuda_buscada){
    bool lo_conoce = false;
    int i = 0;
    while(i < tope_ayudas_jugador && !lo_conoce){
        if(ayudas_jugador[i].codigo == ayuda_buscada){
            lo_conoce = true;
        }     
        i++;    
    }

    return lo_conoce;
}

/* PRE: Recibe un entero mayor a 0 que representa la vida del jugador.
   POST: Se le suma al entero el valor de VIDA_POR_POCION sin sobrepasar VIDA_INICIAL.
         Muestra el valor de 'vida' que se ha restaurado. */
void consumir_pocion(int* vida_jugador){
    if((*vida_jugador) + VIDA_POR_POCION > VIDA_INICIAL){
        printf("La poción te ha restaurado %i Puntos de Vida.\n", VIDA_INICIAL - (*vida_jugador)); 
        (*vida_jugador) = VIDA_INICIAL;                
    } else { 
        (*vida_jugador) += VIDA_POR_POCION;
        printf("La poción te ha restaurado %i Puntos de Vida.\n", VIDA_POR_POCION);
    }
}

/* PRE: Recibe un caracter que representa una ayuda encontrada.
   POST: Muestra por pantalla la ayuda encontrada. */
void mostrar_ayuda_encontrada(char codigo_ayuda_encontrada){
    switch(codigo_ayuda_encontrada){
        case CODIGO_IMPEDIMENTA:
            printf("Has aprendido un nuevo Hechizo: 'Impedimenta'. Con este hechizo podras defenderte de los Escregutos, ('%c').\n", CODIGO_ESCREGUTO);
        break;
        case CODIGO_RIDDIKULUS:
            printf("Has aprendido un nuevo Hechizo: 'Ridikkulus'. Con este hechizo podras defenderte de los Boggarts, ('%c').\n", CODIGO_BOGGART);                    
        break;
        case CODIGO_POCION:
            printf("Has encontrado una pocion. ");
        break;
        case CODIGO_ESFINGE:
            printf("Has encontrado una 'Esfinge'. La posición de la Copa ('%c') es revelada.\n", CODIGO_COPA);
        break;
    }
}

/* PRE: Recibe un vector de tipo ayuda_t con su tope y un entero mayor o igual a 0 que representa un elemento.
   POST: El elemento indicado será eliminado del vector. */
void eliminar_ayuda(ayuda_t ayudas[TOTAL_AYUDAS], int* tope_ayudas, int posicion){
    ayudas[posicion] = ayudas[(*tope_ayudas) - 1];
    (*tope_ayudas)--;       
}

/* PRE: Recibe un variable de tipo coordenada_t, un vector de tipo obstaculo_t con su tope, y una entero que representa la posición de un elemento del vector.
   POST: Devuelve true si hay un elemento del vector cuyo campo 'posicion' sea igual a la variable de tipo coordenada_t.
         El entero almacenará el valor de la posicion del elemento en caso de que se encuentre una igualdad, o -1 si no se encuentra igualdad. */
int hay_obstaculo_en_posicion_jugador(coordenada_t posicion_buscada, obstaculo_t obstaculos[TOTAL_OBSTACULOS], int tope_obstaculos, int* indice_posicion_buscada){
    bool hay_un_elemento = false;
    (*indice_posicion_buscada) = -1;
    int i = 0;
        while((i < tope_obstaculos) && (!hay_un_elemento)){
        if(posiciones_iguales(posicion_buscada, obstaculos[i].posicion)){
            hay_un_elemento = true;
            (*indice_posicion_buscada) = i;     
        } 
        i++;   
    }

    return hay_un_elemento;
}

/* PRE: Recibe un caracter que representa un obstaculo, un entero que representa el daño a recibir, un vector de de tipo ayuda_t y su correspondiente tope
   POST: Revisará si el vector tiene la ayuda necesaria para evitar el daño, y en caso de que no modificará el entero según corresponda. */
void realizar_danio(char codigo_obstaculo, int* vida_jugador, ayuda_t ayudas_jugador[TOTAL_AYUDAS], int tope_ayudas_jugador){
    switch(codigo_obstaculo){
        case CODIGO_ESCREGUTO:
            if(conoce_el_hechizo(ayudas_jugador, tope_ayudas_jugador, CODIGO_IMPEDIMENTA)){
                printf("Delante de ti hay un Escreguto de Cola Explosiva. Como has aprendido Impedimenta, evitas el daño\n");
            } else {
                printf("Eres atacado por un Escreguto de Cola Explosiva. Pierdes %i puntos de vida\n", DANIO_ESCREGUTO);
                (*vida_jugador) -= DANIO_ESCREGUTO;                 
            }
        break;
        case CODIGO_ACROMANTULA:
            printf("Eres atacado por una Acromantula!! Pierdes %i puntos de vida\n", DANIO_ACROMANTULA);
            (*vida_jugador) -= DANIO_ACROMANTULA;
        break;
        case CODIGO_BOGGART:
            if(conoce_el_hechizo(ayudas_jugador, tope_ayudas_jugador, CODIGO_RIDDIKULUS)){
                printf("Delante de ti hay un Boggart. Como has aprendido Ridikkulus, evitas el daño.\n");                      
            } else {
                printf("Eres atacado por un Boggart. Pierdes %i puntos de vida\n", DANIO_BOGGART);
                (*vida_jugador) -= DANIO_BOGGART;
            }
        break;
    }
}

/* PRE: Recibe un vector de tipo obstaculo_t con su tope y un entero mayor o igual a 0.
   POST: El elemento señalado por el entero será eliminado del vector. */
void eliminar_obstaculo(obstaculo_t obstaculos[TOTAL_OBSTACULOS], int* tope_obstaculos, int posicion){
    obstaculos[posicion] = obstaculos[(*tope_obstaculos)-1];
    (*tope_obstaculos)--;
}

/*
 * Actualizará el juego. Restará vida si el jugador está sobre un obstáculo 
 * o lo eliminará si cuenta con el hechizo, aprenderá hechizos y todo lo 
 * que pueda suceder luego de un turno.
 */
void actualizar_juego(juego_t* juego){
    int indice_posicion_buscada;

    (*juego).jugador.vida -= VIDA_PERDIDA_POR_TURNO;

    if((*juego).jugador.vida > 0){
        if(hay_ayuda_en_posicion_jugador((*juego).jugador.posicion, (*juego).ayudas, (*juego).tope_ayudas, &indice_posicion_buscada)){
            mostrar_ayuda_encontrada((*juego).ayudas[indice_posicion_buscada].codigo);
            if((*juego).ayudas[indice_posicion_buscada].codigo == CODIGO_POCION){
                consumir_pocion(&(*juego).jugador.vida);
            } else {
                agregar_ayuda_al_jugador((*juego).jugador.ayudas, &(*juego).jugador.tope_ayudas, (*juego).ayudas, indice_posicion_buscada);
            }
            eliminar_ayuda((*juego).ayudas, &(*juego).tope_ayudas, indice_posicion_buscada);        
        }

        if(indice_posicion_buscada == -1){
            if(hay_obstaculo_en_posicion_jugador((*juego).jugador.posicion, (*juego).obstaculos, (*juego).tope_obstaculos, &indice_posicion_buscada)){
                realizar_danio((*juego).obstaculos[indice_posicion_buscada].codigo, &(*juego).jugador.vida, (*juego).jugador.ayudas, (*juego).jugador.tope_ayudas);
                eliminar_obstaculo((*juego).obstaculos, &(*juego).tope_obstaculos, indice_posicion_buscada);            
            }   
        }
    }
}

/*
 * Devolverá el estado del juego, 1 ganado, 0 en curso, -1 perdido.
 */
int estado_juego(juego_t juego){
    int resultado_del_juego = JUEGO_EN_CURSO;
    if(juego.jugador.vida <= 0){
        resultado_del_juego = JUEGO_PERDIDO; 
    } else if(posiciones_iguales(juego.jugador.posicion, juego.copa.posicion)){
        resultado_del_juego = JUEGO_GANADO;    
    } else if(posiciones_iguales(juego.rival.posicion, juego.copa.posicion)){
        resultado_del_juego = JUEGO_PERDIDO;
    }
    
    return resultado_del_juego;
}

/* PRE: Recibe dos matrices de igual tamaño.
   POST: La matriz 1 es igual a la matriz 2. */
void copiar_laberintos(char laberinto_1[TAMANIO][TAMANIO], char laberinto_2[TAMANIO][TAMANIO]){
    for(int i = 0; i < TAMANIO; i++){
        for(int j = 0; j < TAMANIO; j++){
            laberinto_1[i][j] = laberinto_2[i][j];        
        }    
    }
}

/*
 * Actualizará la matriz mostrada al usuario, con los elementos presentes 
 * en el juego.
 */
void actualizar_laberinto(juego_t juego, char laberinto[TAMANIO][TAMANIO]){
    bool mostrar_la_copa = false;

    copiar_laberintos(laberinto, juego.laberinto_original);
    for(int i = 0; i < juego.tope_ayudas; i++){
        laberinto[juego.ayudas[i].posicion.fil][juego.ayudas[i].posicion.col] = juego.ayudas[i].codigo;  
    }

    for(int i = 0; i < juego.tope_obstaculos; i++){
        laberinto[juego.obstaculos[i].posicion.fil][juego.obstaculos[i].posicion.col] = juego.obstaculos[i].codigo;
    }

    for(int i = 0; i < juego.jugador.tope_ayudas; i++){ 
        if(juego.jugador.ayudas[i].codigo == CODIGO_ESFINGE){
            mostrar_la_copa = true;        
        }  
    }

    if(juego.jugador.vida < VIDA_PARA_MOSTRAR_COPA){
        mostrar_la_copa = true;
    }    

    if(mostrar_la_copa){
        laberinto[juego.copa.posicion.fil][juego.copa.posicion.col] = juego.copa.codigo;    
    }

    laberinto[juego.rival.posicion.fil][juego.rival.posicion.col] = juego.rival.codigo;
    laberinto[juego.jugador.posicion.fil][juego.jugador.posicion.col] = juego.jugador.codigo;
}

/*
 * Mostrará el laberinto por pantalla.
 */
void mostrar_laberinto(char laberinto[TAMANIO][TAMANIO]){
    for(int i = 0; i < TAMANIO; i++){
        for(int j = 0; j < TAMANIO; j++){
            printf(" %c", laberinto[i][j]) ;       
        }
        printf("\n");
    }
}
