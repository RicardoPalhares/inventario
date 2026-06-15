//Integrantes: Leonardo Lopes Prado - 809
// Gabriel Gregùrio de Olveira - 2324
// Isaac Assis Costa Lopes - 2314 
// Ricardo de Oliveira Palhares - 2294
// Gabriel Genghini Craveiro - 2274
// Marcus Vinicius Duarte - 2296

/*
 * main.cpp - Interface grafica do inventario (Windows API)
 *
 * Este arquivo cuida das janelas, botoes e campos de texto.
 * A logica dos itens (arvore, grafo, validacoes) fica em opcoes_menu.cpp.
 */

/* ====================== */
/* BIBLIOTECAS            */
/* ====================== */
/* windows.h  - API do Windows: cria janelas, botoes, caixas de texto, mensagens */
/* commdlg.h  - Dialogo padrao para escolher arquivo (.txt) no botao "+" */
/* stdio.h    - Leitura de arquivo com fopen/fread (importacao em lote) */
/* string     - Texto dinamico (std::string) para montar linhas e mensagens */
/* opcoes_menu.h - Declaracoes das funcoes e estruturas do inventario (Item, Ponto, arvore) */

#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <string>
#include "opcoes_menu.h"

using namespace std;

/* ====================== */
/* PROTOTIPOS DAS JANELAS */
/* ====================== */
/* Cada janela do programa tem uma funcao "Proc" que recebe os eventos
   (clique, criacao, fechamento). O Windows chama essas funcoes automaticamente. */

LRESULT CALLBACK MenuProc(HWND,UINT,WPARAM,LPARAM);         /* tela inicial com os botoes do menu */
LRESULT CALLBACK InserirProc(HWND,UINT,WPARAM,LPARAM);      /* formulario de novo item */
LRESULT CALLBACK SimilaridadeProc(HWND,UINT,WPARAM,LPARAM); /* ligar dois itens com um peso */
LRESULT CALLBACK BuscarProc(HWND,UINT,WPARAM,LPARAM);     /* buscar itens parecidos */
LRESULT CALLBACK VerificarProc(HWND,UINT,WPARAM,LPARAM);  /* checar se um nome existe */
LRESULT CALLBACK ContarProc(HWND,UINT,WPARAM,LPARAM);       /* contar itens por propriedade */
LRESULT CALLBACK RemoverProc(HWND,UINT,WPARAM,LPARAM);      /* remover itens pouco raros */
LRESULT CALLBACK ListarProc(HWND,UINT,WPARAM,LPARAM);       /* listar itens com ordenacao */

/* ====================== */
/* CONSTANTES DA INTERFACE*/
/* ====================== */

#define ESTILO_JANELA (WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_VISIBLE) /* janela simples, sem redimensionar */
#define ID_LISTA_ITENS 902        /* identificador da listbox na tela de listar */
#define ID_BTN_DETALHES 903       /* botao "Ver detalhes" */
#define ID_BTN_FECHAR_LISTA 904   /* botao "Fechar" da listagem */
#define ID_COMBO_ORDEM 905        /* combo "Alfabetico" / "Raridade" */

/* ====================== */
/* VARIAVEIS GLOBAIS      */
/* ====================== */

/* Estrutura principal do inventario: arvore binaria de busca pelos nomes */
treenodeptr root = NULL;

/* Referencia da janela principal (menu) para restaurar foco ao fechar sub-janelas */
HWND janelaPrincipal = NULL;

/* Controle da listagem de itens (compartilhado pela janela Listar) */
HWND listaItens;

/* Handles dos campos de texto/combo de cada tela.
   HWND = "ponteiro" para um controle visual criado com CreateWindow. */
HWND campoNome;
HWND campoDono;
HWND campoProp;
HWND campoRaridade;
HWND campoId1;
HWND campoId2;
HWND campoPeso;
HWND campoBase;
HWND campoMin;
HWND campoIgnorar;
HWND campoVerificarNome;
HWND campoPropriedade;
HWND campoRaridadeMin;
HWND campoQtdPontos;
HWND campoPontoX;
HWND campoPontoY;
HWND labelPontos;

/* Pontos do contorno enquanto o usuario vai clicando em "+" na tela Inserir */
Ponto pontosInserir[100];
int pontosInseridos = 0;

/* ====================== */
/* FUNCOES AUXILIARES     */
/* ====================== */

/* Verifica se o texto do campo tem apenas espacos ou esta vazio */
bool campoVazio(const char* s)
{
    for(int i=0; s[i]; i++)
    {
        if(s[i]!=' ' && s[i]!='\t')
            return false;
    }
    return true;
}

/* Converte o indice selecionado no combo para o id real do item no vetor itens[] */
int id_item_ativo_pelo_combo(int sel)
{
    if(sel < 0)
        return -1;

    int pos = 0;
    for(int i=0; i<N; i++)
    {
        if(itens[i].ativo)
        {
            if(pos == sel)
                return i;
            pos++;
        }
    }
    return -1;
}

/* Le o id gravado no combo (funciona mesmo quando a lista exclui um item) */
int id_item_pelo_combo(HWND combo, int sel)
{
    if(sel < 0 || !combo)
        return -1;

    return (int)SendMessage(combo, CB_GETITEMDATA, sel, 0);
}

/* Preenche um combo com itens ativos, opcionalmente excluindo um id (evita similaridade consigo mesmo) */
void preencher_combo_itens(HWND combo, int idExcluir)
{
    SendMessage(combo, CB_RESETCONTENT, 0, 0);

    for(int i=0; i<N; i++)
    {
        if(itens[i].ativo && i != idExcluir)
        {
            int idx = SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)itens[i].nome.c_str());
            SendMessage(combo, CB_SETITEMDATA, idx, i);
        }
    }
}

/* Le uma linha "X Y" e separa as duas coordenadas (usado na importacao de arquivo) */
bool ler_ponto(const string &linha, int &x, int &y)
{
    size_t pos = linha.find(' ');
    if(pos == string::npos)
        return false;

    x = atoi(linha.substr(0, pos).c_str());
    y = atoi(linha.substr(pos + 1).c_str());
    return true;
}

