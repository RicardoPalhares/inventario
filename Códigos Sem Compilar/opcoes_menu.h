#ifndef OPCOES_MENU_H
#define OPCOES_MENU_H

#include <string>
#include <list>

using namespace std;

struct Ponto {
    int x;
    int y;
};

struct Item {
    int id;
    string nome;
    string dono;
    string propriedade_magica;
    int raridade;
    Ponto contorno[100];
    int num_pontos;
    bool ativo;
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

extern int id;
extern int N;
extern Item itens[1000];
extern list<Aresta> grafo[1000];

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
void remover_menos_raros(treenodeptr &root);
void remover_nome(treenodeptr &p, string nome);
treenodeptr menor(treenodeptr p);
double area_triangulo(Ponto a, Ponto b, Ponto c);
int orientacao(Ponto p1, Ponto p2, Ponto p3);
bool poligono_convexo(Ponto p[], int n);

int inserir_item_gui(treenodeptr &root, const string &nome, const string &dono, const string &prop, int raridade, Ponto pontos[], int qtd);
bool cadastrar_similaridade_gui(int id1, int id2, int peso);
int id_item_pelo_nome(const string &nome);
string buscar_similares_gui(int C, int x, const string &j);
bool verificar_existencia_gui(treenodeptr root, const string &nome);
string listar_ordem_alfabetica_gui(treenodeptr root);
string listar_ordem_raridade_gui();
string contar_propriedade_gui(const string &prop);
string remover_menos_raros_gui(treenodeptr &root, int R);
bool existe_itens_ativos();
string detalhes_item_gui(int idItem);

#endif
