#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <string.h>
#include "api.h"


/**
 * ######### Funções internas da biblioteca #########
 * ##################################################
 **/
Mapa *new_mapa() 
{
    Mapa *m = malloc(sizeof(Mapa));
    m->firstC = NULL;
    m->lastC = NULL;
    m->numCidades = 0;
    return m;
}

Cidade *procura_cidade(Mapa *m, char *codigo) 
{
    /* Inicializa o ponteiro aux com a primeira cidade do mapa */
    Cidade *aux = m->firstC;

    /* Itera através de todas as cidades no mapa */
    while (aux)
    {
        /* Compara o código da cidade atual com o código fornecido */
        if (strcmp(aux->codigo, codigo) == 0)
            /* Se os códigos forem iguais, retorna a cidade atual */
            return aux;

        /* Atualiza o ponteiro aux para a próxima cidade */
        aux = aux->nextC;
    }

    /* Se a cidade não for encontrada, retorna NULL */
    return NULL;
}

Cidade *percorre_cidades(Cidade *cidade_atual) 
{
    /** Retorna o ponteiro para a próxima cidade na lista **/
    if (cidade_atual != NULL)
    {
        return cidade_atual->nextC;
    }

    /** Retorna NULL se a cidade_atual for NULL **/
    return NULL;
}

Lig *procura_ligacoes(Cidade *cidade, char *codigo_destino) 
{
    /* Inicializa o ponteiro aux com a primeira ligação da cidade */
    Lig *aux = cidade->first;

    /* Itera através de todas as ligações da cidade */
    while (aux) {
        /* Compara o código de destino da ligação atual com o código fornecido */
        if (strcmp(aux->destino, codigo_destino) == 0)
            /* Se os códigos de destino forem iguais, retorna a ligação atual */
            return aux;

        /* Atualiza o ponteiro aux para a próxima ligação */
        aux = aux->nextL;
    }

    /* Se a ligação não for encontrada, retorna NULL */
    return NULL;
}

Lig *percorre_ligacoes(Lig *cidade_atual) 
{
    /** Retorna o ponteiro para a próxima cidade na lista **/
    if (cidade_atual != NULL)
    {
        return cidade_atual->nextL;
    }

    /** Retorna NULL se a cidade_atual for NULL **/
    return NULL;
}

Path *inicializa_todos(Mapa *m, char *cidadeOrigem, char *indice) {
    Path *todos = malloc(m->numCidades * sizeof(Path));  /* Aloca memória para o array de Path */
    Cidade *c = m->firstC;  /* Ponteiro para a primeira cidade no mapa */
    int i;
    for (i = 0; i < m->numCidades; i++) {
        todos[i].cidade = c;  /* Atribui o ponteiro da cidade atual ao campo cidade do elemento Path correspondente */
        todos[i].processed = NAO;  /* Define o status de processamento como NAO */
        todos[i].totalValue = (strcmp(c->codigo, cidadeOrigem) == 0) ? 0 : __DBL_MAX__;  /* Define o valor total do caminho como 0 se a cidade atual for a cidade de origem, caso contrário, define-o como o maior valor possível (__DBL_MAX__) */
        if (strcmp(c->codigo, cidadeOrigem) == 0) {
            todos[i].totalPath = strdup(cidadeOrigem);  /* Cria uma cópia da cidade de origem como caminho total */
        } else {
            todos[i].totalPath = NULL;  /* Define como NULL para cidades diferentes da cidade de origem */
        }
        c = c->nextC;  /* Atualiza o ponteiro para a próxima cidade na próxima iteração do loop */
    }
    return todos;  /* Retorna o array de Path inicializado */
}

Bool existe_nao_processado(Path *todos, int numCidades) {
    int i;
    for (i = 0; i < numCidades; i++) {
        if (todos[i].processed == NAO) {  /* Verifica se o status de processamento do caminho é NAO */
            return SIM;  /* Retorna SIM se houver pelo menos um caminho não processado */
        }
    }
    return NAO;  /* Retorna NAO se todos os caminhos estiverem processados */
}

