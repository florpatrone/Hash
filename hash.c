#define _POSIX_C_SOURCE 200809L 
#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#define BORRAR_NODO true

#define FACTOR_CARGA 2
#define CAPACIDAD_INICIAL 17

/* Definiciones previas:
    Baldes: lista enlazada que en sus nodos contiene como dato un puntero a otra lista enlazada.
            Los baldes tendrán índices del 0 a m (siendo m el largo del hash).
    
    Listas: listas enlazadas apuntadas por el puntero del balde en un determinado índice.
    Ej: balde en la posición 3 tiene un puntero que señala a una lista enlazada donde se encuentra, en cada nodo,
        una clave para la cual la función de hashing devolvió un 3 y su respectivo valor.

    Iterador_hash: itera la lista enlazada de baldes con punteros.
    Iterador_lista: itera la lista enlazada que se encuentra dentro del balde elegido.
*/

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
    void (*funcion_destruccion)(void*);
};

/* Definicion del struct iterador hash */
struct hash_iter{
    void* hash;
    size_t balde_actual;
    size_t iterados;
};

/***************************
* Primitivas del Campo
****************************/

campo_t* campo_crear(char *clave, void *dato){
    campo_t* campo = malloc(sizeof(campo_t));

    if (campo == NULL) return NULL;
    
    campo->clave = clave;
    campo->valor = dato;

    return campo;
}

void campo_destruir(campo_t* campo){
    free(campo->clave);
    free(campo);
}

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

void *_hash_obtener(const hash_t* hash, const char *clave, size_t indice_balde, bool borrar_nodo){
    
    if (!clave){
        return NULL;
    }
    
    if (hash->cantidad == indice_balde){         // Si ya recorrí toda la lista
        return NULL;
    }
    
    lista_t* lista = hash->baldes[indice_balde];        // lista enlazada correspondiente a ese índice
    if (!lista || lista_esta_vacia(lista)){
        return NULL;
    }
    lista_iter_t *iterador_lista = lista_iter_crear(lista);
    void* valor;
    while (!lista_iter_al_final(iterador_lista)){
        campo_t* campo = lista_iter_ver_actual(iterador_lista);
        if (campo->clave != clave){
            lista_iter_avanzar(iterador_lista);
            continue;
        }
        valor = campo->valor;
        if (borrar_nodo){
            lista_iter_borrar(iterador_lista);
        }
        lista_iter_destruir(iterador_lista);
        return valor;
    }
    lista_iter_destruir(iterador_lista);
    return _hash_obtener(hash, clave, ++indice_balde, borrar_nodo);
}
bool hash_redimensionar_capacidad(hash_t *hash, size_t (*operacion) (hash_t*, size_t*, size_t)){
    size_t primos[] = {2, 3, 5, 7, 11 , 13, 17 ,19, 23, 29, 31, 37};
    size_t n = 12;
    size_t nueva_capacidad = (*operacion)(hash, primos, n);

    lista_t** lista_auxiliar = realloc(hash->baldes,nueva_capacidad * sizeof(lista_t*));
    if (!lista_auxiliar){
        return false;
    }
    hash->baldes = lista_auxiliar;
    hash->capacidad = nueva_capacidad;
    return true;
}
size_t busqueda_binaria(size_t *arreglo, size_t inicio, size_t final, size_t buscado){

    size_t medio = inicio + (final-inicio)/2;
    
    if (arreglo[medio] == buscado){
        return medio;
    }
    if (arreglo[medio] > buscado){
        return busqueda_binaria(arreglo, inicio, medio-1,buscado);
    }
    return busqueda_binaria(arreglo, medio+1, final, buscado);
}
size_t aumentar_capacidad(hash_t *hash, size_t *primos, size_t n){
    if (hash->capacidad < primos[n-1]){                         // Si la capacidad es menor a 37
        n = busqueda_binaria(primos,0,n,hash->capacidad);
        n++;
        return primos[n];
    }

    size_t m = 0;
    size_t nueva_capacidad = m*m + m + 41;        // formula para conseguir primos desde el 41 hasta el 1601 (Wikipedia)
    while (nueva_capacidad <= hash->capacidad){
        m++;
        nueva_capacidad = m*m + m + 41;
    }
    return nueva_capacidad;
}

size_t reducir_capacidad(hash_t *hash, size_t *primos, size_t n){
    if (hash->capacidad > primos[n-1]){                         // Si la capacidad es menor a 37
        n = busqueda_binaria(primos,0,n,hash->capacidad);
        n--;
        return primos[n];
    }
    size_t m = 0;
    size_t nueva_capacidad = m*m + m + 41;        // formula para conseguir primos desde el 41 hasta el 1601 (Wikipedia)
    while (nueva_capacidad != hash->capacidad){
        m--;
        nueva_capacidad = m*m + m + 41;
    }
    return nueva_capacidad;
}

