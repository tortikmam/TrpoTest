BLAS_PATH = ../OpenBLAS
BLAS_CHECK_FILE = $(BLAS_PATH)/Makefile

CXX = g++
CXXFLAGS = -O3 -fopenmp -I$(BLAS_PATH) -I./include
LDFLAGS = $(BLAS_PATH)/libopenblas.a -lpthread -lm -fopenmp

# Твой основной файл
TARGET = lab6
SOURCE = lab6.cpp

all: setup_blas $(TARGET)

setup_blas:
	@if [ ! -f "$(BLAS_CHECK_FILE)" ]; then \
		echo "--- OpenBLAS не найден в $(BLAS_PATH). Проверь путь! ---"; \
	fi
	@if [ ! -f "$(BLAS_PATH)/libopenblas.a" ]; then \
		$(MAKE) -C $(BLAS_PATH) NO_SHARED=1 ONLY_CBLAS=1 NOFORTRAN=1; \
	fi

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

run: all
	@echo "------------------------------------"
	@echo "Запуск $(TARGET)..."
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

distclean: clean
	$(MAKE) -C $(BLAS_PATH) clean
	rm -f $(BLAS_PATH)/libopenblas.a

.PHONY: all run clean setup_blas distclean