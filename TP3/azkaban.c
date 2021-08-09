#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define COMANDO_PERDONABLES "perdonables"
#define COMANDO_LIBERAR "liberar"
#define COMANDO_MOSTRAR_LIBERADOS "mostrar_liberados"
#define COMANDO_AYUDA "ayuda"
#define COMANDO_ACTUALIZAR "actualizar"

#define MAX_NOMBRE 200
#define MAX_FECHA 7
#define CONDUCTA_BUENA 'B'
#define CONDUCTA_REGULAR 'R'
#define CONDUCTA_MALA 'M'
#define MAX_NOMBRE_ARCHIVO 30

#define ARCHIVO_CRUCIO "crucio.dat"
#define ARCHIVO_IMPERIUS "imperius.dat"
#define ARCHIVO_LIBERADOS "liberados_"
#define EXTENSION_LIBERADOS ".txt"

typedef struct preso{
    char nombre[MAX_NOMBRE];
    unsigned int edad;
    char conducta;
    unsigned int pabellon;
    unsigned int celda;
    char fecha[MAX_FECHA]; //aaaamm
    int maldicion_realizada;
} preso_t;

/*
    PRE: Recibe el FILE* de un archivo binario desde el que se va a leer un registro y una variable de tipo preso_t.
    POST: La variable de tipo preso_t contendrá un registro leido en el archivo.
          Devuelve la cantidad de registros leidos (1 si pudo leer).
*/
size_t leer_preso(FILE* archivo, preso_t* preso){
    return fread(preso, sizeof(preso_t), 1, archivo);
}

/*
    PRE: Recibe el FILE* de un archivo binario donde se va a escribir un registro, y una variable de tipo preso_t que contiene el registro a escribir.
    POST: Se escribe en el archivo la variable de tipo preso_t.
          Devuelve la cantidad de registros escritos (1 si pudo escribir).
*/
size_t escribir_preso(FILE* archivo, preso_t preso){
    return fwrite(&preso, sizeof(preso_t), 1, archivo);
}

/*
    PRE: Recibe el FILE* de un archivo de texto desde el que se va a leer un nombre, y un string.
    POST: El string contendrá el nombre leido del archivo.
          Devuelve la cantidad de elementos leidos. 
*/
int leer_nombre(FILE* archivo, char nombre[]){
    return fscanf(archivo, "%[^\n]\n", nombre);
}

/*
    PRE: Recibe el FILE* de un archivo de texto y un string.
    POST: Se escribe en el archivo el string.
          Devuelve la cantidad de elementos escritos.
*/
int escribir_nombre(FILE* archivo, char nombre[]){
    return fprintf(archivo, "%s\n", nombre);
}

/*
    PRE: Recibe un string con el nombre de un archivo y otro string con el formato 'aaaamm'.
    POST: Concatena al nombre del archivo, la fecha ingresada y la extension indicada por la constante EXTENSION_LIBERADOS. 
*/
void inicializar_nombre_liberados(char nombre_archivo[], char fecha[]){
    strcat(nombre_archivo, fecha);
    strcat(nombre_archivo, EXTENSION_LIBERADOS); 
}