/* Le varias linhas "x y" de um bloco de texto e monta o array de pontos do contorno */
bool ler_contorno_texto(const char* texto, int qtd, Ponto pontos[])
{
    string s = texto;
    s += '\n';
    string linha = "";
    int lidos = 0;

    for(int i=0; i<(int)s.size(); i++)
    {
        if(s[i]=='\n')
        {
            if(linha != "")
            {
                if(lidos >= qtd)
                    return false;

                if(!ler_ponto(linha, pontos[lidos].x, pontos[lidos].y))
                    return false;

                lidos++;
                linha = "";
            }
        }
        else if(s[i] != '\r')
        {
            linha += s[i];
        }
    }

    return lidos == qtd;
}

/* Atualiza o texto "Pontos cadastrados: N" na tela de inserir item */
void atualizar_label_pontos()
{
    char buf[50];
    wsprintf(buf,"Pontos cadastrados: %d",pontosInseridos);
    SetWindowText(labelPontos,buf);
}

/* Descobre qual janela abriu a janela atual (para devolver o foco ao menu) */
HWND obter_janela_pai(HWND hwnd)
{
    HWND pai = GetWindow(hwnd, GW_OWNER);
    if(!pai)
        pai = (HWND)GetWindowLongPtr(hwnd, GWLP_HWNDPARENT);
    if(!pai)
        pai = janelaPrincipal;
    return pai;
}

