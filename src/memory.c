/**
 * Grupo 38
 * Gonçalo Pinto fc58178
 * Pedro Piló fc58179
 * Duarte Taleigo fc58760
*/

#include "memory.h"
#include "main.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

/* Função que reserva uma zona de memória partilhada com tamanho indicado
* por size e nome name, preenche essa zona de memória com o valor 0, e 
* retorna um apontador para a mesma. Pode concatenar o resultado da função
* getuid() a name, para tornar o nome único para o processo.
*/ 
void* create_shared_memory(char* name, int size) {
    int id = getuid();
    char buffer[200];
    sprintf(buffer, "%d", id);
    strcat(buffer, name);
    int sh_mem = shm_open(buffer, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (sh_mem == -1) {
        perror("shm_open error");
        exit;
    }
    int is_size = ftruncate(sh_mem, size);
    if (is_size == -1) {
        perror("ftruncate error");
        exit;
    }
    void* pointer = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, sh_mem, 0);
    if (pointer == MAP_FAILED) {
        perror("mmap error");
        exit;
    }
    memset(pointer, 0, size);
    return pointer;  
}

/* Função que reserva uma zona de memória dinâmica com tamanho indicado
* por size, preenche essa zona de memória com o valor 0, e retorna um 
* apontador para a mesma.
*/
void* create_dynamic_memory(int size) {
    void* maluco = malloc(size);
    memset(maluco, 0, size);
    return maluco;
}

/* Função que liberta uma zona de memória dinâmica previamente reservada.
*/
void destroy_shared_memory(char* name, void* ptr, int size) {
    int id = getuid();
    char buffer[200];
    sprintf(buffer, "%d", id);
    strcat(buffer, name);
    munmap(ptr, size);
    shm_unlink(buffer);
}

/* Função que liberta uma zona de memória partilhada previamente reservada.
*/
void destroy_dynamic_memory(void* ptr) {
    free(ptr);
}

/* Função que escreve uma operação no buffer de memória partilhada entre a Main
* e os clientes. A operação deve ser escrita numa posição livre do buffer, 
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_main_client_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op) {
    for(int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 0) {
            buffer->buffer[i] = *op;
            buffer->ptrs[i] = 1;
            return;
        }
    }
}

/* Função que escreve uma operação no buffer de memória partilhada entre os clientes
* e intermediários. A operação deve ser escrita numa posição livre do buffer, 
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_client_interm_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op) {  
    if (((buffer->ptrs->in + 1) % buffer_size) == buffer->ptrs->out) {  //buffer cheio
       return;
    }
    buffer->buffer[buffer->ptrs->in] = *op;
    buffer->ptrs->in = (buffer->ptrs->in + 1) % buffer_size;
}

/* Função que escreve uma operação no buffer de memória partilhada entre os intermediários
* e as empresas. A operação deve ser escrita numa posição livre do buffer, 
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_interm_enterp_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op) {
    for(int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 0) {
            buffer->buffer[i] = *op;
            buffer->ptrs[i] = 1;
            return;
        }
    }
}

/* Função que lê uma operação do buffer de memória partilhada entre a Main
* e os clientes, se houver alguma disponível para ler que seja direcionada ao cliente especificado.
* A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
* Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_main_client_buffer(struct rnd_access_buffer* buffer, int client_id, int buffer_size, struct operation* op) {
    for(int i = 0; i < buffer_size; i++) {
        if(buffer->ptrs[i] == 1) {
            if (client_id == buffer->buffer[i].requesting_client) {
                (*op) = buffer->buffer[i];
                buffer->ptrs[i] = 0;
                return;
            }
        }
    }
    op->id = -1;
}

/* Função que lê uma operação do buffer de memória partilhada entre os clientes e intermediários,
* se houver alguma disponível para ler (qualquer intermediário pode ler qualquer operação).
* A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
* Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_client_interm_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op) {
    int writ = buffer->ptrs->in;
    int read = buffer->ptrs->out;
    if (writ == read) {
        op->id = -1;
    }else {
        (*op) = buffer->buffer[read];
        buffer->ptrs->out = (buffer->ptrs->out + 1) % buffer_size;
        return;
    }
}

/* Função que lê uma operação do buffer de memória partilhada entre os intermediários e as empresas,
* se houver alguma disponível para ler dirijida à empresa especificada. A leitura deve
* ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo. Se não houver
* nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_interm_enterp_buffer(struct rnd_access_buffer* buffer, int enterp_id, int buffer_size, struct operation* op) {
    for(int i = 0; i < buffer_size; i++) {
        if(buffer->ptrs[i] == 1) {
            if (enterp_id == buffer->buffer[i].requested_enterp) {
                (*op) = buffer->buffer[i];
                buffer->ptrs[i] = 0;
                return;
            }
        }
    }
    op->id = -1;
}