/*
    PRE: Recibe un string con el nombre que corresponderá al nombre de un nuevo archivo.
    POST: Generá un archivo binario con aquellos presos a perdonar y con el nombre.
          recibido como argumento. Si ese archivo existe, se sobreescribe.
*/
int generar_archivo_perdonables(char nombre_archivo[]){
    preso_t preso_crucio;
    preso_t preso_imperius;
    size_t leido_imperius;
    size_t leido_crucio;
    bool pude_escribir = true;
    
    FILE* crucio = fopen(ARCHIVO_CRUCIO, "r");
    if (crucio == NULL) {
        printf("Error al abrir el archivo: %s.\n", ARCHIVO_CRUCIO);
        return -1;
    }
   
    FILE* imperius = fopen(ARCHIVO_IMPERIUS, "r");
   
    if (imperius == NULL) {
        printf("Error al abrir el archivo: %s.\n", ARCHIVO_IMPERIUS);
        fclose(crucio);
        return -1;
    }
    
    FILE* perdonables = fopen(nombre_archivo, "w");
    
    if (perdonables == NULL) {
        printf("Error al abrir el archivo: %s.\n", nombre_archivo);
        fclose(crucio);
        fclose(imperius);
        return -1;
    }
     
    leido_crucio = leer_preso(crucio, &preso_crucio);
    leido_imperius = leer_preso(imperius, &preso_imperius);
    while((leido_imperius > 0) && (leido_crucio > 0) && (pude_escribir)){
        if(strcmp(preso_crucio.nombre, preso_imperius.nombre) == 0){
            leido_crucio = leer_preso(crucio, &preso_crucio);
            leido_imperius = leer_preso(imperius, &preso_imperius);
        } else if(strcmp(preso_crucio.nombre, preso_imperius.nombre) > 0){
            pude_escribir = escribir_preso(perdonables, preso_imperius);
            leido_imperius = leer_preso(imperius, &preso_imperius);
        } else {
            pude_escribir = escribir_preso(perdonables, preso_crucio);
            leido_crucio = leer_preso(crucio, &preso_crucio);          
        }
    }
    
    while((leido_imperius > 0) && (pude_escribir)){
        pude_escribir = escribir_preso(perdonables, preso_imperius);
        leido_imperius = leer_preso(imperius, &preso_imperius);
    }
    
    while((leido_crucio > 0) && (pude_escribir)){
        pude_escribir = escribir_preso(perdonables, preso_crucio);
        leido_crucio = leer_preso(crucio, &preso_crucio);
    }
    
    fclose(crucio);
    fclose(imperius);
    fclose(perdonables);
    return 0;
}

/*  
    PRE: Recibe un string que contiene el nombre de un archivo binario, un string con el formato 'aaaamm' y un caracter de conducta.
    POST: Creará un archivo de texto con los nombres de los presos que cumplan con
          las condiciones necesarias (fecha de liberación que igual a la ingresada o que ya haya pasado y conducta igual o mejor a la ingresada).
*/
int generar_archivo_liberados(char nombre_archivo_perdonables[], char fecha[], char* conducta){
    FILE* perdonables = fopen(nombre_archivo_perdonables, "r");
    if(perdonables == NULL){
        printf("Error al abrir el archivo %s\n", nombre_archivo_perdonables);
        return -1;
    }
    
    char nombre_archivo_liberados[MAX_NOMBRE_ARCHIVO] = ARCHIVO_LIBERADOS;
    inicializar_nombre_liberados(nombre_archivo_liberados, fecha);
    
    FILE* liberados = fopen(nombre_archivo_liberados, "r");
    if(liberados != NULL){
        printf("El archivo de liberados con esa fecha ya existe.\n");
        fclose(perdonables);
        fclose(liberados);
        return -1;
    } else {        
        liberados = fopen(nombre_archivo_liberados, "w");
        if(liberados == NULL){
            printf("Error al abrir el archivo %s\n.", nombre_archivo_liberados);
            fclose(perdonables);
            return -1;
        }
        
        int fecha_liberacion = atoi(fecha);
        preso_t preso_actual;
        size_t leido = leer_preso(perdonables, &preso_actual);
        bool pude_escribir = true;
        while((leido > 0) && (pude_escribir)){
            if(atoi(preso_actual.fecha) <= fecha_liberacion){
                switch(*conducta){
                    case CONDUCTA_BUENA:
                        if(preso_actual.conducta == CONDUCTA_BUENA){
                            pude_escribir = escribir_nombre(liberados, preso_actual.nombre);
                        }
                    break;
                    case CONDUCTA_REGULAR:
                        if(preso_actual.conducta == CONDUCTA_BUENA || preso_actual.conducta == CONDUCTA_REGULAR){
                            pude_escribir = escribir_nombre(liberados, preso_actual.nombre);
                        }
                    break;
                    case CONDUCTA_MALA:
                        pude_escribir = escribir_nombre(liberados, preso_actual.nombre);
                    break;
                }
            }
            leido = leer_preso(perdonables, &preso_actual);
        }
    }

    fclose(perdonables);
    fclose(liberados);
    return 0;
}

