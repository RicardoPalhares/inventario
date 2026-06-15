# Inventario

## Instalacao rapida

Baixe o repositorio e execute o arquivo **`inventario.exe`** na raiz do projeto.
Nao precisa compilar para usar o programa.

## Codigo-fonte

Os arquivos para compilar estao na pasta **`Códigos Sem Compilar/`**:

- `main.cpp` - interface grafica (janelas e botoes)
- `opcoes_menu.cpp` / `opcoes_menu.h` - logica do inventario (arvore, grafo, validacoes)
- `inventario.fpj` / `inventario.layout` - projeto Falcon C++
- `EXEMPLE.txt` - exemplo para importar itens pelo botao "+"

Se caso queira testar pelo opcoes_menu sem layout

## Compilar e rodar

No terminal, dentro da pasta de codigo:

```
cd "Códigos Sem Compilar"

# Compilar (gera o executavel na raiz do projeto)
g++ main.cpp opcoes_menu.cpp -o ../inventario.exe -mwindows

# Rodar
cd ..
./inventario.exe
```

## Salvar no Github

Use o `publicar.bat` na raiz do projeto, ou no terminal:

```
git add .
git commit -m "mensagem"
git push
```

## Caso de Teste

```
1
Espada Flamejante
Aragorn
fogo
85
4
0 0
2 0
2 2
0 2

1
Manto das Sombras
Legolas
invisibilidade
72
4
0 0
3 0
3 1
0 1

1
Cajado da Tempestade
Gandalf
raio
95
3
0 0
2 0
1 3

1
Adaga Envenenada
Legolas
veneno
40
4
0 0
1 0
1 4
0 4

1
Escudo Solar
Aragorn
luz
60
4
0 0
4 0
4 2
0 2

2
Espada Flamejante
90
Cajado da Tempestade
Manto das Sombras
55
Adaga Envenenada
Espada Flamejante
30
Manto das Sombras
Cajado da Tempestade
70
Escudo Solar

3
0
25
Legolas

3
2
60
Aragorn

3
0
80
Aragorn

5

```
