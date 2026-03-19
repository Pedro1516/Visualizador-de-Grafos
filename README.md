# Visualizador de Grafos (Raylib + WebAssembly)

Aplicação interativa para criação e visualização de grafos desenvolvida em **C** utilizando **Raylib**.
O projeto também possui uma versão web compilada para **WebAssembly** utilizando **Emscripten**.

A aplicação permite criar vértices, arestas e executar algoritmos clássicos de grafos diretamente na interface gráfica.

---

## Funcionalidades

* Criar vértices
* Criar arestas entre vértices
* Editar peso das arestas
* Remover elementos do grafo
* Executar **BFS**
* Executar **DFS**
* Limpar animações
* Visualização gráfica em tempo real

---

## Interface

A interface permite manipular o grafo através de botões laterais.

Operações disponíveis:

| Botão           | Função                        |
| --------------- | ----------------------------- |
| Criar Vértice   | Adiciona um novo vértice      |
| Criar Aresta    | Conecta dois vértices         |
| Excluir         | Remove vértices ou arestas    |
| Editar Peso     | Define o peso de uma aresta   |
| BFS             | Executa busca em largura      |
| DFS             | Executa busca em profundidade |
| Limpar Animação | Reseta a visualização         |

---

## Estrutura do Projeto

```
.
├── docs/
│   ├── grafo.html
│   ├── grafo.js
│   ├── grafo.wasm
│   ├── fonts/
│   │   └── Oswald.ttf
│   └── assets/
│       ├── menu_icon.png
│       └── ze_do_grafo.png
├── main.c
├── interface.c
├── interface.h
└── raylib/
```

---

## Compilação (Linux)

Instale as dependências da **Raylib** e compile com:

```
gcc main.c interface.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o grafo
```

Executar:

```
./grafo
```

---

## Compilação para Web

Para gerar a versão web é necessário instalar o **Emscripten**.

Compilação:

```
emcc main.c interface.c raylib/src/libraylib.web.a \
-Iraylib/src \
-o docs/grafo.html \
-s USE_GLFW=3 \
-s ASYNCIFY \
-s ALLOW_MEMORY_GROWTH=1 \
--preload-file docs/fonts \
--preload-file docs/assets \
-s EXPORTED_FUNCTIONS='["_main","_importar_grafo_web"]' \
EXPORTED_RUNTIME_METHODS='["ccall"]'

```

Depois execute um servidor local:

```
python3 -m http.server
```

Acesse no navegador:

```
http://localhost:8000/grafo.html
```

---

## Tecnologias Utilizadas

* C
* Raylib
* Emscripten
* WebAssembly

---

## Algoritmos Implementados

* **BFS (Breadth-First Search)**
* **DFS (Depth-First Search)**

Os algoritmos são visualizados diretamente no grafo através de animação.

---

## Objetivo do Projeto

Este projeto foi desenvolvido com fins educacionais para estudar:

* Estruturas de dados
* Algoritmos de grafos
* Programação gráfica em C
* Compilação de aplicações nativas para WebAssembly

---

## Visualize o Programa

https://pedro1516.github.io/Visualizador-de-Grafos/grafo.html

---

## Licença

Este projeto está licenciado sob a licença MIT.
Veja o arquivo `LICENSE` para mais detalhes.