/* Reativa e traz para frente a janela que estava por tras do dialogo */
void restaurar_janela_pai(HWND hwnd)
{
    HWND pai = obter_janela_pai(hwnd);

    if(!pai || !IsWindow(pai))
        return;

    EnableWindow(pai, TRUE);

    if(IsIconic(pai))
        ShowWindow(pai, SW_RESTORE);

    SetWindowPos(pai, HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    SetWindowPos(pai, HWND_NOTOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

    SetForegroundWindow(pai);
}

/* Fecha a sub-janela e restaura o menu principal */
void fechar_janela(HWND hwnd)
{
    restaurar_janela_pai(hwnd);
    DestroyWindow(hwnd);
}

/* Percorre a arvore em ordem e adiciona os nomes na listbox (ordenacao alfabetica) */
void preencher_lista_alfabetica(HWND lista, treenodeptr p, bool usado[])
{
    if(p == NULL)
        return;

    preencher_lista_alfabetica(lista, p->left, usado);

    for(int i=0; i<N; i++)
    {
        if(!usado[i] && itens[i].ativo && itens[i].nome == p->info)
        {
            usado[i] = true;
            int idx = SendMessage(lista, LB_ADDSTRING, 0, (LPARAM)itens[i].nome.c_str());
            SendMessage(lista, LB_SETITEMDATA, idx, i);
            break;
        }
    }

    preencher_lista_alfabetica(lista, p->right, usado);
}

/* Ordena os itens ativos por raridade (maior primeiro) e preenche a listbox */
void preencher_lista_raridade(HWND lista)
{
    Item aux[1000];
    int qtd = 0;
    int i, j;

    for(i=0; i<N; i++)
    {
        if(itens[i].ativo)
        {
            aux[qtd] = itens[i];
            qtd++;
        }
    }

    for(i=0; i<qtd-1; i++)
    {
        for(j=i+1; j<qtd; j++)
        {
            if(aux[j].raridade > aux[i].raridade)
            {
                Item temp = aux[i];
                aux[i] = aux[j];
                aux[j] = temp;
            }
        }
    }

    for(i=0; i<qtd; i++)
    {
        int idx = SendMessage(lista, LB_ADDSTRING, 0, (LPARAM)aux[i].nome.c_str());
        SendMessage(lista, LB_SETITEMDATA, idx, aux[i].id);
    }
}

/* Limpa a listbox e recarrega conforme o modo: 0 = alfabetico, 1 = raridade */
void atualizar_lista_itens(HWND lista, int modo)
{
    SendMessage(lista, LB_RESETCONTENT, 0, 0);

    if(modo == 0)
    {
        bool usado[1000];
        int i;
        for(i=0; i<1000; i++)
            usado[i] = false;
        preencher_lista_alfabetica(lista, root, usado);
    }
    else
    {
        preencher_lista_raridade(lista);
    }
}

/* ====================== */
/* IMPORTACAO DE ARQUIVO  */
/* ====================== */
/* O botao "+" do menu abre um .txt no formato do EXEMPLE.txt.
   Primeiro le os itens; depois, se existir secao "Similaridade", le os pares. */

/* Ignora linhas de titulo/explicacao do arquivo de exemplo */
bool linha_cabecalho(const string& linha)
{
    return linha.find("Inserindo") != string::npos
        || linha.find("Similaridade") != string::npos
        || linha.find("Formato") != string::npos
        || linha.find("botao") != string::npos
        || linha.find("importar") != string::npos
        || linha.find("mesmo arquivo") != string::npos
        || linha.find("Formato:") != string::npos
        || linha.find("Ou em uma linha") != string::npos;
}

/* Verifica se a linha contem somente digitos (peso da similaridade) */
bool linha_apenas_numero(const string& linha)
{
    if(linha.empty())
        return false;

    for(int i=0; i<(int)linha.size(); i++)
    {
        if(linha[i] < '0' || linha[i] > '9')
            return false;
    }

    return true;
}

/* Remove espacos no inicio e no fim de um texto */
string trim_texto(const string& s)
{
    int ini = 0;
    int fim = (int)s.size() - 1;

    while(ini <= fim && (s[ini]==' ' || s[ini]=='\t'))
        ini++;
    while(fim >= ini && (s[fim]==' ' || s[fim]=='\t'))
        fim--;

    if(ini > fim)
        return "";

    return s.substr(ini, fim - ini + 1);
}

/* Divide uma linha "nome1 ; peso ; nome2" em tres partes */
bool ler_similaridade_linha(const string& linha, string& nome1, int& peso, string& nome2)
{
    size_t p1 = linha.find(';');
    if(p1 == string::npos)
        return false;

    size_t p2 = linha.find(';', p1 + 1);
    if(p2 == string::npos)
        return false;

    nome1 = trim_texto(linha.substr(0, p1));
    string pesoTxt = trim_texto(linha.substr(p1 + 1, p2 - p1 - 1));
    nome2 = trim_texto(linha.substr(p2 + 1));

    if(nome1.empty() || nome2.empty() || pesoTxt.empty())
        return false;

    for(int i=0; i<(int)pesoTxt.size(); i++)
    {
        if(pesoTxt[i] < '0' || pesoTxt[i] > '9')
            return false;
    }

    peso = atoi(pesoTxt.c_str());
    return true;
}

/* Carrega o arquivo inteiro para uma string em memoria */
bool ler_arquivo_txt(const char* caminho, string& conteudo)
{
    FILE* arquivo = fopen(caminho, "rb");
    if(!arquivo)
        return false;

    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    fseek(arquivo, 0, SEEK_SET);

    if(tamanho <= 0 || tamanho > 500000)
    {
        fclose(arquivo);
        return false;
    }

    char* buffer = new char[tamanho + 1];
    if(fread(buffer, 1, tamanho, arquivo) != (size_t)tamanho)
    {
        delete[] buffer;
        fclose(arquivo);
        return false;
    }

    buffer[tamanho] = '\0';
    conteudo = buffer;
    delete[] buffer;
    fclose(arquivo);
    return true;
}

/* Abre a janela "Abrir arquivo" do Windows filtrando apenas .txt */
bool escolher_arquivo_txt(HWND hwnd, char* caminho, int tamMax)
{
    char arquivo[MAX_PATH] = "";
    OPENFILENAME ofn;

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = arquivo;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "Arquivos de texto (*.txt)\0*.txt\0Todos (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if(!GetOpenFileName(&ofn))
        return false;

    strncpy(caminho, arquivo, tamMax);
    caminho[tamMax - 1] = '\0';
    return true;
}

/* Le blocos de 4 linhas (nome, dono, prop, raridade) + qtd pontos + coordenadas */
void importar_itens_de_texto(const string& texto, int& importados, int& rejeitados)
{
    string conteudo = texto;
    conteudo += '\n';
    string linha = "";

    string dados[4];
    int fase = 0;
    int qtdPontos = 0;
    Ponto pontos[100];
    int pontosLidos = 0;

    importados = 0;
    rejeitados = 0;

    for(int i=0; i<(int)conteudo.size(); i++)
    {
        if(conteudo[i]=='\n')
        {
            if(linha!="")
            {
                if(linha_cabecalho(linha))
                {
                    linha = "";
                    continue;
                }

                if(fase < 4)
                {
                    dados[fase++] = linha;
                }
                else if(fase == 4)
                {
                    qtdPontos = atoi(linha.c_str());
                    pontosLidos = 0;
                    fase = 5;
                }
                else if(fase == 5)
                {
                    if(pontosLidos < qtdPontos && ler_ponto(linha, pontos[pontosLidos].x, pontos[pontosLidos].y))
                        pontosLidos++;

                    if(pontosLidos == qtdPontos)
                    {
                        int rar = atoi(dados[3].c_str());
                        if(!campoVazio(dados[0].c_str()) && !campoVazio(dados[1].c_str())
                            && !campoVazio(dados[2].c_str()) && rar>=0 && rar<=100 && qtdPontos>=3)
                        {
                            int res = inserir_item_gui(root, dados[0], dados[1], dados[2], rar, pontos, qtdPontos);
                            if(res == 1)
                                importados++;
                            else
                                rejeitados++;
                        }
                        else
                        {
                            rejeitados++;
                        }

                        fase = 0;
                        qtdPontos = 0;
                        pontosLidos = 0;
                    }
                }

                linha = "";
            }
        }
        else if(conteudo[i]!='\r')
        {
            linha += conteudo[i];
        }
    }
}

/* Le similaridades por nome: "item1 ; peso ; item2" ou 3 linhas (item1, peso, item2) */
void importar_similaridades_de_texto(const string& texto, int& importadas)
{
    string conteudo = texto;
    conteudo += '\n';
    string linha = "";
    string nome1 = "";
    string nome2 = "";
    int peso = 0;
    int fase = 0;

    importadas = 0;

    for(int i=0; i<(int)conteudo.size(); i++)
    {
        if(conteudo[i]=='\n')
        {
            linha = trim_texto(linha);

            if(linha != "")
            {
                if(linha_cabecalho(linha))
                {
                    fase = 0;
                    nome1 = "";
                    nome2 = "";
                }
                else if(linha.find(';') != string::npos)
                {
                    if(ler_similaridade_linha(linha, nome1, peso, nome2))
                    {
                        int id1 = id_item_pelo_nome(nome1);
                        int id2 = id_item_pelo_nome(nome2);

                        if(id1 != -1 && id2 != -1 && cadastrar_similaridade_gui(id1, id2, peso))
                            importadas++;
                    }

                    fase = 0;
                    nome1 = "";
                    nome2 = "";
                }
                else if(fase == 0)
                {
                    nome1 = linha;
                    fase = 1;
                }
                else if(fase == 1)
                {
                    if(linha_apenas_numero(linha))
                    {
                        peso = atoi(linha.c_str());
                        fase = 2;
                    }
                    else
                    {
                        fase = 0;
                        nome1 = "";
                        nome2 = "";
                    }
                }
                else if(fase == 2)
                {
                    nome2 = linha;

                    int id1 = id_item_pelo_nome(nome1);
                    int id2 = id_item_pelo_nome(nome2);

                    if(id1 != -1 && id2 != -1 && cadastrar_similaridade_gui(id1, id2, peso))
                        importadas++;

                    fase = 0;
                    nome1 = "";
                    nome2 = "";
                }
            }

            linha = "";
        }
        else if(conteudo[i]!='\r')
        {
            linha += conteudo[i];
        }
    }
}

/* Fluxo completo do botao "+": escolhe arquivo, divide em itens/similaridades e mostra resumo */
void importar_de_arquivo(HWND hwnd)
{
    char caminho[MAX_PATH];

    if(!escolher_arquivo_txt(hwnd, caminho, MAX_PATH))
        return;

    string conteudo;
    if(!ler_arquivo_txt(caminho, conteudo))
    {
        MessageBox(hwnd,"Nao foi possivel abrir o arquivo.","Erro",MB_OK);
        return;
    }

    size_t pos = conteudo.find("Similaridade");
    string parteItens = (pos != string::npos) ? conteudo.substr(0, pos) : conteudo;
    string parteSim = (pos != string::npos) ? conteudo.substr(pos) : "";

    int itensImportados = 0;
    int itensRejeitados = 0;
    int simImportadas = 0;

    importar_itens_de_texto(parteItens, itensImportados, itensRejeitados);

    if(!parteSim.empty())
        importar_similaridades_de_texto(parteSim, simImportadas);

    char msg[200];
    wsprintf(msg,"Itens: %d importados, %d rejeitados\nSimilaridades: %d importadas",
        itensImportados, itensRejeitados, simImportadas);
    MessageBox(hwnd, msg, "Importacao", MB_OK);
}

/* Pega o item selecionado na listbox e exibe nome, dono, raridade, contorno etc. */
void mostrar_detalhes_lista(HWND hwnd, HWND lista)
{
    int sel = SendMessage(lista, LB_GETCURSEL, 0, 0);

    if(sel == LB_ERR)
    {
        MessageBox(hwnd,"Selecione um item da lista.","Aviso",MB_OK);
        return;
    }

    int idItem = SendMessage(lista, LB_GETITEMDATA, sel, 0);
    string detalhes = detalhes_item_gui(idItem);

    MessageBox(hwnd, detalhes.c_str(), "Detalhes do Item", MB_OK);
}

/* ====================== */
/* WINMAIN - PONTO DE     */
/* ENTRADA DO PROGRAMA    */
/* ====================== */
/* WinMain substitui o main() em programas com interface Windows.
   Aqui registramos cada tipo de janela e entramos no loop de mensagens. */

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR,int)
{
    WNDCLASSEX wc; /* estrutura que descreve uma "classe" de janela reutilizavel */

    /* Configuracao padrao compartilhada por todas as janelas */
    wc.cbSize=sizeof(WNDCLASSEX);
    wc.style=0;
    wc.cbClsExtra=0;
    wc.cbWndExtra=0;
    wc.hInstance=hInst;
    wc.hIcon=NULL;
    wc.hIconSm=NULL;
    wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground=GetSysColorBrush(COLOR_BTNFACE);
    wc.lpszMenuName=NULL;


    /* Registra cada tela ligando o nome da classe a sua funcao Proc */
    wc.lpfnWndProc=MenuProc;
    wc.lpszClassName="MenuPrincipal";
    RegisterClassEx(&wc);

    wc.lpfnWndProc=InserirProc;
    wc.lpszClassName="JanelaInserir";
    RegisterClassEx(&wc);
    
    wc.lpfnWndProc=SimilaridadeProc;
	wc.lpszClassName="JanelaSimilaridade";
	RegisterClassEx(&wc);
	
	wc.lpfnWndProc=BuscarProc;
	wc.lpszClassName="JanelaBuscar";
	RegisterClassEx(&wc);
	
	wc.lpfnWndProc=VerificarProc;
	wc.lpszClassName="JanelaVerificar";
	RegisterClassEx(&wc);

	wc.lpfnWndProc=ContarProc;
	wc.lpszClassName="JanelaContar";
	RegisterClassEx(&wc);

	wc.lpfnWndProc=RemoverProc;
	wc.lpszClassName="JanelaRemover";
	RegisterClassEx(&wc);

	wc.lpfnWndProc=ListarProc;
	wc.lpszClassName="JanelaListar";
	RegisterClassEx(&wc);

    /* Cria a janela principal e guarda o handle em janelaPrincipal */
    janelaPrincipal = CreateWindow(
        "MenuPrincipal",
        "Bolsa Devoradora",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        200,100,420,455,
        NULL,NULL,hInst,NULL
    );

    /* Loop infinito: o Windows envia mensagens (cliques, teclas, fechar) ate WM_QUIT */
    MSG msg;

    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}



/* ====================== */
/* MENU PRINCIPAL         */
/* ====================== */
/* Tela inicial com os botoes de cada operacao do inventario.
   WM_CREATE  = monta os botoes na abertura
   WM_COMMAND = reage ao clique em cada botao (IDs 1 a 10)
   WM_DESTROY = encerra o programa ao fechar a janela */

LRESULT CALLBACK MenuProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
    switch(msg)
    {

    case WM_CREATE: /* aqui criamos todos os botoes do menu */
    {
        int bx = 36;
        int bw = 328;
        int bh = 38;
        int gap = 8;
        int y = 30;

		CreateWindow("BUTTON","Inserir Item",
		WS_VISIBLE|WS_CHILD,
		bx, y, bw - 48 - gap, bh,
		hwnd,(HMENU)1,NULL,NULL);

		CreateWindow("BUTTON","+",
		WS_VISIBLE|WS_CHILD,
		bx + bw - 48, y, 48, bh,
		hwnd,(HMENU)10,NULL,NULL);

        y += bh + gap;

        CreateWindow("BUTTON","Cadastrar Similaridade",
        WS_VISIBLE|WS_CHILD,
        bx, y, bw, bh,
        hwnd,(HMENU)2,NULL,NULL);
        y += bh + gap;

        CreateWindow("BUTTON","Buscar Similares",
        WS_VISIBLE|WS_CHILD,
        bx, y, bw, bh,
        hwnd,(HMENU)3,NULL,NULL);
        y += bh + gap;

        CreateWindow("BUTTON","Verificar Existencia",
        WS_VISIBLE|WS_CHILD,
        bx, y, bw, bh,
        hwnd,(HMENU)4,NULL,NULL);
        y += bh + gap;

        CreateWindow("BUTTON","Listar Itens",
        WS_VISIBLE|WS_CHILD,
        bx, y, bw, bh,
        hwnd,(HMENU)5,NULL,NULL);
        y += bh + gap;

        CreateWindow("BUTTON","Contar Propriedade",
        WS_VISIBLE|WS_CHILD,
        bx, y, bw, bh,
        hwnd,(HMENU)7,NULL,NULL);
        y += bh + gap;

        CreateWindow("BUTTON","Remover Menos Raros",
        WS_VISIBLE|WS_CHILD,
        bx, y, bw, bh,
        hwnd,(HMENU)8,NULL,NULL);
        y += bh + gap;

        CreateWindow("BUTTON","Sair",
        WS_VISIBLE|WS_CHILD,
        bx, y, bw, bh,
        hwnd,(HMENU)9,NULL,NULL);

        return 0;
    }



    case WM_COMMAND: /* clique em um botao: LOWORD(wp) traz o ID do botao */

        switch(LOWORD(wp))
        {

            case 1: /* Inserir Item - abre formulario manual */

                CreateWindow(
                    "JanelaInserir",
                    "Inserir Item",
                    ESTILO_JANELA,
                    300,150,420,400,
                    hwnd,NULL,NULL,NULL
                );

                return 0;



            case 2: /* Cadastrar Similaridade */

				CreateWindow(
					 "JanelaSimilaridade",
					 "Cadastrar Similaridade",
					 ESTILO_JANELA,
					 300,150,380,300,
					 hwnd,NULL,NULL,NULL
				);
				
				return 0;



            case 3: /* Buscar Similares */

				CreateWindow(
					 "JanelaBuscar",
					 "Buscar Similares",
					 ESTILO_JANELA,
					 300,150,400,300,
					 hwnd,NULL,NULL,NULL
				);
				
				return 0;



            case 4: /* Verificar Existencia */

                CreateWindow(
                    "JanelaVerificar",
                    "Verificar Existencia",
                    ESTILO_JANELA,
                    300,150,400,220,
                    hwnd,NULL,NULL,NULL
                );

                return 0;



            case 5: /* Listar Itens */

                if(!existe_itens_ativos())
                {
                    MessageBox(hwnd,"Nenhum item cadastrado.","Listar Itens",MB_OK);
                    return 0;
                }

                CreateWindow(
                    "JanelaListar",
                    "Listar Itens",
                    ESTILO_JANELA,
                    280,120,400,360,
                    hwnd,NULL,NULL,NULL
                );

                return 0;

            case 7: /* Contar Propriedade */

                CreateWindow(
                    "JanelaContar",
                    "Contar Propriedade",
                    ESTILO_JANELA,
                    300,150,400,220,
                    hwnd,NULL,NULL,NULL
                );

                return 0;

            case 8: /* Remover Menos Raros */

                CreateWindow(
                    "JanelaRemover",
                    "Remover Menos Raros",
                    ESTILO_JANELA,
                    300,150,400,220,
                    hwnd,NULL,NULL,NULL
                );

                return 0;



            case 9: /* Sair */

                DestroyWindow(hwnd);
                return 0;

			case 10: /* botao "+" - importar itens e similaridades de um .txt */

			    importar_de_arquivo(hwnd);
			    return 0;
        }

        return 0;



    case WM_DESTROY: /* usuario fechou a janela principal */
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd,msg,wp,lp);
}




