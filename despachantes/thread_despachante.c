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
#define fileset "/home/joaquim/joaquim/ifb/so/paralegrep/fileset/"
typedef int bool;

#define true 1
#define false 0

typedef struct arquivos Arquivo;

struct arquivos
{
    int numero_arq; //serve como contador e como id
    char *arquivo;  //o diretório de cada arquivo
    char *arquivo_backup;
    Arquivo *prox;
};

typedef struct despachantes
{
    int id;
    pthread_t t_d;
    char dir;     //diretório de análise dos arquivos
    int t_op[10]; //numero de threads operarias funcionando
} despachante;

despachante t_d; //inicialização global da única thread despachante

int qtd_arq = 0;

/*Função para concatenar ponteiro char*/
char* concatenar(char *original, char *add) {

    int tamanho = strlen(original) + strlen(add) + 1;
    char s[tamanho];

    strcpy(s, original);
    strcat(s, add);
    fprintf(stderr,"\n Após concatenar: %s", s);
    char *retorno = s;
    return retorno;
    // while (*original)
    //     original++;

    // while (*add) {
    //     *original = *add;
    //     add++;
    //     original++;
    // }
    // *original = '\0';
}

/*Esta função faz o backup de cada arquivo que ela encontra
 para realizar a comparação com o mesmo arquivo caso este seja modificado*/
int realiza_backup(Arquivo *novo){
    char ch;
    FILE *_arquivo_original, *_arquivo_backup_ ;
    fprintf(stderr, "Realizando o backup");


    //abre o arquivo original
    _arquivo_original = fopen(novo->arquivo, "r");
    
    if (_arquivo_original == NULL){
      fprintf(stderr, "Não foi possível abrir o novo arquivo...\n");
      exit(1);
    }
     
    _arquivo_backup_ = fopen(novo->arquivo_backup, "w");
 
   if (_arquivo_backup_ == NULL){
        fclose(_arquivo_original);
        fprintf(stderr, "Não foi possível criar um novo arquivo\n");
        exit(1);
   }

    while ((ch = fgetc(_arquivo_original)) != EOF)
        fputc(ch, _arquivo_backup_);
        
        
    fclose(_arquivo_original);
    fclose(_arquivo_original);

}


/*Esta função realiza a inserção de novos arquivos na lista de arquivos da thread*/
int insere_arquivo(Arquivo *lista_arquivos, char *dir_arq, char *nome_arq)
{
    Arquivo *novo = (Arquivo *)malloc(sizeof(Arquivo));
    //Caso não consiga criar um novo dado retorna nulo
    if (novo == NULL) return false;
    else{
        
        novo->numero_arq = qtd_arq++; //colocara aqui uma forma para pegar a quantidade de arquivos que já tem e somar mais um
        novo->prox = lista_arquivos;
        novo->arquivo = dir_arq;
        char *back_file_path = fileset;
        fprintf(stderr, "\nConcatenando: \n %s%s", back_file_path, nome_arq);
        novo->arquivo_backup = concatenar(back_file_path, nome_arq);
        //fprintf(stderr, "\nCriando novo arquivo");
        
        printf("Novo arquivo adicionado: %s", dir_arq);
        
        /*Realiza o backup de cada novo arquivo*/
        realiza_backup(novo);
        return true;
    }
    // for(int perc_arquivos = 0; percinsere_arquivo_arquivos<=n_max; perc_arquivos++){
    //     fprintf(stderr,"Arquivo %d com o diretório: %s\n", perc_arquivos, t_d.n_files[perc_arquivos]->arquivo);
    // }
}

/*Inicializa a lista encadeada de arquivos*/
Arquivo* lst_cria(void)
{
    return NULL;
}

/* Função varre o diretório e pesquisa todos os arquivos, também serve para subdiretórios*/
// void vasculha_dir(char *dir_int, int prof, Arquivo *lista_arquivos)
// {   
//     fprintf(stderr, "\n\n\nDiretório a ser analisado:");
//     fprintf(stderr,"%s", dir_int);
//     DIR *dp;
//     struct dirent *entry;
//     struct stat statbuf;
//     //Caso não consiga abrir uma pasta entra
//     if ((dp = opendir(dir_int)) == NULL)
//     {
//         fprintf(stderr, "\n\n Pasta Nula");
//         //aqui é caso dê erro ao abrir o diretório
//     }
//     //chdir(dir_int);
    
//     while ((entry = readdir(dp)) != NULL)
//     {
//         lstat(entry->d_name, &statbuf);
//         if (S_ISDIR(statbuf.st_mode))
//         {
//             /* Found a directory, but ignore . and .. */
//             if (strcmp(".", entry->d_name) == 0 ||
//                 strcmp("..", entry->d_name) == 0)
//                 continue;
//             fprintf(stderr, "%*s%s/\n", prof, "", entry->d_name);
//             insere_arquivo(lista_arquivos, dir_int, entry->d_name);
//             /* Parte do código que permite busca recursiva para outras  pasta */
//             //vasculha_dir(entry->d_name, prof + 1, lista_arquivos);
//         }
//         else
//             printf("%*s%s\n", prof, "", entry->d_name);
//     }
//     //chdir("..");
//     closedir(dp);
// }
void vasculha_dir(char *dir_int, int prof, Arquivo *lista_arquivos){
    DIR *d;
    struct dirent *dir;
    d = opendir(dir_int);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(".", dir->d_name) == 0 ||
                 strcmp("..", dir->d_name) == 0)
                 continue;
            insere_arquivo(lista_arquivos, dir_int, dir->d_name);
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

void *trata_thread(char *caminho)
{
    Arquivo* lista_arquivos;
    lista_arquivos = lst_cria();
    /*
        lista_arquivos = insere_arquivo(lista_arquivos, )
        colocar essa linha de código na parte que reconhece os arquivos

    */
    while (1)
    {   
        fprintf(stderr, "Iniciando nova busca");
        //printf("\n\n\n Iniciando nova busca");
        vasculha_dir(caminho, 0, lista_arquivos);
        sleep(5); /*espera 5 segundos e executa de novo*/
    }

    //pthread_exit(NULL);
}

int main()
{
    int flag;
    char diretorio_prog[FILENAME_MAX];
    GetCurrentDir(diretorio_prog, FILENAME_MAX );

    printf("A criar uma nova thread\n");
    printf("\n Diretório do programa: %s", diretorio_prog);
    flag = pthread_create(&t_d.t_d, NULL, trata_thread(fileset), NULL);

    if (flag != 0)
        printf("Erro na criação da thread despachante thread\n");

    //trata_thread(NULL);

    pthread_exit(NULL);

    return 0; /* O programa não vai chegar aqui.         */
}