/*
    PRE: Recibe un string con el formato: 'aaaamm'.
    POST: Mostrará por pantalla el archivo de presos liberados de ese año/mes.
*/
int mostrar_liberados(char fecha[]){
    char preso_actual[MAX_NOMBRE];
    char nombre_archivo[MAX_NOMBRE_ARCHIVO] = ARCHIVO_LIBERADOS;
    inicializar_nombre_liberados(nombre_archivo, fecha);

    FILE* liberados = fopen(nombre_archivo, "r");
    if(liberados == NULL){
        printf("Error al abrir el archivo %s\n", nombre_archivo);
        return -1;
    }
    
    int leido = leer_nombre(liberados, preso_actual);
   
    while(leido > 0){
        printf("%s\n", preso_actual);
        leido = leer_nombre(liberados, preso_actual);
    }
    
    fclose(liberados);
    return 0;
}

/*
    PRE: Recibe dos string que corresponden a nombres de archivos, un archivo binario con presos cargados y un archivo de texto con nombres.
    POST: El archivo binario contendrá todos los presos que tenia al principio menos aquellos cuyo nombre aparezca en el archivo de texto.
*/
int actualizar_archivos_auxiliar(char nombre_archivo_liberados[], char nombre_archivo_presos[]){
    FILE* liberados = fopen(nombre_archivo_liberados, "r");
    if(liberados == NULL){
        printf("Error al abrir el archivo %s\n", nombre_archivo_liberados);
        return -1;
    }
    
    FILE* archivo_presos = fopen(nombre_archivo_presos, "r");
    if(archivo_presos == NULL){
        printf("Error al abrir el archivo %s\n", nombre_archivo_presos);
        fclose(liberados);
        return -1;
    }
    
    FILE* nuevo_archivo = fopen("archivo_auxiliar.dat", "w");
    if(nuevo_archivo == NULL){
        printf("Error!\n");
        fclose(liberados);
        fclose(archivo_presos);
        return -1;
    }
    
    preso_t preso_actual;
    char nombre_preso[MAX_NOMBRE];
    
    int leido_liberados = leer_nombre(liberados, nombre_preso);
    size_t leido_presos = leer_preso(archivo_presos, &preso_actual);
    bool pude_escribir = true;
    
    while((leido_liberados > 0) && (leido_presos > 0) && (pude_escribir)){
        if(strcmp(nombre_preso, preso_actual.nombre) == 0){
            leido_liberados = leer_nombre(liberados, nombre_preso);
            leido_presos = leer_preso(archivo_presos, &preso_actual);
        } else if(strcmp(nombre_preso, preso_actual.nombre) > 0){
            pude_escribir = escribir_preso(nuevo_archivo, preso_actual);
            leido_presos = leer_preso(archivo_presos, &preso_actual);
        } else {
            leido_liberados = leer_nombre(liberados, nombre_preso);
        }
    }
    
    while((leido_presos > 0) && (pude_escribir)){
        pude_escribir = escribir_preso(nuevo_archivo, preso_actual);
        leido_presos = leer_preso(archivo_presos, &preso_actual);
    }
    
    fclose(liberados);
    fclose(archivo_presos);
    remove(nombre_archivo_presos);
    rename("archivo_auxiliar.dat", nombre_archivo_presos);
    fclose(nuevo_archivo);
    return 0;
}

/*
    PRE: Recibe un string de inicializado con el formato: 'aaaamm'.
    POST: Eliminará de los archivos binarios, los presos cuyos nombres aparezcan en el archivo 'liberados'.
*/
int actualizar_archivos(char fecha[]){
    char nombre_archivo_liberados[MAX_NOMBRE_ARCHIVO] = ARCHIVO_LIBERADOS;
    inicializar_nombre_liberados(nombre_archivo_liberados, fecha);   
    actualizar_archivos_auxiliar(nombre_archivo_liberados, ARCHIVO_CRUCIO);
    actualizar_archivos_auxiliar(nombre_archivo_liberados, ARCHIVO_IMPERIUS);
    return 0;
}

