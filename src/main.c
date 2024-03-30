/**
 * Grupo 38
 * Gonçalo Pinto fc58178
 * Pedro Piló fc58179
 * Duarte Taleigo fc58760
*/

#include "memory.h"
#include "process.h"
#include "main.h"
#include "synchronization.h"
#include "configuration.h"
#include "aptime.h"
#include "apsignal.h"
#include "log.h"
#include "stats.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


char* filename;

int main(int argc, char *argv[]) {
    //init data structures
    struct main_data* data = create_dynamic_memory(sizeof(struct main_data));
    struct comm_buffers* buffers = create_dynamic_memory(sizeof(struct comm_buffers));
    buffers->main_client = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    buffers->client_interm = create_dynamic_memory(sizeof(struct circular_buffer));
    buffers-> interm_enterp = create_dynamic_memory(sizeof(struct rnd_access_buffer));

    // init semaphore data structure
    struct semaphores* sems = create_dynamic_memory(sizeof(struct semaphores));
    sems->main_client = create_dynamic_memory(sizeof(struct prodcons));
    sems->client_interm = create_dynamic_memory(sizeof(struct prodcons));
    sems->interm_enterp = create_dynamic_memory(sizeof(struct prodcons));

    //execute main code
    main_args(argc, argv, data);
    create_dynamic_memory_buffers(data);
    create_shared_memory_buffers(data, buffers);
    create_semaphores(data, sems);
    launch_processes(buffers, data, sems);
    user_interaction(buffers, data, sems);
    
    //release memory before terminating
    destroy_dynamic_memory(data);
    destroy_dynamic_memory(buffers->main_client);
    destroy_dynamic_memory(buffers->client_interm);
    destroy_dynamic_memory(buffers->interm_enterp);
    destroy_dynamic_memory(buffers);
    destroy_dynamic_memory(sems->main_client);
    destroy_dynamic_memory(sems->client_interm);
    destroy_dynamic_memory(sems->interm_enterp);
    destroy_dynamic_memory(sems);

}

/* Função que lê os argumentos da aplicação, nomeadamente o número
* máximo de operações, o tamanho dos buffers de memória partilhada
* usados para comunicação, e o número de clientes, de intermediários e de
* empresas. Guarda esta informação nos campos apropriados da
* estrutura main_data.
*/
void main_args(int argc, char* argv[], struct main_data* data) {
    if (argc == 2) {
        filename = argv[1];
        char** args = readFile(filename);
        sscanf(args[0], "%d", &data->max_ops);  
        sscanf(args[1], "%d", &data->buffers_size);
        sscanf(args[2], "%d", &data->n_clients);
        sscanf(args[3], "%d", &data->n_intermediaries);
        sscanf(args[4], "%d", &data->n_enterprises);
        data->log_filename = args[5]; 
        data->statistics_filename = args[6]; 
        sscanf(args[7], "%d", &data->alarm_time);
    }else {
        printf ("Error with the args");
    }
}

/* Função que reserva a memória dinâmica necessária para a execução
* do AdmPor, nomeadamente para os arrays *_pids e *_stats da estrutura 
* main_data. Para tal, pode ser usada a função create_dynamic_memory.
*/
void create_dynamic_memory_buffers(struct main_data* data) {
    data->client_pids = create_dynamic_memory(sizeof(int) * data->n_clients);
    data->intermediary_pids = create_dynamic_memory(sizeof(int) * data->n_intermediaries);	
	data->enterprise_pids = create_dynamic_memory(sizeof(int) * data->n_enterprises);		
	data->client_stats = create_dynamic_memory(sizeof(int) * data->n_clients);		
	data->intermediary_stats = create_dynamic_memory(sizeof(int) * data->n_intermediaries);
	data->enterprise_stats = create_dynamic_memory(sizeof(int) * data->n_enterprises);
}

