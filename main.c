#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>
#include "server.h"
#include "client.h"
#include "config.h"
#include "pcg/pcg_utils.h"


// para complilar use : gcc main.c server.c client.c bank.c pcg/pcg_variants.c pcg/pcg_utils.c -o banco -lpthread -lm
// para rodar use ./banco -t 5 -c 10 -g 500000 -s 200000


Config config;  // Global config variable

void ler_parametros(int argc, char *argv[]) {
    // Inicializa a config global
    config.num_trabalhadoras = 3;
    config.num_clientes = 3;
    config.taxa_geracao_requisicoes = 1000;
    config.tempo_servico = 1000;

    int opt;
    while ((opt = getopt(argc, argv, "t:c:g:s:")) != -1) {
        switch (opt) {
            case 't':
                config.num_trabalhadoras = atoi(optarg);
                break;
            case 'c':
                config.num_clientes = atoi(optarg);
                break;
            case 'g':
                config.taxa_geracao_requisicoes = atoi(optarg);
                break;
            case 's':
                config.tempo_servico = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Uso: %s [-t num_trabalhadoras] [-c num_clientes] [-g taxa_requisicoes] [-s tempo_servico]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[]) {
    init_pcg_rng();
    ler_parametros(argc, argv);
    
    init_banco(&banco);
    init_fila(&fila_requisicoes);
    init_threads_trabalhadoras(config.num_trabalhadoras);

    pthread_t thread_servidor_id;
    pthread_t thread_clientes[config.num_clientes];

    // Criar thread do servidor
    pthread_create(&thread_servidor_id, NULL, thread_servidor, NULL);

    // Criar threads trabalhadoras
    for (int i = 0; i < config.num_trabalhadoras; i++) {
        pthread_create(&threads_trabalhadoras[i].thread, NULL, 
                      thread_trabalhadora, &threads_trabalhadoras[i]);
    }

    // Criar threads dos clientes
    for (int i = 0; i < config.num_clientes; i++) {
        int* id_cliente = malloc(sizeof(int));
        *id_cliente = i + 1;
        pthread_create(&thread_clientes[i], NULL, thread_cliente, id_cliente);
    }

    // Aguardar threads (código existente)
    pthread_join(thread_servidor_id, NULL);
    for (int i = 0; i < config.num_trabalhadoras; i++) {
        pthread_join(threads_trabalhadoras[i].thread, NULL);
    }
    for (int i = 0; i < config.num_clientes; i++) {
        pthread_join(thread_clientes[i], NULL);
    }

    return 0;
}

