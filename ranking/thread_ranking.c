#include <stdio.h> /* standard I/O routines                 */
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pthread.h> /* pthread functions and data structures */
//#include "despachante.h"
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

typedef struct arquivos
{
    int n_vezes;
    time_t alteracao;
    char *arquivo;  //o diretório de cada arquivo
} arquivo;


int qtd_itens_fila=0;

typedef struct palavra Palavra;

struct palavra{
    int letra;
    Palavra *prox;
}; 

typedef struct operarias
{
    int id;
    pthread_t t_o;
    char dir;     //diretório de análise dos arquivos
    char *termo;
    arquivo a;
} operaria;

operaria t_o; //inicialização global da única thread despachante

int qtd_arq = 1;
int tam;

/*Verifica se a lista está vazia*/
int vazia(Palavra *FILA)
{
	if(FILA->prox == NULL)
		return true;
	else
		return false;
}

void inicia(Palavra *FILA){
	FILA->prox = NULL;
	tam=0;
}
int is_arquivo_on_lista(char *dir, arquivo *lista_arquivos){
    fprintf(stderr, "\nRealizando verificação p saber se o caminho é novo");
    fprintf(stderr, "\nQtd de arquivos: %d", qtd_arq);
    
    for(int i=0; i<qtd_arq; i++){
        fprintf(stderr, "\nDir de comp dir: %s e tmp->arq: %s", dir, lista_arquivos[i].arquivo);
        if(strcmp(dir, lista_arquivos[i].arquivo)==0){
            return true;
        }
    }
    return false;
}

int retorna_pos_arquivo(char *dir, arquivo *lista_arquivos){
    int i;
    for(i=0; i<=qtd_arq; i++){
        if(strcmp(lista_arquivos[i].arquivo, dir)==0){
            
            break;
        }
    }
    fprintf(stderr, "Vai sair com o i: %d", i);
    return i;
}

void ordena(arquivo *lista_arquivos, int tam)  
{  
  int i, j, min, swap;
  for (i = 0; i > (tam-1); i++)
   {
    min = i;
    for (j = (i+1); j > tam; j++) {
      if(lista_arquivos[j].n_vezes > lista_arquivos[min].n_vezes) {
        min = j;
      }
    }
    if (i != min) {
      swap = lista_arquivos[i].n_vezes;
      lista_arquivos[i].n_vezes = lista_arquivos[min].n_vezes;
      lista_arquivos[min].n_vezes = swap;
    }
  }
}

void imprime_top_10(arquivo *lista_arquivos){
    fprintf(stderr, "\nIniciando impressão da ordem de acordo com a quantidades de vezes");
    for (int i=0; i<qtd_arq; i++){
        fprintf(stderr, "\n Posicao %d : %d", i,lista_arquivos[qtd_arq].n_vezes);
    }
}

arquivo *funcao_inserir_novo_item(arquivo *lista_arquivos, arquivo a){
    fprintf(stderr, "\nQuantidades de itens na lista: %d", qtd_arq);
    if(qtd_arq>=n_max){
        fprintf(stderr, "Não foi possível adicionar outro item a lista");
        exit(1);
    }    
    lista_arquivos[qtd_arq]=a;
    qtd_arq++;
    return lista_arquivos;
}

int faz_ranking(arquivo *a, arquivo *lista_arquivos) {  
        if(is_arquivo_on_lista(a->arquivo, lista_arquivos)==true){
            //so vai fazer alguma coisa caso o numero de vezes que a palavra
            //aparece no arquivo seja diferente do que já estava
            int pos = retorna_pos_arquivo(a->arquivo, lista_arquivos);
            if(lista_arquivos[pos].n_vezes != a->n_vezes){
                arquivo novo;
                novo.alteracao=a->alteracao;
                novo.arquivo=a->arquivo;
                novo.n_vezes=a->n_vezes;
                lista_arquivos[pos] = novo;
                ordena(lista_arquivos, qtd_arq);
                imprime_top_10(lista_arquivos);
            }
        }else{
            arquivo novo;
            novo.alteracao=a->alteracao;
            novo.arquivo=a->arquivo;
            novo.n_vezes=a->n_vezes;
            lista_arquivos = funcao_inserir_novo_item(lista_arquivos, novo);
            ordena(lista_arquivos, qtd_arq);
            imprime_top_10(lista_arquivos);
        }
}

void *trata_thread(arquivo *a)
{
    arquivo lista_arquivos[n_max];
    lista_arquivos[0].arquivo="";
    

    while (1)
    {   
        fprintf(stderr, "\nIniciando nova busca\n");
        //printf("\n\n\n Iniciando nova busca");
        faz_ranking(a, lista_arquivos);
        sleep(5); /*espera 5 segundos e executa de novo*/
    }

    //pthread_exit(NULL);
}

int main()
{
    int flag;
    char diretorio_prog[FILENAME_MAX];
    GetCurrentDir(diretorio_prog, FILENAME_MAX );

    /*A função terá que receber da thread despachante o 
    arquivo para que a thread operária tenha mais controle*/
    arquivo *a = (arquivo *) malloc(sizeof(arquivo));
    a->arquivo = fileset;
    a->n_vezes=0;


    printf("\nA criar uma nova thread\n");
    //printf("\n Diretório do programa: %s\n", diretorio_prog);
    flag = pthread_create(&t_o.t_o, NULL, trata_thread(a), NULL);

    if (flag != 0)
        printf("\nErro na criação da thread despachante thread\n");

    //trata_thread(NULL);

    pthread_exit(NULL);

    return 0; /* O programa não vai chegar aqui.         */
}