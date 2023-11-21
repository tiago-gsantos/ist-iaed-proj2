/* iaed-23 - Tiago Santos - ist1106329 - project2 */

#include "project2.h"
#include "lists-macros.c"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>



/* ----- Funcao main ----- */

/* Le input do user linha a linha e executa os comandos correspondentes, 
   libertando a memoria utilizada no final 
 */
int main(void){
    char comando;
    char buffer[BUFFER_SIZE];
    
    System *sys = (System *) calloc(1, sizeof(System));
    /* Se nao houve memoria para sys */
    if(sys == NULL){
        printf("No memory.\n");
        exit(1);
    }

    /* Le o primeiro char de cada linha até ao comando q */
    while((comando = getchar()) != 'q' && comando != EOF){
        /* Le o resto da linha ate ao \n */
        sys->num_args = le_linha(buffer, sys->args);
        
        /* Chama a funcao correspondente ao comando */
        switch(comando){
            case 'c':
                comando_c(sys);
                break;
            case 'p':
                comando_p(sys);
                break;
            case 'l':
                comando_l(sys);
                break;
            case 'i':
                comando_i(sys);
                break;
            case 'r':
                comando_r(sys);
                break;
            case 'e':
                comando_e(sys);
                break;
            case 'a':
                comando_a(sys);
                break;
        }
    }
    /* Liberta memoria */
    comando_a(sys);
    free(sys);

    return 0;
}



/* ----- Funcao de Parsing ----- */

/* Le uma linha do input para a string buffer, separando cada argumento com '\0'
 * e guarda todos os argumentos no vetor args
 * @param buffer --> string onde é guardada a linha lida
 * @param args --> vetor onde serao guardados os argumentos de um comando
 * @return num_args --> numero de argumentos de um comando
 */
int le_linha(char buffer[BUFFER_SIZE], char *args[MAX_ARGS]){
    char c;
    int aspas = FALSE, palavra = FALSE, num_args = 0, i=0;

    /* Le caracter a caracter ate ao \n */
    while((c = getchar()) != '\n' && c != EOF){

        if(c == '"' && aspas == FALSE) /* Primeiras aspas */
            aspas = TRUE;
        else if(c == '"' && aspas == TRUE) /* Segundas aspas */
            aspas = FALSE;
        else if(c == ' ' && aspas == FALSE){
            if(palavra == TRUE){ /* Fim de uma palavra */
                palavra = FALSE;
                buffer[i++] = '\0';
            }
        }
        else{
            buffer[i] = c;
            if(palavra == FALSE){ /* Inicio de uma palavra */
                args[num_args++] = &buffer[i]; 
                palavra = TRUE;
            }
            i++;
        }
    }

    buffer[i] = '\0';

    return num_args;
}



/* ----- Funcoes dos comandos ----- */

/* Executa o comando c - comportamento depende do numero de argumentos
 * @param sys --> ponteiro para a estrutura sistema
 */
void comando_c(System *sys){
    int len=0;

    Carreira *ptr_c;
    FIND_NODE(ptr_c, sys->tail_c, sys->args[0]);

    switch(sys->num_args){
        case 0:
            print_carreiras(sys);
            break;
        case 1:
            if(ptr_c == NULL) /* Se carreira nao existir */
                NEW_NODE(Carreira, ptr_c, sys->head_c, sys->tail_c, sys);
            else
                print_paragens_carreira(ptr_c, FALSE); /* Ordem normal */
            break;
        case 2:
            len = strlen(sys->args[1]);
            /* Se args[1] for alguma das abreviacoes validas de "inverso" */
            if(len >= 3 && strncmp(sys->args[1], "inverso", len) == 0)
                print_paragens_carreira(ptr_c, TRUE); /* Ordem inversa */
            else
                printf("incorrect sort option.\n"); /* ERRO */
            break;
    }
}


/* Executa o comando p - comportamento depende do numero de argumentos
 * @param sys --> ponteiro para a estrutura sistema
 */