/* ====================== */
/* JANELA INSERIR ITEM    */
/* ====================== */
/* Coleta nome, dono, propriedade, raridade e pontos do contorno.
   Botao 101 (+) adiciona um ponto; botao 100 (Salvar) chama inserir_item_gui. */

LRESULT CALLBACK InserirProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
    switch(msg)
    {

    case WM_CREATE: /* cria labels, campos EDIT e botoes da tela */

        pontosInseridos = 0;

        CreateWindow(
        "STATIC","Nome:",
        WS_VISIBLE|WS_CHILD,
        30,30,80,25,
        hwnd,NULL,NULL,NULL);

        campoNome=
        CreateWindow(
        "EDIT","",
        WS_VISIBLE|WS_CHILD|WS_BORDER,
        120,30,180,25,
        hwnd,NULL,NULL,NULL);



        CreateWindow(
        "STATIC","Dono:",
        WS_VISIBLE|WS_CHILD,
        30,70,80,25,
        hwnd,NULL,NULL,NULL);

        campoDono=
        CreateWindow(
        "EDIT","",
        WS_VISIBLE|WS_CHILD|WS_BORDER,
        120,70,180,25,
        hwnd,NULL,NULL,NULL);



        CreateWindow(
        "STATIC","Propriedade:",
        WS_VISIBLE|WS_CHILD,
        30,110,80,25,
        hwnd,NULL,NULL,NULL);

        campoProp=
        CreateWindow(
        "EDIT","",
        WS_VISIBLE|WS_CHILD|WS_BORDER,
        120,110,180,25,
        hwnd,NULL,NULL,NULL);



        CreateWindow(
        "STATIC","Raridade:",
        WS_VISIBLE|WS_CHILD,
        30,150,80,25,
        hwnd,NULL,NULL,NULL);

        campoRaridade=
        CreateWindow(
        "EDIT","",
        WS_VISIBLE|WS_CHILD|WS_BORDER,
        120,150,180,25,
        hwnd,NULL,NULL,NULL);

        CreateWindow(
        "STATIC","Qtd pontos:",
        WS_VISIBLE|WS_CHILD,
        30,190,80,25,
        hwnd,NULL,NULL,NULL);

        campoQtdPontos=
        CreateWindow(
        "EDIT","",
        WS_VISIBLE|WS_CHILD|WS_BORDER,
        120,190,180,25,
        hwnd,NULL,NULL,NULL);

        CreateWindow(
        "STATIC","X:",
        WS_VISIBLE|WS_CHILD,
        30,230,20,25,
        hwnd,NULL,NULL,NULL);

        campoPontoX=
        CreateWindow(
        "EDIT","",
        WS_VISIBLE|WS_CHILD|WS_BORDER,
        55,230,70,25,
        hwnd,NULL,NULL,NULL);

        CreateWindow(
        "STATIC","Y:",
        WS_VISIBLE|WS_CHILD,
        140,230,20,25,
        hwnd,NULL,NULL,NULL);

        campoPontoY=
        CreateWindow(
        "EDIT","",
        WS_VISIBLE|WS_CHILD|WS_BORDER,
        165,230,70,25,
        hwnd,NULL,NULL,NULL);

        CreateWindow(
        "BUTTON","+",
        WS_VISIBLE|WS_CHILD,
        250,228,35,30,
        hwnd,(HMENU)101,NULL,NULL);

        labelPontos=
        CreateWindow(
        "STATIC","Pontos cadastrados: 0",
        WS_VISIBLE|WS_CHILD,
        30,270,200,25,
        hwnd,NULL,NULL,NULL);

        CreateWindow(
        "BUTTON","Salvar",
        WS_VISIBLE|WS_CHILD,
        140,320,120,35,
        hwnd,(HMENU)100,NULL,NULL);

        return 0;



    case WM_COMMAND:

        if(LOWORD(wp)==101) /* botao "+" : guarda coordenada X,Y no vetor pontosInserir */
        {
            char xTxt[20];
            char yTxt[20];
            char qtdTxt[20];

            GetWindowText(campoPontoX,xTxt,20);
            GetWindowText(campoPontoY,yTxt,20);
            GetWindowText(campoQtdPontos,qtdTxt,20);

            if(campoVazio(xTxt) || campoVazio(yTxt))
            {
                MessageBox(hwnd,"Informe X e Y do ponto.","Erro",MB_OK);
                return 0;
            }

            int qtd=atoi(qtdTxt);
            if(qtd<3 || qtd>100)
            {
                MessageBox(hwnd,"Informe a quantidade de pontos (3 a 100).","Erro",MB_OK);
                return 0;
            }

            if(pontosInseridos >= qtd)
            {
                MessageBox(hwnd,"Todos os pontos ja foram cadastrados.","Erro",MB_OK);
                return 0;
            }

            pontosInserir[pontosInseridos].x = atoi(xTxt);
            pontosInserir[pontosInseridos].y = atoi(yTxt);
            pontosInseridos++;

            SetWindowText(campoPontoX,"");
            SetWindowText(campoPontoY,"");
            atualizar_label_pontos();

            return 0;
        }

        if(LOWORD(wp)==100) /* botao Salvar: valida campos e insere na arvore/grafo */
        {
            char nome[100];
            char dono[100];
            char prop[100];
            char rar[20];
            char qtdTxt[20];

            GetWindowText(campoNome,nome,100);
            GetWindowText(campoDono,dono,100);
            GetWindowText(campoProp,prop,100);
            GetWindowText(campoRaridade,rar,20);
            GetWindowText(campoQtdPontos,qtdTxt,20);

            if(campoVazio(nome) || campoVazio(dono) || campoVazio(prop) || campoVazio(rar)
                || campoVazio(qtdTxt))
            {
                MessageBox(hwnd,"Preencha todos os campos.","Erro",MB_OK);
                return 0;
            }

            int raridade=atoi(rar);
            if(raridade<0 || raridade>100)
            {
                MessageBox(hwnd,"Raridade deve ser de 0 a 100.","Erro",MB_OK);
                return 0;
            }

            int qtd=atoi(qtdTxt);
            if(qtd<3 || qtd>100)
            {
                MessageBox(hwnd,"Informe de 3 a 100 pontos no contorno.","Erro",MB_OK);
                return 0;
            }

            if(pontosInseridos != qtd)
            {
                MessageBox(hwnd,"Cadastre todos os pontos com o botao +.","Erro",MB_OK);
                return 0;
            }

            int res = inserir_item_gui(root, nome, dono, prop, raridade, pontosInserir, qtd);
            if(res == 0)
            {
                MessageBox(hwnd,"Limite de 1000 itens atingido.","Erro",MB_OK);
                return 0;
            }
            if(res == -2)
            {
                MessageBox(hwnd,"Item rejeitado. O contorno nao forma um poligono convexo.","Erro",MB_OK);
                return 0;
            }

            MessageBox(
              hwnd,
              "Item inserido!",
              "Sucesso",
              MB_OK
            );

            fechar_janela(hwnd);

            return 0;
        }

        return 0;



    case WM_CLOSE:
        fechar_janela(hwnd);
        return 0;
    }

    return DefWindowProc(hwnd,msg,wp,lp);
}

