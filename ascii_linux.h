#ifndef ASCII_LINUX_H
#define ASCII_LINUX_H

void asciiInicializarLinux(int qualidade);
void asciiAtualizarLinux(const unsigned char *dados, int larguraImagem,int alturaImagem, int bytesPorPixel);
void asciiDesenharLinux(void);
void asciiLiberarLinux(void);

#endif
