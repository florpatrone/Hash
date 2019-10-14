#define _POSIX_C_SOURCE 200809L 
#include "hash.h"
#include "lista.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BORRAR_NODO true
#define AUMENTAR true
#define FACTOR_CARGA 2
#define CAPACIDAD_INICIAL 19
#define CTE_AUMENTO 2
#define CTE_REDUCCION 2
#define CRITERIO_REDUCCION 4

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
    void (*destruir_dato)(void*);
};

/* Definicion del struct iterador hash */
struct hash_iter{
    const hash_t* hash;
    size_t balde_actual;
    lista_iter_t* balde_iter;
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
    size_t hash = 5381;
    int c;
    
    while ((c = *str++))
        hash = ((hash << 5) + hash) + (unsigned long)c; /* hash * 33 + c */
    
    return hash%cantidad;
}

campo_t *_hash_obtener(const hash_t* hash, const char *clave, size_t indice_balde, bool borrar_nodo){
    if (!clave){
        return NULL;
    }
    
    if (hash->capacidad == indice_balde){         // Si ya recorrí toda la lista
        return NULL;
    }
    
    lista_t* lista = hash->baldes[indice_balde];        // lista enlazada correspondiente a ese índice
    if (!lista || lista_esta_vacia(lista)){
        return NULL;
    }
    lista_iter_t *iterador_lista = lista_iter_crear(lista);
    if (!iterador_lista) return NULL;
    
    while (!lista_iter_al_final(iterador_lista)){
        campo_t* campo = lista_iter_ver_actual(iterador_lista);

        if (strcmp(campo->clave,clave) != 0){
            lista_iter_avanzar(iterador_lista);
            continue;
        }
        if (borrar_nodo){
            campo_t* campo_borrar = campo;
            campo = campo_crear(strdup(campo->clave),campo->valor);
            if (!campo) return NULL;
        
            lista_iter_borrar(iterador_lista);
            campo_destruir(campo_borrar);
        }
        lista_iter_destruir(iterador_lista);
        return campo;
    }
    lista_iter_destruir(iterador_lista);
    return _hash_obtener(hash, clave, ++indice_balde, borrar_nodo);
}

void pre_setear_lista(lista_t** lista, size_t n){
    for (size_t i = 0; i < n; i++){
        lista[i] = NULL;
    }    
}

bool transferir_datos(hash_t* hash, size_t nueva_capacidad){
    lista_t** baldes = malloc(sizeof(lista_t*)*nueva_capacidad);
    if (!baldes) return false;
    pre_setear_lista(baldes,nueva_capacidad);
    
    for (size_t i = 0; i < hash->capacidad; i++){
        lista_t* lista_hash_orig = hash->baldes[i];

        if (lista_hash_orig == NULL){
            continue;
        }
        lista_iter_t * iterador_lista = lista_iter_crear(lista_hash_orig);
        
        while (!lista_iter_al_final(iterador_lista)){
            campo_t* campo = lista_iter_ver_actual(iterador_lista);                
            size_t num_hash = funcion_hash(campo->clave,nueva_capacidad);

            if (baldes[num_hash] == NULL){
                baldes[num_hash] = lista_crear();
            }

            lista_insertar_ultimo(baldes[num_hash],campo);
            lista_iter_borrar(iterador_lista);
        }
        lista_iter_destruir(iterador_lista);
        lista_destruir(lista_hash_orig,NULL);
    }
    
    free(hash->baldes);
    hash->baldes = baldes;

    return true;
}

bool hash_redimensionar_capacidad(hash_t *hash, size_t (*operacion) (hash_t*, size_t*, size_t)){
    size_t primos[] = {2, 3, 5, 7, 11 , 13, 17 ,19, 23, 29, 31, 37};
    size_t n = 12;
    size_t nueva_capacidad = (*operacion)(hash, primos, n);

    if (transferir_datos(hash, nueva_capacidad)){
        hash->capacidad = nueva_capacidad;
        return true;
    }
    return false;
}

size_t busqueda_mayores(size_t buscado,size_t inicio,size_t fin,bool condicion){
    size_t m = inicio + ((fin-inicio)/2);
    size_t actual = (m*m) + m + 41;
    size_t anterior = (m-1*m-1) + m-1 + 41;

    if (actual < buscado){
        return busqueda_mayores(buscado,m+1,fin,condicion);
    }
    if (anterior < buscado){
        return condicion ? actual : anterior;
    }
    return busqueda_mayores(buscado,inicio,m,condicion);
}

size_t busqueda_menores(size_t buscado, size_t* arreglo, size_t inicio, size_t fin, bool condicion){
    size_t m = inicio + ( (fin-inicio)/2);
    size_t actual = arreglo[m];
    size_t anterior = arreglo[m-1];

    if (actual < buscado){
        return busqueda_menores(buscado,arreglo,m+1,fin,condicion);
    }
    if (anterior < buscado){
        return condicion ? actual : anterior;
    }
    return busqueda_menores(buscado,arreglo,inicio,m,condicion);
}

size_t aumentar_capacidad(hash_t *hash, size_t *primos, size_t n){
    size_t capacidad = hash->capacidad*CTE_AUMENTO;

    if (capacidad <= primos[n-1]){
        return busqueda_menores(capacidad,primos,0,n,AUMENTAR);
    }
    if (capacidad > 41){
        return busqueda_mayores(capacidad,0,40,AUMENTAR);
    }
    return 41;
}

