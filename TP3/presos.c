#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NOMBRE 200
#define MAX_FECHA 7
#define CONDUCTA_BUENA 'B'
#define CONDUCTA_REGULAR 'R'
#define CONDUCTA_MALA 'M'
#define CRUCIO 1
#define IMPERIUS 2
#define MAX_NOMBRE_ARCHIVO 30

#define ARCHIVO_CRUCIO "crucio.dat"
#define ARCHIVO_IMPERIUS "imperius.dat"
#define ARCHIVO_PERDONABLES "perdonables.dat"
#define ARCHIVO_LIBERABLES "liberables_"
#define EXTENSION_LIBERABLES ".txt"

#define COMANDO_PERDONABLES "perdonables"
#define COMANDO_LIBERAR "liberar"
#define COMANDO_MOSTRAR_LIBERADOS "mostrar_liberados"
#define COMANDO_AYUDA "ayuda"
#define COMANDO_ACTUALIZAR "actualizar"

size_t leer_preso(preso_t* preso, FILE* archivo){
    return fread(preso, sizeof(preso_t), 1, archivo);
}

size_t escribir_preso(preso_t preso, FILE* archivo){
    return fwrite(&preso, sizeof(preso_t), 1, archivo);
}

int leer_nombre(char nombre[], FILE* archivo){
    return fscanf(archivo, "%[^\n]\n", nombre);
}

int escribir_nombre(char nombre[], FILE* archivo){
    return fprintf(archivo, "%s\n", nombre);
}

void inicializar_nombre_archivo(char fecha[], char nombre_archivo[]){
    strcat(nombre_archivo, ARCHIVO_LIBERABLES);
    strcat(nombre_archivo, fecha);
    strcat(nombre_archivo, ".txt"); 
}

/*
    Genera un archivo binario con aquellos presos a perdonar y con el nombre
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
     
    leido_crucio = leer_preso(&preso_crucio, crucio);
    leido_imperius = leer_preso(&preso_imperius, imperius);
    while((leido_imperius != 0) && (leido_crucio != 0) && (pude_escribir)){
        if(strcmp(preso_crucio.nombre, preso_imperius.nombre) == 0){
            leido_crucio = leer_preso(&preso_crucio, crucio);
            leido_imperius = leer_preso(&preso_imperius, imperius);
        } else if(strcmp(preso_crucio.nombre, preso_imperius.nombre) > 0){
            pude_escribir = escribir_preso(preso_imperius, perdonables);
            leido_imperius = leer_preso(&preso_imperius, imperius);
        } else {
            pude_escribir = escribir_preso(preso_crucio, perdonables);
            leido_crucio = leer_preso(&preso_crucio, crucio);          
        }
    }
    
    while((leido_imperius != 0) && (pude_escribir)){
        pude_escribir = escribir_preso(preso_imperius, perdonables);
        leido_imperius = leer_preso(&preso_imperius, imperius);
    }
    
    while((leido_crucio != 0) && (pude_escribir)){
        pude_escribir = escribir_preso(preso_crucio, perdonables);
        leido_crucio = leer_preso(&preso_crucio, crucio);
    }
    
    fclose(crucio);
    fclose(imperius);
    fclose(perdonables);
    return 0;
}

/*
    Creará un archivo de texto con los nombres de los presos que cumplan con
    las condiciones necesarias.
*/
int generar_archivo_liberables(char nombre_archivo_perdonables[], char fecha[], char* conducta){
    FILE* perdonables = fopen(nombre_archivo_perdonables, "r");
    if(perdonables == NULL){
        printf("Error al abrir el archivo %s\n", nombre_archivo_perdonables);
        return -1;
    }
    
    char nombre_archivo_liberables[MAX_NOMBRE_ARCHIVO];
    inicializar_nombre_archivo(fecha, nombre_archivo_liberables);
    
    FILE* liberables = fopen(nombre_archivo_liberables, "r");
    if(liberables != NULL){
        printf("El archivo de liberables con esa fecha ya existe.\n");
        fclose(perdonables);
        fclose(liberables);
        return -1;
    } else {        
        liberables = fopen(nombre_archivo_liberables, "w");
        if(liberables == NULL){
            printf("Error al abrir el archivo %s\n.", nombre_archivo_liberables);
            fclose(perdonables);
            return -1;
        }
        
        int fecha_liberacion = atoi(fecha);
        preso_t preso_actual;
        size_t leido = leer_preso(&preso_actual, perdonables);
        bool pude_escribir = true;
        while((leido != 0) && (pude_escribir)){
            if(atoi(preso_actual.fecha) <= fecha_liberacion){
                switch(*conducta){
                    case CONDUCTA_BUENA:
                        if(preso_actual.conducta == CONDUCTA_BUENA){
                            pude_escribir = escribir_nombre(preso_actual.nombre, liberables);
                        }
                    break;
                    case CONDUCTA_REGULAR:
                        if(preso_actual.conducta == CONDUCTA_BUENA || preso_actual.conducta == CONDUCTA_REGULAR){
                            pude_escribir = escribir_nombre(preso_actual.nombre, liberables);
                        }
                    break;
                    case CONDUCTA_MALA:
                        pude_escribir = escribir_nombre(preso_actual.nombre, liberables);
                    break;
                }
            }
            leido = leer_preso(&preso_actual, perdonables);
        }
    }

    fclose(perdonables);
    fclose(liberables);
    return 0;
}

