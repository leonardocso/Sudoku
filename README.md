# Sudoku em C com PDCurses

Jogo de Sudoku para terminal em Windows, escrito em C e usando a biblioteca PDCurses.

## Funcionalidades

- Tabuleiro 9x9 jogavel no terminal
- Navegacao por setas
- Numeros fixos destacados
- Conflitos visuais em vermelho
- Reinicio do puzzle atual
- Sorteio de um novo puzzle

## Controles

- `Setas`: mover a selecao
- `1` a `9`: preencher a celula
- `0`, `Backspace` ou `Delete`: limpar a celula
- `N`: novo jogo
- `R`: reiniciar puzzle atual
- `Q` ou `Esc`: sair

## Compilacao no Windows

Com MinGW e PDCurses instalado:

```bat
gcc main.c sudoku.c -o sudoku.exe -lpdcurses
```

Se a sua distribuicao usar outro nome para a biblioteca, troque o parametro final, por exemplo:

```bat
gcc main.c sudoku.c -o sudoku.exe -lpdcurseswin
```
