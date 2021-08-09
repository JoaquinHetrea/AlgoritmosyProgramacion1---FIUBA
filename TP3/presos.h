#define MAX_NOMBRE 200
#define MAX_FECHA 7
#define COMANDO_PERDONABLES "perdonables"
#define COMANDO_LIBERAR "liberar"
#define COMANDO_MOSTRAR_LIBERADOS "mostrar_liberados"
#define COMANDO_AYUDA "ayuda"
#define COMANDO_ACTUALIZAR "actualizar"

typedef struct preso{
    char nombre[MAX_NOMBRE];
    unsigned int edad;
    char conducta;
    unsigned int pabellon;
    unsigned int celda;
    char fecha[MAX_FECHA]; //aaaamm
    int maldicion_realizada;
} preso_t;

size_t leer_preso(preso_t* preso, FILE* archivo);

size_t escribir_preso(preso_t preso, FILE* archivo);

int leer_nombre(char nombre[], FILE* archivo);

int escribir_nombre(char nombre[], FILE* archivo);

void inicializar_nombre_archivo(char fecha[], char nombre_archivo[]);

int generar_archivo_perdonables(char nombre_archivo[]);

int generar_archivo_liberables(char nombre_archivo_perdonables[], char fecha[], char* conducta);

int mostrar_liberados(char fecha[]);

int actualizar_archivos_auxiliar(char nombre_archivo_liberables[], char nombre_archivo_presos[]);

int actualizar_archivos(char fecha[]);

void mostrar_ayuda();
