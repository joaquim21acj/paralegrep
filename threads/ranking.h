#ifndef ranking   /* Include guard */
#define ranking

#include <pthread.h> 
#include "operaria.h"

typedef struct ranking
{
    int id;
    pthread_t t_r;
    char dir;     //diretório de análise dos arquivos
    char *termo;
    arquivo a;
} ranking;

void *trata_thread_ranking(arquivo *a);

#endif