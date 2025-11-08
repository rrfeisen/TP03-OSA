#include "SistemaGerenciador.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

SistemaGerenciador::SistemaGerenciador() {
    _carregarIndicesPrimarios();
    _carregarIndicesSecundarios();
    _carregarListaDisponiveis();
}

SistemaGerenciador::~SistemaGerenciador() {
    _salvarIndicesPrimarios();
    _salvarIndicesSecundarios();
    _salvarListaDisponiveis();
}

// Busca binária no vetor de índices primários
long SistemaGerenciador::_buscarIndicePrimario(int matricula) {
    long esquerda = 0;
    long direita = indices_primarios.size() - 1;
    
    while (esquerda <= direita) {
        long meio = esquerda + (direita - esquerda) / 2;
        
        if (indices_primarios[meio].getMatricula() == matricula) {
            return meio;
        }
        
        if (indices_primarios[meio].getMatricula() < matricula) {
            esquerda = meio + 1;
        } else {
            direita = meio - 1;
        }
    }
    
    return -1;
}

// Busca linear no vetor de índices secundários
long SistemaGerenciador::_buscarIndiceSecundario(const std::string& curso) {
    for (size_t i = 0; i < indices_secundarios.size(); i++) {
        if (indices_secundarios[i].getCurso() == curso) {
            return i;
        }
    }
    return -1;
}

void SistemaGerenciador::_carregarIndicesPrimarios() {
    indices_primarios.clear();
    std::ifstream arquivo(arquivo_indices_primarios, std::ios::binary);
    
    if (!arquivo) return;
    
    IndicePrimario indice;
    while (arquivo.peek() != EOF) {
        indice.desserializar(arquivo);
        indices_primarios.push_back(indice);
    }
    
    arquivo.close();
}

void SistemaGerenciador::_carregarIndicesSecundarios() {
    indices_secundarios.clear();
    std::ifstream arquivo(arquivo_indices_cursos, std::ios::binary);
    
    if (!arquivo) return;
    
    IndiceSecundario indice;
    while (arquivo.peek() != EOF) {
        indice.desserializar(arquivo);
        indices_secundarios.push_back(indice);
    }
    
    arquivo.close();
}

void SistemaGerenciador::_carregarListaDisponiveis() {
    while (!lista_disponiveis.empty()) {
        lista_disponiveis.pop();
    }
    
    std::ifstream arquivo(arquivo_disponiveis, std::ios::binary);
    
    if (!arquivo) return;
    
    long offset;
    while (arquivo.read(reinterpret_cast<char*>(&offset), sizeof(offset))) {
        lista_disponiveis.push(offset);
    }
    
    arquivo.close();
}

void SistemaGerenciador::_salvarIndicesPrimarios() {
    std::ofstream arquivo(arquivo_indices_primarios, std::ios::binary | std::ios::trunc);
    
    for (const auto& indice : indices_primarios) {
        indice.serializar(arquivo);
    }
    
    arquivo.close();
}

void SistemaGerenciador::_salvarIndicesSecundarios() {
    std::ofstream arquivo(arquivo_indices_cursos, std::ios::binary | std::ios::trunc);
    
    for (const auto& indice : indices_secundarios) {
        indice.serializar(arquivo);
    }
    
    arquivo.close();
}

void SistemaGerenciador::_salvarListaDisponiveis() {
    std::ofstream arquivo(arquivo_disponiveis, std::ios::binary | std::ios::trunc);
    
    std::stack<long> temp = lista_disponiveis;
    std::vector<long> offsets;
    
    while (!temp.empty()) {
        offsets.push_back(temp.top());
        temp.pop();
    }
    
    for (auto it = offsets.rbegin(); it != offsets.rend(); ++it) {
        arquivo.write(reinterpret_cast<const char*>(&(*it)), sizeof(long));
    }
    
    arquivo.close();
}

long SistemaGerenciador::_obterProximoRRNListaInvertida() {
    std::ifstream arquivo(arquivo_lista_invertida, std::ios::binary | std::ios::ate);
    
    if (!arquivo) return 0;
    
    long tamanho = arquivo.tellg();
    arquivo.close();
    
    return tamanho / NoListaInvertida::tamanhoRegistro();
}

