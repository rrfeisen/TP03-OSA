#include "IndiceSecundario.h"
#include <cstring>

using namespace std;

// Implementação de NoListaInvertida
NoListaInvertida::NoListaInvertida() : matricula(0), proximo_rrn(-1) {}

NoListaInvertida::NoListaInvertida(int mat, long prox) 
    : matricula(mat), proximo_rrn(prox) {}

int NoListaInvertida::getMatricula() const { return matricula; }
long NoListaInvertida::getProximoRRN() const { return proximo_rrn; }

void NoListaInvertida::setMatricula(int mat) { matricula = mat; }
void NoListaInvertida::setProximoRRN(long prox) { proximo_rrn = prox; }

void NoListaInvertida::serializar(ostream& out) const {
    out.write(reinterpret_cast<const char*>(&matricula), sizeof(matricula));
    out.write(reinterpret_cast<const char*>(&proximo_rrn), sizeof(proximo_rrn));
}

void NoListaInvertida::desserializar(istream& in) {
    in.read(reinterpret_cast<char*>(&matricula), sizeof(matricula));
    in.read(reinterpret_cast<char*>(&proximo_rrn), sizeof(proximo_rrn));
}

size_t NoListaInvertida::tamanhoRegistro() {
    return sizeof(int) + sizeof(long);
}

// Implementação de IndiceSecundario
IndiceSecundario::IndiceSecundario() : primeiro_rrn(-1) {
    memset(curso, 0, sizeof(curso));
}

IndiceSecundario::IndiceSecundario(const string& cur, long rrn) 
    : primeiro_rrn(rrn) {
    memset(curso, 0, sizeof(curso));
    strncpy(curso, cur.c_str(), sizeof(curso) - 1);
}

string IndiceSecundario::getCurso() const { 
    return string(curso); 
}

long IndiceSecundario::getPrimeiroRRN() const { 
    return primeiro_rrn; 
}

void IndiceSecundario::setCurso(const string& cur) {
    memset(curso, 0, sizeof(curso));
    strncpy(curso, cur.c_str(), sizeof(curso) - 1);
}

void IndiceSecundario::setPrimeiroRRN(long rrn) { 
    primeiro_rrn = rrn; 
}

void IndiceSecundario::serializar(ostream& out) const {
    out.write(curso, sizeof(curso));
    out.write(reinterpret_cast<const char*>(&primeiro_rrn), sizeof(primeiro_rrn));
}

void IndiceSecundario::desserializar(istream& in) {
    in.read(curso, sizeof(curso));
    in.read(reinterpret_cast<char*>(&primeiro_rrn), sizeof(primeiro_rrn));
}
