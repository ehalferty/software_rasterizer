#!/bin/bash

x86_64-w64-mingw32-g++ main.c -lgdi32 -Wl,--subsystem,windows -o "app.exe" && ./*exe*

# gcc -D UNICODE -D _UNICODE -c myfile.c -o myfile.o
# windres -i resource.rc -o resource.o
# gcc -o myapplication.exe myfile.o resource.o -s -lcomctl32 -Wl,--subsystem,windows
