#ifndef operaria   /* Include guard */
#define operaria

#include <time.h>
#include <pthread.h> 

struct arquivo{
    int n_vezes;
    time_t alteracao;
    char *arquivo;  //o diretório de cada arquivo
    char *caminho_p_arquivo;
};


struct operarias{
    int id;
    pthread_t t_o;
    char dir;     //diretório de análise dos arquivos
    char *termo;
    struct arquivo a;
};

int threads_operarias[10];

char *dir_trabalho;


void *trata_thread_operaria(struct arquivo *a, char *argumento, struct arquivo *lista_arquivos, int *is_alive);

#endif // FOO_H_