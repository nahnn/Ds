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
# ou mingw32-make).

CONFIG ?= Debug

ifeq ($(filter $(CONFIG),Debug Release),)
$(error CONFIG deve ser Debug ou Release (recebido: $(CONFIG)))
endif

# Pasta deste Makefile, independente de onde make foi chamado
TP_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
CG_DIR := $(abspath $(TP_DIR)/../../cg)

# Confere a estrutura antes de qualquer comando (funciona em qualquer SO)
ifeq ($(wildcard $(CG_DIR)/CMakeLists.txt),)
  $(error Biblioteca cg nao encontrada em $(CG_DIR). Esta pasta deve estar em Ds/apps/tp)
endif

ifeq ($(OS),Windows_NT)
  PLATFORM := Windows
  ifneq ($(filter %msys %cygwin,$(MAKE_HOST)),)
    # make do MSYS2/Cygwin/Git Bash (ex.: "Built for x86_64-pc-msys"): as receitas
    # rodam em /bin/sh e abspath devolve caminhos POSIX (/c/Users/...). O sh executa
    # .bat por esse caminho e converte para o formato do Windows sozinho.
    BUILD_CG = "$(CG_DIR)/build.bat" $(CONFIG)
    BUILD_TP = "$(TP_DIR)/build.bat" $(CONFIG)
    EXE := $(TP_DIR)/cgdemo.exe
  else
    # make nativo do Windows (GnuWin32, ezwinports, mingw32-make): as receitas rodam
    # pelo cmd.exe e abspath devolve C:/Users/...; troca / por \ para o cmd
    win = $(subst /,\,$(1))
    BUILD_CG = "$(call win,$(CG_DIR)/build.bat)" $(CONFIG)
    BUILD_TP = "$(call win,$(TP_DIR)/build.bat)" $(CONFIG)
    EXE := $(call win,$(TP_DIR)/cgdemo.exe)
  endif
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
  EXE := $(TP_DIR)/cgdemo
endif

.PHONY: all cg tp run clean info

all: tp

info:
	@echo Plataforma: $(PLATFORM)  Config: $(CONFIG)
	@echo cg: $(CG_DIR)
	@echo tp: $(TP_DIR)

cg:
	$(BUILD_CG)

tp: cg
	$(BUILD_TP)

# Rode a partir de apps/tp (ou com make -C apps/tp): o programa usa a pasta assets
run: tp
	"$(EXE)"

# Apaga so o build do tp; o build do cg nao e nosso
clean:
	cmake -E rm -rf "$(TP_DIR)/build"
