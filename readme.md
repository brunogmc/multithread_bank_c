# Sistema Bancário Multithread

## Características Principais

### 1. Arquitetura do Sistema
- Sistema bancário simulado com operações concorrentes
- Implementação em C utilizando pthreads para gerenciamento de threads
- Arquitetura cliente-servidor com threads trabalhadoras (worker threads)

### 2. Componentes Principais
- **Servidor**: Gerencia requisições e coordena threads trabalhadoras
- **Clientes**: Geram requisições de operações bancárias
- **Threads Trabalhadoras**: Processam as requisições da fila
- **Banco**: Mantém as contas e gerencia operações financeiras

### 3. Operações Suportadas
- Depósitos e saques (valores positivos e negativos)
- Transferências entre contas
- Balanço geral do banco (a cada 10 operações)

## Estratégias de Implementação

### 1. Controle de Concorrência
- Uso de mutexes para proteção de recursos compartilhados
- Implementação de locks hierárquicos para evitar deadlocks em transferências
- Condições de sincronização (pthread_cond) para coordenação de threads

### 2. Gerenciamento de Requisições
- Fila circular thread-safe para armazenamento de requisições
- Sistema produtor-consumidor entre clientes e trabalhadoras
- Balanceamento automático de carga entre threads trabalhadoras

### 3. Geração de Dados Realistas
- Distribuição normal para valores de transações
- Limites realistas para valores de operações
- Randomização controlada usando PCG (Permuted Congruential Generator)

### 4. Configuração Flexível
- Parâmetros ajustáveis via linha de comando:
  - Número de threads trabalhadoras (-t)
  - Número de clientes (-c)
  - Taxa de geração de requisições (-g)
  - Tempo de serviço (-s)

## Como Compilar e Executar

### Compilação
```bash
gcc main.c server.c client.c bank.c pcg/pcg_variants.c pcg/pcg_utils.c -o banco -lpthread -lm
```

### Execução
```bash
./banco -t 5 -c 10 -g 500 -s 200
```

Onde:
- `-t`: número de threads trabalhadoras
- `-c`: número de clientes
- `-g`: taxa de geração de requisições (em milissegundos)
- `-s`: tempo de serviço (em milissegundos)

## Medidas de Segurança
- Proteção contra race conditions
- Prevenção de deadlocks
- Validação de entradas e limites
- Gerenciamento seguro de memória

## Limitações e Considerações
- Número fixo de contas bancárias
- Sistema em execução contínua
- Operações síncronas
- Necessidade de encerramento manual do programa

## Exemplos de Execução

### Exemplo 1: Baixa Concorrência
```bash
./banco -t 3 -c 5 -g 1000 -s 500
```
Neste exemplo, temos:
- 3 threads trabalhadoras
- 5 clientes
- Intervalo de 1 segundo entre requisições
- Tempo de processamento de 0.5 segundos
Ideal para visualizar o funcionamento básico do sistema com baixa carga.

Saída típica:
```
Cliente 2 solicitando deposito de 450.32 na sua conta 2.
Cliente 4 solicitando transferencia de 250.75 da sua conta 4 para conta 1.
Deposito de 450.32 na conta 2. Saldo atual: 850.32
Transferencia de 250.75 da conta 4 para 1
Adicionando requisicao de balanco apos 10 operacoes
Balanço Geral:
Conta 1: Saldo = 750.75
Conta 2: Saldo = 850.32
Conta 3: Saldo = 300.00
Conta 4: Saldo = 249.25
Conta 5: Saldo = 500.00
```

### Exemplo 2: Configuração Balanceada
```bash
./banco -t 5 -c 10 -g 500 -s 200
```
Neste exemplo, temos:
- 5 threads trabalhadoras
- 10 clientes
- Intervalo de 0.5 segundos entre requisições
- Tempo de processamento de 0.2 segundos
Configuração equilibrada para uso típico.

Saída típica:
```
Cliente 3 solicitando deposito de 525.75 na sua conta 3.
Cliente 7 solicitando transferencia de 300.00 da conta 7 para conta 2.
Deposito de 525.75 na conta 3. Saldo atual: 1025.75
Transferencia de 300.00 da conta 7 para 2
Adicionando requisicao de balanco apos 10 operacoes
Iniciando processamento de balanco geral...
[...]
```

### Exemplo 3: Alta Concorrência
```bash
./banco -t 10 -c 20 -g 100 -s 50
```
Neste exemplo, temos:
- 10 threads trabalhadoras
- 20 clientes
- Intervalo de 0.1 segundos entre requisições
- Tempo de processamento de 0.05 segundos
Demonstra o comportamento do sistema sob carga intensa.

Saída típica:
```
Cliente 1 solicitando transferencia de 325.50 da sua conta 1 para conta 3.
Cliente 5 solicitando deposito de 600.00 na sua conta 5.
Cliente 2 solicitando saque de 200.00 na sua conta 2.
Transferencia de 325.50 da conta 1 para 3
Deposito de 600.00 na conta 5. Saldo atual: 1100.00
Deposito de -200.00 na conta 2. Saldo atual: 300.00
[...]
```
