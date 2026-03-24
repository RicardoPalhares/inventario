#ifndef OPCOES_MENU_H
#define OPCOES_MENU_H
#include <string>

using namespace std;

struct Item {
    int id;
	string nome;
    string dono;
    string propriedade_magica;
    int raridade;
};

struct Aresta {
    int id1;
    int id2;
    int peso;
};

// Variaveis Gloabais para comunicacao entre os arquivos
extern int id;      
extern int N;
extern Item itens[1000];

void mostrar_opcoes();
void inserir_item();
void cadastrar_similaridade();
void buscar_similares();
void verificar_existencia();
void listar_ordem_alfabetica();
void listar_ordem_raridade();
void contar_propriedade();
void remover_menos_raros();



#endif