void comando_p(System *sys){ 
    Paragem *ptr_p;
    FIND_NODE(ptr_p, sys->tail_p, sys->args[0]);
    
    switch(sys->num_args){
        case 0:
            print_paragens(sys);
            break;
        case 1:
            if(ptr_p == NULL) /* Se a paragem nao existir */
                printf("%s: no such stop.\n", sys->args[0]); /* ERRO */
            else /* print localizacao da paragem */
                printf("%16.12f %16.12f\n", ptr_p->latitude, ptr_p->longitude);
            break;
        case 3:
            if(ptr_p == NULL){ /* Se a paragem nao existir */
                /* Cria a paragem */
                NEW_NODE(Paragem, ptr_p, sys->head_p, sys->tail_p, sys);
                /* Atualiza campos */
                ptr_p->cap = 5;
                ptr_p->nomes_carreiras = (char **) malloc(ptr_p->cap * sizeof(char *));
                check_memory((void *) ptr_p->nomes_carreiras, sys);
                ptr_p->latitude = strtod(sys->args[1], NULL);
                ptr_p->longitude = strtod(sys->args[2], NULL);
            }
            else
                printf("%s: stop already exists.\n", sys->args[0]); /* ERRO */
            break;
    }
}


/* Executa o comando l - verifica possiveis erros e cria ligacao
 * @param sys --> ponteiro para a estrutura sistema
 */
void comando_l(System *sys){
    Carreira *ptr_c;
    Paragem *ptr_po, *ptr_pd;

    FIND_NODE(ptr_c, sys->tail_c, sys->args[0]);
    FIND_NODE(ptr_po, sys->tail_p, sys->args[1]);
    FIND_NODE(ptr_pd, sys->tail_p, sys->args[2]);

    /* Se nao houver erros cria ligacao */
    if(erros_l(ptr_c, ptr_po, ptr_pd, sys->args) == FALSE)
        if(cria_ligacao(ptr_c, ptr_po, ptr_pd, sys) == FALSE)
            printf("link cannot be associated with bus line.\n"); /* ERRO */
}


/* Executa o comando i - print todas as paragens que pertencem a mais que uma
 * carreira e todas as carreiras que nelas passam por ordem alfabetica
 * @param sys --> ponteiro para a estrutura sistema
 */
void comando_i(System *sys){
    Paragem *atual;
    int i;

    for(atual = sys->head_p; atual != NULL; atual = atual->next){
        if(atual->num_carreiras > 1){
            printf("%s %d:", atual->nome, atual->num_carreiras);

            quicksort(atual->nomes_carreiras, 0, atual->num_carreiras - 1);

            for(i=0; i < atual->num_carreiras; i++)
                printf(" %s", atual->nomes_carreiras[i]);
            printf("\n");
        }
    }
}


/* Executa o comando r - remove carreira da lista de carreiras e liberta a
 * memoria usada pela carreira
 * @param sys --> ponteiro para a estrutura sistema
 */
void comando_r(System *sys){
    Carreira *ptr_c;
    FIND_NODE(ptr_c, sys->tail_c, sys->args[0]);

    if(ptr_c == NULL){
        printf("%s: no such line.\n", sys->args[0]); /* ERRO */
        return;
    }

    /* Remove carreira da lista de carreiras */
    RM_NODE(ptr_c, sys->head_c, sys->tail_c);

    /* Liberta a memoria do percurso da carreira e remove a carreira do vetor
    de carreiras de todas as paragens do percurso */
    if(ptr_c->tail_l != NULL){
        while(ptr_c->tail_l->prev != NULL){
            rm_carreira_paragem(ptr_c->nome, ptr_c->tail_l->nome_paragem, sys->tail_p);
            ptr_c->tail_l = ptr_c->tail_l->prev;
            free(ptr_c->tail_l->next);
        }
        rm_carreira_paragem(ptr_c->nome, ptr_c->tail_l->nome_paragem, sys->tail_p);
        free(ptr_c->head_l);
    }
    free(ptr_c->nome);
    free(ptr_c);
}


/* Executa o comando e - remove paragem da lista de paragens e do percurso de
 * todas as carreiras do sistema
 * @param sys --> ponteiro para a estrutura sistema
 */
void comando_e(System *sys){
    int i;
    Paragem *ptr_p;

    FIND_NODE(ptr_p, sys->tail_p, sys->args[0]);
    
    if(ptr_p == NULL){
        printf("%s: no such stop.\n", sys->args[0]);
        return;
    }

    RM_NODE(ptr_p, sys->head_p, sys->tail_p);

    /* Cicla por todas as carreiras do vetor de carreiras da paragem */
    for(i=0; i < ptr_p->num_carreiras; i++){
        Ligacao *atual;
        Carreira *ptr_c;
        FIND_NODE(ptr_c, sys->tail_c, ptr_p->nomes_carreiras[i]);

        /* Remove paragem da carreira */
        for(atual = ptr_c->head_l; atual != NULL;){
            if(strcmp(atual->nome_paragem, ptr_p->nome) == 0)
                atual = rm_ligacao_percurso(ptr_c, atual);
            else atual = atual->next;
        }
    }

    free(ptr_p->nomes_carreiras);
    free(ptr_p->nome);
    free(ptr_p);
}


