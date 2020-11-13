#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int dia;
    int mes;
    int ano;
} dma;

typedef struct
{
    dma cadastro;
    dma obito;
    int classificacao;
    char municipio[25];
    dma idade;
    int comPulmao;
    int comCardio;
    int comRenal;
    int comDiabetes;
    int comTabagismo;
    int comObesidade;
    int ficouInternado;
} caso;

caso leCaso(FILE *arq);
dma leDma(FILE *arq);
int simOuNao(FILE *arq);
void imprimeCasos(caso vetor[]);

int main()
{
    caso *ptrCasos;
    ptrCasos = (caso*)malloc(202362*sizeof(caso));

    FILE *arquivo = fopen("covid19ES.csv", "r");

    char cabecalho[201];
    fgets(cabecalho, 201, arquivo);

    int i;
    for (i = 0; i < 202362; i++)
    {
        ptrCasos[i] = leCaso(arquivo);
    }
    fclose(arquivo);

    printf("%s", cabecalho);
    imprimeCasos(ptrCasos);

    free(ptrCasos);
    return 0;
}

caso leCaso(FILE *arq)
{
    // lê uma linha do csv
    caso temp;
    temp.cadastro = leDma(arq);
    temp.obito = leDma(arq);
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
    fscanf(arq, "\"%d anos, %d meses, %d dias\",", &temp.idade.ano, &temp.idade.mes, &temp.idade.dia);

    temp.comPulmao = simOuNao(arq);
    temp.comCardio = simOuNao(arq);
    temp.comRenal = simOuNao(arq);
    temp.comDiabetes = simOuNao(arq);
    temp.comTabagismo = simOuNao(arq);
    temp.comObesidade = simOuNao(arq);

    // lê "ficouInternado"
    // Sim -> 1 / Não -> 2 / Não informado -> 3
    c = fgetc(arq); i = 0;
    char d;
    while (d = fgetc(arq))
    {
        if (d == '\n' || d == EOF) break;
        i++;
    }
    if (c == 'S') temp.ficouInternado = 1;
    else if (i < 13) temp.ficouInternado = 2;
    else temp.ficouInternado = 3;

    return temp;
}

dma leDma(FILE *arq)
{
    // lê dia, mês e ano
    dma temp;
    fscanf(arq, "%d-%d-%d,", &temp.ano, &temp.mes, &temp.dia);
    return temp;
}

int simOuNao(FILE *arq)
{
    // "Sim" -> 1 / "Não" -> 0
    char c, d;
    c = fgetc(arq);
    while (d = fgetc(arq))
    {
        if (d == ',') break;
    }
    if (c == 'S') return 1;
    else return 0;
}

void imprimeCasos(caso vetor[])
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

        printf("\"%d anos, %d meses, %d dias\",", vetor[i].idade.ano, vetor[i].idade.mes, vetor[i].idade.dia);

        if (vetor[i].comPulmao == 1) printf("Sim,");
        else printf("Não,");

        if (vetor[i].comCardio == 1) printf("Sim,");
        else printf("Não,");

        if (vetor[i].comRenal == 1) printf("Sim,");
        else printf("Não,");

        if (vetor[i].comDiabetes == 1) printf("Sim,");
        else printf("Não,");

        if (vetor[i].comTabagismo == 1) printf("Sim,");
        else printf("Não,");

        if (vetor[i].comObesidade == 1) printf("Sim,");
        else printf("Não,");

        if (vetor[i].ficouInternado == 1) printf("Sim\n");
        else if (vetor[i].ficouInternado == 2) printf("Não\n");
        else printf("Não Informado\n");
    }
}
