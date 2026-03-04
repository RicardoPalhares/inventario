#include <iostream>
#include "opcoes_menu.h" 

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
        cout << "9. Sair do Inventario" << endl;
        cout << "Escolha uma opcao: " << endl;
}

void inserir_item(item i){
	cout << endl;
    cout << "Insira os dados:" << endl;
    cout << "Nome do item: ";
    getline(cin >> ws, item.nome);
    cout << endl;
    cout << "Nome do dono: ";
    getline(cin >> ws, item.dono);
    cout << endl;
    cout << "Propriedade magica: ";
    getline(cin >> ws, item.propriedade_magica);
    cout << endl;
    cout << "Raridade: ";
    cin >> item.raridade;
    cout << endl;
}

void cadastrar_similaridade(){
	cout << endl;
    cout << "Funcionalidade em construcao..." << endl;
    cout << endl;
}

void buscar_similares(){
	cout << endl;
    cout << "Funcionalidade em construcao..." << endl;
    cout << endl;
}

void verificar_existencia(){
	cout << endl;
    cout << "Funcionalidade em construcao..." << endl;
    cout << endl;
}

void listar_alfabetica(){
	cout << endl;
    cout << "Funcionalidade em construcao..." << endl;
    cout << endl;
}

void listar_raridade(){
	cout << endl;
    cout << "Funcionalidade em construcao..." << endl;
    cout << endl;
}

void contar_propriedade(){
	cout << endl;
    cout << "Funcionalidade em construcao..." << endl;
    cout << endl;
}

void remover_menos_raros(){
	cout << endl;
    cout << "Funcionalidade em construcao..." << endl;
    cout << endl;

}