/* Executa o comando a - liberta toda a memoria utilizada até ao momento
 * deixando apenas a estrutura sistema
 * @param sys --> ponteiro para a estrutura sistema
 */
void comando_a(System *sys){
    /* Free carreiras do sistema */
    while(sys->tail_c != NULL){
        /* Free ligacoes da carreira */
        while(sys->tail_c->tail_l != NULL)
            FREE_NODE(sys->tail_c->head_l, sys->tail_c->tail_l);

        free(sys->tail_c->nome);
        FREE_NODE(sys->head_c, sys->tail_c);
    }

    /* Free paragens do sistema */
    while(sys->tail_p != NULL){
        free(sys->tail_p->nome);
        free(sys->tail_p->nomes_carreiras);
        FREE_NODE(sys->head_p, sys->tail_p);
    }
}



/* ----- Funcoes auxiliares ----- */


/* Ve se o programa conseguiu alocar memoria para o ponteiro e se nao conseguiu
 * da erro, liberta a memoria usada pelo programa e termina o programa.
 * @param ptr --> ponteiro que queremos verificar se e ou nao NULL
 * param sys --> ponteiro para a estrutura sistema */
void check_memory(void *ptr, System *sys){
    if(ptr == NULL){
        printf("No memory.\n");
        comando_a(sys);
        free(sys);
        exit(1);
    }
}


/* Lista todas as carreiras do sistema e alguns dos seus dados
 * param sys --> ponteiro para a estrutura sistema
 */
void print_carreiras(System *sys){
    Carreira *atual;

    for(atual = sys->head_c; atual != NULL; atual = atual->next){
        printf("%s ", atual->nome);
        if(atual->num_pargens == 0)
            printf("0 ");
        else
            printf("%s %s %d ", atual->head_l->nome_paragem, 
                                atual->tail_l->nome_paragem,
                                atual->num_pargens);
        printf("%.2f %.2f\n", atual->custo, atual->duracao);
    }
}


/* Lista todas as paragens de uma carreira pela ordem pedida
 * @param ptr_c --> ponteiro para a carreira
 * @param inverso --> pode ter valores TRUE ou FALSE
 */
void print_paragens_carreira(Carreira *ptr_c, int inverso){
    Ligacao *atual;

    if(ptr_c->num_pargens != 0){
        if(inverso == TRUE){
            for(atual=ptr_c->tail_l; atual->prev != NULL; atual = atual->prev)
                printf("%s, ", atual->nome_paragem);
            printf("%s\n", atual->nome_paragem);
        }
        else{
            for(atual=ptr_c->head_l; atual->next != NULL; atual = atual->next)
                printf("%s, ", atual->nome_paragem);
            printf("%s\n", atual->nome_paragem);
        }
    }
}


/* Lista todas as paragens do sistema e alguns dos seus dados 
 * param sys --> ponteiro para a estrutura sistema
 */
void print_paragens(System *sys){
    Paragem *atual;

    if(sys->head_p != NULL)
        for(atual = sys->head_p; atual != NULL; atual = atual->next)
            printf("%s: %16.12f %16.12f %d\n", atual->nome, 
                                               atual->latitude, 
                                               atual->longitude, 
                                               atual->num_carreiras); 
}


/* Verifica se os dados da criacao da ligacao sao validos
 * @param ptr_c --> ponteiro para carreira
 * @param ptr_po--> ponteiro para paragem de origem da ligacao
 * @param ptr_pd --> ponteiro para paragem de destino da ligacao
 * @param args --> vetor com os argumentos de um comando
 * @return --> FALSE se nao houver erros e TRUE caso contrario
 */
int erros_l(Carreira *ptr_c, Paragem *ptr_po, Paragem *ptr_pd, char *args[MAX_ARGS]){
    /* Se carreira nao existir */
    if(ptr_c == NULL){          
        printf("%s: no such line.\n", args[0]); /* ERRO */
        return TRUE;
    }
    /* Se paragem origem nao existir */
    else if(ptr_po == NULL){
        printf("%s: no such stop.\n", args[1]); /* ERRO */
        return TRUE;
    }
    /* Se paragem destino nao existir */
    else if(ptr_pd == NULL){
        printf("%s: no such stop.\n", args[2]); /* ERRO */
        return TRUE;
    }
    /* Se custo ou duracao forem negativos */
    else if((args[3][0] == '-' || args[4][0] == '-')){
        printf("negative cost or duration.\n"); /* ERRO */
        return TRUE;
    }
    return FALSE;
}


