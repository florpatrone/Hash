#include "hash.h"
#include <stdlib.h>
#include <stdbool.h>

unsigned long funcion_hash(char *str, size_t cantidad) { //Utiliza el algoritmo 'djb2'
    unsigned long hash = 5381;
    int c;
    
    while ((c = *str++))
        hash = ((hash << 5) + hash) + (unsigned long)c; /* hash * 33 + c */
    
    return hash%cantidad;
}