void SistemaGerenciador::_adicionarAoIndiceSecundario(int matricula, const std::string& curso) {
    long idx = _buscarIndiceSecundario(curso);
    
    long novo_rrn = _obterProximoRRNListaInvertida();
    
    if (idx == -1) {
        // Curso não existe, criar novo índice
        IndiceSecundario novo_indice(curso, novo_rrn);
        indices_secundarios.push_back(novo_indice);
        
        // Criar primeiro nó da lista invertida
        NoListaInvertida novo_no(matricula, -1);
        std::ofstream arquivo(arquivo_lista_invertida, std::ios::binary | std::ios::app);
        novo_no.serializar(arquivo);
        arquivo.close();
    } else {
        // Curso existe, adicionar no início da lista
        long antigo_primeiro = indices_secundarios[idx].getPrimeiroRRN();
        
        // Criar novo nó apontando para o antigo primeiro
        NoListaInvertida novo_no(matricula, antigo_primeiro);
        std::ofstream arquivo(arquivo_lista_invertida, std::ios::binary | std::ios::app);
        novo_no.serializar(arquivo);
        arquivo.close();
        
        // Atualizar índice para apontar para o novo nó
        indices_secundarios[idx].setPrimeiroRRN(novo_rrn);
    }
}

void SistemaGerenciador::_removerDoIndiceSecundario(int matricula, const std::string& curso) {
    long idx = _buscarIndiceSecundario(curso);
    
    if (idx == -1) return;
    
    long rrn_atual = indices_secundarios[idx].getPrimeiroRRN();
    long rrn_anterior = -1;
    
    std::fstream arquivo(arquivo_lista_invertida, std::ios::binary | std::ios::in | std::ios::out);
    
    while (rrn_atual != -1) {
        arquivo.seekg(rrn_atual * NoListaInvertida::tamanhoRegistro());
        
        NoListaInvertida no;
        no.desserializar((std::ifstream&)arquivo);
        
        if (no.getMatricula() == matricula) {
            // Encontrou o nó a remover
            if (rrn_anterior == -1) {
                // É o primeiro nó
                indices_secundarios[idx].setPrimeiroRRN(no.getProximoRRN());
            } else {
                // Não é o primeiro, atualizar o nó anterior
                arquivo.seekg(rrn_anterior * NoListaInvertida::tamanhoRegistro());
                NoListaInvertida no_anterior;
                no_anterior.desserializar((std::ifstream&)arquivo);
                no_anterior.setProximoRRN(no.getProximoRRN());
                
                arquivo.seekp(rrn_anterior * NoListaInvertida::tamanhoRegistro());
                no_anterior.serializar((std::ofstream&)arquivo);
            }
            break;
        }
        
        rrn_anterior = rrn_atual;
        rrn_atual = no.getProximoRRN();
    }
    
    arquivo.close();
}

Aluno SistemaGerenciador::_lerAlunoPorOffset(long offset) {
    std::ifstream arquivo(arquivo_alunos, std::ios::binary);
    arquivo.seekg(offset);
    
    Aluno aluno;
    aluno.desserializar(arquivo);
    
    arquivo.close();
    return aluno;
}

