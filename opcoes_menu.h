#ifndef OPCOES_MENU_H
#define OPCOES_MENU_H
#include <string>
using namespace std;

struct item
{
	string nome;
	string dono;
	string propriedade_magica;
	int raridade;
};

void mostrar_opcoes();
void inserir_item(item itens[]);
void cadastrar_similaridade();
void buscar_similares();
void verificar_existencia();
void listar_alfabetica();
void listar_raridade();
void contar_propriedade();
void remover_menos_raros();


#endif

