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


int is_arquivo_on_lista(char *dir, arquivo *lista_arquivos){
    fprintf(stderr, "\nRealizando verificação p saber se o caminho é novo");
    fprintf(stderr, "\nQtd de arquivos: %d", qtd_arq);
    
    for(int i=0; i<qtd_arq; i++){
        fprintf(stderr, "\nDir de comp dir: %s e tmp->arq: %s", dir, lista_arquivos[i].arquivo);
        if(strcmp(dir, lista_arquivos[i].arquivo)==0){
            return false;
        }
    }
    return true;
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

int faz_ranking(arquivo *a, arquivo *lista_arquivos) {  
    if(is_arquivo_on_lista(a->arquivo, lista_arquivos)){
        //so vai fazer alguma coisa caso o numero de vezes que a palavra
        //aparece no arquivo seja diferente do que já estava
        int pos = retorna_pos_arquivo(a, lista_arquivos);
        if(lista_arquivos[pos].n_vezes != a->n_vezes){
            arquivo novo;
            novo.alteracao=a->alteracao;
            novo.arquivo=a->arquivo;
            novo.n_vezes=a->n_vezes;
            lista_arquivos[pos] = novo;
            lista_arquivos = ordena(lista_arquivos);
            chama a função para mostrar novo ranking;
        }
    }else{
        lista_arquivos = funcao_inserir_novo_item(lista_arquivos, a);
        lista_arquivos = ordena(lista_arquivos);
        chama a função para mostrar novo ranking;


    }
}

void *trata_thread(arquivo *a)
{
    arquivo lista_arquivos[n_max];

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