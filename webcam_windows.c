#include <windows.h>
#include <mfapi.h>
#include <stdio.h>
#include "ascii_windows.h"
#include "camera_windows.h"
#include "webcam_windows.h"

static HWND hwndPrincipal = NULL;

static int lerQualidade(void) {
    int valor;
    while (1) {
        printf("\nDigite a qualidade desejada (0-100): ");
        fflush(stdout);
        if (scanf("%d", &valor) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("\nValor invalido!\n");
            continue;
        }
        if (valor < 0 || valor > 100) {
            printf("\nDigite um valor entre 0 e 100.\n");
            continue;
        }
        return valor;
    }
}

static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        asciiDesenhar(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

int executarWebcamWindows(void) {
    HINSTANCE hInstance = GetModuleHandleA(NULL);
    int nCmdShow = SW_SHOWDEFAULT;
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        MessageBoxA(NULL, "Erro ao inicializar COM!", "Erro", MB_ICONERROR);
        return 1;
    }

    hr = MFStartup(MF_VERSION, MFSTARTUP_FULL);
    if (FAILED(hr)) {
        MessageBoxA(NULL, "Erro ao iniciar Media Foundation!", "Erro", MB_ICONERROR);
        CoUninitialize();
        return 1;
    }

    int qualidade = lerQualidade();
    hr = cameraInicializar();
    if (FAILED(hr)) {
        char mensagem[256];
        sprintf(mensagem, "Erro ao iniciar webcam!\nHRESULT: 0x%08lX", (unsigned long)hr);
        MessageBoxA(NULL, mensagem, "Erro", MB_ICONERROR);
        MFShutdown();
        CoUninitialize();
        return 1;
    }

    WNDCLASSA wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = windowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "WebcamASCII";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    if (!RegisterClassA(&wc)) {
        MessageBoxA(NULL, "Erro ao registrar a janela!", "Erro", MB_ICONERROR);
        cameraLiberar();
        MFShutdown();
        CoUninitialize();
        return 1;
    }

    hwndPrincipal = CreateWindowExA(0, "WebcamASCII", "Webcam ASCII - Media Foundation",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
        NULL, NULL, hInstance, NULL);
    if (hwndPrincipal == NULL) {
        cameraLiberar();
        MFShutdown();
        CoUninitialize();
        return 1;
    }

    asciiInicializar(hwndPrincipal, qualidade);
    asciiAjustarJanela(hwndPrincipal);
    ShowWindow(hwndPrincipal, nCmdShow);
    UpdateWindow(hwndPrincipal);

    MSG msg;
    while (1) {
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) goto fim;
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        cameraCapturar(hwndPrincipal);
    }

fim:
    cameraLiberar();
    asciiLiberar();
    MFShutdown();
    CoUninitialize();
    return 0;
}