/* Função que reserva a memória partilhada necessária para a execução do
* AdmPor. É necessário reservar memória partilhada para todos os buffers da
* estrutura comm_buffers, incluindo os buffers em si e respetivos
* pointers, assim como para o array data->results e variável data->terminate.
* Para tal, pode ser usada a função create_shared_memory. O array data->results
* deve ser limitado pela constante MAX_RESULTS.
*/
void create_shared_memory_buffers(struct main_data* data, struct comm_buffers* buffers) {
    buffers->main_client->buffer = create_shared_memory(STR_SHM_MAIN_CLIENT_BUFFER, data->buffers_size*sizeof(struct operation));
    buffers->main_client->ptrs = create_shared_memory(STR_SHM_MAIN_CLIENT_PTR, data->buffers_size*sizeof(int));

    buffers->client_interm->buffer = create_shared_memory(STR_SHM_CLIENT_INTERM_BUFFER, data->buffers_size*sizeof(struct operation));
    buffers->client_interm->ptrs = create_shared_memory(STR_SHM_CLIENT_INTERM_PTR, sizeof(struct pointers));

	buffers->interm_enterp->buffer = create_shared_memory(STR_SHM_INTERM_ENTERP_BUFFER, data->buffers_size*sizeof(struct operation));
    buffers->interm_enterp->ptrs = create_shared_memory(STR_SHM_INTERM_ENTERP_PTR, data->buffers_size*sizeof(int));

    data->results = create_shared_memory(STR_SHM_RESULTS, MAX_RESULTS*sizeof(struct operation));
    data->terminate = create_shared_memory(STR_SHM_TERMINATE, sizeof(int));
}

/* Função que inicia os processos dos clientes, intermediários e
* empresas. Para tal, pode usar as funções launch_*,
* guardando os pids resultantes nos arrays respetivos
* da estrutura data.
*/
void launch_processes(struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) { 
    setInfo(data, buffers, sems, getpid());
    for (int i = 0; i < data->n_clients ; i++) {
        data->client_pids[i] = launch_client(i, buffers, data, sems);
    }  
    for (int i = 0; i < data->n_intermediaries ; i++) {
        data->intermediary_pids[i] = launch_interm(i, buffers, data, sems);
    }
    for (int i = 0; i < data->n_enterprises ; i++) {
        data->enterprise_pids[i] = launch_enterp(i, buffers, data, sems);
    }
    mainAlarm(data, data->alarm_time);
    mainSignal(getpid());
    
}

/* Função que faz interação do utilizador, podendo receber 4 comandos:
* op - cria uma nova operação, através da função create_request
* status - verifica o estado de uma operação através da função read_status
* stop - termina o execução do AdmPor através da função stop_execution
* help - imprime informação sobre os comandos disponiveis
*/
void user_interaction(struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    int op_counter = 0;
    char string [20];
    while (1) {
        printf("Selecione uma opcao: OP, STATUS, STOP, HELP\n");
        scanf("%20s", string);
        struct timespec time = get_current_time(); //isto devia tar fora do loop para n tar sempre a criar novos objetos?
        if (strcmp (string, "OP") == 0) {
            writeLog(data->log_filename, string, &time);
            create_request(&op_counter, buffers, data, sems);
        } else if (strcmp (string, "STATUS") == 0) {
            writeLog(data->log_filename, string, &time);
            read_status(data, sems);
        } else if (strcmp (string, "HELP") == 0) {
            writeLog(data->log_filename, "help", &time);
            printf("op - cria uma nova operação, através da função create_request \n status - verifica o estado de uma operação através da função read_status \n stop - termina o execução do AdmPor através da função stop_execution \n help - imprime informação sobre os comandos disponiveis \n");
        } else if (strcmp (string, "STOP") == 0) {
            writeLog(data->log_filename, "stop", &time);
            stop_execution(data, buffers, sems);
            return;
        }  
    }
}

/* Cria uma nova operação identificada pelo valor atual de op_counter e com os 
* dados introduzidos pelo utilizador na linha de comandos, escrevendo a mesma 
* no buffer de memória partilhada entre main e clientes. Imprime o id da 
* operação e incrementa o contador de operações op_counter. Não deve criar 
* mais operações para além do tamanho do array data->results.
*/
void create_request(int* op_counter, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    // Ver o q fazer com os semaforos!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if (*op_counter < MAX_RESULTS) {
        int requesting_client;
        int requested_enterp;
        scanf("%d", &requesting_client);
        scanf("%d", &requested_enterp);
        //------------------------------------------------------------------------------------------
        //escrevendo a mesma no buffer de memória partilhada entre main e clientes
        struct operation newOperation;
        newOperation.id = *op_counter;
        newOperation.requesting_client=requesting_client;
        newOperation.requested_enterp=requested_enterp;
        //------------------------------------------------------------------------------------------
        newOperation.start_time = get_current_time();
        //------------------------------------------------------------------------------------------
        produce_begin(sems->main_client);
        write_main_client_buffer(buffers->main_client, data->buffers_size, &newOperation);
        produce_end(sems->main_client);
        //------------------------------------------------------------------------------------------
        char int_str[20];
        sprintf(int_str, "%d", newOperation.id);
        printf("id da operacao: %s", int_str);
        printf("\n");
        //------------------------------------------------------------------------------------------
        (*op_counter)++;
    }    
}

