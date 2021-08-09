#include <stdio.h>
#include <stdbool.h>

#define BUENA 'B'
#define REGULAR 'R'
#define MALA 'M'
#define MINIMA_EDAD 17
#define MAXIMO_caracteristica 10
#define MINIMO_caracteristica 0
#define NO 'N'

//----------FUNCIONES DE VERIFICACION DE DATOS----------

bool edad_valida(int edad_alumno){
//Precondición: Recibe un número entero
//Postcondición: Devuelve true si el número es mayor a 0
    return (edad_alumno > 0);
}

bool caracteristica_valido(int caracteristica_alumno){
//Precondición: Recibe un número entero
//Postcondición: Devuelve true si el número está entre 0 y 10 (inclusives)
    return ((caracteristica_alumno >= MINIMO_caracteristica) && (caracteristica_alumno <= MAXIMO_caracteristica));    
}

bool capacidad_magica_valida(char capacidad_magica){
//Precondición: Recibe un caracter
//Postcondición: Devuelve true si el caracter es igual a: 'B', 'R' o 'M'
    return ((capacidad_magica == BUENA) || (capacidad_magica == REGULAR) || (capacidad_magica == MALA));    
}

//----------FUNCIONES DE INTERACCIÓN CON EL USUARIO----------

void pedir_edad(int* edad_alumno){
//Precondición: Recibe un entero
//Postcondición: El entero (edad_alumno) tiene un valor mayor a 0
    printf("Ingrese la edad del alumno: ");
    scanf(" %i", edad_alumno);
    while(!edad_valida(*edad_alumno)){
        printf("La edad ingresada no es válida. Debe ser un número mayor a 0.\nIngrese la edad del alumno: ");
        scanf(" %i", edad_alumno);    
    }
}

void pedir_inteligencia(int* inteligencia_alumno){
//Precondición: Recibe un entero
//Postcondición: El entero (inteligencia_alumno) tiene un valor entre 0 y 10 (inclusive)
    printf("Ingrese la inteligencia del alumno: ");
    scanf(" %i", inteligencia_alumno);
    while(!caracteristica_valido(*inteligencia_alumno)){
        printf("La inteligencia ingresada no es válida. Debe ser un número entre 0 y 10 (inclusives).\nIngrese la inteligencia del alumno: ");
        scanf(" %i", inteligencia_alumno);  
    }
}

void pedir_fuerza(int* fuerza_alumno){
//Precondición: Recibe un entero
//Postcondición: El entero (fuerza_alumno) tiene un valor entre 0 y 10 (inclusive)
    printf("Ingrese la fuerza del alumno: ");
    scanf(" %i", fuerza_alumno);
    while(!caracteristica_valido(*fuerza_alumno)){
        printf("La fuerza ingresada no es válida. Debe ser un número entre 0 y 10 (inclusives).\nIngrese la fuerza del alumno: ");
        scanf(" %i", fuerza_alumno);  
    }
}

void pedir_capacidad_magica(char* capacidad_magica_alumno){
//Precondición: Recibe un caracter
//Postcondición: El caracter (capacidad_magica_alumno) tiene alguno de los siguientes valores: 'B', 'R' o 'M'
    printf("Ingrese la capacidad mágica del alumno: ");
    scanf(" %c", capacidad_magica_alumno);
    while(!capacidad_magica_valida(*capacidad_magica_alumno)){
        printf("La capacidad mágica ingresada no es válida. Solo puede ser uno de los siguientes caracteres: 'B' (Buena), 'R' (Regular) o 'M' (Mala).\nIngrese la capacidad mágica del alumno: ");
        scanf(" %c", capacidad_magica_alumno);  
    }
}

void pedir_respuesta(char* respuesta){
//Precondición: Recibe un caracter
//Postcondición: El caracter tiene un valor asignado
    printf("Desea ingresar un alumno? (N para NO, cualquier otra tecla para sí): ");
    scanf(" %c", respuesta);   
}

//---------FUNCIÓN DE CONTINUACIÓN----------

bool desea_ingresar_alumnos(char respuesta){
//Precondición: Recibe un caracter
//Postcondición: Devuelve true si el usuario ingresa el caracter 'N'
    return(respuesta == NO);
}

