#include <stdio.h> /* standard I/O routines                 */
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pthread.h> /* pthread functions and data structures */
#include "operaria.h"
#include "ranking.h"
//#include "despachante.h"
#define n_max 100

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#define backup "/home/joaquim/ifb/so/paralegrep/backup_files/"
#define fileset "/home/joaquim/ifb/so/paralegrep/fileset/"
typedef int bool;

#define true 1
#define false 0

// typedef struct arquivos arquivo;

// struct arquivos
// {
//     time_t alteracao;
//     int n_vezes; //Qtas vezes a palavra aparece no arquivo
//     char *arquivo;  //o diretório de cada arquivo
//     char *caminho_p_arquivo;
// };



typedef struct despachantes
{
    int id;
    pthread_t t_d;
    char dir;     //diretório de análise dos arquivos
    pthread_t t_o[10]; //numero de threads operarias funcionando
    pthread_mutex_t mutex[10];
    int is_alive[10];
} despachante;

despachante t_d; //inicialização global da única thread despachante

int qtd_arq_d = 0;

/*Função para concatenar ponteiro char*/
char * concatenar(char *original, char *add) {

    int tamanho = strlen(original) + strlen(add) + 1;
    char *s = (char *) malloc(sizeof(char)*tamanho);

    strcpy(s, original);
    strcat(s, add);
    return s;
    //return final;
}



int retorna_pos_arquivo_d(char *dir, struct arquivo *lista_arquivos){
    int i;
    for(i=0; i<=qtd_arq_d; i++){
        if(strcmp(lista_arquivos[i].arquivo, dir)==0){
            
            break;
        }
    }
    fprintf(stderr, "Vai sair com o i: %d", i);
    return i;
}

/*Esta função realiza a inserção de novos arquivos na lista de arquivos da thread*/
struct arquivo insere_arquivo(struct arquivo *lista_arquivos, char *dir_arq, char *nome_arq, int is_novo)
{
    if(qtd_arq_d==(n_max-1)){
        fprintf(stderr, "Foi atingido o tamanho máximo de arquivos");

        exit(1);
    }

    struct arquivo novo;
    //Caso não consiga criar um novo dado retorna nulo
               
        novo.arquivo = nome_arq;
        novo.caminho_p_arquivo=dir_arq;

        /*Junta o caminho p/ a pasta de arquivos de backup + o nome do arquivo backup*/
        // char *back_file_path = backup;
        // char *temp1 = concatenar(back_file_path, nome_arq);
        //novo->arquivo_backup = temp1;
        
        struct stat st;
        char *tmp = concatenar(dir_arq, nome_arq);
        if(stat(tmp, &st)){
            perror("stat");
            exit(1);
        }
        time_t t = st.st_mtime; /*st_mtime is type time_t */
        novo.alteracao = t;
        novo.n_vezes = 0; /*Verifica a posição que o arquivo vai ser coloado
        se for um novo coloca na proxima posição
        se for atualização coloca na posição antiga*/
        if(is_novo==false){
            int pos = retorna_pos_arquivo_d(nome_arq, lista_arquivos);
            lista_arquivos[pos]=novo;
            fprintf(stderr, "\narquivo atualizado %s", lista_arquivos[pos].arquivo);
        }else{
            lista_arquivos[qtd_arq_d]=novo;
            qtd_arq_d++;
            fprintf(stderr, "\nFoi inserido um novo arquivo na lista %s", lista_arquivos[qtd_arq_d-1].arquivo);
        }


        return novo;
}

int caminho_eh_novo(char *dir, struct arquivo *lista_arquivos){
    fprintf(stderr, "\nRealizando verificação p saber se o caminho é novo");
    fprintf(stderr, "\nQtd de arquivos: %d", qtd_arq_d);
    
    for(int i=0; i<qtd_arq_d; i++){
        fprintf(stderr, "\nDir de comp dir: %s e tmp->arq: %s", dir, lista_arquivos[i].arquivo);
        if(strcmp(dir, lista_arquivos[i].arquivo)==0){
            return false;
        }
    }
    return true;
}

