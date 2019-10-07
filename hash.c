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

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    if ((hash->cantidad / hash->capacidad) >= FACTOR_CARGA){
        if (!hash_redimensionar_capacidad(hash,aumentar_capacidad)) return false;
    } 

    char* copia_clave = strdup(clave);
    campo_t* campo = campo_crear(copia_clave,dato);

    if (campo == NULL){
       free(copia_clave);
       return false;
    }

    size_t num_hash = funcion_hash(copia_clave,hash->capacidad);
    lista_t* balde = hash->baldes[num_hash];

    if (!lista_insertar_ultimo(balde,campo)){
       free(copia_clave);
       free(campo);
       return false;
    }

    hash->cantidad++;
    return true;
}

/* void *hash_borrar(hash_t *hash, const char *clave){

} */

/* void *hash_obtener(const hash_t *hash, const char *clave){

} */

bool hash_pertenece(const hash_t *hash, const char *clave){
    size_t num_hash = funcion_hash(clave,hash->capacidad);
    lista_t* balde = hash->baldes[num_hash];
    lista_iter_t* iterador = lista_iter_crear(balde);

    if (iterador == NULL) return false; //falso negativo

    while (!lista_iter_al_final(iterador)){
        campo_t* campo = lista_iter_ver_actual(iterador);
        
        if (strcmp(campo->clave,clave) == 0){
            lista_iter_destruir(iterador);
            return true;
        }
    }

    lista_iter_destruir(iterador);
    return false;
}

/* size_t hash_cantidad(const hash_t *hash){

} */

/* void hash_destruir(hash_t *hash){

} */

/* Primitivas del campo */

campo_t* campo_crear(char *clave, void *dato){
    campo_t* campo = malloc(sizeof(campo_t));

    if (campo == NULL) return NULL;
    
    campo->clave = clave;
    campo->valor = dato;

    return campo;
}