bool seguir_ingresando(char respuesta){
//Precondición: Recibe un caracter
//Postcondición: Devuelve true si el usuario ingresa el caracter 'N'
    return(respuesta == NO);
}

//----------FUNCIONES DE COMPARACIÓN----------

bool edad_suficiente(int edad_alumno){
//Precondición: Recibe un entero positivo
//Postcondición: Devuelve true si el entero es mayor a MINIMA_EDAD 
    return(edad_alumno >= MINIMA_EDAD);
}

void comparar_caracteristica(int caracteristica_alumno, int caracteristica_campeon, int* contador){
//Preconcidiones: Recibe dos enteros que tienen valores entre 0 y 10, y un entero con valor 0 (contador)
//Postcondiciones: Aumenta la variable 'contador' si el primer entero es mayor al segundo
//                 Reduce la variable 'contador' si el primer entero es menor al segundo
    if(caracteristica_alumno > caracteristica_campeon){
        (*contador)++;
    }
    else if(caracteristica_alumno < caracteristica_campeon){
        (*contador)--;
    }
}   

void comparar_capacidad_magica(char capacidad_magica_alumno, char capacidad_magica_campeon, int* contador){
//Precondiciones: Recibe dos caracteres, cuyos valores solo pueden ser 'B', 'R' o 'M', y un entero con valor 0(contador)
//Postcondicion: Aumenta la variable 'contador' si el primer caracter es mayor al segundo de acuerdo con la regla:  B > R > M
    if((capacidad_magica_alumno == BUENA) && (capacidad_magica_campeon != BUENA)) contador++;
    else if((capacidad_magica_alumno == MALA) && (capacidad_magica_campeon == REGULAR)) contador++;   
} 

//----------FUNCIÓN DE MOSTRAR CAMPEÓN----------

void mostrar_campeon(int numero_campeon){
//Precondición: Recibe un entero 
//Postcondición: Muestra por pantalla el número del campeón, en caso de que haya
    if(numero_campeon != 0){
        printf("El campeón es el número %i :)\n", numero_campeon);
    } else printf("No hay campeón :(\n"); 
}

int main(){

    int contador = 0;
    int edad_alumno; 
    int inteligencia_alumno, inteligencia_campeon; //"CARACTERISTICAS"
    int fuerza_alumno, fuerza_campeon;             //"CARACTERISTICAS"
    int numero_alumno = 0, numero_campeon = 0;
    char capacidad_magica_alumno, capacidad_magica_campeon;
    char respuesta;
    
    pedir_respuesta(&respuesta);
    if(desea_ingresar_alumnos(respuesta)) {
        printf("No hay campeón :(\n");            
        return 0;
    }

    do {   
        numero_alumno++;
        pedir_edad(&edad_alumno);
        if(edad_suficiente(edad_alumno)){
            pedir_inteligencia(&inteligencia_alumno);
            pedir_fuerza(&fuerza_alumno);
            pedir_capacidad_magica(&capacidad_magica_alumno);
            printf("-----LOS DATOS HAN SIDO INGRESADOS CORRECTAMENTE-----\n\n"); 

            if(numero_campeon == 0){
                contador = 1;            
            } else {
                comparar_caracteristica(inteligencia_alumno, inteligencia_campeon, &contador);
                if(contador == 0) comparar_caracteristica(fuerza_alumno, fuerza_campeon, &contador);
                if(contador == 0) comparar_capacidad_magica(capacidad_magica_alumno, capacidad_magica_campeon, &contador);
            }

            if(contador == 1) {
                numero_campeon = numero_alumno;
                inteligencia_campeon = inteligencia_alumno;
                fuerza_campeon = fuerza_alumno;
                capacidad_magica_campeon = capacidad_magica_alumno;            
            } 

            contador = 0;

        } else {
            printf("Este alumno no puede participar. Los participantes deben tener al menos 17 años\n\n");
        }
        pedir_respuesta(&respuesta);
    } while(!seguir_ingresando(respuesta));

    mostrar_campeon(numero_campeon);
    return 0;
}
