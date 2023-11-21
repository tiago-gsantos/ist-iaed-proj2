/* iaed-23 - Tiago Santos - ist1106329 - project2 */


/* ---- Constantes e estados ----- */

#define MAX_ARGS 5        /* Numero maximo de argumentos de um comando */
#define BUFFER_SIZE 65536 /* Tamanho maximo de uma linha no input*/

#define FALSE 0
#define TRUE 1



/* ----- Estruturas ----- */

typedef struct ligacao{
    char *nome_paragem;
    double custo; /* custo é guardado apenas na origem da ligacao */
    double duracao; /* duracao é guardada apenas na origem da ligacao */
    struct ligacao *prev, *next;
} Ligacao;

typedef struct carreira{
    char *nome;
    Ligacao *head_l, *tail_l;
    int num_pargens;
    double custo;   /* Custo total de todas as ligacoes da carreira */
    double duracao; /* Duracao total de todas as ligacoes da carreira */
    struct carreira *prev, *next;
} Carreira;

typedef struct paragem{
    char *nome;
    char **nomes_carreiras; /* Vetor com os nomes das carreiras que intersetam na paragem */
    int cap; /* Capacidade do vetor de carreiras */
    int num_carreiras; /* Numero de carreiras no vetor de carreiras */
    double latitude;
    double longitude;
    struct paragem *prev, *next;
} Paragem;

typedef struct system{
    char *args[MAX_ARGS]; /* Vetor com os argumentos de um comando */
    int num_args;
    Carreira *head_c, *tail_c;
    Paragem *head_p, *tail_p;
} System;



/* ----- Prototipos ----- */

int le_linha(char buffer[BUFFER_SIZE], char *args[MAX_ARGS]);

void comando_c(System *sys);
void comando_p(System *sys);
void comando_l(System *sys);
void comando_i(System *sys);
void comando_r(System *sys);
void comando_e(System *sys);
void comando_a(System *sys);

void check_memory(void *ptr, System *sys);

void print_carreiras(System *sys);
void print_paragens_carreira(Carreira *ptr_c, int inverso);

void print_paragens(System *sys);

int erros_l(Carreira *ptr_c, Paragem *ptr_po, Paragem *ptr_pd, char *args[MAX_ARGS]);
int cria_ligacao(Carreira *ptr_c, Paragem *ptr_po, Paragem *ptr_pd, System *sys);
Ligacao * NEW_l(Carreira *ptr_c, Paragem *ptr_p, System *sys);
void atualiza_custo_duracao(Ligacao *ptr_l, Carreira *ptr_c, char *args[MAX_ARGS]);
void add_carreira_paragem(char *nome_c, Paragem *ptr_p, System *sys);

void rm_carreira_paragem(char *nome_c, char *nome_p, Paragem *tail_p);

Ligacao *rm_ligacao_percurso(Carreira *ptr_c, Ligacao *ptr_l);

void quicksort(char **a, int left, int right);
int partition(char **a, int left, int right);
void troca(char **s1, char **s2);

