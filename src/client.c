/**
 * Grupo 38
 * Gonçalo Pinto fc58178
 * Pedro Piló fc58179
 * Duarte Taleigo fc58760
*/

#include "memory.h"
#include "main.h"
#include "client.h"
#include "aptime.h"
#include "synchronization.h"

#include <stdio.h>
#include <time.h>


/* Função principal de um Cliente. Deve executar um ciclo infinito onde em 
* cada iteração lê uma operação da main e se data->terminate ainda 
* for igual a 0, processa-a. Operações com id igual a -1 são ignoradas
* (op inválida) e se data->terminate for igual a 1 é porque foi dada ordem
* de terminação do programa, portanto deve-se fazer return do número de 
* operações processadas. Para efetuar estes passos, pode usar os outros
* métodos auxiliares definidos em client.h.
*/
int execute_client(int client_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    struct operation op;
    int counter = 0; 
    while (1) {
        client_get_operation(&op, client_id, buffers, data, sems);
        if (*data->terminate == 1) {
            return counter;
        }else if (op.id != -1) {
            client_process_operation(&op, client_id, data, &counter, sems);
            client_send_operation(&op, buffers, data, sems);
        }
    }
}


/* Função que lê uma operação do buffer de memória partilhada entre a main
* e clientes que seja direcionada a client_id. Antes de tentar ler a operação, deve
* verificar se data->terminate tem valor 1. Em caso afirmativo, retorna imediatamente da função.
*/
void client_get_operation(struct operation* op, int client_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    if (*data->terminate == 1) {
        return;
    }else {
        consume_begin(sems->main_client);
        read_main_client_buffer(buffers->main_client, client_id, data->buffers_size, op);
        if (op->id == -1) {
            sem_post(sems->main_client->full);
            sem_post(sems->main_client->mutex);
        } else {
            op->client_time = get_current_time();
            consume_end(sems->main_client);
            
        }
    }
} 

/* Função que processa uma operação, alterando o seu campo receiving_client para o id
* passado como argumento, alterando o estado da mesma para 'C' (client), e 
* incrementando o contador de operações. Atualiza também a operação na estrutura data.
*/
void client_process_operation(struct operation* op, int client_id, struct main_data* data, int* counter, struct semaphores* sems) {
    op->receiving_client = client_id;
    op->status = 'C';
    semaphore_mutex_lock(sems->results_mutex);
    data->results[op->id] = *op;
    semaphore_mutex_unlock(sems->results_mutex);
    (*counter)++;
}

/* Função que escreve uma operação no buffer de memória partilhada entre
* clientes e intermediários.
*/
void client_send_operation(struct operation* op, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    produce_begin(sems->client_interm);
    write_client_interm_buffer(buffers->client_interm, data->buffers_size, op);
    produce_end(sems->client_interm);
}