/* ====================== */
/* JANELA SIMILARIDADE    */
/* ====================== */
/* Liga dois itens existentes com um peso (aresta no grafo de similaridade).
   Os combos listam apenas itens ativos; Salvar (ID 200) chama cadastrar_similaridade_gui. */

LRESULT CALLBACK SimilaridadeProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
	switch(msg)
	{
	
		case WM_CREATE: /* monta combos de itens e campo de peso */
		
		CreateWindow("STATIC","ID 1:",
		WS_VISIBLE|WS_CHILD,
		30,30,80,25,
		hwnd,NULL,NULL,NULL);
		
		campoId1 = CreateWindow(
	    "COMBOBOX","",
	    WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST,
	    120,30,150,200,
	    hwnd,NULL,NULL,NULL);
		
		
		CreateWindow("STATIC","ID 2:",
		WS_VISIBLE|WS_CHILD,
		30,70,80,25,
		hwnd,NULL,NULL,NULL);
		
		campoId2 = CreateWindow(
	    "COMBOBOX","",
	    WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST,
	    120,70,150,200,
	    hwnd,NULL,NULL,NULL);
	    
	    preencher_combo_itens(campoId1, -1);
	    preencher_combo_itens(campoId2, -1);
		
		
		CreateWindow("STATIC","Peso:",
		WS_VISIBLE|WS_CHILD,
		30,110,80,25,
		hwnd,NULL,NULL,NULL);
		
		campoPeso=
		CreateWindow("EDIT","",
		WS_VISIBLE|WS_CHILD|WS_BORDER,
		120,110,150,25,
		hwnd,NULL,NULL,NULL);
		
		
		CreateWindow("BUTTON","Salvar",
		WS_VISIBLE|WS_CHILD,
		120,170,120,35,
		hwnd,(HMENU)200,NULL,NULL);

		return 0;
		
		
		
		case WM_COMMAND:

		if(HIWORD(wp)==CBN_SELCHANGE && (HWND)lp==campoId1)
		{
			int sel1 = SendMessage(campoId1, CB_GETCURSEL, 0, 0);
			int idExcluir = (sel1 == CB_ERR) ? -1 : id_item_pelo_combo(campoId1, sel1);
			preencher_combo_itens(campoId2, idExcluir);
			return 0;
		}
			
		if(LOWORD(wp)==200) /* Salvar: le ids selecionados e registra a similaridade */
		{
			char p[20];
			
			int sel1 = SendMessage(campoId1, CB_GETCURSEL, 0, 0);
			int sel2 = SendMessage(campoId2, CB_GETCURSEL, 0, 0);
			GetWindowText(campoPeso,p,20);
			
			if(sel1 == -1 || sel2 == -1)
		    {
		        MessageBox(hwnd,"Selecione os dois itens.","Erro",MB_OK);
		        return 0;
		    }
			
			int id1 = id_item_pelo_combo(campoId1, sel1);
			int id2 = id_item_pelo_combo(campoId2, sel2);
			int peso=atoi(p);

			if(id1 == id2)
			{
			    MessageBox(hwnd,"Selecione dois itens diferentes.","Erro",MB_OK);
			    return 0;
			}

			if(campoVazio(p))
			{
			    MessageBox(hwnd,"Informe o peso da similaridade.","Erro",MB_OK);
			    return 0;
			}

			if(!cadastrar_similaridade_gui(id1, id2, peso))
			{
			    MessageBox(hwnd,"Itens invalidos ou removidos.","Erro",MB_OK);
			    return 0;
			}
			
			MessageBox(
				 hwnd,
				 "Similaridade cadastrada!",
				 "Sucesso",
				 MB_OK
			);
		
			fechar_janela(hwnd);
		
			return 0;
		}
	
		return 0;

		case WM_CLOSE:
		    fechar_janela(hwnd);
		    return 0;
	}
	
	return DefWindowProc(hwnd,msg,wp,lp);
}

