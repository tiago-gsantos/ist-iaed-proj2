/* iaed-23 - Tiago Santos - ist1106329 - project2 */

#include <string.h>
#include <stdlib.h>

/* ----- Macros ----- */
/* Estes defines servem para manipular nodes de listas utilizando abstracao */


/* Cria um node de um certo tipo, alocando memoria para a estrutura e para o
 * nome, copiando o nome dos argumentos e adicionando-o ao fim da lista 
 * correspondente ao tipo do node
 * @param type --> tipo da estrutura s
 * @param s --> ponteiro para a estrutura que queremos adicionar a lista
 * @param head --> cabeca da lista
 * @param tail --> cauda da lista
 * @param sys --> ponteiro para a estrutura sistema
 */
#define NEW_NODE(type, s, head, tail, sys) do { \
    int len = strlen(sys->args[0]); \
    s = (type *) calloc(1, sizeof(type)); \
    check_memory((void *) s, sys); \
    \
    s->nome = (char *) malloc(sizeof(char) * (len+1)); \
    check_memory((void *) s->nome, sys); \
    strcpy(s->nome, sys->args[0]); \
    s->nome[len] = '\0'; \
    \
    if(head == NULL) head = s; \
    else{ \
        tail->next = s; \
        s->prev = tail; \
    } \
    tail = s; \
} while (0)


/* Encontra o node numa lista que tenha o nome dado. Este nome pode vir NULL
 * @param s --> ponteiro para a estrutura que queremos encontrar
 * @param tail --> cauda da lista
 * @param nome_s --> nome do node que queremos encontrar
 */
#define FIND_NODE(s, tail, nome_s) do { \
    if(nome_s == NULL) \
        s = NULL; \
    else{ \
        for(s = tail; s != NULL; s = s->prev) \
            if(strcmp(s->nome, nome_s) == 0) \
                break; \
    } \
} while (0)


/* Remove o node dado da lista atualizando os nodes prev e next
 * @param s --> ponteiro para a estrutura que queremos remover da lista
 * @param head --> cabeca da lista
 * @param tail --> cauda da lista
 */
#define RM_NODE(s, head, tail) do { \
    if(s->prev != NULL) \
        s->prev->next = s->next; \
    if(s->next != NULL) \
        s->next->prev = s->prev; \
    if(s->prev == NULL) \
        head = s->next; \
    if(s->next == NULL) \
        tail = s->prev; \
} while (0)


/* Remove o ultimo node da lista e liberta a sua memoria
 * @param head --> cabeca da lista
 * @param tail --> cauda da lista
 */
#define FREE_NODE(head, tail) do { \
    tail = tail->prev; \
    \
    if(tail != NULL) \
        free(tail->next); \
    else \
        free(head); \
    head = NULL; \
    tail = NULL; \
} while (0)