/*
    POST: Muestra por pantalla el listado de comandos y explica al usuario cómo se utilizan.
*/
void mostrar_ayuda(){
    printf("Comando PERDONABLES: \n");
    printf("\tDe dos archivos con los presos que realizaron cada maldición, crea uno con aquellos presos que pueden ser perdonados.\n");
    printf("\t\tComando identificador: '%s'\n", COMANDO_PERDONABLES);
    printf("\t\tArgumentos: \n");
    printf("\t\t\t* 'nombre que quiere que tenga el nuevo archivo'\n");
    printf("\t\tEjemplo: \n");
    printf("\t\t\t./azkaban %s <nombre_archivo>\n\n", COMANDO_PERDONABLES);
    printf("Comando LIBERAR: \n");
    printf("\tCrea un archivo de texto con los nombres de los presos que cumplan con: fecha de liberación igual o anterior a la ingresada, y conducta igual o mejor a la ingresada.\n");
    printf("\t\tComando identificador: '%s'\n", COMANDO_LIBERAR);
    printf("\t\tArgumentos: \n");
    printf("\t\t\t* Nombre del archivo de presos perdonables.\n");
    printf("\t\t\t* año/mes (formato aaaamm)\n");
    printf("\t\t\t* conducta (%c: buena - %c: regular - %c: mala)\n", CONDUCTA_BUENA, CONDUCTA_REGULAR, CONDUCTA_MALA);
    printf("\t\tEjemplo de la línea de comando: \n");
    printf("\t\t\t./azkaban %s <archivo_perdonables>.dat 201906 B\n\n", COMANDO_LIBERAR);
    printf("Comando ACTUALIZAR: \n");
    printf("\tActualizará ambos archivos binaios en los cuales figuran los presos que realizaron cada maldición.\n");
    printf("\t\tComando identificador: '%s'\n", COMANDO_ACTUALIZAR);
    printf("\t\tArgumentos: \n");
    printf("\t\t\t* Fecha del archivo que se toma para actualizar.\n");
    printf("\t\tEjemplo de la línea de comando: \n");
    printf("\t\t\t./azkaban %s 201906\n\n", COMANDO_ACTUALIZAR);
    printf("Comando MOSTRAR LIBERADOS: \n");
    printf("\tMuestra por pantalla el archivo de presos liberados de una determinada fecha.\n");
    printf("\t\tComando identificador: '%s'\n", COMANDO_MOSTRAR_LIBERADOS);
    printf("\t\tArgumentos: \n");
    printf("\t\t\t* Fecha del archivo que se quiere mostrar.\n");
    printf("\t\tEjemplo de la línea de comando: \n");
    printf("\t\t\t./azkaban %s 201906\n\n", COMANDO_MOSTRAR_LIBERADOS);
    printf("Comando AYUDA: \n");
    printf("\tMuestra los posibles comandos del sistema y como se utiliza cada uno\n");
    printf("\t\tComando identificador: '%s'\n", COMANDO_AYUDA);
    printf("\t\tEjemplo de la línea de comando: \n");
    printf("\t\t\t./azkaban %s\n", COMANDO_AYUDA);
}

int main(int argc, char *argv[]){
        
    if(argc == 2){
        if(strcmp(argv[1], COMANDO_AYUDA) == 0){
            mostrar_ayuda();
        }
    } else if(argc == 3){
        if(strcmp(argv[1], COMANDO_PERDONABLES) == 0){
            generar_archivo_perdonables(argv[2]);
        } else if(strcmp(argv[1], COMANDO_MOSTRAR_LIBERADOS) == 0){
            mostrar_liberados(argv[2]);
        } else if(strcmp(argv[1], COMANDO_ACTUALIZAR) == 0){
            actualizar_archivos(argv[2]);
        }
    } else if(argc == 5){
        if(strcmp(argv[1], COMANDO_LIBERAR) == 0){
            generar_archivo_liberados(argv[2], argv[3], argv[4]);
        }
    }
    
    return 0;
}