/* ====================== */
/* JANELA BUSCAR          */
/* ====================== */
/* A partir de um item base, lista similares com peso >= minimo,
   ignorando itens de um dono escolhido. Botao 300 chama buscar_similares_gui. */

LRESULT CALLBACK BuscarProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
	switch(msg)
	{
	
		case WM_CREATE: /* preenche combos com itens e donos distintos */
		
		CreateWindow("STATIC","Item base:",
		WS_VISIBLE|WS_CHILD,
		30,30,80,25,
		hwnd,NULL,NULL,NULL);
		
		campoBase = CreateWindow(
	    "COMBOBOX","",
	    WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST,
	    120,30,150,200,
	    hwnd,NULL,NULL,NULL);
		
		
		CreateWindow("STATIC","Minimo:",
		WS_VISIBLE|WS_CHILD,
		30,70,80,25,
		hwnd,NULL,NULL,NULL);
		
		campoMin=
		CreateWindow("EDIT","",
		WS_VISIBLE|WS_CHILD|WS_BORDER,
		120,70,150,25,
		hwnd,NULL,NULL,NULL);
		
		
		CreateWindow("STATIC","Ignorar dono:",
		WS_VISIBLE|WS_CHILD,
		15,110,110,25,
		hwnd,NULL,NULL,NULL);
		
		campoIgnorar = CreateWindow(
	    "COMBOBOX","",
	    WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST,
	    120,110,150,200,
	    hwnd,NULL,NULL,NULL);

	    preencher_combo_itens(campoBase, -1);

		if(SendMessage(campoBase, CB_GETCOUNT, 0, 0) > 0)
		    SendMessage(campoBase, CB_SETCURSEL, 0, 0);
		
		for(int i=0; i<N; i++)
		{
		    if(!itens[i].ativo)
		        continue;

		    bool existe=false;
		
		    for(int j=0; j<i; j++)
		    {
		        if(itens[j].ativo && itens[i].dono == itens[j].dono)
		            existe=true;
		    }
		
		    if(!existe)
		        SendMessage(campoIgnorar, CB_ADDSTRING, 0, (LPARAM)itens[i].dono.c_str());
		}

		if(SendMessage(campoIgnorar, CB_GETCOUNT, 0, 0) > 0)
		    SendMessage(campoIgnorar, CB_SETCURSEL, 0, 0);
		
		
		CreateWindow("BUTTON","Buscar",
		WS_VISIBLE|WS_CHILD,
		120,170,120,35,
		hwnd,(HMENU)300,NULL,NULL);

		return 0;
		
		
		
		case WM_COMMAND:
		
		if(LOWORD(wp)==300) /* Buscar: monta resultado e exibe em MessageBox */
		{
			char b[20];
			char j[100];
			b[0] = '\0';
			j[0] = '\0';
			
			GetWindowText(campoMin,b,20);
			
			int C = id_item_pelo_combo(campoBase, SendMessage(campoBase, CB_GETCURSEL, 0, 0));
			int selDono = SendMessage(campoIgnorar, CB_GETCURSEL, 0, 0);

			if(C == -1)
		    {
		        MessageBox(hwnd,"Selecione um item base.","Erro",MB_OK);
		        return 0;
		    }

			if(campoVazio(b))
			{
			    MessageBox(hwnd,"Informe o valor minimo de similaridade.","Erro",MB_OK);
			    return 0;
		    }
		
		    if(selDono == -1)
		    {
		        MessageBox(hwnd,"Selecione um dono para ignorar.","Erro",MB_OK);
		        return 0;
		    }

			SendMessage(campoIgnorar, CB_GETLBTEXT, selDono, (LPARAM)j);

			int x=atoi(b);
			string texto = buscar_similares_gui(C, x, j);
			
			MessageBox(
				 hwnd,
				 texto.c_str(),
				 "Similares",
				 MB_OK
			);
			
			return 0;
		}
		
		return 0;

		case WM_CLOSE:
		    fechar_janela(hwnd);
		    return 0;
	}	
	
	return DefWindowProc(hwnd,msg,wp,lp);
}

