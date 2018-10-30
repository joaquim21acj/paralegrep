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
#define fileset "/home/joaquim/joaquim/ifb/so/paralegrep/fileset/file.txt"
typedef int bool;

#define true 1
#define false 0

typedef struct arquivos
{
    int numero_arq; //serve como contador e como id
    int n_vezes;
    char *arquivo;  //o diretório de cada arquivo
    char *arquivo_backup; //o diretório de backup do arquivo
} arquivo;

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

int qtd_arq = 0;
int tam;


/*Inicializa a lista encadeada de arquivos*/
Palavra* lst_cria(Palavra *FILA)
{
    FILA->prox = NULL;
	tam=0;
}


Palavra *aloca()
{
	Palavra *novo=(Palavra *) malloc(sizeof(Palavra));
	if(!novo){
		fprintf(stderr, "\nSem memoria disponivel!\n");
		exit(1);
	}else{
		return novo;
	}
}

/*Esta função realiza a inserção de novos arquivos na lista de arquivos da thread*/
int insere_caracter(Palavra *lista_arquivos, int valor)
{
    Palavra *novo = (Palavra *)malloc(sizeof(Palavra));
    //Caso não consiga criar um novo dado retorna nulo
    if (novo == NULL) return false;
    else{
        novo->letra=valor;
        novo->prox=lista_arquivos;
        return true;
    }
}

void inicia(Palavra *FILA){
	FILA->prox = NULL;
	tam=0;
}

int ocorrencias(arquivo *a, char *palavra) {  
    fprintf(stderr, "\nIniciando contagem de ocorrencias no arquivo\n");
    FILE *_arquivo_;
    _arquivo_ = fopen(a->arquivo, "r");
    if (_arquivo_ == NULL){
      fprintf(stderr, "\nNão foi possível abrir o novo arquivo...\n");
      exit(1);
    }

    /*Criação da fila que simboliza a palavra*/
    Palavra *FILA = (Palavra *) malloc(sizeof(Palavra));
    if(!FILA){
        fprintf(stderr, "\nNão foi possível criar!\n\nSem memória!\n\n\n");
        exit(1);
    }else{
        inicia(FILA);
    }

    /*Início da leitura e contagem*/
    /*Estou seguindo este tutorial
    https://www.cprogressivo.net/2014/05/Filas-em-C-Como-Programar-Tutorial-Estrutura-de-Dados-Dinamica-Queue.html*/
    int ch = fgetc(_arquivo_);
    while (ch != EOF){
        if((ch == 32)||(ch == 10)){
            /*Chamar a função que faz a comparação*/
            /*Incrementar o contador que diz quantas vezes a palavra apareceu no arquivo*/
            /*Chamar a função que limpa a lista*/
        }


        fprintf(stderr, "%d", ch);//10 == space
        fprintf(stderr, "%c", ch);
        //fputc(ch, _arquivo_backup_);
        ch = fgetc(_arquivo_);
        }
}

void *trata_thread(arquivo *a)
{

    while (1)
    {   
        fprintf(stderr, "\nIniciando nova busca\n");
        //printf("\n\n\n Iniciando nova busca");
        ocorrencias(a, "asdf");
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


    printf("\nA criar uma nova thread\n");
    printf("\n Diretório do programa: %s\n", diretorio_prog);
    flag = pthread_create(&t_o.t_o, NULL, trata_thread(a), NULL);

    if (flag != 0)
        printf("\nErro na criação da thread despachante thread\n");

    //trata_thread(NULL);

    pthread_exit(NULL);

    return 0; /* O programa não vai chegar aqui.         */
}