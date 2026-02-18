# Variables
DOC_DIR = docs
BUILD_DIR = $(DOC_DIR)/build
DOC_NAME = investigacion_dimmer
MAIN_TEX = $(DOC_DIR)/$(DOC_NAME).tex

# Comando latexmk con directorios separados
# -output-directory: donde van los auxiliares
# -emulate-aux-dir: ayuda a que algunos paquetes no se confundan
LATEX_CMD = latexmk -pdf -output-directory=$(BUILD_DIR)

# Comando por defecto
all: pdf

pdf: $(MAIN_TEX)
	@mkdir -p $(BUILD_DIR)
	$(LATEX_CMD) $(MAIN_TEX)
	@# Movemos el PDF de la carpeta build a docs para que sea fácil de encontrar
	@cp $(BUILD_DIR)/$(DOC_NAME).pdf $(DOC_DIR)/

clean:
	@rm -rf $(BUILD_DIR)
	@echo "Carpeta build eliminada."

watch:
	@mkdir -p $(BUILD_DIR)
	$(LATEX_CMD) -pvc $(MAIN_TEX)

.PHONY: all pdf clean watch