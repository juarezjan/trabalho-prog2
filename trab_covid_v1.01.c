#include <stdio.h>
#include <stdlib.h>

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
int sim_ou_nao(FILE *p_arq);
void imprime_casos(t_caso vetor[]);
void le_municipio(t_caso *temp, FILE *p_arq);
void le_idade(t_caso *temp, FILE *p_arq);
void class_int(t_caso *temp, FILE *p_arq);
void le_ficou_internado(t_caso *temp, FILE *p_arq);

int main()
{
    t_caso *p_casos;
    p_casos = malloc(202362 * sizeof(t_caso));

    FILE *p_arquivo = fopen("covid19ES.csv", "r");

    char cabecalho[201];
    fgets(cabecalho, 201, p_arquivo);

    int i;
    for (i = 0; i < 202362; i++)
    {
        p_casos[i] = le_caso(p_arquivo);
    }
    fclose(p_arquivo);

    printf("%s", cabecalho);
    imprime_casos(p_casos);

    free(p_casos);
    return 0;
}

t_caso le_caso(FILE *p_arq)
{
    // lê uma linha do csv
    t_caso temp;
    temp.cadastro = le_data(p_arq);
    temp.obito = le_data(p_arq);
    // converte a classificação em um int
    // "Confirmados" -> 1 / "Descartados" -> 2 / "Suspeito" -> 3
    class_int(&temp, p_arq);
    //

    le_municipio(&temp, p_arq);

    le_idade(&temp, p_arq);

    temp.com_pulmao = sim_ou_nao(p_arq);
    temp.com_cardio = sim_ou_nao(p_arq);
    temp.com_renal = sim_ou_nao(p_arq);
    temp.com_diabetes = sim_ou_nao(p_arq);
    temp.com_tabagismo = sim_ou_nao(p_arq);
    temp.com_obesidade = sim_ou_nao(p_arq);

    // lê "ficou_internado"
    // "Sim" -> 1 / "Não" -> 2 / "Não informado" -> 3 / "Ignorado" -> 4
    le_ficou_internado(&temp, p_arq);

    return temp;
}

t_data le_data(FILE *p_arq)
{
    // lê dia, mês e ano
    t_data temp;
    fscanf(p_arq, "%d-%d-%d,", &temp.ano, &temp.mes, &temp.dia);
    return temp;
}

int sim_ou_nao(FILE *p_arq)
{
    // "Sim" -> 1 / "Não" -> 0 / "-" -> 2
    char c, d;
    c = fgetc(p_arq);
    while (d = fgetc(p_arq))
    {
        if (d == ',')
            break;
    }
    if (c == 'S')
        return 1;
    else if (c == '-')
        return 2;
    else
        return 0;
}

void imprime_casos(t_caso vetor[])
{
    int i;
    for (i = 0; i < 202362; i++)
    {
        printf("%d-", vetor[i].cadastro.ano);
        if (vetor[i].cadastro.mes < 10)
            printf("0%d-", vetor[i].cadastro.mes);
        else
            printf("%d-", vetor[i].cadastro.mes);
        if (vetor[i].cadastro.dia < 10)
            printf("0%d,", vetor[i].cadastro.dia);
        else
            printf("%d,", vetor[i].cadastro.dia);

        if (vetor[i].obito.ano == 0)
            printf("0000-00-00,");
        else
        {
            printf("%d-", vetor[i].obito.ano);
            if (vetor[i].obito.mes < 10)
                printf("0%d-", vetor[i].obito.mes);
            else
                printf("%d-", vetor[i].obito.mes);
            if (vetor[i].obito.dia < 10)
                printf("0%d,", vetor[i].obito.dia);
            else
                printf("%d,", vetor[i].obito.dia);
        }

        if (vetor[i].classificacao == 1)
            printf("Confirmados,");
        else if (vetor[i].classificacao == 2)
            printf("Descartados,");
        else
            printf("Suspeito,");

        printf("%s,", vetor[i].municipio);

        if (vetor[i].idade.mes >= 0)
            printf("\"%d anos, %d meses, %d dias\",", vetor[i].idade.ano, vetor[i].idade.mes, vetor[i].idade.dia);
        else if (vetor[i].idade.mes == -1)
            printf("\"%d anos, %d meses, %d dias\",", vetor[i].idade.ano, vetor[i].idade.mes, vetor[i].idade.dia);
        else
            printf("\"%d anos, %d dias\",", vetor[i].idade.ano, vetor[i].idade.mes);

        if (vetor[i].com_pulmao == 1)
            printf("Sim,");
        else if (vetor[i].com_pulmao == 2)
            printf("-,");
        else
            printf("Não,");

        if (vetor[i].com_cardio == 1)
            printf("Sim,");
        else if (vetor[i].com_cardio == 2)
            printf("-,");
        else
            printf("Não,");

        if (vetor[i].com_renal == 1)
            printf("Sim,");
        else if (vetor[i].com_renal == 2)
            printf("-,");
        else
            printf("Não,");

        if (vetor[i].com_diabetes == 1)
            printf("Sim,");
        else if (vetor[i].com_diabetes == 2)
            printf("-,");
        else
            printf("Não,");

        if (vetor[i].com_tabagismo == 1)
            printf("Sim,");
        else if (vetor[i].com_tabagismo == 2)
            printf("-,");
        else
            printf("Não,");

        if (vetor[i].com_obesidade == 1)
            printf("Sim,");
        else if (vetor[i].com_obesidade == 2)
            printf("-,");
        else
            printf("Não,");

        if (vetor[i].ficou_internado == 1)
            printf("Sim\n");
        else if (vetor[i].ficou_internado == 2)
            printf("Não\n");
        else if (vetor[i].ficou_internado == 4)
            printf("Ignorado\n");
        else
            printf("Não Informado\n");
    }
}

void le_municipio(t_caso *temp, FILE *p_arq)
{
    int i;
    char c;
    for (i = 0; i < 25; i++)
        temp->municipio[i] = '\0';
    for (i = 0; c = fgetc(p_arq); i++)
    {
        if (c == ',')
            break;
        else
            temp->municipio[i] = c;
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
            if (d == ',')
                break;
        }
    }
}

// converte a classificação em um int
// "Confirmados" -> 1 / "Descartados" -> 2 / "Suspeito" -> 3
void class_int(t_caso *temp, FILE *p_arq)
{
    char c, d;
    int i;
    c = fgetc(p_arq);
    while (d = fgetc(p_arq))
    {
        if (d == ',')
            break;
    }
    if (c == 'C')
        temp->classificacao = 1;
    else if (c == 'D')
        temp->classificacao = 2;
    else
        temp->classificacao = 3;
}

// lê "ficou_internado"
// "Sim" -> 1 / "Não" -> 2 / "Não informado" -> 3 / "Ignorado" -> 4
void le_ficou_internado(t_caso *temp, FILE *p_arq)
{
    char c, d;
    int i = 0;
    c = fgetc(p_arq);
    while (d = fgetc(p_arq))
    {
        if (d == '\n' || d == EOF)
            break;
        i++;
    }
    if (c == 'S')
        temp->ficou_internado = 1;
    else if (c == 'I')
        temp->ficou_internado = 4;
    else if (i < 13)
        temp->ficou_internado = 2;
    else
        temp->ficou_internado = 3;
}
