# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g

# Nome do executável
TARGET = sistema_academico

# Arquivos fonte e objeto
SOURCES = main.cpp Aluno.cpp IndicePrimario.cpp IndiceSecundario.cpp SistemaGerenciador.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Regra principal
all: $(TARGET)

# Regra para criar o executável
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Regra para compilar arquivos .cpp em .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Dependências dos headers
Aluno.o: Aluno.cpp Aluno.h
IndicePrimario.o: IndicePrimario.cpp IndicePrimario.h
IndiceSecundario.o: IndiceSecundario.cpp IndiceSecundario.h
SistemaGerenciador.o: SistemaGerenciador.cpp SistemaGerenciador.h Aluno.h IndicePrimario.h IndiceSecundario.h
main.o: main.cpp SistemaGerenciador.h

# Limpeza dos arquivos compilados
clean:
	rm -f $(OBJECTS) $(TARGET)

# Limpeza dos arquivos de dados
cleandata:
	rm -f *.dat

# Limpeza completa
cleanall: clean cleandata

# Executar o programa
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean cleandata cleanall run
