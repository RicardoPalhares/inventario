#ifndef OPCOES_MENU_H
#define OPCOES_MENU_H

#include <string>
#include <list>

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

struct treenode {
    string info;
    treenode *left;
    treenode *right;
};
typedef treenode* treenodeptr;

// Variaveis globais
extern int id;
extern int N;
extern Item itens[1000];
extern list<Aresta> grafo[1000];

// Funcoes
void mostrar_opcoes();
void tInsert(treenodeptr &p, string x);
void inserir_item(treenodeptr &root);
void cadastrar_similaridade();
void buscar_similares();
bool buscar_nome(treenodeptr p, string x);
void verificar_existencia(treenodeptr root);
void listar_inordem(treenodeptr p);
void listar_ordem_alfabetica(treenodeptr root);
void listar_ordem_raridade();
void contar_propriedade();
void remover_menos_raros();

#endif