#ifndef CAMERA_H
#define CAMERA_H

#include <windows.h>

HRESULT cameraInicializar(int indiceDispositivo);
void cameraCapturar(HWND hwnd);
void cameraLiberar(void);

#endif