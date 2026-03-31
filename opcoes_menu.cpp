#include <iostream>
#include <string>
#include <list>
#include "opcoes_menu.h" 

using namespace std;

void mostrar_opcoes(){
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
list<Aresta> grafo[1000];

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
    	cout << nome_item << " inserido(a) na Bolsa Devoradora" << endl;
    	cout << endl;
    	id++;
    	N++;
	}
}

void cadastrar_similaridade(){
	int peso;
	int id1, id2; //papel de  origem, destino
	string op;
		
	cout << "Insira os ids dos item a serem cadastrados com similaridade: " << endl;
	cin >> id1 >> id2;
	if((id1 < N && id1 >= 0) && (id2 < N && id2 >= 0)){
		cout << "Itens a serem cadastrados: " << id1 << " -> " << itens[id1].nome << ", " << id2 << " -> " << itens[id2].nome << endl;
		cout << "Tem certeza que deseja cadastrar esses itens? [y/n]" << endl;
		cin >> op;
		if(op == "y" || op == "Y"){
			cout << "Insira o valor da similaridade entre eles: " << endl;
			cin >> peso;
	
			grafo[id1].push_back({id1, id2, peso});
			grafo[id2].push_back({id2, id1, peso});
		}
		else{
			cout << "Se quiser realizar o cadastro entre dois itens aperte 2" << endl;
		}
	}
	else if((id1 > N || id1 < 0) || (id2 > N || id2 < 0)){
		cout << "Id invalido, voce deve inserir o valor para o id de 0 ate " << N - 1 << endl;
	}
}

void buscar_similares(){
    int C;      // id do item base
    int x;      // similaridade minima
    string j;   // jogador ignorado

    cout << "Digite o id do item base: ";
    cin >> C;

    cout << "Digite o valor minimo de similaridade: ";
    cin >> x;

    cout << "Digite o nome do jogador que deve ser ignorado: ";
    cin >> j;

    list<Aresta>::iterator it;
	bool encontrou = false;
	
    for(it = grafo[C].begin(); it != grafo[C].end(); it++){
        int id_similar = it->id2;

        if(it->peso > x){
            if(itens[id_similar].dono != j){
                cout << itens[id_similar].nome << ", ";
                
                encontrou = true;
            }
        }
    }

    if(encontrou == false){
        cout << "Nenhum item encontrado com esses criterios." << endl;
    }
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