/* ====================== */
/* JANELA VERIFICAR       */
/* ====================== */
/* Busca um nome na arvore binaria. Botao 400 chama verificar_existencia_gui. */

LRESULT CALLBACK VerificarProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
    switch(msg)
    {
    case WM_CREATE: /* campo de texto para o nome do item */

        CreateWindow("STATIC","Nome do item:",
        WS_VISIBLE|WS_CHILD,
        30,30,120,25,
        hwnd,NULL,NULL,NULL);

        campoVerificarNome = CreateWindow(
        "EDIT","",
        WS_VISIBLE|WS_CHILD|WS_BORDER,
        30,60,300,25,
        hwnd,NULL,NULL,NULL);

        CreateWindow("BUTTON","Verificar",
        WS_VISIBLE|WS_CHILD,
        120,110,120,35,
        hwnd,(HMENU)400,NULL,NULL);

        return 0;

    case WM_COMMAND:

        if(LOWORD(wp)==400) /* Verificar: consulta a arvore pelo nome digitado */
        {
            char nome[100];
            GetWindowText(campoVerificarNome,nome,100);

            if(campoVazio(nome))
            {
                MessageBox(hwnd,"Digite o nome do item.","Erro",MB_OK);
                return 0;
            }

            if(verificar_existencia_gui(root, nome))
                MessageBox(hwnd,"Item encontrado!","Resultado",MB_OK);
            else
                MessageBox(hwnd,"Item nao encontrado!","Resultado",MB_OK);

            fechar_janela(hwnd);
            return 0;
        }

        return 0;

    case WM_CLOSE:
        fechar_janela(hwnd);
        return 0;
    }

    return DefWindowProc(hwnd,msg,wp,lp);
}

/* ====================== */
/* JANELA LISTAR          */
/* ====================== */
/* Mostra todos os itens em uma listbox, com ordenacao alfabetica ou por raridade.
   Duplo clique ou "Ver detalhes" abre as informacoes completas do item. */

