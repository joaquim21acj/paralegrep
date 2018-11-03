#ifndef operaria   /* Include guard */
#define operaria

#include <time.h>
#include <pthread.h> 

typedef struct arquivos arquivo;

struct arquivos{
    int n_vezes;
    time_t alteracao;
    char *arquivo;  //o diretório de cada arquivo
    char *caminho_p_arquivo;
};

int threads_operarias[10];

char *dir_trabalho;


void *trata_thread_operaria(arquivo *a, char *argumento);

#endif // FOO_H_