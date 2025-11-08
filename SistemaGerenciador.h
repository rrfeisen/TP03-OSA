#ifndef SISTEMA_GERENCIADOR_H
#define SISTEMA_GERENCIADOR_H

#include "Aluno.h"
#include "IndicePrimario.h"
#include "IndiceSecundario.h"
#include <vector>
#include <stack>
#include <string>

class SistemaGerenciador {
private:
    // Estruturas em memória RAM
    std::vector<IndicePrimario> indices_primarios;
    std::vector<IndiceSecundario> indices_secundarios;
    std::stack<long> lista_disponiveis;
    
    // Nomes dos arquivos
    const std::string arquivo_alunos = "alunos.dat";
    const std::string arquivo_indices_primarios = "indices_primarios.dat";
    const std::string arquivo_indices_cursos = "indices_cursos.dat";
    const std::string arquivo_lista_invertida = "lista_invertida_cursos.dat";
    const std::string arquivo_disponiveis = "disponiveis.dat";
    
    // Métodos auxiliares privados
    long _buscarIndicePrimario(int matricula);
    long _buscarIndiceSecundario(const std::string& curso);
    
    void _carregarIndicesPrimarios();
    void _carregarIndicesSecundarios();
    void _carregarListaDisponiveis();
    
    void _salvarIndicesPrimarios();
    void _salvarIndicesSecundarios();
    void _salvarListaDisponiveis();
    
    void _adicionarAoIndiceSecundario(int matricula, const std::string& curso);
    void _removerDoIndiceSecundario(int matricula, const std::string& curso);
    
    long _obterProximoRRNListaInvertida();
    
    Aluno _lerAlunoPorOffset(long offset);

public:
    SistemaGerenciador();
    ~SistemaGerenciador();
    
    void iniciar();
    void carregarDeCSV(const std::string& arquivo_csv);
    
    void inserirAluno();
    bool removerAlunoPorMatricula();
    void buscarAlunoPorMatricula();
    void buscarAlunosPorCurso();
    void listarTodosAlunos();
    
    void exibirMenu();
};

#endif
