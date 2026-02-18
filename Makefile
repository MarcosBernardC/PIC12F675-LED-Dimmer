# --- Configuración de LaTeX ---
DOC_DIR = docs
DOC_BUILD = $(DOC_DIR)/build
DOC_NAME = investigacion_dimmer
MAIN_TEX = $(DOC_DIR)/$(DOC_NAME).tex
LATEX_CMD = latexmk -pdf -output-directory=$(DOC_BUILD)

# --- Configuración de XC8 (Firmware) ---
CC = /opt/microchip/xc8/v2.40/bin/xc8-cc
CHIP = 12f675
SRC_DIR = src
SRC_BUILD = $(SRC_DIR)/build
TARGET_HEX = $(SRC_DIR)/main.hex
SOURCES = $(SRC_DIR)/main.c

# --- Reglas Principales ---

all: firmware latex

# Compila el firmware moviendo la basura a src/build
firmware: $(SOURCES)
	@mkdir -p $(SRC_BUILD)
	$(CC) -mcpu=$(CHIP) $(SOURCES) -o $(SRC_BUILD)/main.elf
	@# Copiamos el .hex final a la raíz de src y borramos lo que sobra fuera
	@cp $(SRC_BUILD)/main.hex $(TARGET_HEX)
	@echo "Firmware compilado exitosamente: $(TARGET_HEX)"

# Compila la documentación
latex: $(MAIN_TEX)
	@mkdir -p $(DOC_BUILD)
	$(LATEX_CMD) $(MAIN_TEX)
	@cp $(DOC_BUILD)/$(DOC_NAME).pdf $(DOC_DIR)/
	@echo "Documentación PDF actualizada en $(DOC_DIR)/"

# Limpieza total de carpetas build y archivos huérfanos
clean:
	@rm -rf $(DOC_BUILD) $(SRC_BUILD)
	@rm -f startup.lst startup.o startup.rlf startup.s __eeprom.d __eeprom.p1
	@echo "Entorno limpio. Carpetas build eliminadas."

# Quema el firmware
flash: firmware
	pk2cmd -P PIC$(CHIP) -M -F $(TARGET_HEX) -T

.PHONY: all firmware latex clean flash