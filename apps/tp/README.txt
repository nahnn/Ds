TP - Computacao Grafica 2026 (Parte 2) - Compilacao
===================================================

Requisitos
----------
- CMake 3.18 ou superior e um compilador C++20.
- Windows: Visual Studio (2022 ou mais recente).
- Linux: g++ e OpenGL/GLFW do sistema (ex.: sudo apt install libglfw3-dev).
- Esta pasta deve estar em $/Ds/apps/tp, com a biblioteca do professor em $/Ds/cg.

Opcao 1: make (Linux, macOS e Windows com GNU Make)
---------------------------------------------------
Dentro de apps/tp:

    make                  compila cg e tp em Debug
    make CONFIG=Release   compila em Release
    make run              compila e executa
    make clean            apaga o build do tp

A partir da raiz do Ds:  make -C apps/tp

O Makefile detecta o sistema operacional e chama os scripts abaixo.
No Windows o make nao vem instalado (ex.: winget install ezwinports.make);
use-o no PowerShell ou no cmd.

Opcao 2: scripts (sem make)
---------------------------
A partir da raiz do Ds, primeiro a biblioteca cg, depois o tp:

  Windows:
    .\cg\build.bat Debug
    .\apps\tp\build.bat Debug

  Linux / macOS:
    ./cg/build.sh Debug
    ./apps/tp/build.sh Debug

Troque Debug por Release se preferir (use a mesma configuracao nos dois).

Execucao
--------
O executavel e gerado em apps/tp: cgdemo.exe (Windows) ou cgdemo (Linux/macOS).
Execute-o de dentro de apps/tp, para que a pasta assets seja encontrada.
