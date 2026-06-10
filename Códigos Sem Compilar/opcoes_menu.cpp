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

static int contar_itens_ativos() {
    int c = 0;
    for (int i = 0; i < N; i++) {
        if (itens[i].ativo)
            c++;
    }
    return c;
}

bool existe_itens_ativos() {
    return contar_itens_ativos() > 0;
}

string detalhes_item_gui(int idItem) {
    if (idItem < 0 || idItem >= N || !itens[idItem].ativo)
        return "Item invalido.";

    string texto = "Nome: ";
    texto += itens[idItem].nome;
    texto += "\nDono: ";
    texto += itens[idItem].dono;
    texto += "\nPropriedade Magica: ";
    texto += itens[idItem].propriedade_magica;
    texto += "\nRaridade: ";
    texto += numero_para_texto(itens[idItem].raridade);
    texto += "\nID: ";
    texto += numero_para_texto(itens[idItem].id);
    texto += "\n\nCoordenadas do contorno:\n";

    if (itens[idItem].num_pontos == 0) {
        texto += "(nenhum ponto cadastrado)";
    } else {
        for (int i = 0; i < itens[idItem].num_pontos; i++) {
            texto += "Ponto ";
            texto += numero_para_texto(i + 1);
            texto += ": (";
            texto += numero_para_texto(itens[idItem].contorno[i].x);
            texto += ", ";
            texto += numero_para_texto(itens[idItem].contorno[i].y);
            texto += ")\n";
        }
    }

    return texto;
}

