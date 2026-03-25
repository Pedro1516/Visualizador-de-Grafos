# Visualizador de Grafos (Raylib + WebAssembly)

Aplicação interativa para criação e visualização de grafos desenvolvida em **C** utilizando **Raylib**.
O projeto também possui uma versão web compilada para **WebAssembly** utilizando **Emscripten**.

A aplicação permite criar vértices, arestas, executar algoritmos clássicos de grafos e importar/exportar grafos diretamente na interface gráfica.

---

## Funcionalidades

* Criar e remover vértices e arestas
* Mover vértices arrastando com o mouse
* Editar peso das arestas
* Navegar pelo canvas com arrastar e zoom (scroll do mouse)
* Suporte a grafos **direcionados** e **não-direcionados**
* Suporte a grafos **ponderados** e **não-ponderados**
* Gerar grafos completos **Kₙ** automaticamente
* Executar e animar **BFS** e **DFS** passo a passo
* Importar grafo a partir de arquivo `.txt` (lista de adjacências)
* Exportar grafo para arquivo `.txt` (lista de adjacências)
* Visualização gráfica em tempo real

---

## Interface

O grafo é manipulado via mouse. Use o **botão direito** para selecionar vértices e arestas, e o **botão esquerdo** para arrastar vértices ou navegar pelo canvas.

### Botões do menu lateral

| Botão                        | Função                                                  |
| ---------------------------- | ------------------------------------------------------- |
| Criar Vértice                | Adiciona um novo vértice ao grafo                       |
| Criar Aresta                 | Conecta os dois vértices selecionados                   |
| Excluir                      | Remove o vértice ou aresta selecionado                  |
| Editar Peso                  | Define o peso da aresta selecionada                     |
| BFS                          | Executa busca em largura a partir do vértice selecionado|
| DFS                          | Executa busca em profundidade a partir do vértice selecionado |
| Limpar Animação              | Reseta as cores das arestas da animação                 |
| Gerar K-Completo             | Gera um grafo completo Kₙ (informe o valor de n)       |
| Limpar Grafo                 | Remove todos os vértices e arestas                      |
| Trocar Para Ponderado / Não Ponderado | Alterna o modo de ponderação do grafo        |
| Trocar Para Direcionado / Não Direcionado | Alterna o modo de direcionamento do grafo |
| Importar Grafo               | Carrega um grafo a partir de arquivo `.txt`             |
| Exportar Grafo               | Salva o grafo atual em arquivo `.txt`                   |

### Controles

| Ação                         | Controle                              |
| ---------------------------- | ------------------------------------- |
| Selecionar vértice / aresta  | Botão direito do mouse                |
| Mover vértice                | Botão esquerdo + arrastar sobre vértice |
| Navegar pelo canvas          | Botão esquerdo + arrastar (sem vértice)|
| Zoom                         | Scroll do mouse                       |

---

## Formato de Arquivo (Importação / Exportação)

O grafo é salvo e carregado no formato de **lista de adjacências** em texto simples:

```
<num_vertices> <ponderado> <direcionado>
<adj>:<peso> <adj>:<peso> ...    ← adjacências do vértice 0
<adj>:<peso> ...                 ← adjacências do vértice 1
...
```

Para grafos não-ponderados, o campo `:<peso>` é omitido.

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

```bash
gcc main.c interface.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o grafo
```

Executar:

```bash
./grafo
```

---

## Compilação para Web

Para gerar a versão web é necessário instalar o **Emscripten**.

Compilação:

```bash
emcc main.c interface.c raylib/src/libraylib.web.a \
  -Iraylib/src \
  -o docs/grafo.html \
  -s USE_GLFW=3 \
  -s ASYNCIFY \
  -s ALLOW_MEMORY_GROWTH=1 \
  --preload-file docs/fonts \
  --preload-file docs/assets \
  -s EXPORTED_FUNCTIONS='["_main","_importar_grafo_web"]' \
  -s EXPORTED_RUNTIME_METHODS='["ccall"]'
```

Inicie um servidor local:

```bash
python3 -m http.server
```

Acesse no navegador:

```
http://localhost:8000/grafo.html
```

---

## Algoritmos Implementados

### BFS (Breadth-First Search)
Executa busca em largura a partir do vértice selecionado. As arestas percorridas são destacadas em **vermelho** conforme o algoritmo avança.

### DFS (Depth-First Search)
Executa busca em profundidade a partir do vértice selecionado. As arestas percorridas são destacadas em **azul**. O algoritmo suporta backtracking animado.

Ambos os algoritmos respeitam a direção das arestas em grafos direcionados e avançam um passo a cada 0,3 segundos para facilitar a visualização.

---

## Tecnologias Utilizadas

* C (C11)
* [Raylib](https://www.raylib.com/)
* [Emscripten](https://emscripten.org/)
* WebAssembly

---

## Objetivo do Projeto

Este projeto foi desenvolvido com fins educacionais para estudar:

* Estruturas de dados (listas de adjacência, filas, pilhas)
* Algoritmos de grafos (BFS, DFS)
* Programação gráfica em C com Raylib
* Compilação de aplicações nativas para WebAssembly

---

## Visualize o Programa

https://pedro1516.github.io/Visualizador-de-Grafos/

---

## Licença

Este projeto está licenciado sob a licença MIT.
Veja o arquivo `LICENSE` para mais detalhes.