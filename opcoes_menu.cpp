#include <iostream>
#include <string>
#include <list>
#include "opcoes_menu.h" 

using namespace std;
typedef treenode* treenodeptr;

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

//-----------------------ARVORE--------------------------
void tInsert(treenodeptr &p, string x) {
    if (p == NULL) {
        p = new treenode;
        p->info = x;
        p->left = NULL;
        p->right = NULL;
    }
    else if (x < p->info) {
        tInsert(p->left, x);
    }
    else {
        tInsert(p->right, x);
    }
}

bool buscar_nome(treenodeptr p, string x) {
    if (p == NULL) return false;

    if (x == p->info) return true;

    if (x < p->info)
        return buscar_nome(p->left, x);
    else
        return buscar_nome(p->right, x);
}

void listar_inordem(treenodeptr p) {
    if (p != NULL) {
        listar_inordem(p->left);
        cout << p->info << endl;
        listar_inordem(p->right);
    }
}

// ---------------------------------------------------------

void inserir_item(treenodeptr &root){
	
	string nome_item, dono, propriedade_magica;
	int raridade;
	
    cout << "-INSIRA OS ITENS-" << endl;
    cout << endl <<  "nome do item: " << endl;
    getline (cin >> ws, nome_item);
    
   tInsert(root, nome_item);
    
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

void verificar_existencia(treenodeptr root) {
    string nome;

    cout << "Digite o nome do item: ";
    getline(cin >> ws, nome);

    if (buscar_nome(root, nome)) {
        cout << "Item encontrado!\n";
    } else {
        cout << "Item nao encontrado!\n";
    }
}

void listar_ordem_alfabetica(treenodeptr root) {
    if (root == NULL) {
        cout << "Arvore vazia!\n";
        return;
    }

    cout << "\nItens em ordem alfabetica:\n";
    listar_inordem(root);
}

void trocar(Item &a, Item &b){
    Item aux = a;
    a = b;
    b = aux;
}

void listar_ordem_raridade(){

    if(N == 0){
        cout << "Nenhum item cadastrado." << endl;
        return;
    }

    Item aux[1000];

    for(int i=0;i<N;i++)
        aux[i] = itens[i];

    for(int i=0;i<N-1;i++){
        for(int j=i+1;j<N;j++){

            if(aux[j].raridade > aux[i].raridade){
                trocar(aux[i], aux[j]);
            }

        }
    }

    cout << "\nItens por raridade:\n";

    for(int i=0;i<N;i++){
        cout
        << aux[i].nome
        << " - raridade: "
        << aux[i].raridade
        << endl;
    }
}

void contar_propriedade(){

    string prop;

    cout << "Digite a propriedade magica: ";
    cin >> prop;

    int contador = 0;

    for(int i=0;i<N;i++){

        if(itens[i].propriedade_magica == prop){
            contador++;
        }

    }

    cout << "Quantidade encontrada: "
         << contador
         << endl;
}


treenodeptr menor(treenodeptr p){
    while(p->left != NULL)
        p = p->left;

    return p;
}

void remover_nome(treenodeptr &p, string nome){
    
    if(p == NULL) return;

    if(nome < p->info)
        remover_nome(p->left, nome);

    else if(nome > p->info)
        remover_nome(p->right, nome);

    else{

        if(p->left == NULL){

            treenodeptr aux = p;
            p = p->right;
            delete aux;
        }

        else if(p->right == NULL){

            treenodeptr aux = p;
            p = p->left;
            delete aux;
        }

        else{

            treenodeptr aux = menor(p->right);

            p->info = aux->info;

            remover_nome(p->right, aux->info);
        }
    }
}

void remover_item_indice(int pos){
    
    for(int i=pos;i<N-1;i++){
        itens[i] = itens[i+1];
    }

    N--;
}

void remover_menos_raros(treenodeptr &root){

    int R;

    cout << "Valor minimo de raridade: ";
    cin >> R;

    int removidos = 0;

    for(int i=0;i<N;){

        if(itens[i].raridade < R){

            string nome_removido = itens[i].nome;

            remover_nome(root, nome_removido);

            remover_item_indice(i);

            removidos++;
        }
        else{
            i++;
        }
    }

    cout << removidos
         << " itens removidos."
         << endl;
}

