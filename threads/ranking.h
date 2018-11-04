#ifndef ranking   /* Include guard */
#define ranking

#include <pthread.h> 
#include "operaria.h"

struct rankings{
    int id;
    pthread_t t_r;
    char dir;     //diretório de análise dos arquivos
    char *termo;
    struct arquivo a;
} ;

struct palavra{
    int letra;
    struct palavra *prox;
}; 
void inicia(struct palavra *FILA);

int vazia(struct palavra *FILA);

void *trata_thread_ranking(struct arquivo *a, struct arquivo *lista_arquivos);

#endif