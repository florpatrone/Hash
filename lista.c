#include "lista.h"
#include <stdlib.h>

/* Definición de la estructura de una lista */
struct lista{
    void* primero; // Apunta al primero [enlistado] de la lista
    void* ultimo; // Apunta al último [enlistado] de la lista
    size_t largo;
};

/* Definición de la estructura de un nodo */
typedef struct nodo{
    void* dato;
    struct nodo* proximo;
} nodo_t;

/* Definición de la estructura del iterador externo */
struct lista_iter{
    lista_t* lista;
    nodo_t* anterior;
    nodo_t* actual;
};

/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * **************************************************************** */

lista_t *lista_crear(void){
    lista_t* lista = malloc(sizeof(lista_t));

    if (lista == NULL){
        return NULL;
    }
    
    lista->primero = NULL;
    lista->ultimo = NULL;
    lista->largo = 0;
    return lista;
}

void lista_destruir(lista_t *lista, void destruir_dato(void *)){
    nodo_t* nodo = lista->primero;
    while (nodo != NULL){
        nodo_t* proximo = nodo->proximo;
        if (destruir_dato != NULL){
            destruir_dato(nodo->dato);
        }
        free(nodo);
        nodo = proximo;
    }
    free(lista);
}

bool lista_esta_vacia(const lista_t *lista){
    return lista->primero == NULL;
}

nodo_t* crear_nodo(void* valor);
bool lista_insertar_primero(lista_t *lista, void *dato){
    nodo_t* nodo = crear_nodo(dato);
    if (nodo == NULL){
        return false;
    }
    if (lista_esta_vacia(lista)){       // lista vacía
        lista->ultimo = nodo;
    } else {                            // lista no vacía
        nodo->proximo = lista->primero;
        
    }
    lista->primero = nodo;
    lista->largo +=1;
    return true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato){
    nodo_t* nodo = crear_nodo(dato);
    if (nodo == NULL){
        return false;
    }

    if (lista_esta_vacia(lista)){           // lista vacía
        lista->primero = nodo;
    } else {                                // lista no vacía
    nodo_t* nodo_ultimo = lista->ultimo;
    nodo_ultimo->proximo = nodo;
    }
    lista->ultimo = nodo;
    lista->largo += 1;
    return true;
}

void *lista_ver_ultimo(const lista_t* lista){
    if( lista_esta_vacia(lista)){
        return NULL;
    }
    nodo_t* nodo_ultimo = lista->ultimo;
    return nodo_ultimo->dato;
}

size_t lista_largo(const lista_t *lista){
    size_t largo = lista->largo;
    return largo;
}

void *lista_ver_primero(const lista_t *lista){
    if( lista_esta_vacia(lista)){
        return NULL;
    }
    nodo_t* nodo_primero = lista->primero;
    return nodo_primero->dato;
}

void *lista_borrar_primero(lista_t *lista){
    if (lista->primero == NULL){                // lista vacía
        return NULL;
    }

    nodo_t* nodo = lista->primero;              // lista no vacía
    lista->primero = nodo->proximo;

    if (lista->primero == NULL){                // lista de un sólo elemento
        lista->ultimo = NULL;
    }
    void* valor = nodo->dato;
    free(nodo);
    lista->largo -= 1;
    return valor;
}

// Crea un nodo que guarda el dato pasado por parámetro y cuyo próximo es NULL
// Si no puede crearse, devuelve NULL
nodo_t* crear_nodo(void* valor) {
    nodo_t* nodo = malloc(sizeof(nodo_t));
    
    if (nodo != NULL){
        nodo->dato = valor;
        nodo->proximo = NULL;
    }

    return nodo;
}

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/

// primitiva del iterador interno
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra){
    nodo_t* elemento = lista->primero;
    bool funcion_aplicada = true;

    while (elemento && funcion_aplicada){
        funcion_aplicada = visitar(elemento->dato,extra);
        elemento = elemento->proximo;
    }   
}

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/

lista_iter_t *lista_iter_crear(lista_t *lista){
    lista_iter_t* iter = malloc(sizeof(lista_iter_t));
    if (iter == NULL){
        return NULL;
    }

    iter->lista = lista;
    iter->anterior = NULL;
    iter->actual = lista->primero;

    return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter){
    lista_t* lista = iter->lista;

    if (lista->largo == 0){                                 // lista vacía
        return false;
    }
    if (iter->actual == NULL){                              // iterador al final
        return false;
    }
    nodo_t* nodo = iter->actual;                            // iterador en el medio o al principio

    iter->anterior = iter->actual;
    iter->actual = nodo->proximo;
    return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter){
    if (iter->actual == NULL){                             // lista vacía o iterador al final
        return NULL;
    }
    return iter->actual->dato;
}   

bool lista_iter_al_final(const lista_iter_t *iter){
    return iter->actual == NULL;
}

void lista_iter_destruir(lista_iter_t *iter){
    free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato){
    nodo_t* nodo = crear_nodo(dato);
    if (nodo == NULL){
        return false;
    }

    lista_t* lista = iter->lista;

    if (lista->largo == 0){                     // lista vacía
        lista->primero = nodo;
        lista->ultimo = nodo;
    } else if (iter->anterior == NULL){         // iterador al inicio de la lista
        nodo->proximo = lista->primero;
        lista->primero = nodo;
    } else if (iter->actual == NULL){          // iterador al final de la lista

        nodo_t* ultimo = lista->ultimo;
        ultimo->proximo = nodo;
        lista->ultimo = nodo;
    } else {                                  // iterador en el medio de la lista
        iter->anterior->proximo = nodo;
        nodo->proximo = iter->actual;
    }
    iter->actual = nodo;
    lista->largo += 1;
    return true;
}

void *lista_iter_borrar(lista_iter_t *iter){
    lista_t* lista = iter->lista;
    
    if (lista->largo == 0 || iter->actual == NULL){         // lista vacía o iterador al final de la lista
        return NULL;
    }

    nodo_t* nodo = iter->actual;

    if (iter->anterior == NULL){                            // iterador al inicio de la lista
        lista->primero = nodo->proximo;
        iter->actual = lista->primero;
    } else {                                                // iterador en el medio de la lista
        iter->anterior->proximo = nodo->proximo;
        iter->actual = iter->anterior->proximo;

        if (iter->actual == NULL){
            lista->ultimo = iter->anterior;
        }
    }
    
    lista->largo -= 1;
    
    if (lista->largo == 1){
        lista->ultimo = lista->primero;
    }
    if (lista->largo == 0){
        lista->ultimo = NULL;
    }

    void* dato = nodo->dato;
    free(nodo);
    return dato;
}