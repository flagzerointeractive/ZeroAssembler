# Compiler
CC = g++

# Detect platform
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# Installation directories
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
INSTALL = install
INSTALL_PROGRAM = $(INSTALL) -m 0755

# Default flags
CFLAGS = -O3 -std=c++17
LDFLAGS = 
PLATFORM_SRC = 

# Sources
ZSM_SRC = Source/ZSM.cpp
ZSM_BIN = ZSM

# Platform-specific settings
ifeq ($(UNAME_S),Darwin)
    # macOS
    ifeq ($(UNAME_M),arm64)
        # Apple Silicon
        CFLAGS += -target arm64-apple-macos14 -mcpu=apple-m4
    else
        # Intel Mac
        CFLAGS += -march=native
    endif
    INSTALL_PROGRAM = $(INSTALL) -m 0755
else ifeq ($(UNAME_S),Linux)
    # Linux
    CC = g++
    CFLAGS += -march=native
    INSTALL_PROGRAM = $(INSTALL) -m 0755
else ifeq ($(OS),Windows_NT)
    # Windows (w64devkit/MinGW)
    CC = g++
    ZSM_BIN = ZSM.exe
    CFLAGS += -march=native
    # Static linking for portability
    LDFLAGS += -static
    # Windows install locations
    PREFIX ?= C:/Program Files/ZSM
    BINDIR = $(PREFIX)
    INSTALL = copy
    INSTALL_PROGRAM = $(INSTALL)
else
    # Fallback for unknown systems
    LDFLAGS +=
endif

# Final sources
SOURCES = $(ZSM_SRC) $(PLATFORM_SRC)

# Default target
all: $(ZSM_BIN)

# Build client
$(ZSM_BIN): $(SOURCES) Source/*.hpp
	$(CC) $(SOURCES) $(CFLAGS) -o $(ZSM_BIN) $(LDFLAGS)

# Platform-specific targets
macos:
	$(MAKE) CC=clang++ UNAME_S=Darwin all

linux:
	$(MAKE) CC=g++ UNAME_S=Linux all

windows:
	$(MAKE) CC=g++ OS=Windows_NT all

# Clean build artifacts
clean:
ifeq ($(OS),Windows_NT)
	del /Q $(ZSM_BIN) 2>nul || exit 0
else
	rm -f $(ZSM_BIN) ZSM.exe
endif

# Install binary
install: $(ZSM_BIN)
ifeq ($(OS),Windows_NT)
	@echo Installing to $(BINDIR)...
	@if not exist "$(BINDIR)" mkdir "$(BINDIR)"
	$(INSTALL_PROGRAM) $(ZSM_BIN) "$(BINDIR)"
	@echo Installation complete!
	@echo Add $(BINDIR) to your PATH to use ZSM from anywhere
else
	@echo Installing to $(BINDIR)...
	@mkdir -p $(BINDIR)
	$(INSTALL_PROGRAM) $(ZSM_BIN) $(BINDIR)/$(ZSM_BIN)
	@echo Installation complete!
	@echo ZSM installed to $(BINDIR)/$(ZSM_BIN)
endif

# Uninstall binary
uninstall:
ifeq ($(OS),Windows_NT)
	@echo Uninstalling from $(BINDIR)...
	@if exist "$(BINDIR)\$(ZSM_BIN)" del "$(BINDIR)\$(ZSM_BIN)"
	@echo Uninstallation complete!
else
	@echo Uninstalling from $(BINDIR)...
	@rm -f $(BINDIR)/$(ZSM_BIN)
	@echo Uninstallation complete!
endif

# Install dependencies help
help:
	@echo "ZSM Cross-Platform Build System"
	@echo "================================"
	@echo ""
	@echo "Targets:"
	@echo "  make          - Auto-detect platform and build"
	@echo "  make macos    - Build for macOS"
	@echo "  make linux    - Build for Linux"
	@echo "  make windows  - Build for Windows"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make install  - Install ZSM binary (may require sudo on Unix)"
	@echo "  make uninstall- Uninstall ZSM binary"
	@echo ""
	@echo "Installation:"
	@echo "  Default install location: $(BINDIR)"
	@echo "  Custom install: make install PREFIX=/custom/path"
	@echo "  Unix/Linux:     sudo make install"
	@echo "  macOS:          sudo make install"
	@echo "  Windows:        make install (run as administrator for system-wide install)"
	@echo ""
	@echo "Current platform detected: $(UNAME_S)"

.PHONY: all macos linux windows clean install uninstall help