Path *getPath(Mapa *m, Path *todos, char *id) {
    Path *p = todos;  /* Inicializa o ponteiro p para o array de caminhos todos */
    int i;

    for (i = 0; i < m->numCidades; i++) {
        if ((!strcmp(p->cidade->codigo, id)) && (p->processed == NAO) && (p->cidade->estado == 1)) {
            return p;  /* Retorna o caminho p se as condições forem atendidas */
        }
        p++;  /* Avança para o próximo elemento do array de caminhos */
    }
    return NULL;  /* Retorna NULL se nenhum caminho correspondente for encontrado */
}

Path *getOpenShortest(Mapa *m, Path *todos) {
    Path *menor = NULL;  /* Inicializa um ponteiro menor como NULL */
    int i;
    for (i = 0; i < m->numCidades; i++) {
        if (todos[i].processed == NAO && (menor == NULL || todos[i].totalValue < menor->totalValue)) {
            menor = &todos[i];  /* Atualiza o ponteiro menor se as condições forem atendidas */
        }
    }
    return menor;  /* Retorna o ponteiro menor */
}

double getLinkValue(Lig *l, char *indice) {
    switch (*indice) {
        case 'H':
            return l->indiceTemporal;  /* Retorna o valor do índice temporal */
            break;
        case 'E':
            return l->indiceEconomico;  /* Retorna o valor do índice econômico */
            break;
        case 'T':
            return l->indiceTuristico;  /* Retorna o valor do índice turístico */
            break;
    }
    return l->indiceTemporal;  /* Retorna o valor padrão do índice temporal caso o caractere do índice seja desconhecido */
}

char *concatPath(char *p, const char *concat) {
    size_t a = strlen(p);  /* Tamanho da string p */
    size_t b = strlen(concat);  /* Tamanho da string concat */
    size_t size_ab = a + 2 + b + 1;  /* Tamanho necessário para a nova string concatenada */

    char *dest = malloc(size_ab);  /* Aloca memória para a nova string concatenada */
    if (dest == NULL) {
        return NULL;  /* Retorna NULL em caso de falha na alocação de memória */
    }

    sprintf(dest, "%s->%s", p, concat);  /* Concatena as strings p e concat com "->" e armazena em dest */
    return dest;  /* Retorna o ponteiro para a nova string concatenada */
}

void imprime_melhor_rota(Path *todos, int numCidades, char *cidadeOrigem, char *cidadeDestino, char *indice) {
    int i;
    for (i = 0; i < numCidades; i++) {

        /* Compara se a rota criada é para o destino */
        if (strcmp(todos[i].cidade->codigo, cidadeDestino) == 0) {
            /* Caso exista rota entra no if*/
            if (todos[i].totalPath != NULL) {
                MSG_ROUTE_HEADER(cidadeOrigem, cidadeDestino, *indice, todos[i].totalValue);
                MSG_ROUTE_ITEM(todos[i].totalPath);
                return;  /* Retorna após imprimir a melhor rota */
            }
        }
    }
    ERROR_NO_ROUTE(cidadeOrigem, cidadeDestino);  /* Imprime uma mensagem de erro se não houver rota disponível */
}

void change_indice(Mapa *m, char *codigo_origem, char *codigo_last, float indice, char type) {
    /* Utiliza a função procura_cidade para encontrar a cidade de origem */
    Cidade *first = procura_cidade(m, codigo_origem);

    /* Utiliza a função procura_cidade para encontrar a cidade de destino */
    Cidade *last = procura_cidade(m, codigo_last);

    /* Verifica se ambas as cidades não existem e exibe erros, caso necessário */
    if (first == NULL && last == NULL) {
        ERROR_NO_CITY(codigo_origem);
        ERROR_NO_CITY(codigo_last);
        return;
    }

    /* Verifica se a cidade de origem não existe e exibe erro */
    if (first == NULL) {
        ERROR_NO_CITY(codigo_origem);
        return;
    }

    /* Verifica se a cidade de destino não existe e exibe erro */
    if (last == NULL) {
        ERROR_NO_CITY(codigo_last);
        return;
    }

    /* Verifica se as cidades de origem e destino são as mesmas e exibe erro */
    if (first->codigo == last->codigo) {
        ERROR_CITY_REPEATED(first->codigo);
        return;
    }

    /* Procura a ligação na cidade de origem */
    Lig *aux = procura_ligacoes(first, codigo_last);

    /* Se a ligação não for encontrada, exibe um erro e retorna */
    if (aux == NULL) {
        ERROR_NO_LINK(codigo_origem, codigo_last);
        return;
    }

    /* Altera o índice turístico, econômico ou temporal, conforme especificado pelo parâmetro 'type' */
    if (type == 'T') {
        aux->indiceTuristico = indice;
        return;
    } 
    if (type == 'E') {
        aux->indiceEconomico = indice;
        return;
    }
    if (type == 'H') {
        aux->indiceTemporal = indice; 
        return;
    }

}

