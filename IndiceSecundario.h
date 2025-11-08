#ifndef INDICE_SECUNDARIO_H
#define INDICE_SECUNDARIO_H

#include <string>
#include <fstream>

using namespace std;

// Estrutura de um nó da lista invertida
class NoListaInvertida {
private:
    int matricula;
    long proximo_rrn; // RRN do próximo nó (-1 se for o último)

public:
    NoListaInvertida();
    NoListaInvertida(int mat, long prox);
    
    int getMatricula() const;
    long getProximoRRN() const;
    
    void setMatricula(int mat);
    void setProximoRRN(long prox);
    
    void serializar(ostream& out) const;
    void desserializar(istream& in);
    
    static size_t tamanhoRegistro();
};

// Índice secundário que aponta para o início da lista invertida
class IndiceSecundario {
private:
    char curso[50];
    long primeiro_rrn; // RRN do primeiro nó da lista invertida

public:
    IndiceSecundario();
    IndiceSecundario(const string& cur, long rrn);
    
    string getCurso() const;
    long getPrimeiroRRN() const;
    
    void setCurso(const string& cur);
    void setPrimeiroRRN(long rrn);
    
    void serializar(ostream& out) const;
    void desserializar(istream& in);
};

#endif
