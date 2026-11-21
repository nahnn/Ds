# Makefile de apps/tp: compila a biblioteca cg e depois o tp, conforme o SO.
#
# Uso (de qualquer pasta):
#   make                     -> Debug (padrão)
#   make CONFIG=Release
#   make -C apps/tp          -> a partir da raiz do Ds
#   make cg | make tp | make run | make clean
#
# Pressupõe a estrutura do Ds: este diretório em $/Ds/apps/tp e a biblioteca em $/Ds/cg.
# Windows: requer GNU Make (ex.: winget install ezwinports.make, choco install make
# ou mingw32-make), executado no PowerShell ou no cmd.

CONFIG ?= Debug

ifeq ($(filter $(CONFIG),Debug Release),)
$(error CONFIG deve ser Debug ou Release (recebido: $(CONFIG)))
endif

# Pasta deste Makefile, independente de onde make foi chamado
TP_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
CG_DIR := $(abspath $(TP_DIR)/../../cg)

ifeq ($(OS),Windows_NT)
  # Windows: os .bat precisam do cmd.exe
  SHELL := cmd.exe
  .SHELLFLAGS := /c
  PLATFORM := Windows
  win = $(subst /,\,$(1))
  BUILD_CG = call "$(call win,$(CG_DIR)/build.bat)" $(CONFIG)
  BUILD_TP = call "$(call win,$(TP_DIR)/build.bat)" $(CONFIG)
  RUN_TP = cd /d "$(call win,$(TP_DIR))" && cgdemo.exe
  CLEAN_TP = if exist "$(call win,$(TP_DIR)/build)" rmdir /s /q "$(call win,$(TP_DIR)/build)"
  CG_CHECK = if not exist "$(call win,$(CG_DIR)/build.bat)" (echo Erro: cg nao encontrado em $(call win,$(CG_DIR)) && exit /b 1)
else
  UNAME_S := $(shell uname -s 2>/dev/null)
  # Linux, macOS e outros Unix: build.sh (CMake; no macOS usa a GLFW de cg/externals)
  ifneq ($(filter Linux Darwin FreeBSD OpenBSD NetBSD,$(UNAME_S)),)
    PLATFORM := $(UNAME_S)
  else
    $(error Sistema operacional nao suportado: "$(UNAME_S)". Use os scripts build.sh/build.bat manualmente.)
  endif
  # bash explicito: o bit de execucao dos .sh pode se perder ao zipar/enviar
  BUILD_CG = bash "$(CG_DIR)/build.sh" $(CONFIG)
  BUILD_TP = bash "$(TP_DIR)/build.sh" $(CONFIG)
  RUN_TP = cd "$(TP_DIR)" && ./cgdemo
  CLEAN_TP = rm -rf "$(TP_DIR)/build"
  CG_CHECK = test -f "$(CG_DIR)/build.sh" || { echo "Erro: cg nao encontrado em $(CG_DIR)"; exit 1; }
endif

.PHONY: all cg tp run clean info

all: tp

info:
	@echo Plataforma: $(PLATFORM)  Config: $(CONFIG)
	@echo cg: $(CG_DIR)
	@echo tp: $(TP_DIR)

cg:
	@$(CG_CHECK)
	$(BUILD_CG)

tp: cg
	$(BUILD_TP)

run: tp
	$(RUN_TP)

# Apaga so o build do tp; o build do cg nao e nosso
clean:
	$(CLEAN_TP)
