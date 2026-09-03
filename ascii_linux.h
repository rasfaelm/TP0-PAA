#ifndef ASCII_LINUX_H
#define ASCII_LINUX_H

void AsciiInicializarLinux(int qualidade);
void AsciiAtualizarLinux(const unsigned char *dados, int larguraImagem,
    int alturaImagem, int bytesPorPixel);
void AsciiDesenharLinux(void);
void AsciiLiberarLinux(void);

#endif