/* Cria uma ligacao e adiciona-a no sitio certo do percurso, atualizando os
 * seus campos e os da carreira que contem a ligacao
 * @param ptr_c --> ponteiro para a carreira que contem a ligacao
 * @param ptr_po --> ponteiro para a paragem de origem da ligacao
 * @param ptr_pd --> ponteiro para a paragem de destino da ligacao
 * @param sys --> ponteiro para a estrutura sistema
 * @return --> TRUE se a ligacao foi criada com sucesso e FALSE caso contrario
 */
int cria_ligacao(Carreira *ptr_c, Paragem *ptr_po, Paragem *ptr_pd, System *sys){
    /* Cria a primeira ligacao */
    if(ptr_c->head_l == NULL){
        Ligacao *l_origem = NEW_l(ptr_c, ptr_po, sys);
        Ligacao *l_destino = NEW_l(ptr_c, ptr_pd, sys);

        ptr_c->head_l = l_origem;
        ptr_c->tail_l = l_destino;
        l_origem->next = l_destino;
        l_destino->prev = l_origem;

        atualiza_custo_duracao(l_origem, ptr_c, sys->args);
        add_carreira_paragem(ptr_c->nome, ptr_po, sys);
        add_carreira_paragem(ptr_c->nome, ptr_pd, sys);

        return TRUE;
    }
    /* Cria uma ligacao no fim da carreira */
    else if(strcmp(ptr_po->nome, ptr_c->tail_l->nome_paragem) == 0){
        Ligacao *l = NEW_l(ptr_c, ptr_pd, sys);

        l->prev = ptr_c->tail_l;
        ptr_c->tail_l->next = l;
        ptr_c->tail_l = l;

        atualiza_custo_duracao(l->prev, ptr_c, sys->args);
        add_carreira_paragem(ptr_c->nome, ptr_pd, sys);

        return TRUE;
    }
    /* Cria uma ligacao no inicio da carreira */
    else if(strcmp(ptr_pd->nome, ptr_c->head_l->nome_paragem) == 0){
        Ligacao *l = NEW_l(ptr_c, ptr_po, sys);

        l->next = ptr_c->head_l;
        ptr_c->head_l->prev = l;
        ptr_c->head_l = l;

        atualiza_custo_duracao(l, ptr_c, sys->args);
        add_carreira_paragem(ptr_c->nome, ptr_po, sys);

        return TRUE;
    }
    return FALSE;
}


/* Aloca memoria para a ligacao e atualiza os seus campos
 * @param ptr_c --> ponteiro para a carreira que contem a ligacao
 * @param ptr_p --> ponteiro para a paragem
 * @param sys --> ponteiro para a estrutura sistema
 * @return l --> ponteiro para a ligacao criada
 */
Ligacao * NEW_l(Carreira *ptr_c, Paragem *ptr_p, System *sys){
    Ligacao *l = (Ligacao *) calloc(1, sizeof(Ligacao));
    check_memory((void *) l, sys);
    
    l->nome_paragem = ptr_p->nome;
    ptr_c->num_pargens++;

    return l;
}


/* Adiciona o custo e duracao a ligacao e a carreira que contem a ligacao
 * @param ptr_l --> ponteiro para a ligacao
 * @param ptr_c --> ponteiro para a carreira
 * @param args --> vetor com os argumentos de um comando
 */
void atualiza_custo_duracao(Ligacao *ptr_l, Carreira *ptr_c, char *args[MAX_ARGS]){
    ptr_l->custo = strtod(args[3], NULL);
    ptr_l->duracao = strtod(args[4], NULL);

    ptr_c->custo += ptr_l->custo;
    ptr_c->duracao += ptr_l->duracao;
}


/* Adiciona o nome de uma carreira ao vetor de carreiras de uma paragem, 
 * realocando memoria para o vetor se necessario 
 * @param nome_c --> nome da carreira que queremos adicionar ao vetor 
 * @param ptr_p --> ponteiro para a paragem que queremos adicionar a carreira
 * @param sys --> ponteiro para a estrutura sistema
 */
