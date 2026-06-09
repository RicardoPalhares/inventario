#include <windows.h>
#include <commdlg.h> // API do 
#include <stdio.h>
#include <string>
#include "opcoes_menu.h"

using namespace std;

LRESULT CALLBACK MenuProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK InserirProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK SimilaridadeProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK BuscarProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK VerificarProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK ContarProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK RemoverProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK ListarProc(HWND,UINT,WPARAM,LPARAM);

#define ESTILO_JANELA (WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_VISIBLE)
#define ID_LISTA_ITENS 902
#define ID_BTN_DETALHES 903
#define ID_BTN_FECHAR_LISTA 904
#define ID_COMBO_ORDEM 905

treenodeptr root = NULL;
HWND janelaPrincipal = NULL;
HWND listaItens;

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

Ponto pontosInserir[100];
int pontosInseridos = 0;

bool campoVazio(const char* s)
{
    for(int i=0; s[i]; i++)
    {
        if(s[i]!=' ' && s[i]!='\t')
            return false;
    }
    return true;
}

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

bool ler_ponto(const string &linha, int &x, int &y)
{
    size_t pos = linha.find(' ');
    if(pos == string::npos)
        return false;

    x = atoi(linha.substr(0, pos).c_str());
    y = atoi(linha.substr(pos + 1).c_str());
    return true;
}

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

void atualizar_label_pontos()
{
    char buf[50];
    wsprintf(buf,"Pontos cadastrados: %d",pontosInseridos);
    SetWindowText(labelPontos,buf);
}

HWND obter_janela_pai(HWND hwnd)
{
    HWND pai = GetWindow(hwnd, GW_OWNER);
    if(!pai)
        pai = (HWND)GetWindowLongPtr(hwnd, GWLP_HWNDPARENT);
    if(!pai)
        pai = janelaPrincipal;
    return pai;
}

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

void fechar_janela(HWND hwnd)
{
    restaurar_janela_pai(hwnd);
    DestroyWindow(hwnd);
}

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

bool linha_cabecalho(const string& linha)
{
    return linha.find("Inserindo") != string::npos
        || linha.find("Similaridade") != string::npos
        || linha.find("Formato") != string::npos
        || linha.find("botao") != string::npos
        || linha.find("importar") != string::npos
        || linha.find("mesmo arquivo") != string::npos;
}

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

bool ler_par_ids(const string& linha, int& id1, int& id2)
{
    size_t pos = linha.find(' ');
    if(pos == string::npos)
        return false;

    string s1 = linha.substr(0, pos);
    string s2 = linha.substr(pos + 1);

    if(s2.find(' ') != string::npos || s1.empty() || s2.empty())
        return false;

    for(int i=0; i<(int)s1.size(); i++)
    {
        if(s1[i] < '0' || s1[i] > '9')
            return false;
    }

    for(int i=0; i<(int)s2.size(); i++)
    {
        if(s2[i] < '0' || s2[i] > '9')
            return false;
    }

    id1 = atoi(s1.c_str());
    id2 = atoi(s2.c_str());
    return true;
}

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