void SistemaGerenciador::inserirAluno() {
    int matricula;
    std::string nome, curso;
    float coeficiente;
    
    std::cout << "\n=== INSERIR NOVO ALUNO ===" << std::endl;
    std::cout << "Matrícula: ";
    std::cin >> matricula;
    std::cin.ignore();
    
    // Verificar se matrícula já existe
    if (_buscarIndicePrimario(matricula) != -1) {
        std::cout << "Erro: Matrícula já cadastrada!" << std::endl;
        return;
    }
    
    std::cout << "Nome: ";
    std::getline(std::cin, nome);
    std::cout << "Curso: ";
    std::getline(std::cin, curso);
    std::cout << "Coeficiente: ";
    std::cin >> coeficiente;
    
    Aluno aluno(matricula, nome, curso, coeficiente);
    
    long offset;
    
    // Verificar se há espaço disponível
    if (!lista_disponiveis.empty()) {
        offset = lista_disponiveis.top();
        lista_disponiveis.pop();
        
        // Escrever no offset disponível
        std::fstream arquivo(arquivo_alunos, std::ios::binary | std::ios::in | std::ios::out);
        arquivo.seekp(offset);
        aluno.serializar((std::ofstream&)arquivo);
        arquivo.close();
    } else {
        // Escrever no final do arquivo
        std::ofstream arquivo(arquivo_alunos, std::ios::binary | std::ios::app);
        offset = arquivo.tellp();
        aluno.serializar(arquivo);
        arquivo.close();
    }
    
    // Adicionar ao índice primário
    IndicePrimario novo_indice(matricula, offset);
    indices_primarios.push_back(novo_indice);
    std::sort(indices_primarios.begin(), indices_primarios.end());
    
    // Adicionar ao índice secundário
    _adicionarAoIndiceSecundario(matricula, curso);
    
    std::cout << "Aluno inserido com sucesso!" << std::endl;
}

bool SistemaGerenciador::removerAlunoPorMatricula() {
    int matricula;
    
    std::cout << "\n=== REMOVER ALUNO ===" << std::endl;
    std::cout << "Matrícula: ";
    std::cin >> matricula;
    
    long idx = _buscarIndicePrimario(matricula);
    
    if (idx == -1) {
        std::cout << "Matrícula não encontrada!" << std::endl;
        return false;
    }
    
    // Obter o byte offset
    long byte_offset = indices_primarios[idx].getByteOffset();
    
    // Ler o aluno para obter o curso
    Aluno aluno = _lerAlunoPorOffset(byte_offset);
    
    // Adicionar à lista de disponíveis
    lista_disponiveis.push(byte_offset);
    
    // Remover do índice primário
    indices_primarios.erase(indices_primarios.begin() + idx);
    
    // Remover do índice secundário
    _removerDoIndiceSecundario(matricula, aluno.getCurso());
    
    std::cout << "Aluno removido com sucesso!" << std::endl;
    return true;
}

void SistemaGerenciador::buscarAlunoPorMatricula() {
    int matricula;
    
    std::cout << "\n=== BUSCAR ALUNO POR MATRÍCULA ===" << std::endl;
    std::cout << "Matrícula: ";
    std::cin >> matricula;
    
    long idx = _buscarIndicePrimario(matricula);
    
    if (idx == -1) {
        std::cout << "Matrícula não encontrada!" << std::endl;
        return;
    }
    
    long byte_offset = indices_primarios[idx].getByteOffset();
    Aluno aluno = _lerAlunoPorOffset(byte_offset);
    
    std::cout << "\n--- Dados do Aluno ---" << std::endl;
    aluno.exibir();
}

void SistemaGerenciador::buscarAlunosPorCurso() {
    std::string curso;
    
    std::cout << "\n=== BUSCAR ALUNOS POR CURSO ===" << std::endl;
    std::cout << "Curso: ";
    std::cin.ignore();
    std::getline(std::cin, curso);
    
    long idx = _buscarIndiceSecundario(curso);
    
    if (idx == -1) {
        std::cout << "Nenhum aluno cadastrado neste curso!" << std::endl;
        return;
    }
    
    long rrn_atual = indices_secundarios[idx].getPrimeiroRRN();
    
    std::cout << "\n--- Alunos do curso " << curso << " ---" << std::endl;
    
    std::ifstream arquivo_lista(arquivo_lista_invertida, std::ios::binary);
    int contador = 0;
    
    while (rrn_atual != -1) {
        arquivo_lista.seekg(rrn_atual * NoListaInvertida::tamanhoRegistro());
        
        NoListaInvertida no;
        no.desserializar(arquivo_lista);
        
        long idx_prim = _buscarIndicePrimario(no.getMatricula());
        if (idx_prim != -1) {
            Aluno aluno = _lerAlunoPorOffset(indices_primarios[idx_prim].getByteOffset());
            
            std::cout << "\n[" << ++contador << "]" << std::endl;
            aluno.exibir();
        }
        
        rrn_atual = no.getProximoRRN();
    }
    
    arquivo_lista.close();
    
    if (contador == 0) {
        std::cout << "Nenhum aluno encontrado." << std::endl;
    }
}

