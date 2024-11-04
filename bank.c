#include <stdio.h>
#include <stdlib.h>
#include "bank.h"
#include "pcg/pcg_utils.h"

void init_conta(Conta* conta, int id, float saldo_inicial) {
    conta->id = id;
    conta->saldo = saldo_inicial;
    pthread_mutex_init(&conta->mutex, NULL);
}

void init_banco(Banco* banco) {
    pthread_mutex_init(&banco->mutex, NULL);
    for (int i = 0; i < NUM_CONTAS; i++) {
        init_conta(&banco->contas[i], i + 1, (float)(pcg32_random_r(&pcg_rng) % 1000 + 100));
    }
}

void deposito(Banco* banco, int id_conta, float valor) {
    if (id_conta < 1 || id_conta > NUM_CONTAS) return;
    Conta* conta = &banco->contas[id_conta - 1];

    pthread_mutex_lock(&conta->mutex);
    conta->saldo += valor;
    printf("Deposito de %.2f na conta %d. Saldo atual: %.2f\n", valor, id_conta, conta->saldo);
    pthread_mutex_unlock(&conta->mutex);
}

void transferencia(Banco* banco, int id_origem, int id_destino, float valor) {
    if (id_origem < 1 || id_origem > NUM_CONTAS || id_destino < 1 || id_destino > NUM_CONTAS || id_origem == id_destino)
        return;

    int primeiro = (id_origem < id_destino) ? id_origem : id_destino;
    int segundo = (id_origem < id_destino) ? id_destino : id_origem;
    
    pthread_mutex_lock(&banco->contas[primeiro-1].mutex);
    pthread_mutex_lock(&banco->contas[segundo-1].mutex);
    
    Conta* conta_origem = &banco->contas[id_origem - 1];
    Conta* conta_destino = &banco->contas[id_destino - 1];

    if (conta_origem->saldo >= valor) {
        conta_origem->saldo -= valor;
        conta_destino->saldo += valor;
        printf("Transferencia de %.2f da conta %d para %d\n", valor, id_origem, id_destino);
    } else {
        printf("Saldo insuficiente para transferencia de %.2f da conta %d para %d\n", valor, id_origem, id_destino);
    }

    pthread_mutex_unlock(&banco->contas[segundo-1].mutex);
    pthread_mutex_unlock(&banco->contas[primeiro-1].mutex);
}

void balanco_geral(Banco* banco) {
    pthread_mutex_lock(&banco->mutex);
    printf("Balanço Geral:\n");
    for (int i = 0; i < NUM_CONTAS; i++) {
        pthread_mutex_lock(&banco->contas[i].mutex);
        printf("Conta %d: Saldo = %.2f\n", banco->contas[i].id, banco->contas[i].saldo);
        pthread_mutex_unlock(&banco->contas[i].mutex);
    }
    pthread_mutex_unlock(&banco->mutex);
}
