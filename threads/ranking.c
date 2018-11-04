#include <stdio.h> /* standard I/O routines                 */
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pthread.h> /* pthread functions and data structures */
#include "operaria.h"
#define n_max 100

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#define backup "/home/joaquim/joaquim/ifb/so/paralegrep/backup_files/"
#define fileset "/home/joaquim/ifb/so/paralegrep/fileset/file.txt"
typedef int bool;

#define true 1
#define false 0

// typedef struct arquivos
// {
//     int n_vezes;
//     time_t alteracao;
//     char *arquivo;  //o diretório de cada arquivo
// } arquivo;
pthread_mutex_t lock;

int qtd_itens_fila_r=0;

struct palavra{
    int letra;
    struct palavra *prox;
}; 

struct rankings{
    int id;
    pthread_t t_r;
    char dir;     //diretório de análise dos arquivos
    char *termo;
    struct arquivo a;
};
struct rankings t_r; //inicialização global da única thread despachante

int qtd_arq_r = 1;
int tam;

/*Verifica se a lista está vazia*/
int vazia(struct palavra *FILA)
{
	if(FILA->prox == NULL)
		return true;
	else
		return false;
}

void inicia(struct palavra *FILA){
	FILA->prox = NULL;
	tam=0;
}
int is_arquivo_on_lista(char *dir, struct arquivo *lista_arquivos){
    fprintf(stderr, "\nQtd de arquivos: %d", qtd_arq_r);
    
    for(int i=0; i<qtd_arq_r; i++){
        fprintf(stderr, "\nDir de comp dir: %s e tmp->arq: %s", dir, lista_arquivos[i].arquivo);
        if(strcmp(dir, lista_arquivos[i].arquivo)==0){
            fprintf(stderr, "\nArquivo esta na lista: %s", dir);
            return true;
        }
    }
    fprintf(stderr, "\nArquivo não esta na lista: %s", dir);
    return false;
}

int retorna_pos_arquivo(char *dir, struct arquivo *lista_arquivos){
    int i;
    for(i=0; i<=qtd_arq_r; i++){
        if(strcmp(lista_arquivos[i].arquivo, dir)==0){
            
            break;
        }
    }
    fprintf(stderr, "Vai sair com o i: %d", i);
    return i;
}

/*Esta função ta ordenando o menor primeiro, mas deveria ordenar para o maior primeiro*/
void ordena(struct arquivo *lista_arquivos, int tam)  
{  
for (int i = 1; i < tam; i++) {
    for (int j = 0; j < i; j++) {
        if (lista_arquivos[i].n_vezes > lista_arquivos[j].n_vezes) {
            struct arquivo temp = lista_arquivos[i];
            lista_arquivos[i] = lista_arquivos[j];
            lista_arquivos[j] = temp;
        }
    }
}
    fprintf(stderr, "\n ordenou e o primeiro item foi o %s", lista_arquivos[0].arquivo);
    fprintf(stderr, "\n ordenou e o primeiro item foi o %s", lista_arquivos[1].arquivo);
}

void imprime_top_10(struct arquivo *lista_arquivos){
    fprintf(stderr, "\nIniciando impressão da ordem de acordo com a quantidades de vezes");
    for (int i=0; i<qtd_arq_r; i++){
        fprintf(stderr, "\n Posicao %d : arquivo: %s n_vezes: %d", i, lista_arquivos[i].arquivo, lista_arquivos[i].n_vezes);
    }
}

struct arquivo *funcao_inserir_novo_item(struct arquivo *lista_arquivos, struct arquivo a){
    fprintf(stderr, "\nQuantidades de itens na lista: %d", qtd_arq_r);
    if(qtd_arq_r>=n_max){
        fprintf(stderr, "Não foi possível adicionar outro item a lista");
        exit(1);
    }    
    lista_arquivos[qtd_arq_r]=a;
    fprintf(stderr, "\nFoi inserido na lista %s", lista_arquivos[qtd_arq_r].arquivo);
    qtd_arq_r++;
    return lista_arquivos;
}

void faz_ranking(struct arquivo *a, struct arquivo *lista_arquivos) {  
        if(is_arquivo_on_lista(a->arquivo, lista_arquivos)==true){
            //so vai fazer alguma coisa caso o numero de vezes que a palavra
            //aparece no arquivo seja diferente do que já estava
            int pos = retorna_pos_arquivo(a->arquivo, lista_arquivos);
            if(lista_arquivos[pos].n_vezes != a->n_vezes){
                struct arquivo novo;
                novo.alteracao=a->alteracao;
                novo.arquivo=a->arquivo;
                novo.n_vezes=a->n_vezes;
                lista_arquivos[pos] = novo;
                ordena(lista_arquivos, qtd_arq_r);
                a = NULL;
                imprime_top_10(lista_arquivos);
            }
        }else{
            struct arquivo novo;
            novo.alteracao=a->alteracao;
            novo.arquivo=a->arquivo;
            novo.n_vezes=a->n_vezes;
            lista_arquivos = funcao_inserir_novo_item(lista_arquivos, novo);
            ordena(lista_arquivos, qtd_arq_r);
            a = NULL;
            imprime_top_10(lista_arquivos);
        }
}

int is_lista_vazia(struct arquivo *a){
    if(a==NULL){
        return true;
    }
    return false;
}

void *trata_thread_ranking(struct arquivo *a, struct arquivo *lista_arquivos)
{

    fprintf(stderr, "\nArquivo recebido %s", a->arquivo);
    fprintf(stderr, "\nQtd de vezes %d", a->n_vezes);
    

    while (1)
    {   
        fprintf(stderr, "\nIniciando nova busca\n");
        //printf("\n\n\n Iniciando nova busca");
        if(is_lista_vazia(a)==false){
            pthread_mutex_lock(&lock);
            faz_ranking(a, lista_arquivos);
            pthread_mutex_unlock(&lock);
        }
        sleep(5); /*espera 5 segundos e executa de novo*/
    }

    //pthread_exit(NULL);
}

// int main()
// {
//     int flag;
//     char diretorio_prog[FILENAME_MAX];
//     GetCurrentDir(diretorio_prog, FILENAME_MAX );

//     /*A função terá que receber da thread despachante o 
//     arquivo para que a thread operária tenha mais controle*/
//     arquivo *a = (arquivo *) malloc(sizeof(arquivo));
//     a->arquivo = fileset;
//     a->n_vezes=0;


//     printf("\nA criar uma nova thread\n");
//     //printf("\n Diretório do programa: %s\n", diretorio_prog);
//     flag = pthread_create(&t_r.t_r, NULL, trata_thread_ranking(a), NULL);

//     if (flag != 0)
//         printf("\nErro na criação da thread despachante thread\n");

//     //trata_thread(NULL);

//     pthread_exit(NULL);

//     return 0; /* O programa não vai chegar aqui.         */
// }