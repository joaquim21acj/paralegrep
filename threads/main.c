#include <stdio.h> /* standard I/O routines                 */
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pthread.h> /* pthread functions and data structures */
//#include "despachante.h"
#include "operaria.h"
#include "ranking.h"
#define n_max 100

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#define backup "/home/joaquim/joaquim/ifb/so/paralegrep/backup_files/"
#define fileset "/home/joaquim/ifb/so/paralegrep/fileset/cabernet.txt"
typedef int bool;

#define true 1
#define false 0

struct ranking t_r; //inicialização global da única thread despachante

struct operaria t_o; //inicialização global da única thread operaria

pthread_t t_o;

pthread_t t_r;

int main()
{
    int flag_operaria;
    char diretorio_prog[FILENAME_MAX];
    GetCurrentDir(diretorio_prog, FILENAME_MAX );

    /*A função terá que receber da thread despachante o 
    arquivo para que a thread operária tenha mais controle*/
    struct arquivo *a = (struct arquivo *) malloc(sizeof(struct arquivo));
    a->arquivo = fileset;
    a->n_vezes=0;


    struct arquivo lista_arquivos[n_max];
    lista_arquivos[0].arquivo="";
    lista_arquivos[0].n_vezes=0;

    printf("\nA criar uma nova thread\n");
    //printf("\n Diretório do programa: %s\n", diretorio_prog);
    flag_operaria = pthread_create(&t_o, NULL, trata_thread_operaria(a, "para", lista_arquivos), NULL);

    if (flag_operaria != 0)
        printf("\nErro na criação da thread operaria thread\n");

    //trata_thread(NULL);

    return 0; /* O programa não vai chegar aqui.         */
}