size_t reducir_capacidad(hash_t *hash, size_t *primos, size_t n){
    size_t capacidad = hash->capacidad/CTE_REDUCCION;

    if (capacidad <= primos[n-1]){
        return busqueda_menores(capacidad,primos,0,n,!AUMENTAR);
    }
    if (capacidad > 41){
        return busqueda_mayores(capacidad,0,40,!AUMENTAR);
    }
    return primos[n-1];
}

lista_iter_t* hash_iter_crear_balde_iter(hash_iter_t* iter){
    lista_t** baldes = iter->hash->baldes;
    size_t* actual = &(iter->balde_actual);

    if (hash_cantidad(iter->hash) == 0) return NULL;

    while (baldes[*actual] == NULL){
        (*actual)++;
    }

    lista_iter_t* balde_iter = lista_iter_crear(baldes[*actual]);

    return balde_iter;
}

/***************************
* Primitivas del Hash
****************************/

hash_t *hash_crear(void (*destruir_dato)(void*)){
    hash_t* hash = malloc(sizeof(hash_t));
    if (!hash){
        return NULL;
    }

    lista_t** baldes = malloc(CAPACIDAD_INICIAL * sizeof(lista_t*));
    if (!baldes){
        return NULL;
    }
    hash->baldes = baldes;
    pre_setear_lista(hash->baldes,CAPACIDAD_INICIAL);

    hash->capacidad = CAPACIDAD_INICIAL;
    hash->cantidad = 0;
    hash->destruir_dato = destruir_dato;
    return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    if ((hash->cantidad / hash->capacidad) >= FACTOR_CARGA){
        if (!hash_redimensionar_capacidad(hash,aumentar_capacidad)) return false;
    } 

    size_t num_hash = funcion_hash(clave,hash->capacidad);
    campo_t* campo = _hash_obtener(hash,clave,num_hash,!BORRAR_NODO);

    if (campo != NULL){
        if (hash->destruir_dato) hash->destruir_dato(campo->valor);
        campo->valor = dato;
        return true;
    }

    char* copia_clave = strdup(clave);

    if (copia_clave == NULL) return false;

    campo = campo_crear(copia_clave,dato);

    if (campo == NULL){
       free(copia_clave);
       return false;
    }

    lista_t** baldes = hash->baldes;

    if (baldes[num_hash] == NULL){
        baldes[num_hash] = lista_crear();
    }

    if ((baldes[num_hash] == NULL) || (!lista_insertar_ultimo(baldes[num_hash],campo)) ){
        campo_destruir(campo);
        return false;
    }
    hash->cantidad++;
    return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
    if (hash_cantidad(hash) == 0 || !clave){
        return NULL;
    }

    if ( (hash->capacidad > CAPACIDAD_INICIAL) && (hash->cantidad/hash->capacidad <= 1/CRITERIO_REDUCCION)) {
        if (!hash_redimensionar_capacidad(hash,reducir_capacidad)) return NULL;
    }

    size_t largo_hash = hash->capacidad;
    size_t indice_balde = funcion_hash(clave, largo_hash);
    campo_t* campo = _hash_obtener(hash, clave, indice_balde, BORRAR_NODO);
    
    if (campo == NULL) return NULL;

    hash->cantidad--;
    void* valor = campo->valor;
    campo_destruir(campo);
    return valor;
}

void *hash_obtener(const hash_t *hash, const char *clave){
    if (hash_cantidad(hash) == 0 || !clave){
        return NULL;
    }

    size_t indice_balde = funcion_hash(clave,hash->capacidad);
    campo_t* campo = _hash_obtener(hash, clave,indice_balde, !BORRAR_NODO);
    return campo ? campo->valor : NULL;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
    if (hash_cantidad(hash) == 0 || !clave) return NULL;

    size_t num_hash = funcion_hash(clave,hash->capacidad);
    campo_t* campo = _hash_obtener(hash, clave, num_hash, !BORRAR_NODO);
    return campo != NULL;
}

size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}

void hash_destruir(hash_t *hash){
    hash_destruir_dato_t destruir_dato = hash->destruir_dato;

    for (int i = 0; i < hash->capacidad ; i++){
        lista_t* balde = hash->baldes[i];

        if (balde == NULL) continue;

        while (!lista_esta_vacia(balde)){
            campo_t* campo = lista_borrar_primero(balde);
            if (destruir_dato != NULL) destruir_dato(campo->valor);
            campo_destruir(campo);
        }
        lista_destruir(balde,NULL);
    }

    free(hash->baldes);
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
    iterador_hash->hash = hash;
    iterador_hash->balde_actual = 0;
    iterador_hash->iterados = 0;
    iterador_hash->balde_iter = hash_iter_crear_balde_iter(iterador_hash);

    return iterador_hash;
}

bool hash_iter_avanzar(hash_iter_t *iter){
    if (hash_iter_al_final(iter)) return false;

    lista_iter_t* balde_iter = iter->balde_iter;

    lista_iter_avanzar(balde_iter);
    iter->iterados++;
    
    if (!lista_iter_al_final(balde_iter) || hash_iter_al_final(iter)) return true;

    iter->balde_actual++;

    lista_iter_t* nuevo_balde_iter = hash_iter_crear_balde_iter(iter);

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
    return iter->iterados == hash_cantidad(iter->hash);
}

void hash_iter_destruir(hash_iter_t* iter){
    if (iter->balde_iter) lista_iter_destruir(iter->balde_iter);
    free(iter);
}
