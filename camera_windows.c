#include "camera_windows.h"
#include "ascii_windows.h"

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <stdio.h>

static IMFSourceReader *reader = NULL;

void cameraCapturar(HWND hwnd) {
    IMFSample *sample = NULL;
    IMFMediaBuffer *mediaBuffer = NULL;
    DWORD flags = 0;
    HRESULT hr = reader->lpVtbl->ReadSample(reader, MF_SOURCE_READER_FIRST_VIDEO_STREAM,
        0, NULL, &flags, NULL, &sample);
    if (FAILED(hr) || sample == NULL) return;

    hr = sample->lpVtbl->ConvertToContiguousBuffer(sample, &mediaBuffer);
    if (FAILED(hr)) {
        sample->lpVtbl->Release(sample);
        return;
    }

    BYTE *dados = NULL;
    DWORD tamanhoMaximo = 0, tamanhoAtual = 0;
    if (SUCCEEDED(mediaBuffer->lpVtbl->Lock(mediaBuffer, &dados, &tamanhoMaximo, &tamanhoAtual))) {
        asciiAtualizar(dados, hwnd);
        mediaBuffer->lpVtbl->Unlock(mediaBuffer);
    }
    mediaBuffer->lpVtbl->Release(mediaBuffer);
    sample->lpVtbl->Release(sample);
}

HRESULT cameraInicializar(void) {
    HRESULT hr;
    IMFAttributes *atributos = NULL, *readerAttributes = NULL;
    IMFActivate **dispositivos = NULL;
    UINT32 quantidade = 0;
    IMFMediaSource *fonte = NULL;
    IMFMediaType *tipo = NULL;

    hr = MFCreateAttributes(&atributos, 1);
    if (FAILED(hr)) goto erro;
    hr = atributos->lpVtbl->SetGUID(atributos, &MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
        &MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
    if (FAILED(hr)) goto erro;
    hr = MFEnumDeviceSources(atributos, &dispositivos, &quantidade);
    if (FAILED(hr) || quantidade == 0) {
        if (SUCCEEDED(hr)) hr = E_FAIL;
        goto erro;
    }
    printf("Quantidade de webcams encontradas: %u\n", quantidade);
    hr = dispositivos[0]->lpVtbl->ActivateObject(dispositivos[0], &IID_IMFMediaSource,
        (void **)&fonte);
    if (FAILED(hr)) goto erro;
    hr = MFCreateAttributes(&readerAttributes, 2);
    if (FAILED(hr)) goto erro;
    readerAttributes->lpVtbl->SetUINT32(readerAttributes, &MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE);
    readerAttributes->lpVtbl->SetUINT32(readerAttributes, &MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
    hr = MFCreateSourceReaderFromMediaSource(fonte, readerAttributes, &reader);
    if (FAILED(hr)) goto erro;
    hr = MFCreateMediaType(&tipo);
    if (FAILED(hr)) goto erro;
    tipo->lpVtbl->SetGUID(tipo, &MF_MT_MAJOR_TYPE, &MFMediaType_Video);
    tipo->lpVtbl->SetGUID(tipo, &MF_MT_SUBTYPE, &MFVideoFormat_RGB32);
    hr = reader->lpVtbl->SetCurrentMediaType(reader, MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, tipo);
    if (FAILED(hr)) goto erro;
    printf("Webcam inicializada com sucesso!\n");

erro:
    if (tipo != NULL) tipo->lpVtbl->Release(tipo);
    if (readerAttributes != NULL) readerAttributes->lpVtbl->Release(readerAttributes);
    if (fonte != NULL) fonte->lpVtbl->Release(fonte);
    if (dispositivos != NULL) {
        for (UINT32 i = 0; i < quantidade; i++) dispositivos[i]->lpVtbl->Release(dispositivos[i]);
        CoTaskMemFree(dispositivos);
    }
    if (atributos != NULL) atributos->lpVtbl->Release(atributos);
    return hr;
}

void cameraLiberar(void) {
    if (reader != NULL) {
        reader->lpVtbl->Release(reader);
        reader = NULL;
    }
}
