#include <stdio.h> /* standard I/O routines                 */
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pthread.h> /* pthread functions and data structures */
//#include "operaria.h"
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

typedef struct arquivos arquivo;

struct arquivos
{
    time_t alteracao;
    int n_vezes; //Qtas vezes a palavra aparece no arquivo
    char *arquivo;  //o diretório de cada arquivo
    char *caminho_p_arquivo;
    };

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
/*
    Thread ranking começa aqui
*/

typedef struct ranking
{
    int id;
    pthread_t t_r;
    char dir;     //diretório de análise dos arquivos
    char *termo;
    arquivo a;
} ranking;

ranking t_r; //inicialização global da única thread despachante

int qtd_arq_r = 1;
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
    fprintf(stderr, "\nQtd de arquivos: %d", qtd_arq_r);
    
    for(int i=0; i<qtd_arq_r; i++){
        fprintf(stderr, "\nDir de comp dir: %s e tmp->arq: %s", dir, lista_arquivos[i].arquivo);
        if(strcmp(dir, lista_arquivos[i].arquivo)==0){
            return true;
        }
    }
    return false;
}

int retorna_pos_arquivo(char *dir, arquivo *lista_arquivos){
    int i;
    for(i=0; i<=qtd_arq_r; i++){
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
    for (int i=0; i<qtd_arq_r; i++){
        fprintf(stderr, "\n Posicao %d : %d", i,lista_arquivos[qtd_arq_r].n_vezes);
    }
}

arquivo *funcao_inserir_novo_item(arquivo *lista_arquivos, arquivo a){
    fprintf(stderr, "\nQuantidades de itens na lista: %d", qtd_arq_r);
    if(qtd_arq_r>=n_max){
        fprintf(stderr, "Não foi possível adicionar outro item a lista");
        exit(1);
    }    
    lista_arquivos[qtd_arq_r]=a;
    qtd_arq_r++;
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
                ordena(lista_arquivos, qtd_arq_r);
                imprime_top_10(lista_arquivos);
            }
        }else{
            arquivo novo;
            novo.alteracao=a->alteracao;
            novo.arquivo=a->arquivo;
            novo.n_vezes=a->n_vezes;
            lista_arquivos = funcao_inserir_novo_item(lista_arquivos, novo);
            ordena(lista_arquivos, qtd_arq_r);
            imprime_top_10(lista_arquivos);
        }
}

void *trata_thread_ranking(arquivo *a)
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

/*
    Thread operaria começa aqui
*/
int qtd_itens_fila=0;

operaria t_o; //inicialização global da única thread despachante

int qtd_arq_o = 0;
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
    /*Estou seguindo este tutorial*/
    int ch = fgetc(_arquivo_);
    while (ch != EOF){
        if((ch == 32)||(ch == 10)){
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
        libera(FILA);  
        trata_thread_ranking(a);
}

void *trata_thread_operaria(arquivo *a, char *argumento)
{
    ocorrencias(a, argumento);
    pthread_exit(NULL);
}



/*
    Thread despachante começa aqui
*/

int qtd_arq = 0;
int tam;

typedef struct despachantes
{
    int id;
    pthread_t t_d;
    char dir;     //diretório de análise dos arquivos
    int t_op[10]; //numero de threads operarias funcionando
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

/*Esta função realiza a inserção de novos arquivos na lista de arquivos da thread*/
arquivo insere_arquivo(arquivo *lista_arquivos, char *dir_arq, char *nome_arq, int is_novo)
{
    if(qtd_arq_d==(n_max-1)){
        fprintf(stderr, "Foi atingido o tamanho máximo de arquivos");

        exit(1);
    }

    arquivo novo;
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
            int pos = retorna_pos_arquivo(nome_arq, lista_arquivos);
            lista_arquivos[pos]=novo;
            fprintf(stderr, "\narquivo atualizado %s", lista_arquivos[pos].arquivo);
        }else{
            lista_arquivos[qtd_arq_d]=novo;
            qtd_arq_d++;
            fprintf(stderr, "\nFoi inserido um novo arquivo na lista %s", lista_arquivos[qtd_arq_d-1].arquivo);
        }


        return novo;
}

int caminho_eh_novo(char *dir, arquivo *lista_arquivos){
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

int arquivo_foi_modificado(arquivo *lista_arquivos, char *dir){
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

    time_t t = st.st_mtime; /*st_mtime is type time_t */
    if (st.st_mtime > lista_arquivos[i].alteracao){
        return true;
    }else{
        return false;
    }
}
int acorda_thread_operaria(arquivo *a, char *argumento){
    pthread_t t_o[10];
    int i;
    for(i=0; i<10; i++){
        if(t_o[i]!=0){
            pthread_create(&t_o[i], NULL, trata_thread_operaria(a, argumento), NULL);
        }
    }
}

void vasculha_dir(char *dir_int, int prof, arquivo *lista_arquivos, char *argumento){
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
                arquivo novo_arquivo = insere_arquivo(lista_arquivos, dir_int, dir->d_name, true);
                arquivo *a_n = &novo_arquivo;
                acorda_thread_operaria(a_n, argumento);
            }else{
                if(arquivo_foi_modificado(lista_arquivos, dir->d_name)){
                    fprintf(stderr, "\narquivo foi alterado");
                    arquivo novo_arquivo =insere_arquivo(lista_arquivos, dir_int, dir->d_name, false);
                    arquivo *a_n = &novo_arquivo;
                    acorda_thread_operaria(a_n, argumento);
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

void *trata_thread(char *caminho, char *argumento)
{
    arquivo lista_arquivos[n_max];
    /*
        lista_arquivos = insere_arqu#define fileset "/home/joaquim/ifb/so/paralegrep/fileset/"ivo(lista_arquivos, )
        colocar essa linha de código na parte que reconhece os arquivos

    */
    while (1)
    {  
        vasculha_dir(caminho, 0, lista_arquivos, argumento);
        sleep(5); /*espera 5 segundos e executa de novo*/
    }

    //pthread_exit(NULL);
}

int main()
{
    int flag;
    char diretorio_prog[FILENAME_MAX];
    GetCurrentDir(diretorio_prog, FILENAME_MAX );

    char *argumento;

    printf("\nA criar uma nova thread\n");
    printf("\n Diretório do programa: %s\n", diretorio_prog);
    flag = pthread_create(&t_d.t_d, NULL, trata_thread(fileset, argumento), NULL);
    //trata_thread(NULL);

    pthread_exit(NULL);

    return 0; /* O programa não vai chegar aqui.         */
}