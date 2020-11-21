// Juarez Jandre Azevedo - 2020100032
// Christian Junji Litzinger State - 2020100735

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define LIN 202362 // nº de linhas (com exclusão do cabeçalho)
#define TRUE 1
#define FALSE 0

typedef struct
{
    int dia;
    int mes;
    int ano;
} t_data;

typedef struct
{
    t_data cadastro;
    t_data obito;
    int classificacao;
    char municipio[25];
    t_data idade;
    int com_pulmao;
    int com_cardio;
    int com_renal;
    int com_diabetes;
    int com_tabagismo;
    int com_obesidade;
    int ficou_internado;
} t_caso;

t_caso le_caso(FILE *p_arq);
t_data le_data(FILE *p_arq);
t_data avanca_dia(t_data data);
int verifica_data(t_data data, t_data data_inicial, t_data data_final);
int sim_ou_nao(FILE *p_arq);
int bissexto(t_data data);
void le_municipio(t_caso *temp, FILE *p_arq);
void le_idade(t_caso *temp, FILE *p_arq);
void class_int(t_caso *temp, FILE *p_arq);
void le_ficou_internado(t_caso *temp, FILE *p_arq);
void ordem_alfabetica(t_caso vetor[], int n, char string[]);

int main()
{
    t_caso *p_casos;
    p_casos = malloc(LIN * sizeof(t_caso));

    FILE *p_arquivo = fopen("covid19ES.csv", "r");

    char cabecalho[201];
    fgets(cabecalho, 201, p_arquivo);

    int i;
    for (i = 0; i < LIN; i++)
    {
        p_casos[i] = le_caso(p_arquivo);
    }
    fclose(p_arquivo);

    char saida[50];
    fgets(saida, 50, stdin);
    size_t tam = strlen(saida);
    saida[tam - 1] = '\0';

    int item3;
    scanf("%d", &item3);
    ordem_alfabetica(p_casos, item3, saida);

    printf("string: %s\n", saida);

    free(p_casos);
    return 0;
}

// lê uma linha do csv
t_caso le_caso(FILE *p_arq)
{
    t_caso temp;
    temp.cadastro = le_data(p_arq);
    temp.obito = le_data(p_arq);
    class_int(&temp, p_arq);
    le_municipio(&temp, p_arq);
    le_idade(&temp, p_arq);
    temp.com_pulmao = sim_ou_nao(p_arq);
    temp.com_cardio = sim_ou_nao(p_arq);
    temp.com_renal = sim_ou_nao(p_arq);
    temp.com_diabetes = sim_ou_nao(p_arq);
    temp.com_tabagismo = sim_ou_nao(p_arq);
    temp.com_obesidade = sim_ou_nao(p_arq);
    le_ficou_internado(&temp, p_arq);
    return temp;
}

t_data le_data(FILE *p_arq)
{
    t_data temp;
    fscanf(p_arq, "%d-%d-%d,", &temp.ano, &temp.mes, &temp.dia);
    return temp;
}

// "sim" -> 1 / "não" -> 0
int sim_ou_nao(FILE *p_arq)
{
    char c, d;
    c = fgetc(p_arq);
    while (d = fgetc(p_arq))
    {
        if (d == ',') break;
    }
    if (c == 'S') return 1;
    else return 0;
}

void le_municipio(t_caso *temp, FILE *p_arq)
{
    int i;
    char c;
    for (i = 0; i < 25; i++) temp->municipio[i] = '\0';
    for (i = 0; c = fgetc(p_arq); i++)
    {
        if (c == ',') break;
        else temp->municipio[i] = c;
    }
}

void le_idade(t_caso *temp, FILE *p_arq)
{
    char d;
    fscanf(p_arq, "\"%d anos, %d", &temp->idade.ano, &temp->idade.mes);
    if (temp->idade.mes >= 0 || temp->idade.ano == -1)
    {
        fscanf(p_arq, " meses, %d dias\",", &temp->idade.dia);
    }
    else
    {
        while (d = fgetc(p_arq))
        {
            if (d == ',') break;
        }
    }
}

// converte a classificação em um int
// "confirmados" -> 1 / não confirmado -> 0
void class_int(t_caso *temp, FILE *p_arq)
{
    char c, d;
    int i;
    c = fgetc(p_arq);
    while (d = fgetc(p_arq))
    {
        if (d == ',') break;
    }
    if (c == 'C') temp->classificacao = 1;
    else temp->classificacao = 0;
}

