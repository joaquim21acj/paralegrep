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
    int numero_arq; //serve como contador e como id
    int n_vezes;
    char *arquivo;  //o diretório de cada arquivo
    char *arquivo_backup; //o diretório de backup do arquivo
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
                
            fprintf(stderr, "\nLimpando o %d item %d", atual->letra, contador);
			free(atual);
            if(contador<qtd_itens_fila-1) atual = proxNode;
            contador++;
            fprintf(stderr, "\nFinalizou um loop");
		}
	}
    fprintf(stderr, "\n\nsaiu do libera");
    qtd_itens_fila=0;
    FILA->prox=NULL;
}

/*Esta função realiza a inserção de caracteres na fila*/
void insere_caracter(Palavra *FILA, int valor)
{
    
    Palavra *novo = aloca();
    
    novo->prox=NULL;
    
    novo->letra=valor;
    fprintf(stderr, "\nEntrou na inserção com a fila %d  \n", novo->letra);
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
    fprintf(stderr, "\nSAIU\n");
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
    fprintf(stderr, "\n quantidade de itens: %d", qtd_itens_fila);
    char s[qtd_itens_fila];
    memset( s, 0x0, qtd_itens_fila);
    fprintf(stderr, "\n item: %s", s);

   	Palavra *tmp;
	tmp = FILA->prox;
    for(int i=0; i<qtd_itens_fila; i++){
        s[i]=tmp->letra;
        
        tmp=tmp->prox;
    }
    fprintf(stderr, "\n%c\n", s[qtd_itens_fila-1]);

    int rc = strncmp(s, word, qtd_itens_fila);
    if(rc==0){
        return true;
    }else{
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
        fprintf(stderr, "Entrou %c", ch);
        if((ch == 32)||(ch == 10)){
            fprintf(stderr, "\nEntrou no if do espaço com %d itens na fila\n", qtd_itens_fila);
            Palavra *tmp = FILA->prox;
            int contador=0;
            while (contador<qtd_itens_fila){
                fprintf(stderr, "\n%c\n", tmp->letra);
                tmp=tmp->prox;
                contador++;
            }
            if(tmp==NULL) fprintf(stderr, "Deu ruim");
            
            /*Chamar a função que faz a comparação que deve retornar 1 para igual e 0 para diferente*/
            if(compara(FILA, word)){

            }else{

            }
            /*Incrementar o contador que diz quantas vezes a palavra apareceu no arquivo*/
            libera(FILA);
        }else{
            insere_caracter(FILA, ch);
        }

        fprintf(stderr, "\nValor em int %d valor em char ", ch);//10 == space
        fprintf(stderr, "%c\n", ch);
        //fputc(ch, _arquivo_backup_);
        ch = fgetc(_arquivo_);
        }
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


    printf("\nA criar uma nova thread\n");
    //printf("\n Diretório do programa: %s\n", diretorio_prog);
    flag = pthread_create(&t_o.t_o, NULL, trata_thread(a), NULL);

    if (flag != 0)
        printf("\nErro na criação da thread despachante thread\n");

    //trata_thread(NULL);

    pthread_exit(NULL);

    return 0; /* O programa não vai chegar aqui.         */
}