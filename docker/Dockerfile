FROM gcc:16.2-trixie

# Evita prompts interativos durante a instalação de pacotes
ENV DEBIAN_FRONTEND=noninteractive

# Permite rodar testes de GUI sem precisar de monitor/display conectado
ENV QT_QPA_PLATFORM=offscreen

RUN apt-get update && apt-get install -y --no-install-recommends \
    # Ferramentas de build
    cmake \
    ninja-build \
    pkg-config \
    # Bibliotecas e ferramentas do Qt6 (inclui moc, uic, rcc)
    qt6-base-dev \
    qt6-base-dev-tools \
    # Dependências gráficas comuns para Qt (headless e X11)
    libgl1-mesa-dev \
    libxkbcommon-x11-0 \
    xvfb \
    # Framework de testes Catch2 (v3 no Debian Trixie)
    catch2 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

CMD ["/bin/bash"]