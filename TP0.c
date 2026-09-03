#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include "webcam_windows.h"
#else
#include "webcam_linux.h"
#endif

#define LINHAS 20
#define COLUNAS 80
#define MAX_FIGURAS 100

void inicializarQuadro(char quadro[LINHAS][COLUNAS]) {
    int i, j;

    for (i = 0; i < LINHAS; i++) {
        for (j = 0; j < COLUNAS; j++) {

            if (i == 0 || i == LINHAS - 1) {
                quadro[i][j] = '-';
            }
            else if (j == 0 || j == COLUNAS - 1) {
                quadro[i][j] = '|';
            }
            else {
                quadro[i][j] = ' ';
            }
        }
    }
}

void imprimirQuadro(char quadro[LINHAS][COLUNAS]) {
    int i, j;

    for (i = 0; i < LINHAS; i++) {
        for (j = 0; j < COLUNAS; j++) {
            printf("%c", quadro[i][j]);
        }

        printf("\n");
    }
}

int posicaoLivre(char quadro[LINHAS][COLUNAS], int linha, int coluna) {

    if (linha <= 0 || linha >= LINHAS - 1 ||
        coluna <= 0 || coluna >= COLUNAS - 1) {

        return 0;
    }

    if (quadro[linha][coluna] != ' ') {
        return 0;
    }

    return 1;
}

int podeA(char quadro[LINHAS][COLUNAS],int linha, int coluna) {
    return posicaoLivre(quadro, linha, coluna);
}

void colocarA(char quadro[LINHAS][COLUNAS], int linha, int coluna) {
    quadro[linha][coluna] = '*';
}

int podeSoma(char quadro[LINHAS][COLUNAS],int linha, int coluna) {

    if (!posicaoLivre(quadro, linha - 1, coluna))
        return 0;

    if (!posicaoLivre(quadro, linha, coluna - 1))
        return 0;

    if (!posicaoLivre(quadro, linha, coluna))
        return 0;

    if (!posicaoLivre(quadro, linha, coluna + 1))
        return 0;

    if (!posicaoLivre(quadro, linha + 1, coluna))
        return 0;

    return 1;
}

void colocarSoma(char quadro[LINHAS][COLUNAS],int linha, int coluna) {
    quadro[linha - 1][coluna] = '*';
    quadro[linha][coluna - 1] = '*';
    quadro[linha][coluna] = '*';
    quadro[linha][coluna + 1] = '*';
    quadro[linha + 1][coluna] = '*';
}

int podeX(char quadro[LINHAS][COLUNAS],int linha, int coluna) {

    if (!posicaoLivre(quadro, linha - 1, coluna - 1))
        return 0;

    if (!posicaoLivre(quadro, linha - 1, coluna + 1))
        return 0;

    if (!posicaoLivre(quadro, linha, coluna))
        return 0;

    if (!posicaoLivre(quadro, linha + 1, coluna - 1))
        return 0;

    if (!posicaoLivre(quadro, linha + 1, coluna + 1))
        return 0;

    return 1;
}


void colocarX(char quadro[LINHAS][COLUNAS],int linha, int coluna) {
    quadro[linha - 1][coluna - 1] = '*';
    quadro[linha - 1][coluna + 1] = '*';
    quadro[linha][coluna] = '*';
    quadro[linha + 1][coluna - 1] = '*';
    quadro[linha + 1][coluna + 1] = '*';
}

void gerarFiguraAleatoria(char quadro[LINHAS][COLUNAS]) {

    int tipo;
    tipo = rand() % 3 + 1;

    switch (tipo) {

        case 1:
        {
            int linha, coluna;

            do {
                linha = rand() % (LINHAS - 2) + 1;
                coluna = rand() % (COLUNAS - 2) + 1;
            } while (!podeA(quadro, linha, coluna));
            colocarA(quadro, linha, coluna);
            break;
        }

        case 2:
        {
            int linha, coluna;

            do {
                linha = rand() % (LINHAS - 4) + 2;
                coluna = rand() % (COLUNAS - 4) + 2;
            } while (!podeSoma(quadro, linha, coluna));
            colocarSoma(quadro, linha, coluna);
            break;
        }

        case 3:
        {
            int linha, coluna;

            do {
                linha = rand() % (LINHAS - 4) + 2;
                coluna = rand() % (COLUNAS - 4) + 2;
            } while (!podeX(quadro, linha, coluna));

            colocarX(quadro, linha, coluna);
            break;
        }
    }
}

void gerarObra(char quadro[LINHAS][COLUNAS],int tipo,int quantidade) {

    int i;

    for (i = 0; i < quantidade; i++) {
        if (tipo == 1) {
            int linha, coluna;
            do {
                linha = rand() % (LINHAS - 2) + 1;
                coluna = rand() % (COLUNAS - 2) + 1;
            } while (!podeA(quadro, linha, coluna));
            colocarA(quadro, linha, coluna);
        }
        else if (tipo == 2) {
            int linha, coluna;
            do {
                linha = rand() % (LINHAS - 4) + 2;
                coluna = rand() % (COLUNAS - 4) + 2;
            } while (!podeSoma(quadro, linha, coluna));
            colocarSoma(quadro, linha, coluna);
        }

        else if (tipo == 3) {
            int linha, coluna;
            do {
                linha = rand() % (LINHAS - 4) + 2;
                coluna = rand() % (COLUNAS - 4) + 2;
            } while (!podeX(quadro, linha, coluna));
            colocarX(quadro, linha, coluna);
        }

        else if (tipo == 4) {
            gerarFiguraAleatoria(quadro);
        }
    }
}

int main(int argc, char **argv) {

    char quadro[LINHAS][COLUNAS];
    int tipo;
    int quantidade;
    char novoQuadro;

    srand(time(NULL));
    printf("PROGRAMA GERADOR DE OBRA DE ARTE:\n");
    printf("=================================\n\n");
    printf("Escolha o tipo de figura basica a ser usada ");
    printf("para criar a obra:\n\n");
    printf("1 - asterisco simples.\n");
    printf("2 - simbolo de soma com asteriscos.\n");
    printf("3 - letra X com asteriscos.\n");
    printf("4 - figuras aleatorias.\n");
    printf("5 - webcam ASCII.\n\n");
    printf("Digite o tipo de figura basica desejada: ");
    scanf("%d", &tipo);

        if (tipo == 5) {
    #ifdef _WIN32
        return ExecutarWebcamWindows();
    #else
        return ExecutarWebcamLinux(argc, argv);
    #endif
        }

        if (tipo < 1 || tipo > 4) {
        printf("\nOpcao ainda nao implementada.\n");
        return 0;
    }

    printf("Digite a quantidade de figuras ");
    printf("(menor ou igual a zero para aleatorio): ");
    scanf("%d", &quantidade);

    if (quantidade <= 0) {
        quantidade = rand() % 100 + 1;
    }

    if (quantidade > MAX_FIGURAS) {
        quantidade = MAX_FIGURAS;
    }


    do {
        inicializarQuadro(quadro);
        gerarObra(quadro, tipo, quantidade);
        printf("\n");
        imprimirQuadro(quadro);
        printf("\nDeseja gerar um novo quadro com os mesmos valores? ");
        printf("(s/n): ");
        scanf(" %c", &novoQuadro);
    } while (novoQuadro == 's' || novoQuadro == 'S');

    return 0;
}