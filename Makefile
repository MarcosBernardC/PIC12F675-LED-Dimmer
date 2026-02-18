# Variables
LATEX_CMD = latexmk -pdf -output-directory=docs
DOC_DIR = docs
DOC_NAME = investigacion_dimmer
MAIN_TEX = $(DOC_DIR)/$(DOC_NAME).tex

# Comando por defecto: Compila el PDF
all: pdf

pdf: $(MAIN_TEX)
	$(LATEX_CMD) $(MAIN_TEX)

# Limpieza profunda usando latexmk
clean:
	$(LATEX_CMD) -C
	# -C elimina incluso el PDF. Si quieres conservar el PDF, usa -c (minúscula)

# Visualización continua (Hot Reload)
# Este comando se queda escuchando cambios y recompila al guardar
watch:
	$(LATEX_CMD) -pvc $(MAIN_TEX)

.PHONY: all pdf clean watch