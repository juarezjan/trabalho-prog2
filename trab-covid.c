#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int dia;
    int mes;
    int ano;
} data;

typedef struct
{
    data cadastro;
    data obito;
    int classificacao;
    char municipio[25];
    data idade;
    int com_pulmao;
    int com_cardio;
    int com_renal;
    int com_diabetes;
    int com_tabagismo;
    int com_obesidade;
    int ficou_internado;
} caso;

caso le_caso(FILE *arq);
data le_data(FILE *arq);
int sim_ou_nao(FILE *arq);
void imprime_casos(caso vetor[]);

int main()
{
    caso *ptrCasos;
    ptrCasos = malloc(202362 * sizeof (caso));

    FILE *arquivo = fopen("covid19ES.csv", "r");

    char cabecalho[201];
    fgets(cabecalho, 201, arquivo);

    int i;
    for (i = 0; i < 202362; i++)
    {
        ptrCasos[i] = le_caso(arquivo);
    }
    fclose(arquivo);

    printf("%s", cabecalho);
    imprime_casos(ptrCasos);

    free(ptrCasos);
    return 0;
}

caso le_caso(FILE *arq)
{
    // lê uma linha do csv
    caso temp;
    temp.cadastro = le_data(arq);
    temp.obito = le_data(arq);
    // converte a classificação em um int
    int i; char c;
    for (i = 0; c = fgetc(arq); i++)
    {
        if (c == ',') break;
        else if (i == 0)
        {
            if (c == 'C') temp.classificacao = 1;
            else if (c == 'D') temp.classificacao = 2;
            else temp.classificacao = 3;
        }
        else continue;
    }
    // lê o nome do município
    for (i = 0; i < 25; i++) temp.municipio[i] = '\0';
    for (i = 0; c = fgetc(arq); i++)
    {
        if (c == ',') break;
        else temp.municipio[i] = c;
    }
    // lê a idade
    fscanf(arq, "\"%d anos, %d", &temp.idade.ano, &temp.idade.mes);
    if (temp.idade.mes >= 0 || temp.idade.ano == -1)
    {
        fscanf(arq, " meses, %d dias\",", &temp.idade.dia);
    }
    else
    {
        while (c = fgetc(arq))
        {
            if (c == ',') break;
        }
    }

    temp.com_pulmao = sim_ou_nao(arq);
    temp.com_cardio = sim_ou_nao(arq);
    temp.com_renal = sim_ou_nao(arq);
    temp.com_diabetes = sim_ou_nao(arq);
    temp.com_tabagismo = sim_ou_nao(arq);
    temp.com_obesidade = sim_ou_nao(arq);

    // lê "ficou_internado"
    // Sim -> 1 / Não -> 2 / Não informado -> 3
    c = fgetc(arq); i = 0;
    char d;
    while (d = fgetc(arq))
    {
        if (d == '\n' || d == EOF) break;
        i++;
    }
    if (c == 'S') temp.ficou_internado = 1;
    else if (c == 'I') temp.ficou_internado = 4;
    else if (i < 13) temp.ficou_internado = 2;
    else temp.ficou_internado = 3;

    return temp;
}

data le_data(FILE *arq)
{
    // lê dia, mês e ano
    data temp;
    fscanf(arq, "%d-%d-%d,", &temp.ano, &temp.mes, &temp.dia);
    return temp;
}

int sim_ou_nao(FILE *arq)
{
    // "Sim" -> 1 / "Não" -> 0
    char c, d;
    c = fgetc(arq);
    while (d = fgetc(arq))
    {
        if (d == ',') break;
    }
    if (c == 'S') return 1;
    else if (c == '-') return 2;
    else return 0;
}

void imprime_casos(caso vetor[])
{
    int i;
    for (i = 0; i < 202362; i++)
    {
        printf("%d-", vetor[i].cadastro.ano);
        if (vetor[i].cadastro.mes < 10) printf("0%d-", vetor[i].cadastro.mes);
        else printf("%d-", vetor[i].cadastro.mes);
        if (vetor[i].cadastro.dia < 10) printf("0%d,", vetor[i].cadastro.dia);
        else printf("%d,", vetor[i].cadastro.dia);

        if (vetor[i].obito.ano == 0) printf("0000-00-00,");
        else
        {
            printf("%d-", vetor[i].obito.ano);
            if (vetor[i].obito.mes < 10) printf("0%d-", vetor[i].obito.mes);
            else printf("%d-", vetor[i].obito.mes);
            if (vetor[i].obito.dia < 10) printf("0%d,", vetor[i].obito.dia);
            else printf("%d,", vetor[i].obito.dia);
        }

        if (vetor[i].classificacao == 1) printf("Confirmados,");
        else if (vetor[i].classificacao == 2) printf("Descartados,");
        else printf("Suspeito,");

        printf("%s,", vetor[i].municipio);

        if (vetor[i].idade.mes >= 0) printf("\"%d anos, %d meses, %d dias\",", vetor[i].idade.ano, vetor[i].idade.mes, vetor[i].idade.dia);
        else if (vetor[i].idade.mes == -1) printf("\"%d anos, %d meses, %d dias\",", vetor[i].idade.ano, vetor[i].idade.mes, vetor[i].idade.dia);
        else printf("\"%d anos, %d dias\",", vetor[i].idade.ano, vetor[i].idade.mes);

        if (vetor[i].com_pulmao == 1) printf("Sim,");
        else if (vetor[i].com_pulmao == 2) printf("-,");
        else printf("Não,");

        if (vetor[i].com_cardio == 1) printf("Sim,");
        else if (vetor[i].com_cardio == 2) printf("-,");
        else printf("Não,");

        if (vetor[i].com_renal == 1) printf("Sim,");
        else if (vetor[i].com_renal == 2) printf("-,");
        else printf("Não,");

        if (vetor[i].com_diabetes == 1) printf("Sim,");
        else if (vetor[i].com_diabetes == 2) printf("-,");
        else printf("Não,");

        if (vetor[i].com_tabagismo == 1) printf("Sim,");
        else if (vetor[i].com_tabagismo == 2) printf("-,");
        else printf("Não,");

        if (vetor[i].com_obesidade == 1) printf("Sim,");
        else if (vetor[i].com_obesidade == 2) printf("-,");
        else printf("Não,");

        if (vetor[i].ficou_internado == 1) printf("Sim\n");
        else if (vetor[i].ficou_internado == 2) printf("Não\n");
        else if (vetor[i].ficou_internado == 4) printf("Ignorado\n");
        else printf("Não Informado\n");
    }
}
