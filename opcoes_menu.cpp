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
	
    cout << "-INSIRA OS ITENS-" << endl;
    cout << endl <<  "nome do item: " << endl;
    getline (cin >> ws, nome_item);
    cout << endl << "dono: " << endl;
    cin >> dono;
    cout << endl <<  "propriedade magica" << endl;
    cin >> propriedade_magica;
    cout << endl <<  "raridade [0 a 100]" << endl;
    while(true)
	{
		if (!(cin >> raridade)) 
		{
    		cout << endl;
            cout << "Por favor, digite um numero!" << endl;
            cout << "Digite o valor correto de raridade: ";
            cout << endl;
            cin.clear(); // Limpa o estado de erro do cin
            cin.ignore(1000, '\n'); // Descarta a entrada inv?lida
		}
		else if(raridade < 0 || raridade > 100){
			cout << endl;
			cout << "O nivel de raridade deve ser no minimo 0 e no maximo 100!" << endl;
			cout << "Digite o valor correto de raridade: ";
			cout << endl;
		}
		else break;
	}

	itens[id] = {id, nome_item, dono, propriedade_magica, raridade};
	cout << endl;
	cout << nome_item << " foi inserido(a) na Bolsa Devoradora, seu id sera: " << id << endl;
 	cout << endl;
 	id++;
 	N++;
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
        cout << "Nenhum item encontrado com esses criterios.";
    }

    cout << endl;
}

void verificar_existencia(){
    cout << "Funcionalidade em construcao..." << endl;
}

void listar_ordem_alfabetica(){
	// apenas teste para verificar o autoincremento do id nos itens
    cout << endl << "-- ITEMS DENTRO DO INVENTARIO --" << endl; 
	for(int i = 0; i < N; i++){
	 	cout << endl <<  "| ID: " << itens[i].id << endl;
        cout << "| Nome: " << itens[i].nome << endl;
        cout << "| Dono: " << itens[i].dono << endl;
        cout << "| Propriedade: " << itens[i].propriedade_magica << endl;
        cout << "| Raridade: " << itens[i].raridade << endl;
        cout << endl;
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