/**
 * ######### Implementação das Funções da Biblioteca#########
 * ###########################################################
 **/

void adicionar_cidade(Mapa *m, char *codigo, char *nome) 
{
   /** Utiliza a função procura_cidade para verificar se a cidade já existe **/
    Cidade *cidade_existente = procura_cidade(m, codigo);
    if (cidade_existente != NULL)
    {
        ERROR_CITY_REPEATED(codigo);
        return;
    }

    /** Inicializa o ponteiro auxiliar para percorrer a lista de cidades **/
    Cidade *aux = m->firstC;
    /** Declara um ponteiro para a nova cidade que será adicionada **/
    Cidade *nova_cidade;

    /** Percorre a lista de cidades até encontrar uma cidade com código maior ou até chegar ao final da lista **/
    while (aux)
    {
        /** Se o código da cidade atual na lista for maior que o novo, interrompe o loop **/
        if (strcmp(aux->codigo, codigo) > 0)
            break;

        /** Atualiza o ponteiro auxiliar para a próxima cidade na lista **/
        aux = aux->nextC;
    }

     nova_cidade = malloc(sizeof(Cidade));

    
    nova_cidade->first = NULL;
    nova_cidade->last = NULL;
    nova_cidade->numLigacoes = 0;
    nova_cidade->estado = 1; 
    
    memset(nova_cidade->codigo, '\0', CITY_ID + 1);
    strncpy(nova_cidade->codigo, codigo, CITY_ID);

    nova_cidade->nome = malloc((MAX_CITY_NAME + 1) * sizeof(char));
    memset(nova_cidade->nome, '\0', MAX_CITY_NAME + 1);
    strncpy(nova_cidade->nome, nome, MAX_CITY_NAME);
    nova_cidade->nome[MAX_CITY_NAME] = '\0';

    nova_cidade->nextC = NULL;
    nova_cidade->prevC = NULL;
    m->numCidades ++;
    /** Se a lista de cidades estiver vazia, insere a nova cidade como única na lista **/
    if (m->firstC == NULL)
    {
        nova_cidade->nextC = NULL;
        nova_cidade->prevC = NULL;
        m->firstC = nova_cidade;
        m->lastC = nova_cidade;
        return;
    }
     
    /** Se aux for NULL, insere a nova cidade no final da lista **/
    if (aux == NULL)
    {
        m->lastC->nextC = nova_cidade;
        nova_cidade->prevC = m->lastC;
        nova_cidade->nextC = NULL;
        m->lastC = nova_cidade;
        return;
    }
    /** Se aux for a primeira cidade da lista, insere a nova cidade antes dela **/
    if (aux->prevC == NULL)
    {
        nova_cidade->nextC = m->firstC;
        nova_cidade->prevC = NULL;
        m->firstC->prevC = nova_cidade;
        m->firstC = nova_cidade;
        return;
    }
    
    /** Insere a nova cidade antes da cidade aux, atualizando os ponteiros **/
    nova_cidade->nextC = aux;
    nova_cidade->prevC = aux->prevC;
    aux->prevC->nextC = nova_cidade;
    aux->prevC = nova_cidade;  
}

void altera_estado(Mapa *m, char *codigo, int estado) {  

    /* Procura a cidade no mapa usando o código fornecido */
    Cidade *city = procura_cidade(m, codigo);

    /* Se a cidade não existe, exibe um erro e retorna */
    if (city == NULL)
    {
        ERROR_NO_CITY(codigo);
        return;
    }

    /* Atualiza o estado da cidade para o novo valor */
    city->estado = estado;
}

