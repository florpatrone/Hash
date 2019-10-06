#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct campo {
    char* clave;
    void* valor;
} campo_t;

struct hash {
    lista_t** lista;
    size_t capacidad;
    size_t cantidad;
    hash_destruir_dato_t* funcion_destruccion;
};

unsigned long funcion_hash(char *str, size_t cantidad) { //Utiliza el algoritmo 'djb2'
    unsigned long hash = 5381;
    int c;
    
    while ((c = *str++))
        hash = ((hash << 5) + hash) + (unsigned long)c; /* hash * 33 + c */
    
    return hash%cantidad;
}