set minimum-version := '1.27.0'

# Diretórios e arquivos
SRC_DIR := "src"
INCLUDE_DIR := "include"
LIB_DIR := "lib"
BIN_DIR := "bin"
APP_NAME := "app"
DLL_EXT := ".so"
ARCH_GNU := if arch() == "aarch64" { "aarch-64-linux-gnu" } else { "x86_64-linux-gnu" }
QT_INCLUDE := "-isystem /usr/" + ARCH_GNU + "/qt6 " + \
              "-isystem /usr/" + ARCH_GNU + "/qt6/QtCore " + \
              "-isystem /usr/" + ARCH_GNU + "/qt6/QtGui " + \
              "-isystem /usr/" + ARCH_GNU + "/qt6/QtWidgets"
QT_LIBS := "-lQt6Widgets -lQt6Gui -lQt6Core"
MOC_BIN := "/usr/lib/qt6/libexec/moc"

# Variáveis
CXX := "g++"
COMPILE_FLAGS := "-I" + INCLUDE_DIR +" -std=c++23"
DLL_COMPILE_FLAGS := "-shared -fPIC"


# Tutorial de texto de commit
[default]
como:
  @printf "Prefixos abaixo seguido de suas funções: \n \
  feat:     nova funcionalidade \n \
  fix:      correção de bug\n \
  docs:     mudança em documentação\n \
  style:    alterações que não modificam o significado do código (Ex: formatação)\n \
  refactor: alteração no código que não adiciona funcionalidade ou corrige bug\n \
  perf:     mudança no código para melhorar performance\n \
  test:     adição ou correção de testes\n \
  build:    alterações que afetam o sistema de build ou dependências externas\n \
  ci:       mudança nos arquivos de config e scripts de CI\n \
  chore:    mudança fora de arquivos de código e teste (manutenção)\n\n \
  Exemplo de uso => fix: Bug da função de fatorial com número negativo resolvido\n"

# Runner do app
run:
  ./{{BIN_DIR}}/{{APP_NAME}}
# Builder do app
build:
  {{CXX}} {{COMPILE_FLAGS}} {{SRC_DIR}}/*.cpp {{LIB_DIR}}/*{{DLL_EXT}} -o {{BIN_DIR}}/{{APP_NAME}} {{QT_LIBS}} {{QT_INCLUDE}} -L{{LIB_DIR}}

# Builder de DLL
build-dll dll-name='*':
  @for pasta in {{SRC_DIR}}/{{dll-name}}/; do \
    if [ -d "$pasta" ]; then \
      nome=$(basename "$pasta"); \
      echo -n "$nome... "; \
      just moc "$pasta" "$nome"; \
      if [ -e {{LIB_DIR}}/lib$nome{{DLL_EXT}} ]; then \
        echo -n "Backup gerado em {{LIB_DIR}}_backup... "; \
        mv {{LIB_DIR}}/lib$nome{{DLL_EXT}} {{LIB_DIR}}_backup; \
      else \
        echo -n "Sem backup... "; \
      fi; \
      {{CXX}} {{DLL_COMPILE_FLAGS}} {{COMPILE_FLAGS}} $pasta/*.cpp -o {{LIB_DIR}}/lib$nome{{DLL_EXT}} -DCOMPILANDO_A_BIBLIOTECA {{QT_INCLUDE}} {{QT_LIBS}}; \
      rm -rf $pasta/moc_$nome.cpp; \
    fi; \
    echo "Sucesso"; \
  done

# Comando auxiliar do builder de DLL
[private]
moc pasta nome:
  @{{MOC_BIN}} --no-notes {{INCLUDE_DIR}}/{{nome}}.hpp -o {{pasta}}/moc_{{nome}}.cpp

# Comando util para gerar o compile_commands.json para clangd parar de apitar erros
bear:
  bear -- just build

# Deletador de .so e EXEs
clean:
  rm -rf {{BIN_DIR}}/{{APP_NAME}} {{BIN_DIR}}/*{{DLL_EXT}} {{LIB_DIR}}/*{{DLL_EXT}} {{BIN_DIR}}/*.o

alias c := commit
# Comando de commit rápido
commit mensagem="":
  @if [ "{{mensagem}}" = "" ]; then \
    just como; \
  else \
    git add .; \
    git commit -m "{{mensagem}}"; \
  fi