void devolve_info_cidade(Mapa *m, char *codigo, int estado) { 

    /* Procura a cidade no mapa usando o código fornecido */
    Cidade *info = procura_cidade(m, codigo);

    /* Se a cidade não existe, exibe um erro e retorna */
    if (info == NULL)
    {
        ERROR_NO_CITY(codigo);
        return;
    }

    /* Exibe informações reduzidas sobre a cidade */
    if (estado == 0)
    {
        MSG_CITY_INFO_HEADER(info->codigo, info->estado, info->numLigacoes, info->nome);
        return;
    }

    /* Inicializa o ponteiro para percorrer as ligações da cidade */
    Lig *pesquisa = info->first;

    /* Exibe informações completas sobre a cidade e suas ligações */
    if (estado == 1)
    {
        MSG_CITY_INFO_HEADER(info->codigo, info->estado, info->numLigacoes, info->nome);
        while (pesquisa != NULL)
        {
            MSG_CITY_INFO_ITEM(pesquisa->destino, pesquisa->indiceTemporal, pesquisa->indiceEconomico, pesquisa->indiceTuristico);
            pesquisa = percorre_ligacoes(pesquisa);
        }
        return;
    }

}

void print_cidades(Mapa *m)
{
    /** Caso BaseDados Vazia**/
    if (m->firstC == NULL)
    {
        ERROR_DB_EMPTY;
    }
    
    /** Inicializa um ponteiro para a primeira cidade do mapa **/
    Cidade *print_city = m->firstC;

    /** Enquanto houver cidades no mapa, percorre a lista de cidades **/
    while (print_city)
    {
        /** Imprime as informações da cidade atual, incluindo código, estado, número de ligações e nome **/
        MSG_CITY_INFO_HEADER(print_city->codigo, print_city->estado, print_city->numLigacoes, print_city->nome);

        /** Atualiza o ponteiro para a próxima cidade na lista usando a função percorre_cidades **/
        print_city = percorre_cidades(print_city);
    }
    
}

void total_cidades(Mapa *m)
{
    MSG_TOTAL_CITIES(m->numCidades);
}

void adiciona_ligacao_cidade(Mapa *m, char *codigo_origem, char *cod_destino){ 

 Cidade *addliga_origem = procura_cidade(m, codigo_origem); /*origem*/
 Cidade *addliga2_destino = procura_cidade(m, cod_destino); /*destino*/

    /*Verifica se origem e destino existe, se as duas n existir -> exibe o print erro para as duas*/
    if (addliga_origem == NULL && addliga2_destino == NULL)
    {
        ERROR_NO_CITY(codigo_origem);
        ERROR_NO_CITY(cod_destino);
        return;
    }
    /*caso contrario, se for apenas origem -> exibe print apenas para origem */
     else if(addliga_origem == NULL)
    {
        ERROR_NO_CITY(codigo_origem);
        return;  
    }
    /*caso contrario, se for apenas destino -> exibe print apenas para destino */
    else if (addliga2_destino == NULL)
    {
        ERROR_NO_CITY(cod_destino);
        return; 
    }

    /* Vai aparecer cidade duplicada, pois verificamos se os codigo da cidade origem = destino é igual*/
    if(addliga_origem == addliga2_destino){
        ERROR_CITY_REPEATED(codigo_origem);
        return;
    }

    /*Ponteiro para acessar as ligações*/
    Lig *liga = procura_ligacoes(addliga_origem, cod_destino);

        if (liga != NULL && strcmp(liga->destino, cod_destino) == 0){
                
                return;
            }

    /*Criar estrutura para as ligações, alocar memoria*/
    Lig *nova_liga = malloc(sizeof(Lig));
    memset(nova_liga->destino, '\0', CITY_ID + 1);
    strncpy(nova_liga->destino, cod_destino, CITY_ID);

    nova_liga->indiceTemporal = 1.00;
    nova_liga->indiceEconomico = 1.00;
    nova_liga->indiceTuristico = 1.00;
    addliga_origem->numLigacoes++;

    /*Não existe Ligação? inserir ligação como unica na lista*/
    if (addliga_origem->first == NULL)
    {
        nova_liga->nextL = NULL; /* o nextL é igual vazio */
        nova_liga->prevL = NULL; /* o prevL é igual vazio */
        addliga_origem->first = nova_liga; /*id cidade origem da primeira é igual = NOVA_LIGA*/
        addliga_origem->last = nova_liga; /*id cidade origem da ultima é igual = NOVA_LIGA*/
        return;
    }

    /* Estrategia: Inserir as ligações no final da lista */
    addliga_origem->last->nextL = nova_liga;    /*ir no ponteiro da lig final e definir que a proxima ligação(nextL) sera a nova_liga. */
    nova_liga->prevL = addliga_origem->last; /*Voltar no ponteiro e definir que o nextL agora é o last*/
    nova_liga->nextL = NULL;  /*Deixar o nextL como NULL para dar abertura a receber novas ligações*/
    addliga_origem->last = nova_liga; /* a ligação LAST sera a nova_liga*/
    return;
}

