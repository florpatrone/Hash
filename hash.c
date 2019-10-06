#define _POSIX_C_SOURCE 200809L 
#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define FACTOR_CARGA 2
#define CAPACIDAD_INICIAL 37

/* Definición del struct campo */
typedef struct campo {
    char* clave;
    void* valor;
} campo_t;

/* Definición del struct hash */
struct hash {
    lista_t** baldes;
    size_t capacidad;
    size_t cantidad;
    hash_destruir_dato_t* funcion_destruccion;
};

/* Funciones auxiliares */

unsigned long funcion_hash(char *str, size_t cantidad) { //Utiliza el algoritmo 'djb2'
    unsigned long hash = 5381;
    int c;
    
    while ((c = *str++))
        hash = ((hash << 5) + hash) + (unsigned long)c; /* hash * 33 + c */
    
    return hash%cantidad;
}

/* bool hash_redimensionar_capacidad(hash_t *hash, size_t (*operacion) (hash_t*)){

} */

/* size_t aumentar_capacidad(hash_t *hash){

} */

/* size_t reducir_capacidad(hash_t *hash){

} */

/* Primitivas del hash */

/* hash_t *hash_crear(hash_destruir_dato_t destruir_dato){

} */

/* bool hash_guardar(hash_t *hash, const char *clave, void *dato){

} */

/* void *hash_borrar(hash_t *hash, const char *clave){

} */

/* void *hash_obtener(const hash_t *hash, const char *clave){

} */

/* bool hash_pertenece(const hash_t *hash, const char *clave){

} */

/* size_t hash_cantidad(const hash_t *hash){

} */

/* void hash_destruir(hash_t *hash){

} */