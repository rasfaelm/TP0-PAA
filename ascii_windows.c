#include "ascii_windows.h"

#define LARGURA_ORIGINAL 640
#define ALTURA_ORIGINAL 480
#define MAX_LARGURA_ASCII 640
#define MAX_ALTURA_ASCII 480
#define FONTE_MINIMA 2
#define COLUNAS_MINIMAS 20
#define LINHAS_MINIMAS 15
#define MARGEM_HORIZONTAL 40
#define MARGEM_VERTICAL 80

static const char caracteresASCII[] = " .,:;irsXA253hMHGS#9B&@";
static char bufferASCII[MAX_ALTURA_ASCII][MAX_LARGURA_ASCII + 1];
static HFONT fonteASCII = NULL;
static int larguraCaractere = 0;
static int alturaCaractere = 0;
static int larguraASCII = 0;
static int alturaASCII = 0;

static char IntensidadeParaASCII(BYTE cinza) {
    int quantidade = sizeof(caracteresASCII) - 1;
    int indice = ((255 - cinza) * (quantidade - 1)) / 255;
    return caracteresASCII[indice];
}

static void InicializarFonte(HWND hwnd, int tamanhoFonte) {
    HDC hdc;
    TEXTMETRICA metricas;

    if (fonteASCII != NULL) DeleteObject(fonteASCII);
    fonteASCII = CreateFontA(tamanhoFonte, 0, 0, 0, FW_NORMAL, FALSE, FALSE,
        FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "Consolas");
    if (fonteASCII == NULL) {
        MessageBoxA(hwnd, "Erro ao criar a fonte!", "Erro", MB_ICONERROR);
        return;
    }

    hdc = GetDC(hwnd);
    HFONT fonteAntiga = SelectObject(hdc, fonteASCII);
    GetTextMetricsA(hdc, &metricas);
    larguraCaractere = metricas.tmAveCharWidth;
    alturaCaractere = metricas.tmHeight;
    SelectObject(hdc, fonteAntiga);
    ReleaseDC(hwnd, hdc);
}

void AsciiInicializar(HWND hwnd, int qualidade) {
    RECT areaTrabalho;
    SystemParametersInfoA(SPI_GETWORKAREA, 0, &areaTrabalho, 0);
    int larguraDisponivel = areaTrabalho.right - areaTrabalho.left - MARGEM_HORIZONTAL;
    int alturaDisponivel = areaTrabalho.bottom - areaTrabalho.top - MARGEM_VERTICAL;
    int fonteMaxima = larguraDisponivel / COLUNAS_MINIMAS;
    int fonteMaxVertical = alturaDisponivel / LINHAS_MINIMAS;
    if (fonteMaxVertical < fonteMaxima) fonteMaxima = fonteMaxVertical;
    if (fonteMaxima > 72) fonteMaxima = 72;
    if (fonteMaxima < FONTE_MINIMA) fonteMaxima = FONTE_MINIMA;

    int tamanhoFonte = fonteMaxima - (qualidade * (fonteMaxima - FONTE_MINIMA)) / 100;
    if (tamanhoFonte < FONTE_MINIMA) tamanhoFonte = FONTE_MINIMA;
    InicializarFonte(hwnd, tamanhoFonte);

    if (larguraCaractere <= 0) larguraCaractere = 1;
    if (alturaCaractere <= 0) alturaCaractere = 1;
    larguraASCII = larguraDisponivel / larguraCaractere;
    alturaASCII = alturaDisponivel / alturaCaractere;
    if (larguraASCII > MAX_LARGURA_ASCII) larguraASCII = MAX_LARGURA_ASCII;
    if (alturaASCII > MAX_ALTURA_ASCII) alturaASCII = MAX_ALTURA_ASCII;
    if (larguraASCII < 1) larguraASCII = 1;
    if (alturaASCII < 1) alturaASCII = 1;
}

void AsciiAjustarJanela(HWND hwnd) {
    RECT rect = { 0, 0, larguraASCII * larguraCaractere, alturaASCII * alturaCaractere };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    SetWindowPos(hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
        SWP_NOMOVE | SWP_NOZORDER);
}

void AsciiAtualizar(const BYTE *dados, HWND hwnd) {
    float escalaX = (float)LARGURA_ORIGINAL / (float)larguraASCII;
    float escalaY = (float)ALTURA_ORIGINAL / (float)alturaASCII;

    for (int y = 0; y < alturaASCII; y++) {
        for (int x = 0; x < larguraASCII; x++) {
            int origemX = LARGURA_ORIGINAL - 1 - (int)(x * escalaX);
            int origemY = (int)(y * escalaY);
            if (origemX < 0) origemX = 0;
            if (origemX >= LARGURA_ORIGINAL) origemX = LARGURA_ORIGINAL - 1;
            if (origemY >= ALTURA_ORIGINAL) origemY = ALTURA_ORIGINAL - 1;
            int indice = (origemY * LARGURA_ORIGINAL + origemX) * 4;
            BYTE blue = dados[indice];
            BYTE green = dados[indice + 1];
            BYTE red = dados[indice + 2];
            BYTE cinza = (BYTE)(0.299 * red + 0.587 * green + 0.114 * blue);
            bufferASCII[y][x] = IntensidadeParaASCII(cinza);
        }
        bufferASCII[y][larguraASCII] = '\0';
    }
    InvalidateRect(hwnd, NULL, FALSE);
}

void AsciiDesenhar(HDC hdc) {
    HFONT fonteAntiga = SelectObject(hdc, fonteASCII);
    for (int y = 0; y < alturaASCII; y++)
        TextOutA(hdc, 0, y * alturaCaractere, bufferASCII[y], larguraASCII);
    SelectObject(hdc, fonteAntiga);
}

void AsciiLiberar(void) {
    if (fonteASCII != NULL) {
        DeleteObject(fonteASCII);
        fonteASCII = NULL;
    }
}

int AsciiLargura(void) { return larguraASCII; }
int AsciiAltura(void) { return alturaASCII; }
int AsciiAlturaCaractere(void) { return alturaCaractere; }