void free_ligacao(Mapa *m, char *codigo_origem, char *codigo_last) 
{
    /* Procura a cidade de origem no mapa */
    Cidade *first = procura_cidade(m, codigo_origem);

    /* Se a cidade de origem não for encontrada, exibe um erro e retorna */
    if (first == NULL) {
        ERROR_NO_CITY(codigo_origem);
        return;
    }

    /* Procura a ligação na cidade de origem */
    Lig *last = procura_ligacoes(first, codigo_last);

    /* Se a ligação não for encontrada, exibe um erro e retorna */
    if (last == NULL) {
        ERROR_NO_LINK(codigo_origem,codigo_last);
        return;
    }

    /* Se a lista de ligações estiver vazia, não faz nada */
    if (first->first == NULL) {
        return;
    }

    /* Diminui o contador de ligações da cidade de origem */
    first->numLigacoes--;

    /* Se last for a primeira ligação da lista, atualiza a primeira ligação */
    if (last->prevL == NULL) {
        first->first = last->nextL;

        /* Atualiza o ponteiro prevL da próxima ligação, se houver */
        if (last->nextL) {
            last->nextL->prevL = NULL;
        }

        /* Libera a memória da ligação last */
        free(last);
        return;
    }

    /* Se last for a última ligação da lista, atualiza a última ligação */
    if (last->nextL == NULL) {
        first->last = last->prevL;
        last->prevL->nextL = NULL;

        /* Libera a memória da ligação last */
        free(last);
        return;
    }

    /* Remove a ligação last da lista, atualizando os ponteiros */
    last->prevL->nextL = last->nextL;
    last->nextL->prevL = last->prevL;

    /* Libera a memória da ligação last */
    free(last);
}

void change_turismo(Mapa *m, char *codigo_origem, char *codigo_last, float indice_t) {
    change_indice(m, codigo_origem, codigo_last, indice_t, 'T');
}

void change_economico(Mapa *m, char *codigo_origem, char *codigo_last, float indice_e) {
    change_indice(m, codigo_origem, codigo_last, indice_e, 'E');
}

void change_temporal(Mapa *m,char *codigo_origem, char *codigo_last, float indice_h){
    change_indice(m, codigo_origem, codigo_last, indice_h, 'H');
}

void remover_cidade(Mapa *m, char *cidade)
{
    Cidade *c = procura_cidade(m, cidade);
    if (c == NULL)
    {
        ERROR_NO_CITY(cidade);
        return;
    }
    

    /* Remove todas as ligações para a cidade a ser removida */
    Cidade *aux = m->firstC;
    
    while (aux)
    {
        Lig *ligacao = procura_ligacoes(aux,cidade);

        /* Se a ligação aponta para a cidade a ser excluída */
        if (ligacao) {
            /* Se for a primeira ligação */
            if (ligacao == aux->first && ligacao == aux->last){
                aux->first = NULL;
                aux->last = NULL;

            }
            else{
                if (ligacao == aux->first) {
                aux->first = ligacao->nextL;
                ligacao->nextL->prevL = NULL;
                } 
                
                /* Se for a última ligação */
                if (ligacao == aux->last) {
                    aux->last = ligacao->prevL;
                    ligacao->prevL->nextL = NULL;
                }
                
                /* Caso geral - Ligação no meio da lista */
                if (ligacao->prevL && ligacao->nextL) {
                    ligacao->prevL->nextL = ligacao->nextL;
                    ligacao->nextL->prevL = ligacao->prevL;
                }
                free(ligacao);
            }
            aux->numLigacoes--;
        }
        aux = aux->nextC;
    }

    Lig *ligacao = c->first;
    while (ligacao)
    {
        Lig *remover_lig = ligacao;
        ligacao = ligacao->nextL;

        free(remover_lig);
    }
    c->first = NULL;
    c->last = NULL;

    


    /* Se cheguei aqui existe e vou apagar a cidade */
    m->numCidades--;

    /* Case seja a única cidade */ 
    if (c->nextC == NULL && c->prevC == NULL)
    {
        free(c->nome);
        free(c);
        m->firstC = NULL;
        m->lastC = NULL;
        return;
    }

    /* Caso seja a primeira */ 
    if (c == m->firstC)
    {
        c->nextC->prevC = NULL;
        m->firstC = c->nextC;
        free(c->nome);
        free(c);
        return;
    }

    /*  Caso seja a última */
    if (c == m->lastC)
    {
        c->prevC->nextC=NULL;
        m->lastC=c->prevC;
        free(c->nome);
        free(c);
        return;
    }
    
    /* se cheguei aqui */ 
    /* Caso Geral - Apagar no meio */ 
    c->prevC->nextC = c->nextC;
    c->nextC->prevC = c->prevC;
    free(c->nome);
    free(c);
    return;
}