void importar_similaridades_de_texto(const string& texto, int& importadas)
{
    string conteudo = texto;
    conteudo += '\n';
    string linha = "";
    int id1 = 0;
    int id2 = 0;
    bool aguardandoPeso = false;

    importadas = 0;

    for(int i=0; i<(int)conteudo.size(); i++)
    {
        if(conteudo[i]=='\n')
        {
            if(linha=="")
            {
                aguardandoPeso = false;
            }
            else if(linha_cabecalho(linha))
            {
                aguardandoPeso = false;
            }
            else if(!aguardandoPeso)
            {
                if(ler_par_ids(linha, id1, id2))
                    aguardandoPeso = true;
            }
            else
            {
                if(linha_apenas_numero(linha))
                {
                    int peso = atoi(linha.c_str());

                    if(cadastrar_similaridade_gui(id1, id2, peso))
                        importadas++;
                }

                aguardandoPeso = false;
            }

            linha = "";
        }
        else if(conteudo[i]!='\r')
        {
            linha += conteudo[i];
        }
    }
}

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
/* WINMAIN                */
/* ====================== */

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR,int)
{
    WNDCLASSEX wc;

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

    janelaPrincipal = CreateWindow(
        "MenuPrincipal",
        "Bolsa Devoradora",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        200,100,420,455,
        NULL,NULL,hInst,NULL
    );


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

LRESULT CALLBACK MenuProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
    switch(msg)
    {

    case WM_CREATE:
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



    case WM_COMMAND:

        switch(LOWORD(wp))
        {

            case 1:

                CreateWindow(
                    "JanelaInserir",
                    "Inserir Item",
                    ESTILO_JANELA,
                    300,150,420,400,
                    hwnd,NULL,NULL,NULL
                );

                return 0;



            case 2:

				CreateWindow(
					 "JanelaSimilaridade",
					 "Cadastrar Similaridade",
					 ESTILO_JANELA,
					 300,150,380,300,
					 hwnd,NULL,NULL,NULL
				);
				
				return 0;



            case 3:

				CreateWindow(
					 "JanelaBuscar",
					 "Buscar Similares",
					 ESTILO_JANELA,
					 300,150,400,300,
					 hwnd,NULL,NULL,NULL
				);
				
				return 0;



            case 4:

                CreateWindow(
                    "JanelaVerificar",
                    "Verificar Existencia",
                    ESTILO_JANELA,
                    300,150,400,220,
                    hwnd,NULL,NULL,NULL
                );

                return 0;



            case 5:

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

            case 7:

                CreateWindow(
                    "JanelaContar",
                    "Contar Propriedade",
                    ESTILO_JANELA,
                    300,150,400,220,
                    hwnd,NULL,NULL,NULL
                );

                return 0;

            case 8:

                CreateWindow(
                    "JanelaRemover",
                    "Remover Menos Raros",
                    ESTILO_JANELA,
                    300,150,400,220,
                    hwnd,NULL,NULL,NULL
                );

                return 0;



            case 9:

                DestroyWindow(hwnd);
                return 0;

			case 10:

			    importar_de_arquivo(hwnd);
			    return 0;
        }

        return 0;



    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd,msg,wp,lp);
}




/* ====================== */
/* JANELA INSERIR ITEM    */
/* ====================== */

LRESULT CALLBACK InserirProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
    switch(msg)
    {

    case WM_CREATE:

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

        if(LOWORD(wp)==101)
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

        if(LOWORD(wp)==100)
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
LRESULT CALLBACK SimilaridadeProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
	switch(msg)
	{
	
		case WM_CREATE:
		
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
	    
	    for(int i=0; i<N; i++)
		{
		    if(itens[i].ativo)
		    {
		        SendMessage(campoId1, CB_ADDSTRING, 0, (LPARAM)itens[i].nome.c_str());
		        SendMessage(campoId2, CB_ADDSTRING, 0, (LPARAM)itens[i].nome.c_str());
		    }
		}
		
		
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
			
		if(LOWORD(wp)==200)
		{
			char p[20];
			
			int sel1 = SendMessage(campoId1, CB_GETCURSEL, 0, 0);
			int sel2 = SendMessage(campoId2, CB_GETCURSEL, 0, 0);
			GetWindowText(campoPeso,p,20);
			
			if(sel1 == -1 || sel2 == -1)
		    {
		        MessageBox(hwnd,"Selecione os itens","Erro",MB_OK);
		        return 0;
		    }
			
			int id1 = id_item_ativo_pelo_combo(sel1);
			int id2 = id_item_ativo_pelo_combo(sel2);
			int peso=atoi(p);

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
LRESULT CALLBACK BuscarProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
	switch(msg)
	{
	
		case WM_CREATE:
		
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

	    for(int i=0; i<N; i++)
		{
		    if(itens[i].ativo)
		        SendMessage(campoBase, CB_ADDSTRING, 0, (LPARAM)itens[i].nome.c_str());
		}

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
		
		if(LOWORD(wp)==300)
		{
			char b[20];
			char j[100];
			b[0] = '\0';
			j[0] = '\0';
			
			GetWindowText(campoMin,b,20);
			
			int C = id_item_ativo_pelo_combo(SendMessage(campoBase, CB_GETCURSEL, 0, 0));
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

LRESULT CALLBACK VerificarProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
    switch(msg)
    {
    case WM_CREATE:

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

        if(LOWORD(wp)==400)
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

LRESULT CALLBACK ListarProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
    switch(msg)
    {
    case WM_CREATE:
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

        if(HIWORD(wp)==CBN_SELCHANGE && LOWORD(wp)==ID_COMBO_ORDEM)
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

LRESULT CALLBACK ContarProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
    switch(msg)
    {
    case WM_CREATE:

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

        if(LOWORD(wp)==700)
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

LRESULT CALLBACK RemoverProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
    switch(msg)
    {
    case WM_CREATE:

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

        if(LOWORD(wp)==800)
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