/* Função que lê um id de operação do utilizador e verifica se a mesma é valida.
* Em caso afirmativo imprime informação da mesma, nomeadamente o seu estado, o 
* id do cliente que fez o pedido, o id da empresa requisitada, e os ids do cliente,
* intermediário, e empresa que a receberam e processaram.
*/
void read_status(struct main_data* data, struct semaphores* sems) {
    int id;
    scanf("%d", &id);
    if (id < MAX_RESULTS) { //necessario o mutexlock de results??????????????
        printf("estado da operacao: ");
        printf("%c\n", data->results[id].status);
        printf("id do cliente que fez o pedido: ");
        printf("%d\n", data->results[id].requesting_client);
        printf("id da empresa requisitada: ");
        printf("%d\n", data->results[id].requested_enterp);
        printf("id do cliente que recebeu pedido: ");
        printf("%d\n", data->results[id].receiving_client);
        printf("id do intermediário que fez entrega: ");
        printf("%d\n", data->results[id].receiving_interm);
        printf("id do empresa que recebeu a encomenda: ");
        printf("%d\n", data->results[id].receiving_enterp);
    }
}

/* Função que termina a execução do programa AdmPor. Deve começar por 
* afetar a flag data->terminate com o valor 1. De seguida, e por esta
* ordem, deve esperar que os processos filho terminem, deve escrever as
* estatisticas finais do programa, e por fim libertar
* as zonas de memória partilhada e dinâmica previamente 
* reservadas. Para tal, pode usar as outras funções auxiliares do main.h.
*/
void stop_execution(struct main_data* data, struct comm_buffers* buffers, struct semaphores* sems) {
    *data->terminate = 1;
    wakeup_processes(data, sems);
    wait_processes(data);
    write_statistics(data);
    destroy_memory_buffers(data, buffers);
    destroy_semaphores(sems);
}

/* Função que espera que todos os processos previamente iniciados terminem,
* incluindo clientes, intermediários e empresas. Para tal, pode usar a função 
* wait_process do process.h.
*/
void wait_processes(struct main_data* data) {
    for (int i = 0; i < data->n_clients ; i++) {
        data->client_stats[i] = wait_process(data->client_pids[i]);
    }  

    for (int i = 0; i < data->n_intermediaries ; i++) {
        data->intermediary_stats[i] = wait_process(data->intermediary_pids[i]);
    }
    for (int i = 0; i < data->n_enterprises ; i++) {
        data->enterprise_stats[i] = wait_process(data->enterprise_pids[i]);
    }
}

/* Função que imprime as estatisticas finais do AdmPor, nomeadamente quantas
* operações foram processadas por cada cliente, intermediário e empresa.
*/
void write_statistics(struct main_data* data) {
    printStats(data);
}

/* Função que liberta todos os buffers de memória dinâmica e partilhada previamente
* reservados na estrutura data.
*/
void destroy_memory_buffers(struct main_data* data, struct comm_buffers* buffers) {
    destroy_dynamic_memory(data->client_pids);
    destroy_dynamic_memory(data->client_stats);

    destroy_dynamic_memory(data->intermediary_pids);
    destroy_dynamic_memory(data->intermediary_stats);

    destroy_dynamic_memory(data->enterprise_pids);
    destroy_dynamic_memory(data->enterprise_stats);

    destroy_shared_memory(STR_SHM_MAIN_CLIENT_BUFFER, buffers->main_client->buffer, data->buffers_size*sizeof(struct operation));
    destroy_shared_memory(STR_SHM_MAIN_CLIENT_PTR, buffers->main_client->ptrs, data->buffers_size*sizeof(int));

    destroy_shared_memory(STR_SHM_CLIENT_INTERM_BUFFER, buffers->client_interm->buffer, data->buffers_size*sizeof(struct operation));
    destroy_shared_memory(STR_SHM_CLIENT_INTERM_PTR, buffers->client_interm->ptrs, sizeof(struct pointers));

    destroy_shared_memory(STR_SHM_INTERM_ENTERP_BUFFER, buffers->interm_enterp->buffer, data->buffers_size*sizeof(struct operation));
    destroy_shared_memory(STR_SHM_INTERM_ENTERP_PTR, buffers->interm_enterp->ptrs, data->buffers_size*sizeof(int));

    destroy_shared_memory(STR_SHM_RESULTS,  data->results, MAX_RESULTS*sizeof(struct operation));
    destroy_shared_memory(STR_SHM_TERMINATE, data->terminate, sizeof(int));
}