void melhor_rota_entre_cidades(Mapa *m, char *cidadeOrigem, char *cidadeDestino, char *indice) {

    /* Verifica se as cidades de origem e destino existem e estão ativas */
    Cidade *origem = procura_cidade(m, cidadeOrigem);
    Cidade *destino = procura_cidade(m, cidadeDestino);
    if (!origem && !destino){
        ERROR_NO_CITY(cidadeOrigem);
        ERROR_NO_CITY(cidadeDestino);
        return;
    }
    
    if (!origem) {
       ERROR_NO_CITY(cidadeOrigem);
        return;
    }
    if (!destino) {
        ERROR_NO_CITY(cidadeDestino);
        return;
    }
    if (origem->estado == 0) {
        ERROR_CITY_INACTIVE(cidadeOrigem);
        return;
    }
    if (destino->estado == 0) {
        ERROR_CITY_INACTIVE(cidadeDestino);
        return;
    }

    /* Inicialização */ 
    Path *todos = inicializa_todos(m, cidadeOrigem, indice);

    /* Enquanto ainda houver cidades não processadas*/
    while (existe_nao_processado(todos, m->numCidades)) {
        /* Encontre a cidade não processada com o menor valor total*/
        Path *p = getOpenShortest(m, todos);

        /* Marque a cidade como processada*/
        p->processed = SIM;

        /* Atualize os valores totais de todas as cidades vizinhas */
        Lig *l = p->cidade->first;
        
        /* Este loop passa por todas as ligações (vizinhos) da cidade atual. */
        while (l) {
            /* A função getPath() é usada para obter a estrutura Path correspondente à cidade destino da ligação atual. */
            Path *vizinho = getPath(m, todos, l->destino);

            /* Se a cidade vizinha existe e ainda não foi processada... */
            if (vizinho && vizinho->processed == NAO) {
                /* Calcula o novo valor total para o vizinho como a soma do valor total do caminho atual e o valor da ligação. */
                double novoValor = p->totalValue + getLinkValue(l, indice);

                /* Se o novo valor total for menor que o valor total atual do vizinho... */
                if (novoValor < vizinho->totalValue) {
                    /* Atualiza o valor total do vizinho. */
                    vizinho->totalValue = novoValor;

                    /* Concatena o código da cidade vizinha ao caminho total atual. */
                    char *newPath = concatPath(p->totalPath, vizinho->cidade->codigo);
                    if (newPath != NULL) {
                        /* Libera a memória do caminho total antigo do vizinho e atualiza com o novo caminho. */
                        free(vizinho->totalPath);
                        vizinho->totalPath = newPath;
                    }
                }
            }

            /* Passa para a próxima ligação. */
            l = l->nextL;
        }

    }

    /* Imprima a melhor rota para a cidade de destino */
    imprime_melhor_rota(todos, m->numCidades, origem->codigo, destino->codigo, indice);
    int i;
    for (i = 0; i < m->numCidades; i++) {
        free(todos[i].totalPath);
    }
    free(todos);

}

