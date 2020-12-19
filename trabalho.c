/*  Juarez Jandre Azevedo - 2020100032 */
/*  Christian Junji Litzinger State - 2020100735 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

#define LIN 202362 // nº de linhas (com exclusão do cabeçalho)
#define TRUE 1
#define FALSE 0

/*  Struct destinado a armazenar datas na forma de inteiros. */
typedef struct
{
    int dia;
    int mes;
    int ano;
} t_data;

/*  Struct destinado a armazenar uma linha do arquivo CSV. */
/*  Apenas o nome dos municípios é recebido como string. Os demais dados são recebidos como int. */
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
t_data le_data_stdin();
t_data avanca_dia(t_data data);
int verifica_data(t_data data, t_data data_inicial, t_data data_final);
int sim_ou_nao(FILE *p_arq);
int bissexto(t_data data);
float media_mortos(t_caso vetor[], t_data data_inicial, t_data data_final);
float prct_mortos_sem_comorbidade(t_caso vetor[], t_data data_inicial, t_data data_final);
float prct_confirmados_internados(t_caso vetor[], char municipio[]);
float prct_mortos(t_caso vetor[], char municipio[]);
float prct_internados_mortos(t_caso vetor[], char municipio[]);
double desvio_padrao_mortos(t_caso vetor[], float media_idades, t_data data_inicial, t_data data_final);
void le_municipio(t_caso *temp, FILE *p_arq);
void le_idade(t_caso *temp, FILE *p_arq);
void class_int(t_caso *temp, FILE *p_arq);
void le_ficou_internado(t_caso *temp, FILE *p_arq);
void ordem_alfabetica(t_caso vetor[], int n, char string[]);
void top_n(t_caso vetor[], int n, t_data data_inicial, t_data data_final, char string[]);
void saida_item7(float prct_mortos_sem_comorbidade, float media_mortos, double desvio_padrao_mortos, char string[]);
void transforma_maiusculo(char string[]);
void saida_item6(float internados_mortos, float mortos, float confirmados_internados, char string[], char municipio[]);
void saida_item4(t_caso vetor[], t_data data_inicial, t_data data_final, char string[]);

int main()
{
    FILE *p_arquivo = fopen("covid19ES.csv", "r");

    char cabecalho[201];
    fgets(cabecalho, 201, p_arquivo);

    t_caso *p_casos = calloc(LIN, sizeof(t_caso));
    int i;
    //Lê todas as linhas do arquivo CSV
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

    t_data item4_inicial, item4_final;
    item4_inicial = le_data_stdin();
    item4_final = le_data_stdin();
    saida_item4(p_casos, item4_inicial, item4_final, saida);

    int item5_numero;
    t_data item5_inicial, item5_final;
    scanf("%d", &item5_numero);
    item5_inicial = le_data_stdin();
    item5_final = le_data_stdin();
    top_n(p_casos, item5_numero, item5_inicial, item5_final, saida);

    getchar();
    char item6[25];
    fgets(item6, 25, stdin);
    tam = strlen(item6);
    item6[tam - 1] = '\0';
    transforma_maiusculo(item6);
    float confirmados_internados = prct_confirmados_internados(p_casos, item6);
    float mortos = prct_mortos(p_casos, item6);
    float internados_mortos = prct_internados_mortos(p_casos, item6);
    saida_item6(internados_mortos, mortos, confirmados_internados, saida, item6);

    float media_m, prct_m_sem_comorbidade;
    double desvio_padrao_m;
    t_data item7_inicial, item7_final;
    item7_inicial = le_data_stdin();
    item7_final = le_data_stdin();
    media_m = media_mortos(p_casos, item7_inicial, item7_final);
    desvio_padrao_m = desvio_padrao_mortos(p_casos, media_m, item7_inicial, item7_final);
    prct_m_sem_comorbidade = prct_mortos_sem_comorbidade(p_casos, item7_inicial, item7_final);
    saida_item7(prct_m_sem_comorbidade, media_m, desvio_padrao_m, saida);

    free(p_casos);
    return 0;
}

/* Item 1 */

/*  Lê uma linha do arquivo CSV, armazena as informações num struct do tipo t_caso e retorna esse struct. */
/*  Recebe como parâmetro um ponteiro do tipo FILE, que remete ao arquivo CSV.*/
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