/* Função que inicializa os semáforos da estrutura semaphores. Semáforos
* *_full devem ser inicializados com valor 0, semáforos *_empty com valor
* igual ao tamanho dos buffers de memória partilhada, e os *_mutex com
* valor igual a 1. Para tal pode ser usada a função semaphore_create.*/
void create_semaphores(struct main_data* data, struct semaphores* sems) {
    sems->main_client->full = semaphore_create(STR_SEM_MAIN_CLIENT_FULL, 0);
    sems->main_client->empty = semaphore_create(STR_SEM_MAIN_CLIENT_EMPTY, data->buffers_size);
    sems->main_client->mutex = semaphore_create(STR_SEM_MAIN_CLIENT_MUTEX, 1);
    
    sems->client_interm->full = semaphore_create(STR_SEM_CLIENT_INTERM_FULL, 0);
    sems->client_interm->empty = semaphore_create(STR_SEM_CLIENT_INTERM_EMPTY, data->buffers_size);
    sems->client_interm->mutex = semaphore_create(STR_SEM_CLIENT_INTERM_MUTEX, 1);

    sems->interm_enterp->full = semaphore_create(STR_SEM_INTERM_ENTERP_FULL, 0);
    sems->interm_enterp->empty = semaphore_create(STR_SEM_INTERM_ENTERP_EMPTY, data->buffers_size);
    sems->interm_enterp->mutex = semaphore_create(STR_SEM_INTERM_ENTERP_MUTEX, 1);

    sems->results_mutex = semaphore_create(STR_SEM_RESULTS_MUTEX, 1);
}

/* Função que acorda todos os processos adormecidos em semáforos, para que
* estes percebam que foi dada ordem de terminação do programa. Para tal,
* pode ser usada a função produce_end sobre todos os conjuntos de semáforos
* onde possam estar processos adormecidos e um número de vezes igual ao
* máximo de processos que possam lá estar.*/
void wakeup_processes(struct main_data* data, struct semaphores* sems) {
    for (int i = 0; i < data->n_clients; i++) {
        produce_end(sems->main_client);
    }
    for (int i = 0; i < data->n_intermediaries; i++) {
        produce_end(sems->client_interm);
    }
    for (int i = 0; i < data->n_enterprises; i++) {
        produce_end(sems->interm_enterp);
    }
}

/* Função que liberta todos os semáforos da estrutura semaphores. */
void destroy_semaphores (struct semaphores* sems) {
    semaphore_destroy(STR_SEM_MAIN_CLIENT_FULL, sems->main_client->full);
    semaphore_destroy(STR_SEM_MAIN_CLIENT_EMPTY, sems->main_client->empty);
    semaphore_destroy(STR_SEM_MAIN_CLIENT_MUTEX, sems->main_client->mutex);

    semaphore_destroy(STR_SEM_CLIENT_INTERM_FULL, sems->client_interm->full);
    semaphore_destroy(STR_SEM_CLIENT_INTERM_EMPTY, sems->client_interm->empty);
    semaphore_destroy(STR_SEM_CLIENT_INTERM_MUTEX, sems->client_interm->mutex);

    semaphore_destroy(STR_SEM_INTERM_ENTERP_FULL, sems->interm_enterp->full);
    semaphore_destroy(STR_SEM_INTERM_ENTERP_EMPTY, sems->interm_enterp->empty);
    semaphore_destroy(STR_SEM_INTERM_ENTERP_MUTEX, sems->interm_enterp->mutex);

    semaphore_destroy(STR_SEM_RESULTS_MUTEX, sems->results_mutex);
}