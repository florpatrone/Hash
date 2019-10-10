#define _POSIX_C_SOURCE 200809L 
#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define BORRAR_NODO true

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

/***************************
* Funciones auxiliares 
****************************/

size_t funcion_hash(const char *str, size_t cantidad) { //Utiliza el algoritmo 'djb2'
    char* copia_clave = NULL;
    copia_clave = strcpy(copia_clave,str);
    
    size_t hash = 5381;
    int c;
    
    while ((c = *str++))
        hash = ((hash << 5) + hash) + (unsigned long)c; /* hash * 33 + c */
    
    return hash%cantidad;
}

void *_hash_obtener(hash_t *hash, const char *clave, size_t indice_balde, bool borrar_nodo){
    
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
        if (campo->clave != clave){                       // compara si campo->clave es lo buscado
            lista_iter_avanzar(iterador):
            continue;
        }
        void *valor = campo->valor;             // si lo es, lo guarda, borra el nodo de la lista y devuelve el valor
        if (borrar_nodo){
            lista_iter_borrar(iterador);
            lista_iter_destruir(iterador);
        }
        return valor;
    }
    lista_iter_destruir(iterador);
    return _hash_obtener(hash,clave,++indice_balde);     // avanza un balde más abajo
}

bool hash_redimensionar_capacidad(hash_t *hash, size_t (*operacion) (hash_t*)){
    size_t *primos = {2, 3, 5, 7, 11 ,1 3, 17 ,19, 23, 29, 31, 37};
    int n = 12;
    size_t nueva_capacidad = (*operacion)(hash,primos, n);

    lista_t** lista_auxiliar = realloc(hash->listas,nueva_capacidad * sizeof(lista_t*));
    if (!lista_auxiliar){
        return false;
    }
    hash->listas = lista_auxiliar;
    hash->capacidad = nueva_capacidad;
    return true;
}
size_t busqueda_binaria(size_t *arreglo, size_t inicio, size_t final, size_t buscado){
    if (final >= inicio){
        size_t medio = inicio + (final-inicio)/2;
        
        if (arreglo[medio] == buscado){
            return medio;
        }
        if (arreglo[medio] > buscado){
            return busqueda_binaria(arreglo, inicio, medio-1,buscado);
        }
        return busqueda_binaria(arreglo, medio+1, final, buscado);
    }
    return NULL
}
size_t aumentar_capacidad(hash_t *hash, int *primos, int n){
    if (hash->capacidad < primos[n-1]){                         // Si la capacidad es menor a 37
        n = busqueda_binaria(primos,0,n,hash->capacidad);
        n++;
        return primos[n];
    }
    int m = 0;
    size_t nueva_capacidad = m**2 + m + 41;        // formula para conseguir primos desde el 41 hasta el 1601 (Wikipedia)
    while (nueva_capacidad <= hash->capacidad){    /* chequear que se repite en reducir, función aparte?*/
        m++;
        nueva_capacidad = m**2 + m + 41;
    }
    return nueva_capacidad
}

size_t reducir_capacidad(hash_t *hash, int *primos, int n){
    if (hash->capacidad > primos[n-1]){                         // Si la capacidad es menor a 37
        n = busqueda_binaria(primos,0,n,hash->capacidad);
        n--;
        return primos[n];
    }
    int m = 0;
    size_t nueva_capacidad = m**2 + m + 41;        // formula para conseguir primos desde el 41 hasta el 1601 (Wikipedia)
    while (nueva_capacidad != hash->capacidad){
        m--;
        nueva_capacidad = m**2 + m + 41;
    }
    return nueva_capacidad
}

/***************************
* Primitivas del Hash
****************************/

/* hash_t *hash_crear(hash_destruir_dato_t destruir_dato){

} */

/* bool hash_guardar(hash_t *hash, const char *clave, void *dato){

} */

void *hash_borrar(hash_t *hash, const char *clave){
    if (hash->cantidad == 0){
        return NULL;
    }
    size_t indice_balde = funcion_hash(clave,cantidad); //cantidad ?
    return _hash_obtener(hash, clave, indice_balde, BORRAR_NODO);
}

void *hash_obtener(const hash_t *hash, const char *clave){
    if (hash->cantidad == 0){
        return NULL;
    }
    size_t indice_balde = funcion_hash(clave,cantidad); //cantidad ?
    return _hash_obtener(hash, clave,indice_balde, !BORRAR_NODO);
}

/* bool hash_pertenece(const hash_t *hash, const char *clave){

} */

/* size_t hash_cantidad(const hash_t *hash){

} */

/* void hash_destruir(hash_t *hash){

} */