/*
    Mostrará por pantalla el archivo de presos liberados en 
    un determinado año/mes.
*/
int mostrar_liberados(char fecha[]){
    char preso_actual[MAX_NOMBRE];
    char nombre_archivo[MAX_NOMBRE_ARCHIVO];
    inicializar_nombre_archivo(fecha, nombre_archivo);

    FILE* liberados = fopen(nombre_archivo, "r");
    if(liberados == NULL){
        printf("Error al abrir el archivo %s\n", nombre_archivo);
        return -1;
    }
    
    bool ha_leido = leer_nombre(preso_actual, liberados);
    while(ha_leido){
        printf("%s\n", preso_actual);
        ha_leido = leer_nombre(preso_actual, liberados);
    }
    
    fclose(liberados);
    return 0;
}

int actualizar_archivos_auxiliar(char nombre_archivo_liberables[], char nombre_archivo_presos[]){
    FILE* liberados = fopen(nombre_archivo_liberables, "r");
    if(liberados == NULL){
        printf("Error al abrir el archivo %s\n", nombre_archivo_liberables);
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
    
    int leido_liberados = leer_nombre(nombre_preso, liberados);
    size_t leido_presos = leer_preso(&preso_actual, archivo_presos);
    bool pude_escribir = true;
    
    while((leido_liberados > 0) && (leido_presos > 0) && (pude_escribir)){
        if(strcmp(nombre_preso, preso_actual.nombre) == 0){
            leido_liberados = leer_nombre(nombre_preso, liberados);
            leido_presos = leer_preso(&preso_actual, archivo_presos);
        } else if(strcmp(nombre_preso, preso_actual.nombre) > 0){
            pude_escribir = escribir_preso(preso_actual, nuevo_archivo);
            leido_presos = leer_preso(&preso_actual, archivo_presos);
        } else {
            leido_liberados = leer_nombre(nombre_preso, liberados);
        }
    }
    
    while((leido_presos > 0) && (pude_escribir)){
        pude_escribir = escribir_preso(preso_actual, nuevo_archivo);
        leido_presos = leer_preso(&preso_actual, archivo_presos);
    }
    
    fclose(liberados);
    fclose(archivo_presos);
    remove(nombre_archivo_presos);
    rename("archivo_auxiliar.dat", nombre_archivo_presos);
    fclose(nuevo_archivo);
    return 0;
}

/*
    Actualizará ambos archivos binarios en los cuales figuran los presos que
    realizaron cada maldición.
*/
int actualizar_archivos(char fecha[]){
    char nombre_archivo_liberables[MAX_NOMBRE_ARCHIVO];
    inicializar_nombre_archivo(fecha, nombre_archivo_liberables);   
    actualizar_archivos_auxiliar(nombre_archivo_liberables, ARCHIVO_CRUCIO);
    actualizar_archivos_auxiliar(nombre_archivo_liberables, ARCHIVO_IMPERIUS);
    return 0;
}

/*
    Muestra por pantalla el listado de comandos y explica al usuario cómo
    se utilizan.
*/
void mostrar_ayuda(){
    printf("Comando PERDONABLES: \n");
    printf("\tDe dos archivos con los presos que realizaron cada maldición, crea uno con aquellos presos que pueden ser perdonados.\n");
    printf("\t\tComando identificador: '%s'\n", COMANDO_PERDONABLES);
    printf("\t\tArgumentos: \n");
    printf("\t\t\t* 'nombre que quiere que tenga el nuevo archivo'\n");
    printf("\t\tEjemplo: \n");
    printf("\t\t\t./azkaban perdonables <nombre_archivo>\n\n");
    printf("Comando LIBERAR: \n");
    printf("\tCreará un archivo de texto col los nombres de los presos que cumplan con las condiciones necesarias.\n");
    printf("\t\tComando identificador: '%s'\n", COMANDO_LIBERAR);
    printf("\t\tArgumentos: \n");
    printf("\t\t\t* Nombre del archivo de presos perdonables.\n");
    printf("\t\t\t* año/mes (formato aaaamm)\n");
    printf("\t\t\t* conducta (%c: buena - %c: regular - %c: mala)\n", CONDUCTA_BUENA, CONDUCTA_REGULAR, CONDUCTA_MALA);
    printf("\t\tEjemplo de la línea de comando: \n");
    printf("\t\t\t./azkaban liberar <archivo_perdonables>.dat 201906 B\n\n");
    printf("Comando ACTUALIZAR: \n");
    printf("\tActualizará ambos archivos binaios en los cuales figuran los presos que realizaron cada maldición.\n");
    printf("\t\tComando identificador: '%s'\n", COMANDO_ACTUALIZAR);
    printf("\t\tArgumentos: \n");
    printf("\t\t\t* Fecha del archivo que se toma para actualizar.\n");
    printf("\t\tEjemplo de la línea de comando: \n");
    printf("\t\t\t./azkaban actualizar 201906\n\n");
    printf("Comando MOSTRAR LIBERADOS: \n");
    printf("\tMostrará por pantalla el archivo de presos liberados en un determinado año/mes.\n");
    printf("\t\tComando identificador: '%s'\n", COMANDO_MOSTRAR_LIBERADOS);
    printf("\t\tArgumentos: \n");
    printf("\t\t\t* Fecha del archivo que se quiere mostrar.\n");
    printf("\t\tEjemplo de la línea de comando: \n");
    printf("\t\t\t./azkaban mostrar_liberados 201906\n");
}

