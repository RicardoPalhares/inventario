#include <iostream>
#include <string>
#include "opcoes_menu.h" 
#include <list>

using namespace std;

void mostrar_opcoes(){
		cout << "--- Bem vindo(a) ao inventario: Bolsa Devoradora ---" << endl;
        cout << "1. Inserir item" << endl;
        cout << "2. Cadastrar similaridade entre itens" << endl;
        cout << "3. Buscar itens similares" << endl;
        cout << "4. Verificar a existencia de um item" << endl;
        cout << "5. Listar itens (ordem alfabetica de nome)" << endl;
        cout << "6. Listar itens (ordem decrescente de raridade)" << endl;
        cout << "7. Contar itens com mesma propriedade magica" << endl;
        cout << "8. Remover itens menos raros" << endl;
        cout << "9. Sair" << endl;
        cout << "Escolha uma opcao: " << endl;
}

Item itens[1000];
int id = 0;
int N = 0;
void inserir_item(){
	string nome_item, dono, propriedade_magica;
	int raridade;
    cout << "Insira os dados dos itens na ordem: nome do item, dono, propriedade magica, raridade" << endl;
    cin >> nome_item >> dono >> propriedade_magica >> raridade;
    
    if(raridade < 0 || raridade > 100){
		while(raridade < 0 || raridade > 100){ // validacao do nivel de raridade
			cout << "O nivel de raridade deve ser no minimo 0 e no maximo 100!" << endl;
			cout << "Digite o valor correto de raridade: ";
			cin >> raridade;
		}
	}
	else{
		itens[id] = {id, nome_item, dono, propriedade_magica, raridade};
    	cout << nome_item << " inserido a Bolsa Devoradora" << endl;
    	id++;
    	N++;
	}
}

void cadastrar_similaridade(){
    int vertices, peso;
	int id1, id2; //papel de  origem, destino
	
	vertices = N;

	
	list<Aresta> grafo[vertices];
	

	cout << "Inisra os ids dos item a serem cadastrados com similaridade: " << endl;
	cin >> id1 >> id2;
		
	cout << "Insira o valor da similaridade: " << endl;
	cin >> peso;
	


	grafo[id1].push_back({id1, id2, peso});
	grafo[id2].push_back({id2, id1, peso});
}


void buscar_similares(){
    cout << "Funcionalidade em construcao..." << endl;
}

void verificar_existencia(){
    cout << "Funcionalidade em construcao..." << endl;
}

void listar_ordem_alfabetica(){
	// apenas teste para verificar o autoincremento do id nos itens
    for(int i = 0; i < N; i++){
		cout << itens[i].id << ", " << itens[i].nome << ", " << itens[i].dono << ", " << itens[i].propriedade_magica << ", " << itens[i].raridade << endl;
	}
}

void listar_ordem_raridade(){
    cout << "Funcionalidade em construcao..." << endl;
}

void contar_propriedade(){
    cout << "Funcionalidade em construcao..." << endl;
}

void remover_menos_raros(){
    cout << "Funcionalidade em construcao..." << endl;
}