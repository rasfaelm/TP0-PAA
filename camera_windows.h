#ifndef CAMERA_H
#define CAMERA_H

#include <windows.h>

HRESULT CameraInicializar(void);
void CameraCapturar(HWND hwnd);
void CameraLiberar(void);

#endif