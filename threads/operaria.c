#include <stdio.h> /* standard I/O routines                 */
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pthread.h> /* pthread functions and data structures */
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

#define backup "/home/joaquim/joaquim/ifb/so/paralegrep/backup_files/"
#define fileset "/home/joaquim/ifb/so/paralegrep/fileset/"
typedef int bool;

#define true 1
#define false 0

typedef struct arquivos arquivo;

struct arquivos{
    int n_vezes;
    time_t alteracao;
    char *arquivo;  //o diretório de cada arquivo
    char *caminho_p_arquivo;
};


int qtd_itens_fila_o=0;

struct operaria t_o; //inicialização global da única thread operaria

int qtd_arq_o = 0;
int tam;

/*Inicializa a lista encadeada de arquivos*/
void lst_cria(struct palavra *FILA)
{
    FILA->prox = NULL;
	tam=0;
}


struct palavra *aloca()
{
	struct palavra *novo=(struct palavra *) malloc(sizeof(struct palavra));
	if(!novo){
		fprintf(stderr, "\nSem memoria disponivel!\n");
		exit(1);
	}else{
		return novo;
	}
}


void libera(struct palavra *FILA)
{   
	if(!vazia(FILA)){
		struct palavra *proxNode, *atual;

		atual = FILA->prox;
        int contador=0;
		while(contador<qtd_itens_fila_o){
			if(contador<qtd_itens_fila_o-1){
                proxNode = atual->prox;
            }else{
                proxNode = NULL;
            }
                
			free(atual);
            if(contador<qtd_itens_fila_o-1) atual = proxNode;
            contador++;
		}
	}
    qtd_itens_fila_o=0;
    FILA->prox=NULL;
}

/*Esta função realiza a inserção de caracteres na fila*/
void insere_caracter(struct palavra *FILA, int valor)
{
    
    struct palavra *novo = aloca();
    
    novo->prox=NULL;
    
    novo->letra=valor;

    //Caso não consiga criar um novo dado retorna nulo
    if (vazia(FILA)){
        FILA->prox=novo;
        //fprintf(stderr, "\nEntrou nO IF  valor do fila: %d\n", FILA->prox->letra);
    } 
    else{
        struct palavra *tmp = FILA->prox;
        while(tmp->prox != NULL) {
            tmp = tmp->prox;
        }
        tmp->prox = novo;
        // struct palavra *tmp = FILA->prox;
        // while(tmp != NULL) {
        //     tmp = tmp->prox;
        // }
        // tmp = novo;
    }
    qtd_itens_fila_o++;
}

int compara(struct palavra *FILA, char *word){
    //fprintf(stderr, "\nIniciando a comparacao qtd de itens na fila %d\n", qtd_itens_fila_o);
    if(vazia(FILA)){
		//fprintf(stderr, "\nFila vazia!\n\n");
		return false;
	}
    char s[qtd_itens_fila_o];
    memset( s, 0x0, qtd_itens_fila_o);

   	struct palavra *tmp;
	tmp = FILA->prox;
    for(int i=0; i<qtd_itens_fila_o; i++){
        s[i]=tmp->letra;
        
        tmp=tmp->prox;
    }
    int tam_word = strlen(word);
    if (tam_word>qtd_itens_fila_o){
        return false;
    }
    int rc = strncmp(s, word, tam_word);
    
    if(rc==0){
        //fprintf(stderr,"\n A comp entre s: %s e word: %s foi igual", s, word);
        return true;
    }else{
        //fprintf(stderr,"\n A comp entre s: %s e word: %s foi diferente", s, word);
        return false;
    }
}

int is_caractere(int ch){
    if (ch == 32) return false;
    if (ch == 10) return false;
    if (ch == 33) return false;
    if (ch == 34) return false;
    if (ch == 40) return false;
    if (ch == 41) return false;
    if (ch == 42) return false;
    if (ch == 44) return false;
    if (ch == 47) return false;
    if (ch == 58) return false;
    if (ch == 59) return false;
    if (ch == 123) return false;
    if (ch == 124) return false;
    if (ch == 125) return false;
    else return true;
}

void ocorrencias(struct arquivo *a, char *word, struct arquivo *lista_arquivos) {  
    fprintf(stderr, "\nIniciando contagem de ocorrencias no arquivo\n");
    FILE *_arquivo_;
    _arquivo_ = fopen(a->arquivo, "r");
    a->n_vezes=0;
    if (_arquivo_ == NULL){
      fprintf(stderr, "\nNão foi possível abrir o novo arquivo...\n");
      exit(1);
    }

    /*Criação da fila que simboliza a palavra*/
    struct palavra *FILA = (struct palavra *) malloc(sizeof(struct palavra));
    if(!FILA){
        fprintf(stderr, "\nNão foi possível criar!\n\nSem memória!\n\n\n");
        exit(1);
    }else{
        inicia(FILA);
    }

    /*Início da leitura e contagem*/
    /*Estou seguindo este tutorial*/
    int ch = fgetc(_arquivo_);
    while (ch != EOF){
        if(is_caractere(ch)==false){
            struct palavra *tmp = FILA->prox;
            int contador=0;
            while (contador<qtd_itens_fila_o){
                tmp=tmp->prox;
                contador++;
            }            
            /*Chamar a função que faz a comparação que deve retornar 1 para igual e 0 para diferente*/
            if(compara(FILA, word)==true){
                a->n_vezes++;
                fprintf(stderr, "\n Comparação foi true e o numero de vezes e: %d", a->n_vezes);
            }
            /*Incrementar o contador que diz quantas vezes a palavra apareceu no arquivo*/
            libera(FILA);
        }else{
            insere_caracter(FILA, ch);
        }
        //fputc(ch, _arquivo_backup_);
        ch = fgetc(_arquivo_);
        }
        libera(FILA);  
        trata_thread_ranking(a, lista_arquivos);
}

void *trata_thread_operaria(struct arquivo *a, char *argumento, struct arquivo *lista_arquivos, int *is_alive)
{   
    is_alive = true;
    ocorrencias(a, argumento, lista_arquivos);
    pthread_exit(NULL);
    is_alive = false;
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
//     flag = pthread_create(&t_o.t_o, NULL, trata_thread_operaria(a), NULL);

//     if (flag != 0)
//         printf("\nErro na criação da thread despachante thread\n");

//     //trata_thread(NULL);

//     pthread_exit(NULL);

//     return 0; /* O programa não vai chegar aqui.         */
// }