#include "configuration.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Função que lê os conteúdos de um ficheiro passado como argumento
*/
char** readFile (char* filename) {
    const int SIZE_OF_BUFFER = 528;
    char buffer[SIZE_OF_BUFFER];
    int num_lines = 0, i = 0;
    char **array;
    FILE* file_ptr = NULL;

    file_ptr = fopen(filename, "r");
    if(file_ptr == NULL){
        fprintf(stderr, "Erro ao abrir o ficheiro\n");
        exit(-1);
    }

    while(fgets(buffer, SIZE_OF_BUFFER, file_ptr) != NULL){
        num_lines++;
    }
    
    array = (char**) malloc(num_lines* sizeof(char*));

    fseek(file_ptr, 0, SEEK_SET);

    while(fgets(buffer, SIZE_OF_BUFFER, file_ptr) != NULL){
        array[i] = (char*) malloc(strlen(buffer+1)*sizeof(char));
        strcpy(array[i], buffer);
        i++;
    }

    return array;
}