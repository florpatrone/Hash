#include <stdbool.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* La lista está planteada como una lista de punteros genéricos. */
typedef struct lista lista_t;

/* El iterador está planteado como una estructura con dos punteros: anterior y actual. */
typedef struct lista_iter lista_iter_t;

typedef unsigned long size_t;

/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t *lista_crear(void);

// Devuelve verdadero si la lista no tiene elementos enlistados, false en caso contrario.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Agrega un nuevo elemento al principio de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, valor se encuentra al principio
// de la lista.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Agrega un nuevo elemento al final de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, valor se encuentra al final
// de la lista.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Saca el primer elemento de la lista. Si la lista tiene elementos, se quita el
// primero de la lista, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista
// contiene un elemento menos, si la lista no estaba vacía.
void *lista_borrar_primero(lista_t *lista);

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void *lista_ver_primero(const lista_t *lista);

// Obtiene el valor del último elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del último, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el último elemento de la lista, cuando no está vacía.
void *lista_ver_ultimo(const lista_t* lista);

// Devuelve el tamaño de la lista. Si la lista está vacía, devuelve 0.
// Pre: la lista fue creada.
size_t lista_largo(const lista_t *lista); 

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *));

/* ******************************************************************
 *                    PRIMITIVAS DE LOS ITERADORES
 * *****************************************************************/

/*************
 *  INTERNOS
 * ***********/

// Primitiva del iterador interno.
// Visitar es una función provista por parámetro
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);

/*************
 *  EXTERNOS
 * ***********/

// Crea un iterador externo nuevo.
// Post: devuelve un iterador cuyo anterior es NULL y su actual
// es el primer elemento de la lista
lista_iter_t *lista_iter_crear(lista_t *lista);

// Avanza una posición iterando sobre la lista a la cual
// el iterador hace referencia. Devuelve true si pudo avanzar y false
// si no pudo. Si la lista está vacía o el iterador está al final, 
// devuelve false.
// Pre: el iterador fue creado
// Post: el iterador avanzó un elemento sobre la lista
bool lista_iter_avanzar(lista_iter_t *iter);

// Devuelve el valor que apunta el actual del iterador.
// Si la lista está vacía o el iterador está al final, 
// devuelve NULL.
// Pre: el iterador fue creado.
// Post: devuelve un valor void*
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Devuelve true si el iterador ya recorrió todos los elementos de la lista
// y se encuentra al final. De lo contrario, devuelve false.
// Si la lista está vacía, se define que el iterador está al final y al principio al mismo tiempo,
// entonces devuelve true.
// Pre: el iterador fue creado.
bool lista_iter_al_final(const lista_iter_t *iter);

// Destruye el iterador.
// Pre: el iterador fue creado.
void lista_iter_destruir(lista_iter_t *iter);

// Inserta un nuevo elemento entre el anterior y el actual del iterador.
// Devuelve true si pudo insertarlo y false en caso contrario.
// Pre: el iterador fue creado.
// Post: la lista tiene un nuevo elemento. El actual del iterador señala
// al elemento nuevo.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Borra de la lista al elemento señalado por el actual del iterador
// y lo devuelve. Si la lista está vacía o el iterador está al final, 
// devuelve false.
// Pre: el iterador fue creado.
// Post: la lista contiene un elemento menos.
void *lista_iter_borrar(lista_iter_t *iter);
