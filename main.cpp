// Leonardo Lopes Prado - 809
// Gabriel Gregório de Olveira - 2324
// Isaac Assis Costa Lopes - 2314 
// Ricardo de Oliveira Palhares - 2294
// Gabriel Genghini Craveiro - 2274
// Marcus Vinicius Duarte - 2296

#include <iostream>
#include <string>
#include "opcoes_menu.h"

using namespace std;

int main(){
    int opcao;
	
	while(true)
	{
		
		mostrar_opcoes();	
    	
    	if (!(cin >> opcao)) {
    		cout << endl;
            cout << "Por favor, digite um numero!" << endl;
            cout << endl;
            cin.clear(); // Limpa o estado de erro do cin
            cin.ignore(1000, '\n'); // Descarta a entrada inválida
            continue;
        }
    	
		switch(opcao)
		{
            case 1: inserir_item(); break;
            case 2: cadastrar_similaridade(); break;
            case 3: buscar_similares(); break;
            case 4: verificar_existencia(); break;
            case 5: listar_alfabetica(); break;
            case 6: listar_raridade(); break;
            case 7: contar_propriedade(); break;
            case 8: remover_menos_raros(); break;
            case 9: return 0;
            default: cout << endl; 
			cout << "Opcao invalida!" << endl; 
			cout << endl; break;
    	}
	}
    
 
    return 0;
}