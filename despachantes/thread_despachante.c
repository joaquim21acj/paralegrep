#include <stdio.h>       /* standard I/O routines                 */
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pthread.h>     /* pthread functions and data structures */
//#include "despachante.h"
#define n_max 100

typedef struct arquivos Arquivo;

struct arquivos{
    int numero_arq; //serve como contador e como id
    char *arquivo;//o diretório de cada arquivo
    Arquivo* prox;
};

typedef struct despachantes{
    int id;
    pthread_t t_d;
    char dir;//diretório de análise dos arquivos
    int t_op [10]; //numero de threads operarias funcionando
} despachante;

despachante t_d;//inicialização global da única thread despachante

/*Esta função realiza a inserção de novos arquivos na lista de arquivos da thread*/
void insere_arquivo(char *dir_arq){
    Arquivo* novo = (Arquivo*) malloc(sizeof(Arquivo));

    // for(int perc_arquivos = 0; perc_arquivos<=n_max; perc_arquivos++){
    //     fprintf(stderr,"Arquivo %d com o diretório: %s\n", perc_arquivos, t_d.n_files[perc_arquivos]->arquivo);
    // }
}

/*Inicializa a lista encadeada de arquivos*/
Arquivo* lst_cria(void){
    return NULL;
}

/* Função varre o diretório e pesquisa todos os arquivos, também serve para subdiretórios*/
void vasculha_dir(char *dir_int, int prof){            
        DIR *dp;
        struct dirent *entry;
        struct stat statbuf;
        //Caso não consiga abrir uma pasta entra 
        if((dp = opendir(dir_int)) == NULL) {
            //chama a função que faz a inserção do arquivo na struct 
            fprintf(stderr,"cannot open directory: %s\n", dir_int);
            novos_arquivos(dir_int);
            return;
        }
        chdir(dir_int);
        while((entry = readdir(dp)) != NULL) {
            lstat(entry->d_name,&statbuf);
            if(S_ISDIR(statbuf.st_mode)) {
                /* Found a directory, but ignore . and .. */
                if(strcmp(".",entry->d_name) == 0 ||
                    strcmp("..",entry->d_name) == 0)
                    continue;
                printf("%*s%s/\n",prof,"",entry->d_name);
                /* Recurse at a new indent level */
                vasculha_dir(entry->d_name,prof+1);
            }
            else printf("%*s%s\n",prof,"",entry->d_name);
        }
        chdir("..");
        closedir(dp);
}




void ler_arquivos(char *argv, char *tip_open){
     /* should check that argc > 1 */
    FILE* file = fopen(argv, tip_open); /*file open fileName with tag r to read content  */
    char line[10];

    while (fgets(line, sizeof(line), file)) {
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        printf("%s", line); 
    }
    /* may check feof here to make a difference between eof and io failure -- network
       timeout for instance */

    fclose(file);
}

void *trata_thread(char *caminho)    {
    while(1){
        vasculha_dir(caminho, 0);
        sleep(5);/*espera 5 segundos e executa de novo*/
   }
   
   //pthread_exit(NULL);

}

 

int main ( ) 

{


   printf("A criar uma nova thread\n");
    int flag;
   flag = pthread_create(&t_d.t_d, NULL, trata_thread("/home/joaquim/joaquim/ifb/so/files/file.txt"), NULL);

   if (flag!=0) printf("Erro na criação da thread despachante thread\n");

   

   //trata_thread(NULL);

   pthread_exit(NULL);

   return 0;   /* O programa não vai chegar aqui.         */ 

}