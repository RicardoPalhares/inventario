#include <windows.h>
#include <string>
#include "opcoes_menu.h"

using namespace std;

LRESULT CALLBACK MenuProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK InserirProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK SimilaridadeProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK BuscarProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK ImportarProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK ImportarSimProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK VerificarProc(HWND,UINT,WPARAM,LPARAM);

treenodeptr root = NULL;

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
HWND campoImportar;
HWND campoImportarSim;
HWND campoVerificarNome;

bool campoVazio(const char* s)
{
    for(int i=0; s[i]; i++)
    {
        if(s[i]!=' ' && s[i]!='\t')
            return false;
    }
    return true;
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
	
	wc.lpfnWndProc=ImportarProc;
	wc.lpszClassName="JanelaImportar";
	RegisterClassEx(&wc);
	
	wc.lpfnWndProc=ImportarSimProc;
	wc.lpszClassName="JanelaImportarSim";
	RegisterClassEx(&wc);

	wc.lpfnWndProc=VerificarProc;
	wc.lpszClassName="JanelaVerificar";
	RegisterClassEx(&wc);

    CreateWindow(
        "MenuPrincipal",
        "Bolsa Devoradora",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        200,100,450,500,
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
    	
    	// Botao principal
		CreateWindow("BUTTON","Inserir Item",
		WS_VISIBLE|WS_CHILD,
		100,30,180,35,
		hwnd,(HMENU)1,NULL,NULL);
		
		// Botao pequeno "+"
		CreateWindow("BUTTON","+",
		WS_VISIBLE|WS_CHILD,
		285,30,35,35,
		hwnd,(HMENU)10,NULL,NULL);

        CreateWindow("BUTTON","Cadastrar Similaridade",
        WS_VISIBLE|WS_CHILD,
        100,75,220,35,
        hwnd,(HMENU)2,NULL,NULL);

        CreateWindow("BUTTON","Buscar Similares",
        WS_VISIBLE|WS_CHILD,
        100,120,220,35,
        hwnd,(HMENU)3,NULL,NULL);

        CreateWindow("BUTTON","Verificar Existencia",
        WS_VISIBLE|WS_CHILD,
        100,165,220,35,
        hwnd,(HMENU)4,NULL,NULL);

        CreateWindow("BUTTON","Listar Itens",
        WS_VISIBLE|WS_CHILD,
        100,210,220,35,
        hwnd,(HMENU)5,NULL,NULL);

        CreateWindow("BUTTON","Listar por Raridade",
        WS_VISIBLE|WS_CHILD,
        100,255,220,35,
        hwnd,(HMENU)6,NULL,NULL);

        CreateWindow("BUTTON","Contar Propriedade",
        WS_VISIBLE|WS_CHILD,
        100,300,220,35,
        hwnd,(HMENU)7,NULL,NULL);

        CreateWindow("BUTTON","Remover Menos Raros",
        WS_VISIBLE|WS_CHILD,
        100,345,220,35,
        hwnd,(HMENU)8,NULL,NULL);

        CreateWindow("BUTTON","Sair",
        WS_VISIBLE|WS_CHILD,
        100,390,220,35,
        hwnd,(HMENU)9,NULL,NULL);

        return 0;



    case WM_COMMAND:

        switch(LOWORD(wp))
        {

            case 1:

                CreateWindow(
                    "JanelaInserir",
                    "Inserir Item",
                    WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                    300,150,400,350,
                    hwnd,NULL,NULL,NULL
                );

                return 0;



            case 2:

				CreateWindow(
					 "JanelaSimilaridade",
					 "Cadastrar Similaridade",
					 WS_OVERLAPPEDWINDOW|WS_VISIBLE,
					 300,150,380,300,
					 hwnd,NULL,NULL,NULL
				);
				
				return 0;



            case 3:

				CreateWindow(
					 "JanelaBuscar",
					 "Buscar Similares",
					 WS_OVERLAPPEDWINDOW|WS_VISIBLE,
					 300,150,400,300,
					 hwnd,NULL,NULL,NULL
				);
				
				return 0;



            case 4:

                CreateWindow(
                    "JanelaVerificar",
                    "Verificar Existencia",
                    WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                    300,150,400,220,
                    hwnd,NULL,NULL,NULL
                );

                return 0;



            case 5:
            {
                string texto = listar_ordem_alfabetica_gui(root);

                MessageBox(
                    hwnd,
                    texto.c_str(),
                    "Listar Itens",
                    MB_OK
                );

                return 0;
            }



            case 6:
            case 7:
            case 8:

                MessageBox(
                  hwnd,
                  "Funcionalidade em construcao",
                  "Aviso",
                  MB_OK
                );

                return 0;



            case 9:

                DestroyWindow(hwnd);
                return 0;

			case 10:

			    CreateWindow(
			        "JanelaImportar",
			        "Importar Itens",
			        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
			        300,150,400,400,
			        hwnd,NULL,NULL,NULL
			    );
			
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
        "BUTTON","Salvar",
        WS_VISIBLE|WS_CHILD,
        120,210,120,35,
        hwnd,(HMENU)100,NULL,NULL);

        return 0;



    case WM_COMMAND:

        if(LOWORD(wp)==100)
        {
            char nome[100];
            char dono[100];
            char prop[100];
            char rar[20];

            GetWindowText(campoNome,nome,100);
            GetWindowText(campoDono,dono,100);
            GetWindowText(campoProp,prop,100);
            GetWindowText(campoRaridade,rar,20);

            if(campoVazio(nome) || campoVazio(dono) || campoVazio(prop) || campoVazio(rar))
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

            if(!inserir_item_gui(root, nome, dono, prop, raridade))
            {
                MessageBox(hwnd,"Limite de 1000 itens atingido.","Erro",MB_OK);
                return 0;
            }

            MessageBox(
              hwnd,
              "Item inserido!",
              "Sucesso",
              MB_OK
            );

            DestroyWindow(hwnd);

            return 0;
        }

        return 0;



    case WM_DESTROY:
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
		    SendMessage(campoId1, CB_ADDSTRING, 0, (LPARAM)itens[i].nome.c_str());
		    SendMessage(campoId2, CB_ADDSTRING, 0, (LPARAM)itens[i].nome.c_str());
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
		
		
		// Botao salvar
		CreateWindow("BUTTON","Salvar",
		WS_VISIBLE|WS_CHILD,
		90,170,100,35,
		hwnd,(HMENU)200,NULL,NULL);
		
		// Botao importar
		CreateWindow("BUTTON","?",
		WS_VISIBLE|WS_CHILD,
		200,170,40,35,
		hwnd,(HMENU)201,NULL,NULL);
		
		return 0;
		
		
		
		case WM_COMMAND:
			
			
		if(LOWORD(wp)==201)
		{
		    CreateWindow(
		        "JanelaImportarSim",
		        "Importar Similaridades",
		        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
		        300,150,400,350,
		        hwnd,NULL,NULL,NULL
		    );
		
		    return 0;
		}
		
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
			
			int id1 = sel1;
			int id2 = sel2;
			int peso=atoi(p);

			if(campoVazio(p))
			{
			    MessageBox(hwnd,"Informe o peso da similaridade.","Erro",MB_OK);
			    return 0;
			}

			if(!cadastrar_similaridade_gui(id1, id2, peso))
			{
			    MessageBox(hwnd,"Ids invalidos.","Erro",MB_OK);
			    return 0;
			}
			
			MessageBox(
				 hwnd,
				 "Similaridade cadastrada!",
				 "Sucesso",
				 MB_OK
			);
		
			DestroyWindow(hwnd);
		
			return 0;
		}
	
		return 0;
	}
	
	return DefWindowProc(hwnd,msg,wp,lp);
}
LRESULT CALLBACK BuscarProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
	switch(msg)
	{
	
		case WM_CREATE:
		
		CreateWindow("STATIC","ID Base:",
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

	    // itens
		for(int i=0; i<N; i++)
		{
		    SendMessage(campoBase, CB_ADDSTRING, 0, (LPARAM)itens[i].nome.c_str());
		}
		
		// donos (sem repetir)
		for(int i=0; i<N; i++)
		{
		    bool existe=false;
		
		    for(int j=0; j<i; j++)
		    {
		        if(itens[i].dono == itens[j].dono)
		            existe=true;
		    }
		
		    if(!existe)
		    {
		        SendMessage(campoIgnorar, CB_ADDSTRING, 0, (LPARAM)itens[i].dono.c_str());
		    }
		}
		
		
		CreateWindow("BUTTON","Buscar",
		WS_VISIBLE|WS_CHILD,
		120,170,120,35,
		hwnd,(HMENU)300,NULL,NULL);
		
		return 0;
		
		
		
		case WM_COMMAND:
		
		if(LOWORD(wp)==300)
		{
			char b[20],j[100];
			
			GetWindowText(campoMin,b,20);
			
			int C = SendMessage(campoBase, CB_GETCURSEL, 0, 0);
			int selDono = SendMessage(campoIgnorar, CB_GETCURSEL, 0, 0);

			SendMessage(campoIgnorar, CB_GETLBTEXT, selDono, (LPARAM)j);
			
			if(C == -1)
		    {
		        MessageBox(hwnd,"Selecione um item base","Erro",MB_OK);
		        return 0;
		    }
		
		    if(selDono == -1)
		    {
		        MessageBox(hwnd,"Selecione um dono","Erro",MB_OK);
		        return 0;
		    }

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
	}	
	
	return DefWindowProc(hwnd,msg,wp,lp);
}

LRESULT CALLBACK ImportarProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
    switch(msg)
    {
        case WM_CREATE:

            CreateWindow("STATIC","Cole os itens :",
            WS_VISIBLE|WS_CHILD,
            20,20,300,20,
            hwnd,NULL,NULL,NULL);

            campoImportar = CreateWindow(
                "EDIT","",
                WS_VISIBLE|WS_CHILD|WS_BORDER|ES_MULTILINE|WS_VSCROLL,
                20,50,340,220,
                hwnd,NULL,NULL,NULL);

            CreateWindow("BUTTON","Importar",
                WS_VISIBLE|WS_CHILD,
                120,290,120,35,
                hwnd,(HMENU)500,NULL,NULL);

            return 0;

        case WM_COMMAND:

            if(LOWORD(wp)==500)
            {
                char buffer[5000];
                GetWindowText(campoImportar, buffer, 5000);

                string texto = buffer;
                texto += '\n';
                string linha = "";

                string dados[4];
                int contador = 0;

                for(int i=0;i<(int)texto.size();i++)
                {
                    if(texto[i]=='\n')
                    {

                        if(linha!="")
                        {
                            dados[contador++] = linha;
                            linha="";
                        }

                        if(contador==4)
                        {
                            int rar = atoi(dados[3].c_str());
                            if(!campoVazio(dados[0].c_str()) && !campoVazio(dados[1].c_str())
                                && !campoVazio(dados[2].c_str()) && rar>=0 && rar<=100)
                            {
                                if(!inserir_item_gui(root, dados[0], dados[1], dados[2], rar))
                                    break;
                            }
                            contador=0;
                        }
                    }
                    else if(texto[i]!='\r')
                    {
                        linha += texto[i];
                    }
                }

                MessageBox(hwnd,"Itens importados com sucesso!","Sucesso",MB_OK);

                DestroyWindow(hwnd);
                return 0;
            }

            return 0;
    }

    return DefWindowProc(hwnd,msg,wp,lp);
}
LRESULT CALLBACK ImportarSimProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
    switch(msg)
    {
        case WM_CREATE:

            CreateWindow("STATIC","Formato: ID -> ID / Peso",
            WS_VISIBLE|WS_CHILD,
            20,20,250,20,
            hwnd,NULL,NULL,NULL);

            campoImportarSim = CreateWindow(
                "EDIT","",
                WS_VISIBLE|WS_CHILD|WS_BORDER|ES_MULTILINE|WS_VSCROLL,
                20,50,340,200,
                hwnd,NULL,NULL,NULL);

            CreateWindow("BUTTON","Importar",
                WS_VISIBLE|WS_CHILD,
                120,270,120,35,
                hwnd,(HMENU)600,NULL,NULL);

            return 0;

        case WM_COMMAND:

            if(LOWORD(wp)==600)
            {
                char buffer[5000];
                GetWindowText(campoImportarSim, buffer, 5000);

                string texto = buffer;
                texto += '\n';
                string linha = "";

                string linhaIDs = "";
                int contador = 0;

                for(int i=0;i<(int)texto.size();i++)
                {
                    if(texto[i]=='\n')
                    {
                        if(linha!="")
                        {
                            if(contador==0)
                            {
                                linhaIDs = linha;
                                contador=1;
                            }
                            else if(contador==1)
                            {
                                int id1, id2, peso;

								// separar "id1 id2"
								string temp = linhaIDs;
								size_t pos = temp.find(' ');
								
								if(pos == string::npos)
								{
								    // formato invalido
								    linha = "";
								    contador = 0;
								    continue;
								}
								
								id1 = atoi(temp.substr(0,pos).c_str());
								id2 = atoi(temp.substr(pos+1).c_str());
								
								// peso
								peso = atoi(linha.c_str());

                                // validando estatistica
                                cadastrar_similaridade_gui(id1, id2, peso);

                                contador=0;
                            }

                            linha="";
                        }
                    }
                    else if(texto[i]!='\r')
                    {
                        linha += texto[i];
                    }
                }

                MessageBox(hwnd,"Similaridades importadas!","Sucesso",MB_OK);

                DestroyWindow(hwnd);
                return 0;
            }

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

            DestroyWindow(hwnd);
            return 0;
        }

        return 0;

    case WM_DESTROY:
        return 0;
    }

    return DefWindowProc(hwnd,msg,wp,lp);
}