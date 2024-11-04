#ifndef SERVER_H
#define SERVER_H

#include "bank.h"
#include <pthread.h>
#include <stdbool.h>

#define MAX_REQUISICOES 100

typedef struct {
    int tipo;
    int id_origem;
    int id_destino;
    float valor;
} Requisicao;

typedef struct {
    pthread_t thread;
    bool ocupada;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    Requisicao* requisicao_atual;
} ThreadTrabalhadora;

typedef struct {
    Requisicao* fila[MAX_REQUISICOES];
    int inicio;
    int fim;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} FilaRequisicoes;

// Variáveis globais para gerenciar threads trabalhadoras
extern ThreadTrabalhadora* threads_trabalhadoras;
extern int num_threads_trabalhadoras;

void init_fila(FilaRequisicoes* fila);
void init_threads_trabalhadoras(int num_threads);
void adicionar_requisicao(FilaRequisicoes* fila, Requisicao* req);
Requisicao* obter_requisicao(FilaRequisicoes* fila);
void* thread_trabalhadora(void* arg);
void* thread_servidor(void* arg);
ThreadTrabalhadora* obter_trabalhadora_disponivel(void);

extern Banco banco;
extern FilaRequisicoes fila_requisicoes;

#endif // SERVER_H