void pre_setear_lista(lista_t** lista){
    for (size_t i = 0; i < CAPACIDAD_INICIAL; i++){
        lista[i] = NULL;
    }    
}


/***************************
* Primitivas del Hash
****************************/

hash_t *hash_crear(void (*destruir_dato)(void*)){
    hash_t* hash = malloc(sizeof(hash_t*));
    if (!hash){
        return NULL;
    }

    lista_t** baldes = malloc(CAPACIDAD_INICIAL * sizeof(lista_t*));
    if (!baldes){
        return NULL;
    }
    hash->baldes = baldes;
    pre_setear_lista(hash->baldes);

    hash->capacidad = CAPACIDAD_INICIAL;
    hash->cantidad = 0;
    hash->funcion_destruccion = destruir_dato;
    return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    if ((hash->cantidad / hash->capacidad) >= FACTOR_CARGA){
        if (!hash_redimensionar_capacidad(hash,aumentar_capacidad)) return false;
    } 

    char* copia_clave = strdup(clave);

    if (copia_clave == NULL) return false;

    campo_t* campo = campo_crear(copia_clave,dato);

    if (campo == NULL){
       free(copia_clave);
       return false;
    }

    size_t num_hash = funcion_hash(copia_clave,hash->capacidad);
    lista_t* balde = hash->baldes[num_hash];

    if (balde == NULL){
        hash->baldes[num_hash] = lista_crear();
    }

    if ( (balde == NULL) || (!lista_insertar_ultimo(balde,campo)) ){//si fallo la creacion de la lista o si fallo la insercion
        campo_destruir(campo);
        return false;
    }

    hash->cantidad++;
    return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
    if (hash->cantidad == 0 || !clave){
        return NULL;
    }

    size_t largo_hash = hash->capacidad;
    size_t indice_balde = funcion_hash(clave, largo_hash);

    return _hash_obtener(hash, clave, indice_balde, BORRAR_NODO);
}

void *hash_obtener(const hash_t *hash, const char *clave){
    if (hash->cantidad == 0 || !clave){
        return NULL;
    }

    size_t indice_balde = funcion_hash(clave,hash->cantidad);
    return _hash_obtener(hash, clave,indice_balde, !BORRAR_NODO);
}

bool hash_pertenece(const hash_t *hash, const char *clave){
    size_t num_hash = funcion_hash(clave,hash->capacidad);
    void* valor = _hash_obtener(hash, clave, num_hash, !BORRAR_NODO);

    return (valor != NULL);
}

size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}

void hash_destruir(hash_t *hash){
    hash_destruir_dato_t funcion_destruccion = hash->funcion_destruccion;

    for (int i = 0; i < hash->capacidad ; i++){
        lista_t* balde = hash->baldes[i];

        if (balde == NULL) continue;

        while (!lista_esta_vacia(balde)){
            campo_t* campo = lista_borrar_primero(balde);
            if (funcion_destruccion != NULL) funcion_destruccion(campo->valor);
            campo_destruir(campo);
        }
        lista_destruir(balde,NULL);
    }
    
    free(hash);
}


/***************************
* Primitivas del Iterador
****************************/

hash_iter_t *hash_iter_crear(const hash_t *hash){
    
    lista_t** arreglo_hash = hash->baldes;
    if (!arreglo_hash){
        return NULL;
    }
    hash_iter_t *iterador_hash = malloc(sizeof(hash_iter_t));
    if (!iterador_hash){
        return NULL;
    }
    void* mem_hash = &hash;
    iterador_hash->hash = mem_hash;
    iterador_hash->balde_actual = 0;
    iterador_hash->iterados = 0;

    return iterador_hash;
}

bool hash_iter_avanzar(hash_iter_t *iter){
    if (hash_iter_al_final(iter)) return false;

    hash_t* hash = iter->hash;
    lista_iter_t* balde_iter = iter->balde_iter;

    lista_iter_avanzar(balde_iter);
    iter->iterados ++;
    
    if (!lista_iter_al_final(balde_iter) || hash_iter_al_final(iter)) return true;

    iter->balde_actual++;

    lista_t* balde = hash->baldes[iter->balde_actual];

    while (balde == NULL){
        iter->balde_actual++;
    }

    lista_iter_t* nuevo_balde_iter = lista_iter_crear(balde);

    if (nuevo_balde_iter == NULL) return false; //falso negativo;

    iter->balde_iter = nuevo_balde_iter;
    lista_iter_destruir(balde_iter);

    return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
    if (hash_iter_al_final(iter)) return NULL;

    campo_t* campo = lista_iter_ver_actual(iter->balde_iter);
    return campo->clave;
}

bool hash_iter_al_final(const hash_iter_t *iter){
    hash_t* hash = iter->hash;
    return iter->iterados == hash->cantidad;
}

void hash_iter_destruir(hash_iter_t* iter){
    free(iter);
}