// lê "ficou_internado"
// "sim" -> 1 / demais tags -> 0
void le_ficou_internado(t_caso *temp, FILE *p_arq)
{
    char c, d;
    int i = 0;
    c = fgetc(p_arq);
    while (d = fgetc(p_arq))
    {
        if (d == '\n' || d == EOF) break;
        i++;
    }
    if (c == 'S') temp->ficou_internado = 1;
    else temp->ficou_internado = 0;
}

// item 3
// exibe, em ordem alfabética, as cidades com mais de "n" casos
void ordem_alfabetica(t_caso vetor[], int n, char string[])
{
    typedef struct
    {
        char municipio[25];
        int total;
    } t_num_casos;

    t_num_casos lista[100];
    t_num_casos temp;
    int i, j;
    int esta_na_lista, num_municipios = 0;

    // assinala o total de casos para cada município
    for (i = 0; i < LIN; i++)
    {
        esta_na_lista = FALSE;
        for (j = 0; j < num_municipios && esta_na_lista == FALSE && vetor[i].classificacao; j++)
        {
            if (strcmp(vetor[i].municipio, lista[j].municipio) == 0)
            {
                esta_na_lista = TRUE;
                lista[j].total++;
            }
        }
        if (esta_na_lista == FALSE && vetor[i].classificacao)
        {
            strcpy(lista[num_municipios].municipio, vetor[i].municipio);
            lista[num_municipios].total = 1;
            num_municipios++;
        }
    }

    // organiza em ordem alfabética
    for (i = 0; i < num_municipios - 1; i++)
    {
        for (j = i + 1; j < num_municipios; j++)
        {
            if (strcmp(lista[i].municipio, lista[j].municipio) > 0)
            {
                temp = lista[i];
                lista[i] = lista[j];
                lista[j] = temp;
            }
        }
    }

    // cria a pasta e escreve o arquivo de saída
    char string_copia[50];
    strcpy(string_copia, string);
    mkdir(string_copia, 0777);
    strcat(string_copia, "item3.txt");
    FILE *p_saida = fopen(string_copia, "w");
    for (i = 0; i < num_municipios; i++)
    {
        if (lista[i].total > n)
        {
            fprintf(p_saida, "- %s: %d casos\n", lista[i].municipio, lista[i].total);
        }
    }
    fclose(p_saida);
}

// verifica se é bissexto
int bissexto(t_data data)
{
    if(data.ano % 4 == 0 && (data.ano % 100 != 0 || data.ano % 400 == 0))
    {
        return TRUE;
    }
    return FALSE;
}

// avança um dia
t_data avanca_dia(t_data data)
{
    data.dia++;
    // se o mes tem 31 dias
    if(data.mes == 1 || data.mes == 3 || data.mes == 5 || data.mes == 7 || data.mes == 8 || data.mes == 10 || data.mes == 12)
    {
        if(data.dia == 32)
        {
        data.dia = 1;
        data.mes++;
            if(data.mes == 13)
            {
                data.mes = 1;
                data.ano++;
            }
        }
    }
    else // se o mes tem 30, 29 ou 28 dias
    {
        if(data.mes == 2)
        {
            if(bissexto(data) == TRUE)
            {
                if(data.dia == 30)
                {
                    data.dia = 1;
                    data.mes++;
                }
            }
            else
            {
                if(data.dia == 29)
                {
                    data.dia = 1;
                    data.mes++;
                }
            }
        }
        else // meses com 30 dias
        {
            if(data.dia == 31)
            {
                data.dia = 1;
                data.mes++;
            }
        }
    }
    return data;
}

// verifica se uma data está entre duas datas (intervalo fechado)
// atenção para a ordem das datas
int verifica_data(t_data data, t_data data_inicial, t_data data_final)
{
    while(data_inicial.dia != data_final.dia || data_inicial.mes != data_final.mes || data_inicial.ano != data_final.ano)
    {
        if(data.dia == data_inicial.dia && data.mes == data_inicial.mes && data.ano == data_inicial.ano)
        {
            return TRUE;
        }
        data_inicial = avanca_dia(data_inicial);
    }
    return FALSE;
}
