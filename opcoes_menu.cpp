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

static string numero_para_texto(int n) {
    if (n == 0)
        return "0";

    string s = "";
    while (n > 0) {
        s = (char)('0' + n % 10) + s;
        n /= 10;
    }
    return s;
}

static void listar_inordem_detalhes(treenodeptr p, string &s, bool usado[]) {
    if (p != NULL) {
        listar_inordem_detalhes(p->left, s, usado);

        for (int i = 0; i < N; i++) {
            if (!usado[i] && itens[i].nome == p->info) {
                usado[i] = true;
                s += itens[i].nome;
                s += "\n";
                s += " - Dono: ";
                s += itens[i].dono;
                s += "\n";
                s += " - Propriedade Magica: ";
                s += itens[i].propriedade_magica;
                s += "\n";
                s += " - Raridade: ";
                s += numero_para_texto(itens[i].raridade);
                s += "\n\n";
                break;
            }
        }

        listar_inordem_detalhes(p->right, s, usado);
    }
}

bool inserir_item_gui(treenodeptr &root, const string &nome, const string &dono, const string &prop, int raridade) {
    if (id >= 1000)
        return false;

    tInsert(root, nome);
    itens[id].id = id;
    itens[id].nome = nome;
    itens[id].dono = dono;
    itens[id].propriedade_magica = prop;
    itens[id].raridade = raridade;
    id++;
    N++;
    return true;
}

bool cadastrar_similaridade_gui(int id1, int id2, int peso) {
    if ((id1 >= 0 && id1 < N) && (id2 >= 0 && id2 < N)) {
        Aresta a1, a2;
        a1.id1 = id1;
        a1.id2 = id2;
        a1.peso = peso;
        a2.id1 = id2;
        a2.id2 = id1;
        a2.peso = peso;
        grafo[id1].push_back(a1);
        grafo[id2].push_back(a2);
        return true;
    }
    return false;
}

string buscar_similares_gui(int C, int x, const string &j) {
    string texto = "";
    bool encontrou = false;

    if (C < 0 || C >= N)
        return "Id de item base invalido.";

    list<Aresta>::iterator it;
    for (it = grafo[C].begin(); it != grafo[C].end(); it++) {
        int id_similar = it->id2;

        if (it->peso > x) {
            if (itens[id_similar].dono != j) {
                if (encontrou)
                    texto += "\n";
                texto += itens[id_similar].nome;
                encontrou = true;
            }
        }
    }

    if (!encontrou)
        texto = "Nenhum item encontrado com esses criterios.";

    return texto;
}

bool verificar_existencia_gui(treenodeptr root, const string &nome) {
    return buscar_nome(root, nome);
}

string listar_ordem_alfabetica_gui(treenodeptr root) {
    if (root == NULL || N == 0)
        return "Nenhum item cadastrado.";

    string texto = "Itens cadastrados: ";
    texto += numero_para_texto(N);
    texto += "\n\nItens em ordem alfabetica:\n\n";

    bool usado[1000];
    for (int i = 0; i < 1000; i++)
        usado[i] = false;

    listar_inordem_detalhes(root, texto, usado);
    return texto;
}

void inserir_item(treenodeptr &root){

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
            cin.clear();
            cin.ignore(1000, '\n');
        }
        else if(raridade < 0 || raridade > 100){
            cout << endl;
            cout << "O nivel de raridade deve ser no minimo 0 e no maximo 100!" << endl;
            cout << "Digite o valor correto de raridade: ";
            cout << endl;
        }
        else break;
    }

    inserir_item_gui(root, nome_item, dono, propriedade_magica, raridade);
    cout << endl;
    cout << nome_item << " foi inserido(a) na Bolsa Devoradora, seu id sera: " << (id - 1) << endl;
    cout << endl;
}

void cadastrar_similaridade(){
    int peso;
    int id1, id2;
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

            cadastrar_similaridade_gui(id1, id2, peso);
        }
        else{
            cout << "Se quiser realizar o cadastro entre dois itens aperte 2" << endl;
        }
    }
    else if((id1 >= N || id1 < 0) || (id2 >= N || id2 < 0)){
        cout << "Id invalido, voce deve inserir o valor para o id de 0 ate " << N - 1 << endl;
    }
}

void buscar_similares(){
    int C;
    int x;
    string j;

    cout << "Digite o id do item base: ";
    cin >> C;

    cout << "Digite o valor minimo de similaridade: ";
    cin >> x;

    cout << "Digite o nome do jogador que deve ser ignorado: ";
    cin >> j;

    cout << buscar_similares_gui(C, x, j) << endl;
}

void verificar_existencia(treenodeptr root) {
    string nome;

    cout << "Digite o nome do item: ";
    getline(cin >> ws, nome);

    if (verificar_existencia_gui(root, nome)) {
        cout << "Item encontrado!\n";
    } else {
        cout << "Item nao encontrado!\n";
    }
}

void listar_ordem_alfabetica(treenodeptr root) {
    cout << listar_ordem_alfabetica_gui(root) << endl;
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