/*  Lê uma data do arquivo CSV, armazena os dados em um struct do tipo t_data e retorna esse struct. */
/*  Recebe como parâmetro um ponteiro do tipo FILE, que remete ao arquivo CSV. */
t_data le_data(FILE *p_arq)
{
    t_data temp;
    fscanf(p_arq, "%d-%d-%d,", &temp.ano, &temp.mes, &temp.dia);
    return temp;
}

/*  Lê uma data da entrada padrão e retorna um struct do tipo t_data contendo a data lida. */
t_data le_data_stdin()
{
    t_data temp;
    scanf("%d-%d-%d", &temp.ano, &temp.mes, &temp.dia);
    return temp;
}

/*  Lê uma string até encontrar uma vírgula. Retorna TRUE caso a primeira letra seja 'S' e FALSE nos demais casos. */
/*  Recebe como parâmetro um ponteiro do tipo FILE, que remete ao arquivo CSV. */
int sim_ou_nao(FILE *p_arq)
{
    char c, d;
    c = fgetc(p_arq);
    while ((d = fgetc(p_arq)))
    {
        if (d == ',') break;
    }
    if (c == 'S') return TRUE;
    else return FALSE;
}

/*  Lê uma string até encontrar uma vírgula e armazena essa string num vetor. */
/*  Recebe como parâmetro: */
/*  1. um ponteiro do tipo t_caso, que localiza o vetor onde a string será armazenada; */
/*  2. um ponteiro do tipo FILE, que remete ao arquivo CSV. */
void le_municipio(t_caso *temp, FILE *p_arq)
{
    int i;
    char c;
    for (i = 0; i < 25; i++) temp->municipio[i] = '\0';
    for (i = 0; (c = fgetc(p_arq)); i++)
    {
        if (c == ',') break;
        else temp->municipio[i] = c;
    }
}

/*  Lê e armazena, em um struct do tipo t_data, a idade do paciente na data do cadastro. */
/*  A função armazena imediatamente dois inteiros (número de anos e de meses). */
/*  Depois disso, a primeira condição identifica se haverá um terceiro número a ser recebido (dias). */
/*  Recebe como parâmetro: */
/*  1. um ponteiro do tipo t_caso, que localiza o struct onde a idade será armazenada; */
/*  2. um ponteiro do tipo FILE, que remete ao arquivo CSV. */
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
        while ((d = fgetc(p_arq)))
        {
            if (d == ',') break;
        }
    }
}

/*  Lê uma string até encontrar uma vírgula. Caso a primeira letra seja 'C', armazena TRUE na variável 'classificacao'. Nos demais casos, armazena FALSE. */
/*  Recebe como parâmetro: */
/*  1. um ponteiro do tipo t_caso, que localiza o struct dentro do qual está a variável 'classificacao'. */
/*  2. um ponteiro do tipo FILE, que remete ao arquivo CSV. */
void class_int(t_caso *temp, FILE *p_arq)
{
    char c, d;
    c = fgetc(p_arq);
    while ((d = fgetc(p_arq)))
    {
        if (d == ',') break;
    }
    if (c == 'C') temp->classificacao = TRUE;
    else temp->classificacao = FALSE;
}

/*  Lê uma string até encontrar uma quebra de linha ou o fim do arquivo. */
/*  Caso a primeira letra seja 'S', armazena TRUE na variável 'ficou_internado'. Nos demais casos, armazena FALSE. */
/*  Recebe como parâmetro: */
/*  1. um ponteiro do tipo t_caso, que localiza o struct dentro do qual está a variável 'ficou_internado'. */
/*  2. um ponteiro do tipo FILE, que remete ao arquivo CSV. */
void le_ficou_internado(t_caso *temp, FILE *p_arq)
{
    char c, d;
    int i = 0;
    c = fgetc(p_arq);
    while ((d = fgetc(p_arq)))
    {
        if (d == '\n' || d == EOF) break;
        i++;
    }
    if (c == 'S') temp->ficou_internado = 1;
    else temp->ficou_internado = 0;
}

/*  Corresponde ao item 3 do trabalho. */

