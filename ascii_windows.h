#ifndef ASCII_H
#define ASCII_H

#include <windows.h>

void asciiInicializar(HWND hwnd, int qualidade);
void asciiAjustarJanela(HWND hwnd);
void asciiDesenhar(HDC hdc);
void asciiAtualizar(const BYTE *dados, HWND hwnd);
void asciiLiberar(void);
int asciiLargura(void);
int asciiAltura(void);
int asciiAlturaCaractere(void);

#endif