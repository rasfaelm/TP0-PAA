#ifndef CAMERA_H
#define CAMERA_H

#include <windows.h>

HRESULT cameraInicializar(void);
void cameraCapturar(HWND hwnd);
void cameraLiberar(void);

#endif