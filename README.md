# Intel 8080 Emulator 🚀

Este projeto é um **emulador do processador Intel 8080**, com o objetivo final de rodar o clássico jogo **Space Invaders** 👾. O Intel 8080 foi um processador de 8 bits lançado em 1974, amplamente utilizado em sistemas embarcados e em arcades, como no Space Invaders.

## 🎯 Objetivo

Desenvolver um emulador que simule o comportamento do processador Intel 8080, permitindo a execução do jogo Space Invaders em hardware moderno. O projeto tem como foco a fidelidade à arquitetura original, garantindo que todas as instruções e o comportamento do sistema sejam corretamente emulados.

## 🔧 Funcionalidades

- Emulação completa do conjunto de instruções do processador Intel 8080.
- Implementação de um sistema de memória para leitura e escrita.
- Suporte a interrupções e periféricos do hardware original.
- Capacidade de carregar e executar o código binário do jogo Space Invaders.
- Tela de renderização e suporte básico para gráficos e som.

## 🏗️ Como Funciona

O emulador lê o código binário do jogo Space Invaders e simula a execução de cada instrução do processador 8080. Ele também simula as interrupções e os periféricos necessários para a execução correta do jogo, como a tela e o controle de entrada.

### ⚙️ Arquitetura do Intel 8080

O Intel 8080 é um processador de 8 bits com 7 registradores de 8 bits, um apontador de pilha e um contador de programa. Este emulador implementa:

- **Conjunto de instruções**: Todas as 256 instruções do 8080, incluindo movimentação de dados, operações lógicas, aritméticas, saltos, chamadas e retornos.
- **Sistema de memória**: Simulação da RAM para suportar o jogo e o espaço de código.
- **Interrupções**: Implementação das interrupções necessárias para o jogo Space Invaders.