static void listar_inordem_detalhes(treenodeptr p, string &s, bool usado[]) {
    if (p != NULL) {
        listar_inordem_detalhes(p->left, s, usado);

        for (int i = 0; i < N; i++) {
            if (!usado[i] && itens[i].ativo && itens[i].nome == p->info) {
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

static string nome_base(const string &nome) {
    if (nome.size() > 3 && nome[0] == '(') {
        size_t fim = nome.find(") ");
        if (fim != string::npos)
            return nome.substr(fim + 2);
    }
    return nome;
}

static int contar_itens_mesmo_nome(const string &nomeBase) {
    int c = 0;
    for (int i = 0; i < N; i++) {
        if (itens[i].ativo && nome_base(itens[i].nome) == nomeBase)
            c++;
    }
    return c;
}

static string nome_com_duplicata(const string &nome) {
    string base = nome_base(nome);
    int iguais = contar_itens_mesmo_nome(base);

    if (iguais == 0)
        return base;

    string resultado = "(";
    resultado += numero_para_texto(iguais + 1);
    resultado += ") ";
    resultado += base;
    return resultado;
}

int inserir_item_gui(treenodeptr &root, const string &nome, const string &dono, const string &prop, int raridade, Ponto pontos[], int qtd) {
    if (id >= 1000)
        return 0;

    if (qtd < 3)
        return -1;

    if (!poligono_convexo(pontos, qtd))
        return -2;

    string nomeFinal = nome_com_duplicata(nome);

    tInsert(root, nomeFinal);
    itens[id].id = id;
    itens[id].nome = nomeFinal;
    itens[id].dono = dono;
    itens[id].propriedade_magica = prop;
    itens[id].raridade = raridade;
    itens[id].num_pontos = qtd;
    itens[id].ativo = true;

    for (int i = 0; i < qtd; i++)
        itens[id].contorno[i] = pontos[i];

    id++;
    N++;
    return 1;
}

bool cadastrar_similaridade_gui(int id1, int id2, int peso) {
    if (id1 == id2)
        return false;

    if ((id1 >= 0 && id1 < N) && (id2 >= 0 && id2 < N) && itens[id1].ativo && itens[id2].ativo) {
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

    if (C < 0 || C >= N || !itens[C].ativo)
        return "Item base invalido ou removido.";

    list<Aresta>::iterator it;
    for (it = grafo[C].begin(); it != grafo[C].end(); it++) {
        int id_similar = it->id2;

        if (it->peso >= x && itens[id_similar].ativo) {
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
    int ativos = contar_itens_ativos();

    if (root == NULL || ativos == 0)
        return "Nenhum item cadastrado.";

    string texto = "Itens cadastrados: ";
    texto += numero_para_texto(ativos);
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

    int qtd;

    cout << "Quantidade de pontos do contorno: ";
    cin >> qtd;

    while(qtd < 3){
        cout << "Poligonos precisam de pelo menos 3 pontos: ";
        cin >> qtd;
    }

    Ponto pontos[100];

    for(int i=0;i<qtd;i++){
        cout << "Ponto " << i+1 << ": ";
        cin >> pontos[i].x >> pontos[i].y;
    }

    if(!poligono_convexo(pontos, qtd)){
        cout << endl;
        cout << "Item rejeitado." << endl;
        cout << "O contorno informado nao forma um poligono convexo." << endl;
        cout << endl;
        return;
    }

    tInsert(root, nome_item);

    itens[id].id = id;
    itens[id].nome = nome_item;
    itens[id].dono = dono;
    itens[id].propriedade_magica = propriedade_magica;
    itens[id].raridade = raridade;
    itens[id].num_pontos = qtd;
    itens[id].ativo = true;

    for(int i=0;i<qtd;i++){
        itens[id].contorno[i] = pontos[i];
    }

    cout << endl;
    cout << nome_item << " foi inserido(a) na Bolsa Devoradora, seu id sera: " << id << endl;
    cout << endl;
    id++;
    N++;
}

void cadastrar_similaridade(){
    int peso;
    int id1, id2;
    string op;

    cout << "Insira os ids dos item a serem cadastrados com similaridade: " << endl;
    cin >> id1 >> id2;
    if(id1 == id2){
        cout << "Os dois itens devem ser diferentes." << endl;
    }
    else if((id1 < N && id1 >= 0) && (id2 < N && id2 >= 0)){
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

void trocar(Item &a, Item &b){
    Item aux = a;
    a = b;
    b = aux;
}

string listar_ordem_raridade_gui() {
    int ativos = contar_itens_ativos();

    if (N == 0 || ativos == 0)
        return "Nenhum item cadastrado.";

    Item aux[1000];
    int qtd = 0;

    for (int i = 0; i < N; i++) {
        if (itens[i].ativo) {
            aux[qtd] = itens[i];
            qtd++;
        }
    }

    for (int i = 0; i < qtd - 1; i++) {
        for (int j = i + 1; j < qtd; j++) {
            if (aux[j].raridade > aux[i].raridade)
                trocar(aux[i], aux[j]);
        }
    }

    string texto = "Itens por raridade:\n\n";

    for (int i = 0; i < qtd; i++) {
        texto += aux[i].nome;
        texto += " - raridade: ";
        texto += numero_para_texto(aux[i].raridade);
        texto += "\n";
    }

    return texto;
}

string contar_propriedade_gui(const string &prop) {
    int contador = 0;

    for (int i = 0; i < N; i++) {
        if (itens[i].ativo && itens[i].propriedade_magica == prop)
            contador++;
    }

    string texto = "Quantidade encontrada: ";
    texto += numero_para_texto(contador);
    return texto;
}

void listar_ordem_raridade(){
    cout << listar_ordem_raridade_gui() << endl;
}

void contar_propriedade(){
    string prop;

    cout << "Digite a propriedade magica: ";
    cin >> prop;

    cout << contar_propriedade_gui(prop) << endl;
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

string remover_menos_raros_gui(treenodeptr &root, int R) {
    int removidos = 0;

    for (int i = 0; i < N; i++) {
        if (itens[i].ativo && itens[i].raridade < R) {
            remover_nome(root, itens[i].nome);
            itens[i].ativo = false;
            removidos++;
        }
    }

    string texto = numero_para_texto(removidos);
    texto += " itens removidos.";
    return texto;
}

void remover_menos_raros(treenodeptr &root)
{
    int R;

    cout << "Valor minimo de raridade: ";
    cin >> R;

    cout << remover_menos_raros_gui(root, R) << endl;
}

double area_triangulo(Ponto a, Ponto b, Ponto c){
    return (
        a.x*b.y - a.y*b.x +
        a.y*c.x - a.x*c.y +
        b.x*c.y - b.y*c.x
    ) / 2.0;
}

int orientacao(Ponto p1, Ponto p2, Ponto p3){

    double area = area_triangulo(p1,p2,p3);

    if(area == 0)
        return 0;

    if(area < 0)
        return 1;

    return 2;
}

bool poligono_convexo(Ponto p[], int n){

    if(n < 3)
        return false;

    int primeira = 0;

    for(int i=0;i<n;i++){

        int o = orientacao(
            p[i],
            p[(i+1)%n],
            p[(i+2)%n]
        );

        if(o != 0){
            if(primeira == 0)
                primeira = o;
            else if(o != primeira)
                return false;
        }
    }

    return true;
}
