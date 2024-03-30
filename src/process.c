/**
 * Grupo 38
 * Gonçalo Pinto fc58178
 * Pedro Piló fc58179
 * Duarte Taleigo fc58760
*/

#include "memory.h"
#include "main.h"
#include "apsignal.h"

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "client.h"
#include "enterprise.h"
#include "intermediary.h"

/* Função que inicia um novo processo cliente através da função fork do SO. O novo
* processo irá executar a função execute_client respetiva, fazendo exit do retorno.
* O processo pai devolve o pid do processo criado.
*/
int launch_client(int client_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    int pid = fork();
    if (pid == -1) { /* Houve algum erro */
        perror("Error launching client");
        exit(1);
    }
    if (pid == 0) { /* Processo filho */
        mainSignal(getpid());
        int num_OP_Exec = execute_client(client_id, buffers, data, sems);
        exit(num_OP_Exec);
    } else { /* Processo pai */
        return pid;
    }
} 

/* Função que inicia um novo processo intermediário através da função fork do SO. O novo
* processo irá executar a função execute_intermediary, fazendo exit do retorno.
* O processo pai devolve o pid do processo criado.
*/
int launch_interm(int interm_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    int pid = fork();
    if (pid == -1) { /* Houve algum erro */
        perror("Error launching interm");
        exit(1);
    }
    if (pid == 0) { /* Processo filho */
        mainSignal(getpid());
        int num_OP_Exec = execute_intermediary(interm_id, buffers, data, sems);
        exit(num_OP_Exec);
    } else { /* Processo pai */
        return pid;
    }
}

/* Função que inicia um novo processo empresa através da função fork do SO. O novo
* processo irá executar a função execute_enterprise, fazendo exit do retorno.
* O processo pai devolve o pid do processo criado.
*/
int launch_enterp(int enterp_id, struct comm_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    int pid = fork();
    if (pid == -1) { /* Houve algum erro */
        perror("Error launching enterp");
        exit(1);
    }
    if (pid == 0) { /* Processo filho */
        mainSignal(getpid());
        int num_OP_Exec = execute_enterprise(enterp_id, buffers, data, sems);
        exit(num_OP_Exec);
    } else { /* Processo pai */
        return pid;
    }
}

/* Função que espera que um processo termine através da função waitpid. 
* Devolve o retorno do processo, se este tiver terminado normalmente.
*/
int wait_process(int process_id) {
    int var = 0;
    waitpid(process_id, &var, 0);
    if (WIFEXITED(var)) {
        return WEXITSTATUS(var); 
    }
    return -1;
}
