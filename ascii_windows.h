#ifndef ASCII_H
#define ASCII_H

#include <windows.h>

void AsciiInicializar(HWND hwnd, int qualidade);
void AsciiAjustarJanela(HWND hwnd);
void AsciiDesenhar(HDC hdc);
void AsciiAtualizar(const BYTE *dados, HWND hwnd);
void AsciiLiberar(void);
int AsciiLargura(void);
int AsciiAltura(void);
int AsciiAlturaCaractere(void);

#endif