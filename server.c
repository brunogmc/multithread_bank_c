#include "server.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

ThreadTrabalhadora* threads_trabalhadoras;
int num_threads_trabalhadoras;
Banco banco;
FilaRequisicoes fila_requisicoes;

void init_fila(FilaRequisicoes* fila) {
    fila->inicio = 0;
    fila->fim = 0;
    pthread_mutex_init(&fila->mutex, NULL);
    pthread_cond_init(&fila->cond, NULL);
}

void adicionar_requisicao(FilaRequisicoes* fila, Requisicao* req) {
    pthread_mutex_lock(&fila->mutex);
    
    // Esperar enquanto a fila estiver cheia
    while ((fila->fim + 1) % MAX_REQUISICOES == fila->inicio) {
        pthread_cond_wait(&fila->cond, &fila->mutex);
    }
    
    fila->fila[fila->fim] = req;
    fila->fim = (fila->fim + 1) % MAX_REQUISICOES;
    
    pthread_cond_signal(&fila->cond);
    pthread_mutex_unlock(&fila->mutex);
}

Requisicao* obter_requisicao(FilaRequisicoes* fila) {
    pthread_mutex_lock(&fila->mutex);
    
    // Esperar enquanto a fila estiver vazia
    while (fila->inicio == fila->fim) {
        pthread_cond_wait(&fila->cond, &fila->mutex);
    }
    
    Requisicao* req = fila->fila[fila->inicio];
    fila->inicio = (fila->inicio + 1) % MAX_REQUISICOES;
    
    pthread_cond_signal(&fila->cond);
    pthread_mutex_unlock(&fila->mutex);
    
    return req;
}

void init_threads_trabalhadoras(int num_threads) {
    num_threads_trabalhadoras = num_threads;
    threads_trabalhadoras = malloc(sizeof(ThreadTrabalhadora) * num_threads);
    
    for (int i = 0; i < num_threads; i++) {
        threads_trabalhadoras[i].ocupada = false;
        pthread_mutex_init(&threads_trabalhadoras[i].mutex, NULL);
        pthread_cond_init(&threads_trabalhadoras[i].cond, NULL);
        threads_trabalhadoras[i].requisicao_atual = NULL;
    }
}

ThreadTrabalhadora* obter_trabalhadora_disponivel(void) {
    while (1) {
        for (int i = 0; i < num_threads_trabalhadoras; i++) {
            pthread_mutex_lock(&threads_trabalhadoras[i].mutex);
            if (!threads_trabalhadoras[i].ocupada) {
                threads_trabalhadoras[i].ocupada = true;
                pthread_mutex_unlock(&threads_trabalhadoras[i].mutex);
                return &threads_trabalhadoras[i];
            }
            pthread_mutex_unlock(&threads_trabalhadoras[i].mutex);
        }
        // Se não encontrar thread disponível, espera um pouco
        usleep(1000);
    }
}

void* thread_trabalhadora(void* arg) {
    ThreadTrabalhadora* self = (ThreadTrabalhadora*)arg;
    
    while (1) {
        // Esperar por uma nova requisição
        pthread_mutex_lock(&self->mutex);
        while (self->requisicao_atual == NULL) {
            pthread_cond_wait(&self->cond, &self->mutex);
        }
        
        // Processar a requisição
        Requisicao* req = self->requisicao_atual;
        pthread_mutex_unlock(&self->mutex);

        if (req->tipo == 1) { // Depósito
            deposito(&banco, req->id_origem, req->valor);
        } else if (req->tipo == 2) { // Transferência
            transferencia(&banco, req->id_origem, req->id_destino, req->valor);
        } else if (req->tipo == 3) { // Balanço geral
            printf("Iniciando processamento de balanco geral...\n");
            balanco_geral(&banco);
            printf("Balanco geral concluido.\n");
        }

        // Liberar a requisição e marcar como disponível
        pthread_mutex_lock(&self->mutex);
        free(self->requisicao_atual);
        self->requisicao_atual = NULL;
        self->ocupada = false;
        pthread_mutex_unlock(&self->mutex);
        
        usleep(config.tempo_servico * 1000);
    }
}

void* thread_servidor(void* arg) {
    int operacoes_processadas = 0;
    
    while (1) {
        // Obter requisição da fila
        Requisicao* req = obter_requisicao(&fila_requisicoes);
        
        // Incrementar contador de operações de clientes
        if (req->tipo != 3) {
            operacoes_processadas++;
            
            // A cada 10 operações, adicionar requisição de balanço
            if (operacoes_processadas % 10 == 0) {
                Requisicao* balanco_req = malloc(sizeof(Requisicao));
                balanco_req->tipo = 3;
                printf("Adicionando requisicao de balanco apos %d operacoes\n", operacoes_processadas);
                adicionar_requisicao(&fila_requisicoes, balanco_req);
            }
        }
        
        // Obter uma thread trabalhadora disponível
        ThreadTrabalhadora* trabalhadora = obter_trabalhadora_disponivel();
        
        // Atribuir a requisição à thread trabalhadora
        pthread_mutex_lock(&trabalhadora->mutex);
        trabalhadora->requisicao_atual = req;
        pthread_cond_signal(&trabalhadora->cond);
        pthread_mutex_unlock(&trabalhadora->mutex);
    }
}

