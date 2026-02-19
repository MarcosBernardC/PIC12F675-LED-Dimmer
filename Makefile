# --- Configuración de LaTeX ---
DOC_DIR    = docs
DOC_BUILD  = $(DOC_DIR)/build
DOC_NAME   = investigacion_dimmer
MAIN_TEX   = $(DOC_DIR)/$(DOC_NAME).tex
LATEX_CMD  = latexmk -pdf -output-directory=$(DOC_BUILD)

# --- Configuración de XC8 (Firmware) ---
CC         = /opt/microchip/xc8/v2.40/bin/xc8-cc
CHIP       = 12F675
SRC_DIR    = src
SRC_BUILD  = $(SRC_DIR)/build
TARGET_HEX = $(SRC_DIR)/main.hex
SOURCES    = $(SRC_DIR)/main.c

# --- Reglas Principales ---

all: firmware latex

# Compila el firmware
firmware: $(SOURCES)
	@mkdir -p $(SRC_BUILD)
	$(CC) -mcpu=$(CHIP) $(SOURCES) -o $(SRC_BUILD)/main.elf
	@cp $(SRC_BUILD)/main.hex $(TARGET_HEX)
	@echo "Firmware compilado: $(TARGET_HEX)"

# Quema el firmware (sin -V, el PICkit mide el voltaje solo)
flash: firmware
	pk2cmd -P PIC$(CHIP) -M -F $(TARGET_HEX)

# Solo quema sin recompilar
burn:
	pk2cmd -P PIC$(CHIP) -M -F $(TARGET_HEX)

# Borra el chip
erase:
	pk2cmd -P PIC$(CHIP) -E

# Lee el chip y guarda en dump.hex
dump:
	pk2cmd -P PIC$(CHIP) -GF dump.hex

# Compila la documentación
latex: $(MAIN_TEX)
	@mkdir -p $(DOC_BUILD)
	$(LATEX_CMD) $(MAIN_TEX)
	@cp $(DOC_BUILD)/$(DOC_NAME).pdf $(DOC_DIR)/
	@echo "PDF actualizado en $(DOC_DIR)/"

# Limpieza total
clean:
	@rm -rf $(DOC_BUILD) $(SRC_BUILD)
	@rm -f startup.lst startup.o startup.rlf startup.s __eeprom.d __eeprom.p1
	@echo "Entorno limpio."

.PHONY: all firmware flash burn erase dump latex clean