/*  Esta função recebe um vetor (que contém os dados do arquivo CSV), um inteiro (número de municípios a serem exibidos) e uma string (nome da pasta de saída). */
/*  Ela itera sobre os dados do vetor e monta um segundo vetor. Esse segundo vetor contém apenas o nome de cada município que aparece na lista e a quantidade correspondente de casos. */
/*  O vetor simplificado é colocado em ordem alfabética e, então, imprime-se o arquivo de saída. */
void ordem_alfabetica(t_caso vetor[], int n, char string[])
{
    typedef struct
    {
        char municipio[25];
        int total;
    } t_num_casos;

    t_num_casos lista[78];
    t_num_casos temp;
    int i, j;
    int esta_na_lista, num_municipios = 0;

    // Assinala o total de casos para cada município
    for (i = 0; i < LIN; i++)
    {
        esta_na_lista = FALSE;
        // Busca, dentro da lista, se o município do caso de covid já teve um caso contabilizado
        // Se verdadeiro, assinala que é verdadeiro e soma no total de casos do municipio
        for (j = 0; j < num_municipios && esta_na_lista == FALSE && vetor[i].classificacao; j++)
        {
            if (strcmp(vetor[i].municipio, lista[j].municipio) == 0)
            {
                esta_na_lista = TRUE;
                lista[j].total++;
            }
        }
        // Se o municipio do caso confirmado ainda não está na lista de municípios, coloca-o na lista
        // E inicializa o seu total de casos como 1
        // Se passou pelo loop e esta_na_lista == FALSE, então o municipio não foi contabilizado
        if (esta_na_lista == FALSE && vetor[i].classificacao)
        {
            strcpy(lista[num_municipios].municipio, vetor[i].municipio);
            lista[num_municipios].total = 1;
            num_municipios++;
        }
    }

    // Organiza em ordem alfabética
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

    // Cria a pasta e escreve o arquivo de saída
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

/* Item 2 */

/* t_data data representa a data a ser analisada. */
/* Verifica se o ano de uma data é bissexto e retorna TRUE se verdadeiro e FALSE senão. */
int bissexto(t_data data)
{
    if(data.ano % 4 == 0 && (data.ano % 100 != 0 || data.ano % 400 == 0))
    {
        return TRUE;
    }
    return FALSE;
}

/* t_data data representa a data em questão. */
/* Dada uma data, avanca um dia e retorna a data avançada. */
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

/* t_data data representa a data a ser analisada, data_inicial a data inicial e data_final a data final. */
/* Verifica se uma data está entre duas datas. Retorna TRUE se está e FALSE senão. */
int verifica_data(t_data data, t_data data_inicial, t_data data_final)
{
    // Resolve o caso da data ser igual à data final
    if(data.dia == data_final.dia && data.mes == data_final.mes && data.ano == data_final.ano) return TRUE;
    while(data_inicial.dia != data_final.dia || data_inicial.mes != data_final.mes || data_inicial.ano != data_final.ano)
    {
        // Avança a data inicial e compara com a data a ser analisada
        // Se for igual retorna TRUE
        if(data.dia == data_inicial.dia && data.mes == data_inicial.mes && data.ano == data_inicial.ano)
        {
            return TRUE;
        }
        data_inicial = avanca_dia(data_inicial);
    }
    return FALSE;
}

/*  Corresponde ao item 5 do trabalho. */
/*  Recebe um vetor (que contém os dados do arquivo CSV), um inteiro (número de municípios a serem exibidos), a data inicial e final do período e uma string (nome da pasta de saída). */
/*  Ela itera sobre os dados do vetor e monta um segundo vetor. Esse segundo vetor contém apenas o nome de cada município que aparece no período e a quantidade correspondente de casos. */
/*  O vetor simplificado é ordenado de acordo com o número de casos e, então, imprime-se o arquivo de saída. */
void top_n(t_caso vetor[], int n, t_data data_inicial, t_data data_final, char string[])
{
    typedef struct
    {
        char municipio[25];
        int total;
    } t_num_casos;

    t_num_casos lista[78];
    t_num_casos temp;
    int i, j;
    int esta_na_lista, num_municipios = 0;

    // Assinala o total de casos para cada município
    // Verifica se a data está no intervalo
    for (i = 0; i < LIN; i++)
    {
        esta_na_lista = FALSE;
        for (j = 0; j < num_municipios && esta_na_lista == FALSE && vetor[i].classificacao; j++)
        {
            // Sai do loop se o caso não está entre as datas solicitadas
            if (verifica_data(vetor[i].cadastro, data_inicial, data_final) == FALSE) break;
            // Verifica se o municipio do caso já foi inicializado, i.e. se já foi contabilizado algum caso no mesmo municipio
            // Assinala que sim caso verdadeiro e soma no total de casos do municipio
            if (strcmp(vetor[i].municipio, lista[j].municipio) == 0)
            {
                esta_na_lista = TRUE;
                lista[j].total++;
            }
        }
        // Inicializa os municípios
        // Se passou pelo loop anterior e continua com esta_na_lista == FALSE, então o municipio ainda não foi contabilizado
        if (esta_na_lista == FALSE && vetor[i].classificacao)
        {
            if (verifica_data(vetor[i].cadastro, data_inicial, data_final) == FALSE) continue;
            else {
                strcpy(lista[num_municipios].municipio, vetor[i].municipio);
                lista[num_municipios].total = 1;
                num_municipios++;
            }
        }
    }

    // Organiza por nº de casos
    for (i = 0; i < num_municipios - 1; i++)
    {
        for (j = i + 1; j < num_municipios; j++)
        {
            if (lista[i].total < lista[j].total)
            {
                temp = lista[i];
                lista[i] = lista[j];
                lista[j] = temp;
            }
        }
    }

    // Escreve o arquivo de saída
    char string_copia[50];
    strcpy(string_copia, string);
    mkdir(string_copia, 0777);
    strcat(string_copia, "item5.txt");
    FILE *p_saida = fopen(string_copia, "w");
    for (i = 0; i < n && i < num_municipios; i++)
    {
        fprintf(p_saida, "- %s: %d casos\n", lista[i].municipio, lista[i].total);
    }
    fclose(p_saida);
}

/*  O parâmetro t_caso vetor[] representa os casos, data_inicial a data inicial, e data_final a data final. */
/*  Calcula e retorna a média de idade dos mortos entre duas datas. */
float media_mortos(t_caso vetor[], t_data data_inicial, t_data data_final)
{
    float media = 0;
    int i;
    int num_pessoas = 0;
    for(i = 0; i < LIN; i++)
    {
        //  percorre os casos verificando se a data de cadastro está entre as duas datas e se o caso morreu com covid
        if(verifica_data(vetor[i].cadastro, data_inicial, data_final) && vetor[i].obito.ano != 0 && vetor[i].classificacao)
        {
            num_pessoas++;
            // soma as idades
            media += (float)vetor[i].idade.ano;
        }
    }
    // calcula a média
    if(num_pessoas == 0) return 0;
    media /= (float)num_pessoas;
    return media;
}

/* O parâmetro t_caso vetor[] representa os casos, media_idades a media das idades dos mortos entre as duas datas. */
/* data_inicial, a data inicial e data_final a data final. */
/* Calcula e retorna o desvio padrao da idade dos mortos entre duas datas. */
double desvio_padrao_mortos(t_caso vetor[], float media_idades, t_data data_inicial, t_data data_final)
{
    int i;
    int num_pessoas = 0;
    double soma = 0;
    double desvio_padrao;
    for(i = 0; i < LIN; i++)
    {
        // Percorre os casos procurando os casos que estão entre as duas datas, mortos e com covid
        if(verifica_data(vetor[i].cadastro, data_inicial, data_final) && vetor[i].obito.ano != 0 && vetor[i].classificacao)
        {
            num_pessoas++;
            // Soma o quadrado da diferença da idade pra media das idades
            soma +=  ((double)(vetor[i].idade.ano - media_idades) * (double)(vetor[i].idade.ano - media_idades));
        }
    }
    // Calcula o desvio padrao
    if(num_pessoas <= 1) return 0;
    desvio_padrao = sqrt(soma / (num_pessoas - 1));
    return desvio_padrao;
}

/* O parâmetro t_caso vetor[] representa os casos, data_inicial a data inicial, e data_final a data final. */
/* Calcula e retorna a porcentagem dos mortos com covid sem comorbidade dentre os mortos com covid entre duas datas. */
float prct_mortos_sem_comorbidade(t_caso vetor[], t_data data_inicial, t_data data_final)
{
    int i;
    int num_pessoas = 0;
    float mortos_sem_comorbidade = 0;
    for(i = 0; i < LIN; i++)
    {
        // Percorre os casos verificando se está entre as datas, se é morto e se teve covid
        if(verifica_data(vetor[i].cadastro, data_inicial, data_final) && vetor[i].obito.ano != 0 && vetor[i].classificacao)
        {
            num_pessoas++;
            // Verifica se é sem comorbidade
            if(vetor[i].com_cardio == 0 && vetor[i].com_diabetes == 0 && vetor[i].com_obesidade == 0 && vetor[i].com_pulmao == 0 && vetor[i].com_renal == 0 && vetor[i].com_tabagismo == 0)
            {
                mortos_sem_comorbidade++;
            }
        }
    }
    if(num_pessoas > 0) return 100 * (float)(mortos_sem_comorbidade / num_pessoas);
    else return 0;
}

/* O parâmetro prct_mortos_sem_comorbidade representa a porcentagem de mortos com covid sem comorbidade dentre os mortos com covid entre duas datas */
/* media_mortos representa a media de idade dos mortos entre duas datas e desvio_padrao_mortos o desvio padrao da idade dos mortos entre duas datas */
/* string[] representa o nome do diretório de saída */
/* Fornece a saida para o item 7 */
void saida_item7(float prct_mortos_sem_comorbidade, float media_mortos, double desvio_padrao_mortos, char string[])
{
    char string_copia[50];
    strcpy(string_copia, string);
    mkdir(string_copia, 0777);
    strcat(string_copia, "item7.txt");
    FILE *p_saida = fopen(string_copia, "w");
    fprintf(p_saida, "A media e desvio padrao da idade: %.3f -- %.3lf\n", media_mortos, desvio_padrao_mortos);
    fprintf(p_saida, "A %% de pessoas que morreram sem comorbidade: %.3f%%", prct_mortos_sem_comorbidade);
    fclose(p_saida);
}

/* Item 6 */

/* Transforma as letras minúsculas em maiúsculas. */
/* O parâmetro string[] representa a string a ser transformada. */
void transforma_maiusculo(char string[])
{
    int i;
    for(i = 0; i < strlen(string); i++)
    {
        if(string[i]>= 'a' && string[i] <= 'z')
        {
            string[i] -= 32;
        }
    }
}

/* O parâmetro t_caso vetor[] representa os casos e o parâmetro municipio[], o nome do municipio a ser analisado. */
/* Calcula e retorna a porcentagem de internados dentre os confirmados de um determinado município. */
float prct_confirmados_internados(t_caso vetor[], char municipio[])
{
    int i = 0;
    int confirmados = 0;
    int internados = 0;
    if(strcmp(municipio, "TODAS") == 0)
    {
        for(i = 0; i < LIN; i++)
        {
            // Percorre os casos procurando os confirmados
            if(vetor[i].classificacao)
            {
                confirmados++;
                // Verifica se o caso confirmado foi internado também
                if(vetor[i].ficou_internado)
                {
                    internados++;
                }
            }
        }
    }
    else
    {
        for(i = 0; i < LIN; i++)
        {
            // Percorre os casos procurando os confirmados de um determinado município
            if(strcmp(vetor[i].municipio, municipio) == 0 && vetor[i].classificacao)
            {
                confirmados++;
                // Verifica se o caso confirmado foi internado também
                if(vetor[i].ficou_internado)
                {
                    internados++;
                }
            }
        }
    }
    if(confirmados == 0) return 0;
    return 100 * ((float)internados / confirmados);
}

/* O parâmetro t_caso vetor[] representa os casos e o parâmetro municipio[] o nome do municipio a ser analisado. */
/* Calcula e retorna a porcentagem de mortos dentre os confirmados de um determinado municipio. */
float prct_mortos(t_caso vetor[], char municipio[])
{
    int i = 0;
    int mortos = 0;
    int confirmados = 0;
    if(strcmp(municipio, "TODAS") == 0)
    {
        for(i = 0; i < LIN; i++)
        {
            // Percorre os casos buscando os confirmados
            if(vetor[i].classificacao)
            {
                confirmados++;
                // Verifica se o caso confirmado foi morto
                if(vetor[i].obito.ano != 0)
                {
                    mortos++;
                }
            }
        }
    }
    else
    {
        for(i = 0; i < LIN; i++)
        {
            // Percorre os casos buscando os confirmados de um determinado municipio
            if(strcmp(vetor[i].municipio, municipio) == 0 && vetor[i].classificacao)
            {
                confirmados++;
                // Verifica se o caso confirmado foi morto
                if(vetor[i].obito.ano != 0)
                {
                    mortos++;
                }
            }
        }
    }

    return 100 *  ((float)mortos / confirmados);
}

/* O parâmetro t_caso vetor[] representa os casos e o parâmetro municipio, o municipio a ser analisado. */
/* Calcula e retorna a porcentagem de internados dentre os mortos com covid de um determinado municipio. */
float prct_internados_mortos(t_caso vetor[], char municipio[])
{
    int i = 0;
    int internados = 0;
    int total = 0;
    if(strcmp(municipio, "TODAS") == 0)
    {
        for(i = 0; i < LIN; i++)
        {
            // Percorre os casos buscando os confirmados mortos
            if(vetor[i].classificacao && vetor[i].obito.ano != 0)
            {
                total++;
                // Verifica se o caso também ficou internado
                if(vetor[i].ficou_internado)
                {
                    internados++;
                }
            }
        }
    }
    else
    {
        for(i = 0; i < LIN; i++)
        {
            // Percorre os casos buscando os confirmados mortos de um municipio
            if(strcmp(vetor[i].municipio, municipio) == 0 && vetor[i].classificacao && vetor[i].obito.ano != 0)
            {
                total++;
                // Verifica se o caso também ficou internado
                if(vetor[i].ficou_internado)
                {
                    internados++;
                }
            }
        }
    }

    return 100 * ((float)internados / total);
}

/* 1. internados_mortos representa a porcentagem de internados dentre os mortos com covid. */
/* 2. mortos representa a porcentagem de mortos dentre os confirmados. */
/* 3. confirmados_internados representa a porcentagem de internados dentre os confirmados. */
/* 4. string[] representa o nome do diretório de saída. */
/* 5. municipio[] representa o nome do município em questão. */
/* Fornece a saída do item 6. */
void saida_item6(float internados_mortos, float mortos, float confirmados_internados, char string[], char municipio[])
{
    char string_copia[50];
    strcpy(string_copia, string);
    mkdir(string_copia, 0777);
    strcat(string_copia, "item6.txt");
    FILE *p_saida = fopen(string_copia, "w");
    fprintf(p_saida, "- Resultados para %s:\n", municipio);
    fprintf(p_saida, "- A %% de pessoas com Covid-19 que ficaram internadas: %.3f%%\n", confirmados_internados);
    fprintf(p_saida, "- A %% de pessoas com Covid-19 que morreram: %.3f%%\n", mortos);
    fprintf(p_saida, "- A %% de pessoas que ficaram internadas e morreram: %.3f%%", internados_mortos);
    fclose(p_saida);
}

/* O parâmetro data_inicial representa a data inicial a ser considerada, e data_final a data final. */
/* O parâmetro t_caso vetor[] representa os casos e o string[] o nome do diretório de saída.  */
/* Calcula o item 4 e fornece a saída.  */
void saida_item4(t_caso vetor[], t_data data_inicial, t_data data_final, char string[])
{
    int qtd = 0, i;

    // Contabiliza a quantidade de casos entre duas datas
    for(i = 0; i < LIN; i++)
    {
        if(verifica_data(vetor[i].cadastro, data_inicial, data_final) && vetor[i].classificacao)
        {
            qtd++;
        }
    }

    // Fornece a saída
    char string_copia[50];
    strcpy(string_copia, string);
    mkdir(string_copia, 0777);
    strcat(string_copia, "item4.txt");
    FILE *p_saida = fopen(string_copia, "w");
    fprintf(p_saida, "- Total de pessoas: %d", qtd);
    fclose(p_saida);
}
