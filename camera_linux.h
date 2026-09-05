#ifndef CAMERA_LINUX_H
#define CAMERA_LINUX_H

int cameraInicializarLinux(const char *dispositivo, int largura, int altura);
int cameraCapturarLinux(void);
const unsigned char *cameraDadosLinux(void);
int cameraLarguraLinux(void);
int cameraAlturaLinux(void);
int cameraBytesPorPixelLinux(void);
void cameraLiberarLinux(void);

#endif
