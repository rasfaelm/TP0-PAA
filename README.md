# Webcam ASCII

O projeto possui duas implementacoes da webcam e um unico ponto de entrada:

- `TP0.c` contem o unico `main` e apresenta o menu do projeto.
- Windows: os arquivos `*_windows.c`, usando Win32 e Media Foundation.
- Linux: os arquivos `*_linux.c`, usando terminal e V4L2.

## Linux

Requisitos:

- Linux com uma webcam V4L2, normalmente `/dev/video0`.
- GCC e headers do kernel Linux.
- Biblioteca de desenvolvimento JPEG (`libjpeg-dev` no Debian/Ubuntu).

Compile com:

```bash
make tp0
```

Execute com a webcam padrao:

```bash
./tp0
```

Ou informe outro dispositivo:

```bash
./tp0 /dev/video2
```

A imagem aparece no terminal. Encerre com `Ctrl+C`.

## Windows

Compile os arquivos originais com MinGW:

```bash
gcc -DINITGUID TP0.c webcam_windows.c ascii_windows.c camera_windows.c -o tp0.exe -lole32 -lmfplat -lmfreadwrite -lmf -luuid -lgdi32 -luser32
```

No menu, escolha `5 - webcam ASCII` para iniciar a camera. Por padrao, a
primeira camera encontrada sera usada. Para selecionar outra camera, informe
seu indice como primeiro argumento:

```powershell
.\tp0.exe 1
```

O indice comeca em `0`; a quantidade e a ordem das cameras sao informadas
quando o programa inicia.