void SistemaGerenciador::listarTodosAlunos() {
    std::cout << "\n=== TODOS OS ALUNOS ===" << std::endl;
    
    if (indices_primarios.empty()) {
        std::cout << "Nenhum aluno cadastrado." << std::endl;
        return;
    }
    
    for (size_t i = 0; i < indices_primarios.size(); i++) {
        Aluno aluno = _lerAlunoPorOffset(indices_primarios[i].getByteOffset());
        std::cout << "\n[" << (i + 1) << "]" << std::endl;
        aluno.exibir();
    }
}

void SistemaGerenciador::carregarDeCSV(const std::string& arquivo_csv) {
    std::ifstream arquivo(arquivo_csv);
    
    if (!arquivo) {
        std::cout << "Arquivo CSV não encontrado: " << arquivo_csv << std::endl;
        return;
    }
    
    std::string linha;
    std::getline(arquivo, linha); // Pular cabeçalho
    
    int count = 0;
    while (std::getline(arquivo, linha)) {
        std::stringstream ss(linha);
        std::string mat_str, nome, curso, coef_str;
        
        std::getline(ss, mat_str, ',');
        std::getline(ss, nome, ',');
        std::getline(ss, curso, ',');
        std::getline(ss, coef_str, ',');
        
        int matricula = std::stoi(mat_str);
        float coeficiente = std::stof(coef_str);
        
        // Verificar se já existe
        if (_buscarIndicePrimario(matricula) != -1) {
            continue;
        }
        
        Aluno aluno(matricula, nome, curso, coeficiente);
        
        // Escrever no final do arquivo
        std::ofstream arq_alunos(arquivo_alunos, std::ios::binary | std::ios::app);
        long offset = arq_alunos.tellp();
        aluno.serializar(arq_alunos);
        arq_alunos.close();
        
        // Adicionar aos índices
        IndicePrimario novo_indice(matricula, offset);
        indices_primarios.push_back(novo_indice);
        _adicionarAoIndiceSecundario(matricula, curso);
        
        count++;
    }
    
    std::sort(indices_primarios.begin(), indices_primarios.end());
    
    arquivo.close();
    std::cout << count << " alunos carregados do CSV!" << std::endl;
}

void SistemaGerenciador::exibirMenu() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  SISTEMA DE GERENCIAMENTO ACADÊMICO" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "1. Inserir aluno" << std::endl;
    std::cout << "2. Remover aluno por matrícula" << std::endl;
    std::cout << "3. Buscar aluno por matrícula" << std::endl;
    std::cout << "4. Buscar alunos por curso" << std::endl;
    std::cout << "5. Listar todos os alunos" << std::endl;
    std::cout << "6. Carregar dados de CSV" << std::endl;
    std::cout << "0. Sair" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Opção: ";
}

void SistemaGerenciador::iniciar() {
    int opcao;
    
    do {
        exibirMenu();
        std::cin >> opcao;
        
        switch (opcao) {
            case 1:
                inserirAluno();
                break;
            case 2:
                removerAlunoPorMatricula();
                break;
            case 3:
                buscarAlunoPorMatricula();
                break;
            case 4:
                buscarAlunosPorCurso();
                break;
            case 5:
                listarTodosAlunos();
                break;
            case 6: {
                std::string arquivo;
                std::cout << "Nome do arquivo CSV: ";
                std::cin >> arquivo;
                carregarDeCSV(arquivo);
                break;
            }
            case 0:
                std::cout << "Encerrando sistema..." << std::endl;
                break;
            default:
                std::cout << "Opção inválida!" << std::endl;
        }
        
    } while (opcao != 0);
}
