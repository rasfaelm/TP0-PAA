#define _DEFAULT_SOURCE
#include "ascii_linux.h"
#include "camera_linux.h"
#include "webcam_linux.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int LerQualidade(void) {
    int valor;
    printf("Qualidade desejada (0-100): ");
    if (scanf("%d", &valor) != 1 || valor < 0 || valor > 100) {
        fprintf(stderr, "Qualidade invalida. Use um valor entre 0 e 100.\n");
        return -1;
    }
    return valor;
}

int ExecutarWebcamLinux(int argc, char **argv) {
    const char *dispositivo = argc > 1 ? argv[1] : "/dev/video0";
    int qualidade = LerQualidade();
    if (qualidade < 0) return EXIT_FAILURE;

    if (!CameraInicializarLinux(dispositivo, 640, 480)) return EXIT_FAILURE;
    AsciiInicializarLinux(qualidade);

    for (;;) {
        if (CameraCapturarLinux()) {
            AsciiAtualizarLinux(CameraDadosLinux(), CameraLarguraLinux(),
                CameraAlturaLinux(), CameraBytesPorPixelLinux());
            AsciiDesenharLinux();
        }
        usleep(33000);
    }

    AsciiLiberarLinux();
    CameraLiberarLinux();
    return EXIT_SUCCESS;
}
