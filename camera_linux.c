#include "camera_linux.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include <jpeglib.h>
#include <setjmp.h>

struct Buffer {
    void *dados;
    size_t tamanho;
};

static int camera;
static struct Buffer *buffers;
static unsigned int quantidadeBuffers;
static const unsigned char *ultimoFrame;
static size_t tamanhoFrame;
static int larguraCamera;
static int alturaCamera;
static int bytesPorPixelCamera = 3;
static unsigned int formatoCamera;
static unsigned char *frameDecodificado;

struct ErroJpeg {
    struct jpeg_error_mgr base;
    jmp_buf contexto;
};

static void erroJpeg(j_common_ptr comum) {
    struct ErroJpeg *erro = (struct ErroJpeg *)comum->err;
    longjmp(erro->contexto, 1);
}

static int ioctlCamera(int comando, void *arg) {
    int resultado;
    do resultado = ioctl(camera, comando, arg); while (resultado == -1 && errno == EINTR);
    return resultado;
}

int cameraInicializarLinux(const char *dispositivo, int largura, int altura) {
    struct v4l2_capability capacidade;
    struct v4l2_format formato;
    struct v4l2_requestbuffers requisicao;

    camera = -1;
    camera = open(dispositivo, O_RDWR | O_NONBLOCK);
    if (camera == -1) {
        perror("Nao foi possivel abrir a webcam");
        return 0;
    }
    if (ioctlCamera(VIDIOC_QUERYCAP, &capacidade) == -1 ||
        !(capacidade.capabilities & V4L2_CAP_VIDEO_CAPTURE) ||
        !(capacidade.capabilities & V4L2_CAP_STREAMING)) {
        fprintf(stderr, "Dispositivo sem suporte a captura V4L2 por streaming.\n");
        cameraLiberarLinux();
        return 0;
    }

    memset(&formato, 0, sizeof(formato));
    formato.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    formato.fmt.pix.width = largura;
    formato.fmt.pix.height = altura;
    formato.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
    formato.fmt.pix.field = V4L2_FIELD_ANY;
    if (ioctlCamera(VIDIOC_S_FMT, &formato) == -1) {
        perror("VIDIOC_S_FMT");
        cameraLiberarLinux();
        return 0;
    }
    if (formato.fmt.pix.pixelformat != V4L2_PIX_FMT_RGB24 &&
        formato.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV &&
        formato.fmt.pix.pixelformat != V4L2_PIX_FMT_MJPEG) {
        fprintf(stderr, "Formato retornado pela webcam nao suportado: %.4s\n",
            (char *)&formato.fmt.pix.pixelformat);
        cameraLiberarLinux();
        return 0;
    }
    larguraCamera = (int)formato.fmt.pix.width;
    alturaCamera = (int)formato.fmt.pix.height;
    bytesPorPixelCamera = formato.fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV ? 2 : 3;
    formatoCamera = formato.fmt.pix.pixelformat;

    memset(&requisicao, 0, sizeof(requisicao));
    requisicao.count = 4;
    requisicao.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    requisicao.memory = V4L2_MEMORY_MMAP;
    if (ioctlCamera(VIDIOC_REQBUFS, &requisicao) == -1 || requisicao.count == 0) {
        perror("VIDIOC_REQBUFS");
        cameraLiberarLinux();
        return 0;
    }

    quantidadeBuffers = requisicao.count;
    buffers = calloc(quantidadeBuffers, sizeof(*buffers));
    for (unsigned int i = 0; i < quantidadeBuffers; i++) {
        struct v4l2_buffer buffer;
        memset(&buffer, 0, sizeof(buffer));
        buffer.type = requisicao.type;
        buffer.memory = requisicao.memory;
        buffer.index = i;
        if (ioctlCamera(VIDIOC_QUERYBUF, &buffer) == -1) {
            cameraLiberarLinux();
            return 0;
        }
        buffers[i].tamanho = buffer.length;
        buffers[i].dados = mmap(NULL, buffer.length, PROT_READ | PROT_WRITE,
            MAP_SHARED, camera, buffer.m.offset);
        if (buffers[i].dados == MAP_FAILED) {
            buffers[i].dados = NULL;
            cameraLiberarLinux();
            return 0;
        }
        if (ioctlCamera(VIDIOC_QBUF, &buffer) == -1) {
            cameraLiberarLinux();
            return 0;
        }
    }

    enum v4l2_buf_type tipo = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctlCamera(VIDIOC_STREAMON, &tipo) == -1) {
        perror("VIDIOC_STREAMON");
        cameraLiberarLinux();
        return 0;
    }
    printf("Webcam Linux inicializada em %s.\n", dispositivo);
    return 1;
}

