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
    hash_destruir_dato_t funcion_destruccion;
};

/* Funciones auxiliares */

size_t funcion_hash(const char *str, size_t cantidad) { //Utiliza el algoritmo 'djb2'
    char* copia_clave = NULL;
    copia_clave = strcpy(copia_clave,str);
    
    size_t hash = 5381;
    int c;
    
    while ((c = *str++))
        hash = ((hash << 5) + hash) + (unsigned long)c; /* hash * 33 + c */
    
    return hash%cantidad;
}

void *_hash_borrar(hash_t *hash, const char *clave, size_t indice_balde){
    
    if (lista_largo(hash->listas)=indice_balde){ //si llega al final del hash
        return NULL;
    }
    
    lista_t *lista = hash->balde[indice_balde]; // selecciona la lista enlazada en el balde actual
    if (lista_esta_vacia(lista) || !lista){
        return NULL;
    }
    
    lista_iter_t *iterador_lista = lista_iter_crear(lista); // iterador para recorrer la lista enlazada
    while (!lista_iter_al_final(iterador)){
        campo_t *campo = lista_iter_ver_actual(iterador); // abre el dato del iterador, que es un campo
        if (campo->clave == clave){                         // compara si campo->clave es lo buscado
            void *valor = campo->valor;                     // si lo es, lo guarda, borra el nodo de la lista y devuelve el valor
            lista_iter_borrar(iterador);
            lista_iter_destruir(iterador);
            return valor;
        }
        lista_iter_avanzar(iterador);                   // si no lo es, avanza sobre la lista y repite el módulo del while
    }
    lista_iter_destruir(iterador);
    return _hash_borrar(hash,clave,++indice_balde);     // avanza un balde más abajo
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

void *hash_borrar(hash_t *hash, const char *clave){
    if (hash_cantidad(hash) == 0){
        return NULL;
    }
    size_t indice_balde = funcion_hash(clave, /*cantidad*/); // ¿cantidad?
    return _hash_borrar(hash, clave, indice_balde);
}

/* void *hash_obtener(const hash_t *hash, const char *clave){

} */

/* bool hash_pertenece(const hash_t *hash, const char *clave){

} */

/* size_t hash_cantidad(const hash_t *hash){

} */

/* void hash_destruir(hash_t *hash){

} */
