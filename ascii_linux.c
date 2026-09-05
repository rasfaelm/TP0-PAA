#include "ascii_linux.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MAX_LARGURA_ASCII 640
#define MAX_ALTURA_ASCII 480
static const char caracteresASCII[] = " .,:;irsXA253hMHGS#9B&@";
static char bufferASCII[MAX_ALTURA_ASCII][MAX_LARGURA_ASCII + 1];
static int larguraASCII;
static int alturaASCII;

static char intensidadeParaAscii(unsigned char cinza) {
    int quantidade = (int)sizeof(caracteresASCII) - 1;
    int indice = ((255 - cinza) * (quantidade - 1)) / 255;
    return caracteresASCII[indice];
}

void asciiInicializarLinux(int qualidade) {
    struct winsize terminal;
    int colunas = 80;
    int linhas = 24;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal) == 0) {
        if (terminal.ws_col > 0) colunas = terminal.ws_col;
        if (terminal.ws_row > 0) linhas = terminal.ws_row;
    }

    larguraASCII = colunas * (50 + qualidade) / 150;
    alturaASCII = (linhas - 1) * (50 + qualidade) / 150;
    if (larguraASCII > MAX_LARGURA_ASCII) larguraASCII = MAX_LARGURA_ASCII;
    if (alturaASCII > MAX_ALTURA_ASCII) alturaASCII = MAX_ALTURA_ASCII;
    if (larguraASCII < 1) larguraASCII = 1;
    if (alturaASCII < 1) alturaASCII = 1;
}

void asciiAtualizarLinux(const unsigned char *dados, int larguraImagem,
    int alturaImagem, int bytesPorPixel) {
    float escalaX = (float)larguraImagem / larguraASCII;
    float escalaY = (float)alturaImagem / alturaASCII;

    for (int y = 0; y < alturaASCII; y++) {
        for (int x = 0; x < larguraASCII; x++) {
            int origemX = larguraImagem - 1 - (int)(x * escalaX);
            int origemY = (int)(y * escalaY);
            int indice = (origemY * larguraImagem + origemX) * bytesPorPixel;
            unsigned char cinza;
            if (bytesPorPixel == 2) {
                cinza = dados[indice];
            } else {
                unsigned char red = dados[indice];
                unsigned char green = dados[indice + 1];
                unsigned char blue = dados[indice + 2];
                cinza = (unsigned char)(0.299 * red + 0.587 * green + 0.114 * blue);
            }
            bufferASCII[y][x] = intensidadeParaAscii(cinza);
        }
        bufferASCII[y][larguraASCII] = '\0';
    }
}

void asciiDesenharLinux(void) {
    printf("\033[H\033[J");
    for (int y = 0; y < alturaASCII; y++) printf("%s\n", bufferASCII[y]);
    fflush(stdout);
}

void asciiLiberarLinux(void) {
    printf("\033[0m\n");
}
