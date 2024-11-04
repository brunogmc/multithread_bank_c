#ifndef BANK_H
#define BANK_H

#include <pthread.h>

#define NUM_CONTAS 5

typedef struct {
    int id;
    float saldo;
    pthread_mutex_t mutex;
} Conta;

typedef struct {
    Conta contas[NUM_CONTAS];
    pthread_mutex_t mutex;
} Banco;

void init_banco(Banco* banco);
void deposito(Banco* banco, int id_conta, float valor);
void transferencia(Banco* banco, int id_origem, int id_destino, float valor);
void balanco_geral(Banco* banco);

#endif // BANK_H