int cameraCapturarLinux(void) {
    struct v4l2_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buffer.memory = V4L2_MEMORY_MMAP;
    if (ioctlCamera(VIDIOC_DQBUF, &buffer) == -1) return 0;
    ultimoFrame = buffers[buffer.index].dados;
    tamanhoFrame = buffer.bytesused;
    if (formatoCamera == V4L2_PIX_FMT_MJPEG) {
        struct jpeg_decompress_struct decodificador;
        struct ErroJpeg erro;
        JSAMPROW linha;

        decodificador.err = jpeg_std_error(&erro.base);
        erro.base.error_exit = erroJpeg;
        if (setjmp(erro.contexto) != 0) {
            jpeg_destroy_decompress(&decodificador);
            ioctlCamera(VIDIOC_QBUF, &buffer);
            return 0;
        }
        jpeg_create_decompress(&decodificador);
        jpeg_mem_src(&decodificador, (unsigned char *)ultimoFrame, tamanhoFrame);
        if (jpeg_read_header(&decodificador, TRUE) != JPEG_HEADER_OK) {
            jpeg_destroy_decompress(&decodificador);
            ioctlCamera(VIDIOC_QBUF, &buffer);
            return 0;
        }
        decodificador.out_color_space = JCS_RGB;
        jpeg_start_decompress(&decodificador);
        size_t tamanhoDecodificado = (size_t)decodificador.output_width *
            decodificador.output_height * decodificador.output_components;
        unsigned char *novoFrame = realloc(frameDecodificado, tamanhoDecodificado);
        if (novoFrame == NULL) {
            jpeg_destroy_decompress(&decodificador);
            ioctlCamera(VIDIOC_QBUF, &buffer);
            return 0;
        }
        frameDecodificado = novoFrame;
        while (decodificador.output_scanline < decodificador.output_height) {
            linha = frameDecodificado + decodificador.output_scanline *
                decodificador.output_width * decodificador.output_components;
            jpeg_read_scanlines(&decodificador, &linha, 1);
        }
        larguraCamera = (int)decodificador.output_width;
        alturaCamera = (int)decodificador.output_height;
        jpeg_finish_decompress(&decodificador);
        jpeg_destroy_decompress(&decodificador);
        ultimoFrame = frameDecodificado;
    }
    if (ioctlCamera(VIDIOC_QBUF, &buffer) == -1) return 0;
    return 1;
}

const unsigned char *cameraDadosLinux(void) {
    (void)tamanhoFrame;
    return ultimoFrame;
}

int cameraLarguraLinux(void) { return larguraCamera; }
int cameraAlturaLinux(void) { return alturaCamera; }
int cameraBytesPorPixelLinux(void) { return bytesPorPixelCamera; }

void cameraLiberarLinux(void) {
    if (camera >= 0) {
        enum v4l2_buf_type tipo = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ioctlCamera(VIDIOC_STREAMOFF, &tipo);
    }
    for (unsigned int i = 0; i < quantidadeBuffers; i++)
        if (buffers[i].dados != NULL) munmap(buffers[i].dados, buffers[i].tamanho);
    free(buffers);
    free(frameDecodificado);
    frameDecodificado = NULL;
    buffers = NULL;
    quantidadeBuffers = 0;
    if (camera >= 0) close(camera);
    camera = -1;
}
