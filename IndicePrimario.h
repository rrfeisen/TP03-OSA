#ifndef INDICE_PRIMARIO_H
#define INDICE_PRIMARIO_H

#include <fstream>

using namespace std;

class IndicePrimario {
private:
    int matricula;
    long byte_offset;

public:
    IndicePrimario();
    IndicePrimario(int mat, long offset);
    
    // Getters
    int getMatricula() const;
    long getByteOffset() const;
    
    // Setters
    void setMatricula(int mat);
    void setByteOffset(long offset);
    
    // Serialização
    void serializar(ostream& out) const;
    void desserializar(istream& in);
    
    // Operador de comparação para ordenação
    bool operator<(const IndicePrimario& outro) const;
};

#endif