void guardar_file(Mapa *m, char *fileName) {
    /* Verificar se o mapa ou o nome do arquivo são nulos */
    if (m == NULL || fileName == NULL) {
        ERROR_FILE_EXTENSION(fileName);
        return;
    }

    /* Verificar se a extensão do arquivo é .sgo */
    char *ext = strrchr(fileName, '.');
    if (!ext || strcmp(ext, ".sgo") != 0) {
        ERROR_FILE_EXTENSION(fileName);
        return;
    }

    /* Abrir o arquivo para escrita */
    FILE *file = fopen(fileName, "w");
    if (!file) {
        ERROR_FILE_EXTENSION(fileName);
        return;
    }

    /* Iterar por cada cidade no mapa */
    Cidade *cidadeAtual = m->firstC;
    while (cidadeAtual != NULL) {
        fprintf(file, ADD_CITY, cidadeAtual->codigo, cidadeAtual->nome);
        cidadeAtual = cidadeAtual->nextC;
    }

  
    cidadeAtual = m->firstC;
    while (cidadeAtual != NULL) {
        Lig *ligacaoAtual = cidadeAtual->first;
        while (ligacaoAtual != NULL) {
            fprintf(file, ADD_LINK, cidadeAtual->codigo, ligacaoAtual->destino);
            ligacaoAtual = ligacaoAtual->nextL;
        }
        cidadeAtual = cidadeAtual->nextC;
    }

    cidadeAtual = m->firstC;
    while (cidadeAtual != NULL) {
        Lig *ligacaoAtual = cidadeAtual->first;
        while (ligacaoAtual != NULL) {
            if (ligacaoAtual->indiceTuristico != 1.00) { 
                fprintf(file, CHANGE_TURISTIC_INDEX, cidadeAtual->codigo, ligacaoAtual->destino, ligacaoAtual->indiceTuristico);
            }
            ligacaoAtual = ligacaoAtual->nextL;
        }
        cidadeAtual = cidadeAtual->nextC;
    }

    cidadeAtual = m->firstC;
    while (cidadeAtual != NULL) {
        Lig *ligacaoAtual = cidadeAtual->first;
        while (ligacaoAtual != NULL) {
            if (ligacaoAtual->indiceEconomico != 1.00) { 
                fprintf(file, CHANGE_ECONOMIC_INDEX, cidadeAtual->codigo, ligacaoAtual->destino, ligacaoAtual->indiceEconomico);
            }
            ligacaoAtual = ligacaoAtual->nextL;
        }
        cidadeAtual = cidadeAtual->nextC;
    }

    cidadeAtual = m->firstC;
    while (cidadeAtual != NULL) {
        Lig *ligacaoAtual = cidadeAtual->first;
        while (ligacaoAtual != NULL) {
            if (ligacaoAtual->indiceTemporal != 1.00) { 
                fprintf(file, CHANGE_TIME_INDEX, cidadeAtual->codigo, ligacaoAtual->destino, ligacaoAtual->indiceTemporal);
            }
            ligacaoAtual = ligacaoAtual->nextL;
        }
        cidadeAtual = cidadeAtual->nextC;
    }
    
    fclose(file);

    MSG_FILE_SAVED(fileName);
}

void free_mapa(Mapa *m) {
    Cidade *cidade = m->firstC; /* Ponteiro para a primeira cidade no mapa */

    /* Percorre todas as cidades no mapa */
    while (cidade != NULL) {
        Lig *ligacao = cidade->first; /* Ponteiro para a primeira ligação da cidade */

        /* Percorre todas as ligações da cidade */
        while (ligacao != NULL) {
            Lig *proximaLigacao = ligacao->nextL; /* Ponteiro para a próxima ligação */
            free(ligacao); /* Libera a memória alocada para a ligação atual */
            ligacao = proximaLigacao; /* Avança para a próxima ligação */
        }

        Cidade *proximaCidade = cidade->nextC; /* Ponteiro para a próxima cidade */
        free(cidade->nome); /* Libera a memória alocada para o nome da cidade */
        free(cidade); /* Libera a memória alocada para a cidade atual */
        cidade = proximaCidade; /* Avança para a próxima cidade */
    }

    free(m); /* Libera a memória alocada para a estrutura do mapa */
}