void add_carreira_paragem(char *nome_c, Paragem *ptr_p, System* sys){
    int i;

    /* Se o nome da carreira ja pertencer ao vetor */
    for(i=0; i < ptr_p->num_carreiras; i++)
        if(strcmp(nome_c, ptr_p->nomes_carreiras[i]) == 0)
            return;
    
    /* Realoca memoria */
    if(ptr_p->num_carreiras == ptr_p->cap){
        ptr_p->cap *= 2;
        ptr_p->nomes_carreiras = (char **) realloc(ptr_p->nomes_carreiras, sizeof(char *) * ptr_p->cap);
        check_memory((void *) ptr_p->nomes_carreiras, sys);
    }

    ptr_p->nomes_carreiras[i] = nome_c;
    ptr_p->num_carreiras++;
}


/* Remove o nome de uma carreira do vetor de carreiras de uma paragem
 * @param nome_c --> nome da carreira que queremos remover do vetor 
 * @param nome_p --> nome da paragem da qual queremos remover a carreira
 * @param tail_p --> ponteiro para a ultima paragem do sistema
 */
void rm_carreira_paragem(char *nome_c, char *nome_p, Paragem *tail_p){
    int i;

    Paragem *ptr_p;
    FIND_NODE(ptr_p, tail_p, nome_p);

    for(i=0; i < ptr_p->num_carreiras; i++)
        if(ptr_p->nomes_carreiras[i] == nome_c){
            /* Trocar o nome da carreira pelo ultimo nome do vetor */
            troca(&ptr_p->nomes_carreiras[i], 
                  &ptr_p->nomes_carreiras[--(ptr_p->num_carreiras)]);
            /* Apontar a posicao do nome da carreira para NULL */
            ptr_p->nomes_carreiras[ptr_p->num_carreiras] = NULL;
        }
}


/* Remove uma ligacao do percurso da carreira, atualizando o custo e duracao
 * da carreira quando necessario
 * @param ptr_c --> ponteiro para a carreira
 * @param ptr_l --> ponteiro para a ligacao que queremos atualizar 
 */
Ligacao *rm_ligacao_percurso(Carreira *ptr_c, Ligacao *ptr_l){
    Ligacao *temp;
    
    /* Se for a primeira ligacao do percurso */
    if(ptr_l == ptr_c->head_l){
        ptr_c->custo -= ptr_l->custo;
        ptr_c->duracao -= ptr_l->duracao;
    }
    /* Se for a ultima ligacao do percurso */
    else if(ptr_l == ptr_c->tail_l){
        ptr_c->custo -= ptr_l->prev->custo;
        ptr_c->duracao -= ptr_l->prev->duracao;
        ptr_l->prev->custo = 0;
        ptr_l->prev->duracao = 0;
    }
    else{
        ptr_l->prev->custo += ptr_l->custo;
        ptr_l->prev->duracao += ptr_l->duracao;
    }

    RM_NODE(ptr_l, ptr_c->head_l, ptr_c->tail_l);
    
    temp = ptr_l;
    ptr_l = ptr_l->next;
    free(temp);

    ptr_c->num_pargens--;

    return ptr_l;
}


/* Ordena um vetor de strings por ordem alfabetica 
 * @param a --> ponteiro para um vetor de strings
 * @param left --> indice do elemento mais a esquerda
 * @param right --> indice do elemento mais a direita 
 */
void quicksort(char **a, int left, int right){
    int i;

    if(right <= left)
        return;

    i = partition(a, left, right);

    quicksort(a, left, i-1);
    quicksort(a, i+1, right);
}


/* Ordena um vetor de strings em torno de um elemento (o pivot) colocando os
   elementos menores que ele a sua esquerda e os maiores a sua direita 
 * @param a --> ponteiro para um vetor de strings
 * @param left --> indice do elemento mais a esquerda
 * @param right --> indice do elemento mais a direita 
 */
int partition(char **a, int left, int right){
    int i = left-1;
    int j = right;

    /* Declarar o pivot como a string mais a direita */
    char *pivot = a[right];

    /* Com dois contadores comecando em lados opostos do vetor ir trocando
    elementos que estejam na posicao errada ate se encontrarem*/
    while(i < j){
        while(strcmp(a[++i], pivot) < 0);
        while(strcmp(pivot, a[--j]) < 0)
            if(j == left)
                break;
        if(i < j)
            troca(&a[i], &a[j]);
    }
    /* Trocar o pivot com o elemento onde os contadores se encontraram*/
    troca(&a[i], &a[right]);
    
    return i;
}


/* Troca o conteudo de duas strings uma com a outra
 * @param s1 --> ponteiro para ponteiro para uma das strings que queremos trocar
 * @param s2 --> ponteiro para ponteiro para a outra string
 */
void troca(char **s1, char **s2){
    char *temp = *s1;
    *s1 = *s2;
    *s2 = temp;
}


