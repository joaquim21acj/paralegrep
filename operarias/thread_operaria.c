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
#define fileset "/home/joaquim/ifb/so/paralegrep/fileset/"
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

int qtd_arq = 0;
int tam;

/*Verifica se a lista está vazia*/
int vazia(Palavra *FILA)
{
	if(FILA->prox == NULL)
		return true;
	else
		return false;
}

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


void libera(Palavra *FILA)
{   
	if(!vazia(FILA)){
		Palavra *proxNode, *atual;

		atual = FILA->prox;
        int contador=0;
		while(contador<qtd_itens_fila){
			if(contador<qtd_itens_fila-1){
                proxNode = atual->prox;
            }else{
                proxNode = NULL;
            }
                
			free(atual);
            if(contador<qtd_itens_fila-1) atual = proxNode;
            contador++;
		}
	}
    qtd_itens_fila=0;
    FILA->prox=NULL;
}

/*Esta função realiza a inserção de caracteres na fila*/
void insere_caracter(Palavra *FILA, int valor)
{
    
    Palavra *novo = aloca();
    
    novo->prox=NULL;
    
    novo->letra=valor;

    //Caso não consiga criar um novo dado retorna nulo
    if (vazia(FILA)){
        FILA->prox=novo;
        //fprintf(stderr, "\nEntrou nO IF  valor do fila: %d\n", FILA->prox->letra);
    } 
    else{
        Palavra *tmp = FILA->prox;
        while(tmp->prox != NULL) {
            tmp = tmp->prox;
        }
        tmp->prox = novo;
        // Palavra *tmp = FILA->prox;
        // while(tmp != NULL) {
        //     tmp = tmp->prox;
        // }
        // tmp = novo;
    }
    qtd_itens_fila++;
}

void inicia(Palavra *FILA){
	FILA->prox = NULL;
	tam=0;
}

int compara(Palavra *FILA, char *word){
    fprintf(stderr, "\nIniciando a comparacao\n");
    if(vazia(FILA)){
		fprintf(stderr, "\nFila vazia!\n\n");
		return false;
	}
    char s[qtd_itens_fila];
    memset( s, 0x0, qtd_itens_fila);

   	Palavra *tmp;
	tmp = FILA->prox;
    for(int i=0; i<qtd_itens_fila; i++){
        s[i]=tmp->letra;
        
        tmp=tmp->prox;
    }

    int rc = strncmp(s, word, qtd_itens_fila);
    
    if(rc==0){
        fprintf(stderr,"\n A comp entre s: %s e word: %s foi igual", s, word);
        return true;
    }else{
        fprintf(stderr,"\n A comp entre s: %s e word: %s foi diferente", s, word);
        return false;
    }
}

int ocorrencias(arquivo *a, char *word) {  
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
            fprintf(stderr, "\nEntrou no if do espaço com %d itens na fila\n", qtd_itens_fila);
            Palavra *tmp = FILA->prox;
            int contador=0;
            while (contador<qtd_itens_fila){
                tmp=tmp->prox;
                contador++;
            }            
            /*Chamar a função que faz a comparação que deve retornar 1 para igual e 0 para diferente*/
            if(compara(FILA, word)){
                a->n_vezes++;
            }
            /*Incrementar o contador que diz quantas vezes a palavra apareceu no arquivo*/
            libera(FILA);
        }else{
            insere_caracter(FILA, ch);
        }
        //fputc(ch, _arquivo_backup_);
        ch = fgetc(_arquivo_);
        }
        fprintf(stderr, "\nQuantidades de vezes que a pal. aparece no arquivo: %d", a->n_vezes);
        libera(FILA);  
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