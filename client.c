#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "client.h"
#include "config.h"
#include "pcg/pcg_utils.h"

// Função para gerar valor de depósito/saque mais realista
float gerar_valor_transacao() {
    // Média de 500, desvio padrão de 300
    float valor = gerar_valor_normal(500.0f, 300.0f);
    // Limita o valor entre -1000 e 1000
    valor = fmax(fmin(valor, 1000.0f), -1000.0f);
    return valor;
}

// Função para gerar valor de transferência mais realista
float gerar_valor_transferencia() {
    // Média de 300, desvio padrão de 200
    float valor = fabs(gerar_valor_normal(300.0f, 200.0f));
    // Limita o valor entre 50 e 800
    valor = fmax(fmin(valor, 800.0f), 50.0f);
    return valor;
}

void* thread_cliente(void* arg) {
    int id_cliente = *((int*)arg);
    free(arg);
    int conta_cliente = id_cliente;

    while (1) {
        Requisicao* req = (Requisicao*) malloc(sizeof(Requisicao));
        req->tipo = (pcg32_random_r(&pcg_rng) % 2) + 1; // 1: depósito, 2: transferência
        req->id_origem = conta_cliente;

        if (req->tipo == 1) { // Depósito
            req->valor = gerar_valor_transacao();
            printf("Cliente %d solicitando %s de %.2f na sua conta %d.\n", 
                id_cliente, 
                (req->valor >= 0) ? "deposito" : "saque",
                fabs(req->valor), 
                req->id_origem);
        } else { // Transferência
            do {
                req->id_destino = (pcg32_random_r(&pcg_rng) % NUM_CONTAS) + 1;
            } while (req->id_destino == req->id_origem);
            
            req->valor = gerar_valor_transferencia();
            printf("Cliente %d solicitando transferencia de %.2f da sua conta %d para conta %d.\n", 
                id_cliente, req->valor, req->id_origem, req->id_destino);
        }

        adicionar_requisicao(&fila_requisicoes, req);
        usleep(config.taxa_geracao_requisicoes*1000);
    }
}

