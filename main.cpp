// Leonardo Lopes Prado - 809
// Gabriel Gregório de Olveira - 2324
// Isaac Assis Costa Lopes - 2314 
// Ricardo de Oliveira Palhares - 2294
// Gabriel Genghini Craveiro - 2274
// Marcus Vinicius Duarte - 2296

#include <iostream>
#include "opcoes_menu.h"

using namespace std;

int main(){
    int opcao;


    mostrar_opcoes();

    while(true)
    {
    cin >> opcao;
    switch(opcao){
            case 1: inserir_item(); break;
            case 2: cadastrar_similaridade(); break;
            case 3: buscar_similares(); break;
            case 4: verificar_existencia(); break;
            case 5: listar_alfabetica(); break;
            case 6: listar_raridade(); break;
            case 7: contar_propriedade(); break;
            case 8: remover_menos_raros(); break;
            case 9: return 0;
            default: cout << "Opcao invalida!" << endl;
            
    }
    }
    return 0;
}
