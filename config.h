#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int num_trabalhadoras;
    int num_clientes;
    int taxa_geracao_requisicoes;
    int tempo_servico;
} Config;

extern Config config;

#endif // CONFIG_H