int arquivo_foi_modificado(struct arquivo *lista_arquivos, char *dir){
    struct stat st; 
    int i;
    for(i=0; i<=qtd_arq_d; i++){
        fprintf(stderr, "\n Indo comparar a lista: %s  e o dir %s",lista_arquivos[i].arquivo, dir);
        if(strcmp(lista_arquivos[i].arquivo, dir)==0){
            char *diretorio = lista_arquivos[i].caminho_p_arquivo;
            char *temp=concatenar(diretorio, lista_arquivos[i].arquivo);
            if(stat(temp, &st)== -1){
                perror("stat");
                exit(1);
            }
            fprintf(stderr, "Vai sair com o i: %d", i);
            break;
        }
    }
    fprintf(stderr, "Saiu com o i: %d", i);

    //time_t t = st.st_mtime; /*st_mtime is type time_t */
    if (st.st_mtime > lista_arquivos[i].alteracao){
        return true;
    }else{  
        return false;
    }
}
void acorda_thread_operaria(struct arquivo *a, char *argumento, struct arquivo *lista_ranking){
    int i;

    for (int i=0; i<10; i++){
        if(t_d.is_alive[i]==false){
            pthread_create(&t_d.t_o[i], NULL, trata_thread_operaria(a, argumento, lista_ranking, t_d.is_alive[i]), NULL);
        }
    }
}

void vasculha_dir(char *dir_int, int prof, struct arquivo *lista_arquivos, char *argumento, struct arquivo *lista_ranking){
    DIR *d;
    struct dirent *dir;
    d = opendir(dir_int);
    if (d) {
        fprintf(stderr, "\nVasculhando diretorio");
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(".", dir->d_name) == 0 ||
                 strcmp("..", dir->d_name) == 0)
                 continue;

            if(caminho_eh_novo(dir->d_name, lista_arquivos)){
                fprintf(stderr, "\nCaminho é novo");
                struct arquivo novo_arquivo = insere_arquivo(lista_arquivos, dir_int, dir->d_name, true);
                struct arquivo *a_n = &novo_arquivo;
                acorda_thread_operaria(a_n, argumento, lista_ranking);
            }else{
                if(arquivo_foi_modificado(lista_arquivos, dir->d_name)){
                    fprintf(stderr, "\narquivo foi alterado");
                    struct arquivo novo_arquivo =insere_arquivo(lista_arquivos, dir_int, dir->d_name, false);
                    struct arquivo *a_n = &novo_arquivo;
                    acorda_thread_operaria(a_n, argumento, lista_ranking);
                }else{
                    fprintf(stderr, "\narquivo não foi modificado");
                }
            }
        }
    closedir(d);
    }
}

void ler_arquivos(char *argv, char *tip_open)
{
    /* should check that argc > 1 */
    FILE *file = fopen(argv, tip_open); /*file open fileName with tag r to read content  */
    char line[10];

    while (fgets(line, sizeof(line), file))
    {
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        printf("%s", line);
    }
    /* may check feof here to make a difference between eof and io failure -- network
       timeout for instance */

    fclose(file);
}

void *trata_thread(char *caminho, char *argumento, struct arquivo *lista_ranking)
{   
    
    struct arquivo lista_arquivos[n_max];
    /*
        lista_arquivos = insere_arqu#define fileset "/home/joaquim/ifb/so/paralegrep/fileset/"ivo(lista_arquivos, )
        colocar essa linha de código na parte que reconhece os arquivos

    */
    while (1)
    {  
        vasculha_dir(caminho, 0, lista_arquivos, argumento, lista_ranking);
        sleep(5); /*espera 5 segundos e executa de novo*/
    }

    //pthread_exit(NULL);
}

// int main()
// {
//     int flag;
//     char diretorio_prog[FILENAME_MAX];
//     GetCurrentDir(diretorio_prog, FILENAME_MAX );

//     char *argumento;

//     printf("\nA criar uma nova thread\n");
//     printf("\n Diretório do programa: %s\n", diretorio_prog);
//     flag = pthread_create(&t_d.t_d, NULL, trata_thread(fileset, argumento), NULL);
//     //trata_thread(NULL);

//     pthread_exit(NULL);

//     return 0; /* O programa não vai chegar aqui.         */
// }