LRESULT CALLBACK ListarProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
    switch(msg)
    {
    case WM_CREATE: /* combo de ordenacao, listbox e botoes de acao */
    {
        CreateWindow(
        "STATIC","Ordenar por:",
        WS_VISIBLE|WS_CHILD,
        20,15,90,20,
        hwnd,NULL,NULL,NULL);

        HWND comboOrdem = CreateWindow(
        "COMBOBOX","",
        WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST|CBS_HASSTRINGS,
        110,12,180,120,
        hwnd,(HMENU)ID_COMBO_ORDEM,NULL,NULL);

        SendMessage(comboOrdem, CB_ADDSTRING, 0, (LPARAM)"Alfabetico");
        SendMessage(comboOrdem, CB_ADDSTRING, 0, (LPARAM)"Raridade");
        SendMessage(comboOrdem, CB_SETCURSEL, 0, 0);

        CreateWindow(
        "STATIC","Selecione um item:",
        WS_VISIBLE|WS_CHILD,
        20,45,200,20,
        hwnd,NULL,NULL,NULL);

        listaItens = CreateWindow(
        "LISTBOX","",
        WS_VISIBLE|WS_CHILD|WS_BORDER|WS_VSCROLL|LBS_NOTIFY,
        20,70,340,170,
        hwnd,(HMENU)ID_LISTA_ITENS,NULL,NULL);

        atualizar_lista_itens(listaItens, 0);

        CreateWindow(
        "BUTTON","Ver detalhes",
        WS_VISIBLE|WS_CHILD,
        60,255,120,35,
        hwnd,(HMENU)ID_BTN_DETALHES,NULL,NULL);

        CreateWindow(
        "BUTTON","Fechar",
        WS_VISIBLE|WS_CHILD,
        200,255,120,35,
        hwnd,(HMENU)ID_BTN_FECHAR_LISTA,NULL,NULL);

        return 0;
    }

    case WM_COMMAND:

        if(HIWORD(wp)==CBN_SELCHANGE && LOWORD(wp)==ID_COMBO_ORDEM) /* mudou Alfabetico/Raridade */
        {
            HWND comboOrdem = GetDlgItem(hwnd, ID_COMBO_ORDEM);
            int modo = SendMessage(comboOrdem, CB_GETCURSEL, 0, 0);
            if(modo != CB_ERR)
                atualizar_lista_itens(listaItens, modo);
            return 0;
        }

        if(LOWORD(wp)==ID_BTN_DETALHES)
        {
            mostrar_detalhes_lista(hwnd, listaItens);
            return 0;
        }

        if(LOWORD(wp)==ID_BTN_FECHAR_LISTA)
        {
            fechar_janela(hwnd);
            return 0;
        }

        if(HIWORD(wp)==LBN_DBLCLK && LOWORD(wp)==ID_LISTA_ITENS)
        {
            mostrar_detalhes_lista(hwnd, listaItens);
            return 0;
        }

        return 0;

    case WM_CLOSE:
        fechar_janela(hwnd);
        return 0;
    }

    return DefWindowProc(hwnd,msg,wp,lp);
}

/* ====================== */
/* JANELA CONTAR          */
/* ====================== */
/* Conta quantos itens ativos possuem a mesma propriedade magica.
   Botao 700 chama contar_propriedade_gui. */

LRESULT CALLBACK ContarProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
    switch(msg)
    {
    case WM_CREATE: /* campo para digitar a propriedade magica */

        CreateWindow("STATIC","Propriedade magica:",
        WS_VISIBLE|WS_CHILD,
        30,30,150,25,
        hwnd,NULL,NULL,NULL);

        campoPropriedade = CreateWindow(
        "EDIT","",
        WS_VISIBLE|WS_CHILD|WS_BORDER,
        30,60,300,25,
        hwnd,NULL,NULL,NULL);

        CreateWindow("BUTTON","Contar",
        WS_VISIBLE|WS_CHILD,
        120,110,120,35,
        hwnd,(HMENU)700,NULL,NULL);

        return 0;

    case WM_COMMAND:

        if(LOWORD(wp)==700) /* Contar: percorre itens[] e soma os que batem com a propriedade */
        {
            char prop[100];
            GetWindowText(campoPropriedade,prop,100);

            if(campoVazio(prop))
            {
                MessageBox(hwnd,"Digite a propriedade magica.","Erro",MB_OK);
                return 0;
            }

            string texto = contar_propriedade_gui(prop);

            MessageBox(hwnd,texto.c_str(),"Resultado",MB_OK);
            fechar_janela(hwnd);
            return 0;
        }

        return 0;

    case WM_CLOSE:
        fechar_janela(hwnd);
        return 0;
    }

    return DefWindowProc(hwnd,msg,wp,lp);
}

/* ====================== */
/* JANELA REMOVER         */
/* ====================== */
/* Remove da arvore e do grafo os itens com raridade menor que o limite informado.
   Botao 800 chama remover_menos_raros_gui. */

LRESULT CALLBACK RemoverProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
    switch(msg)
    {
    case WM_CREATE: /* campo para a raridade minima de corte */

        CreateWindow("STATIC","Raridade minima:",
        WS_VISIBLE|WS_CHILD,
        30,30,150,25,
        hwnd,NULL,NULL,NULL);

        campoRaridadeMin = CreateWindow(
        "EDIT","",
        WS_VISIBLE|WS_CHILD|WS_BORDER,
        30,60,300,25,
        hwnd,NULL,NULL,NULL);

        CreateWindow("BUTTON","Remover",
        WS_VISIBLE|WS_CHILD,
        120,110,120,35,
        hwnd,(HMENU)800,NULL,NULL);

        return 0;

    case WM_COMMAND:

        if(LOWORD(wp)==800) /* Remover: desativa itens abaixo do limite e limpa a arvore */
        {
            char rar[20];
            GetWindowText(campoRaridadeMin,rar,20);

            if(campoVazio(rar))
            {
                MessageBox(hwnd,"Digite a raridade minima.","Erro",MB_OK);
                return 0;
            }

            int R = atoi(rar);
            if(R < 0 || R > 100)
            {
                MessageBox(hwnd,"Raridade deve ser de 0 a 100.","Erro",MB_OK);
                return 0;
            }

            string texto = remover_menos_raros_gui(root, R);

            MessageBox(hwnd,texto.c_str(),"Resultado",MB_OK);
            fechar_janela(hwnd);
            return 0;
        }

        return 0;

    case WM_CLOSE:
        fechar_janela(hwnd);
        return 0;
    }

    return DefWindowProc(hwnd,msg,wp,lp);
}
