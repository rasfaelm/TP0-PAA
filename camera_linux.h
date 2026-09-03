#ifndef CAMERA_LINUX_H
#define CAMERA_LINUX_H

int CameraInicializarLinux(const char *dispositivo, int largura, int altura);
int CameraCapturarLinux(void);
const unsigned char *CameraDadosLinux(void);
int CameraLarguraLinux(void);
int CameraAlturaLinux(void);
int CameraBytesPorPixelLinux(void);
void CameraLiberarLinux(void);

#endif
