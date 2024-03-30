/**
 * Grupo 38
 * Gonçalo Pinto fc58178
 * Pedro Piló fc58179
 * Duarte Taleigo fc58760
*/

#include "memory.h"
#include "main.h"
#include "enterprise.h"
#include "aptime.h"
#include "synchronization.h"

#include <stdio.h>
#include <time.h>

/* Função principal de uma Empresa. Deve executar um ciclo infinito onde em 
* cada iteração lê uma operação e se e data->terminate ainda for igual a 0, processa-a.
* Operações com id igual a -1 são ignoradas (op inválida) e se data->terminate for igual
* a 1 é porque foi dada ordem de terminação do programa, portanto deve-se fazer return do
* número de operações processadas. Para efetuar estes passos, pode usar os outros
* métodos auxiliares definidos em enterprise.h.
*/
int execute_enterprise(int enterp_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    struct operation op;
    int counter = 0; 
    while (1) {
        enterprise_receive_operation(&op, enterp_id, buffers, data, sems);
        if (*data->terminate == 1) {
            return counter;
        }else if (op.id != -1) {
            enterprise_process_operation(&op, enterp_id, data, &counter, sems);
        }
    }
}


/* Função que lê uma operação do buffer de memória partilhada entre
* os intermediários e as empresas que seja direcionada à empresa enterprise_id.
* Antes de tentar ler a operação, o processo deve verificar se data->terminate
* tem valor 1. Em caso afirmativo, retorna imediatamente da função.
*/
void enterprise_receive_operation(struct operation* op, int enterp_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    if (*data->terminate == 1) {
        return;
    }else {
        consume_begin(sems->interm_enterp);
        read_interm_enterp_buffer(buffers->interm_enterp, enterp_id, data->buffers_size, op);
        if (op->id == -1) {
            sem_post(sems->interm_enterp->full);
            sem_post(sems->interm_enterp->mutex);
        } else {
            consume_end(sems->interm_enterp);
            op->enterp_time = get_current_time();
        }
    }
}


/* Função que processa uma operação, alterando o seu campo receiving_enterp para o id
* passado como argumento, alterando o estado da mesma para 'E' ou 'A' conforme o número
* máximo de operações já tiver sido atingido ou não, e incrementando o contador de operações.
* Atualiza também a operação na estrutura data.
*/
void enterprise_process_operation(struct operation* op, int enterp_id, struct main_data* data, int* counter, struct semaphores* sems) {
    op->receiving_enterp = enterp_id;
    if (op->id < data->max_ops) {
        op->status = 'E';
    }else {
        op->status = 'A';
    }
    semaphore_mutex_lock(sems->results_mutex);
    data->results[op->id] = *op;
    semaphore_mutex_unlock(sems->results_mutex);
    (*counter)++;
}