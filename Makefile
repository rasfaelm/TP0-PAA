CC ?= gcc
CFLAGS ?= -Wall -Wextra -std=c11 -O2

tp0: TP0.c webcam_linux.c ascii_linux.c camera_linux.c
	$(CC) $(CFLAGS) TP0.c webcam_linux.c ascii_linux.c camera_linux.c -o tp0 -ljpeg

tp0-windows:
	$(CC) -DINITGUID TP0.c webcam_windows.c ascii_windows.c camera_windows.c -o tp0.exe -lole32 -lmfplat -lmfreadwrite -lmf -luuid -lgdi32 -luser32

clean:
	rm -f tp0 tp0.exe

.PHONY: tp0 tp0-windows clean
