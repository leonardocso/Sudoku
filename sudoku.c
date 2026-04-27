#include <curses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sudoku.h"

#define BOARD_SIZE 9
#define CELL_WIDTH 4
#define CELL_HEIGHT 2
#define PUZZLE_COUNT 3

typedef struct {
    int puzzle[BOARD_SIZE][BOARD_SIZE];
    int solution[BOARD_SIZE][BOARD_SIZE];
} Puzzle;

typedef struct {
    int board[BOARD_SIZE][BOARD_SIZE];
    bool fixed[BOARD_SIZE][BOARD_SIZE];
    int solution[BOARD_SIZE][BOARD_SIZE];
    int selected_row;
    int selected_col;
    int mistakes;
    bool finished;
} GameState;

static const Puzzle kPuzzles[PUZZLE_COUNT] = {
    {
        {
            {5, 3, 0, 0, 7, 0, 0, 0, 0},
            {6, 0, 0, 1, 9, 5, 0, 0, 0},
            {0, 9, 8, 0, 0, 0, 0, 6, 0},
            {8, 0, 0, 0, 6, 0, 0, 0, 3},
            {4, 0, 0, 8, 0, 3, 0, 0, 1},
            {7, 0, 0, 0, 2, 0, 0, 0, 6},
            {0, 6, 0, 0, 0, 0, 2, 8, 0},
            {0, 0, 0, 4, 1, 9, 0, 0, 5},
            {0, 0, 0, 0, 8, 0, 0, 7, 9}
        },
        {
            {5, 3, 4, 6, 7, 8, 9, 1, 2},
            {6, 7, 2, 1, 9, 5, 3, 4, 8},
            {1, 9, 8, 3, 4, 2, 5, 6, 7},
            {8, 5, 9, 7, 6, 1, 4, 2, 3},
            {4, 2, 6, 8, 5, 3, 7, 9, 1},
            {7, 1, 3, 9, 2, 4, 8, 5, 6},
            {9, 6, 1, 5, 3, 7, 2, 8, 4},
            {2, 8, 7, 4, 1, 9, 6, 3, 5},
            {3, 4, 5, 2, 8, 6, 1, 7, 9}
        }
    },
    {
        {
            {0, 2, 0, 6, 0, 8, 0, 0, 0},
            {5, 8, 0, 0, 0, 9, 7, 0, 0},
            {0, 0, 0, 0, 4, 0, 0, 0, 0},
            {3, 7, 0, 0, 0, 0, 5, 0, 0},
            {6, 0, 0, 0, 0, 0, 0, 0, 4},
            {0, 0, 8, 0, 0, 0, 0, 1, 3},
            {0, 0, 0, 0, 2, 0, 0, 0, 0},
            {0, 0, 9, 8, 0, 0, 0, 3, 6},
            {0, 0, 0, 3, 0, 6, 0, 9, 0}
        },
        {
            {1, 2, 3, 6, 7, 8, 9, 4, 5},
            {5, 8, 4, 2, 3, 9, 7, 6, 1},
            {9, 6, 7, 1, 4, 5, 3, 2, 8},
            {3, 7, 2, 4, 6, 1, 5, 8, 9},
            {6, 9, 1, 5, 8, 3, 2, 7, 4},
            {4, 5, 8, 7, 9, 2, 6, 1, 3},
            {8, 3, 6, 9, 2, 4, 1, 5, 7},
            {2, 1, 9, 8, 5, 7, 4, 3, 6},
            {7, 4, 5, 3, 1, 6, 8, 9, 2}
        }
    },
    {
        {
            {0, 0, 0, 2, 6, 0, 7, 0, 1},
            {6, 8, 0, 0, 7, 0, 0, 9, 0},
            {1, 9, 0, 0, 0, 4, 5, 0, 0},
            {8, 2, 0, 1, 0, 0, 0, 4, 0},
            {0, 0, 4, 6, 0, 2, 9, 0, 0},
            {0, 5, 0, 0, 0, 3, 0, 2, 8},
            {0, 0, 9, 3, 0, 0, 0, 7, 4},
            {0, 4, 0, 0, 5, 0, 0, 3, 6},
            {7, 0, 3, 0, 1, 8, 0, 0, 0}
        },
        {
            {4, 3, 5, 2, 6, 9, 7, 8, 1},
            {6, 8, 2, 5, 7, 1, 4, 9, 3},
            {1, 9, 7, 8, 3, 4, 5, 6, 2},
            {8, 2, 6, 1, 9, 5, 3, 4, 7},
            {3, 7, 4, 6, 8, 2, 9, 1, 5},
            {9, 5, 1, 7, 4, 3, 6, 2, 8},
            {5, 1, 9, 3, 2, 6, 8, 7, 4},
            {2, 4, 8, 9, 5, 7, 1, 3, 6},
            {7, 6, 3, 4, 1, 8, 2, 5, 9}
        }
    }
};

static void copy_grid(int dest[BOARD_SIZE][BOARD_SIZE],
                      const int src[BOARD_SIZE][BOARD_SIZE])
{
    memcpy(dest, src, sizeof(int) * BOARD_SIZE * BOARD_SIZE);
}

static void init_colors(void)
{
    if (!has_colors()) {
        return;
    }

    start_color();
    use_default_colors();
    init_pair(1, COLOR_WHITE, -1);
    init_pair(2, COLOR_CYAN, -1);
    init_pair(3, COLOR_YELLOW, -1);
    init_pair(4, COLOR_BLACK, COLOR_WHITE);
    init_pair(5, COLOR_GREEN, -1);
    init_pair(6, COLOR_RED, -1);
}

static void start_new_game(GameState *game)
{
    int index = rand() % PUZZLE_COUNT;
    const Puzzle *chosen = &kPuzzles[index];

    copy_grid(game->solution, chosen->solution);

    // Mapeia os números de 1 a 9 aleatoriamente para gerar uma nova solução
    int map[10];
    for (int i = 1; i <= 9; ++i) map[i] = i;
    for (int i = 1; i <= 9; ++i) {
        int j = i + rand() % (10 - i);
        int temp = map[i]; map[i] = map[j]; map[j] = temp;
    }
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            game->solution[r][c] = map[game->solution[r][c]];
        }
    }

    // Embaralha as linhas dentro de cada bloco 3x3
    for (int band = 0; band < 3; ++band) {
        for (int i = 0; i < 3; ++i) {
            int r1 = band * 3 + i;
            int r2 = band * 3 + rand() % 3;
            for (int c = 0; c < BOARD_SIZE; ++c) {
                int temp = game->solution[r1][c];
                game->solution[r1][c] = game->solution[r2][c];
                game->solution[r2][c] = temp;
            }
        }
    }

    // Embaralha as colunas dentro de cada bloco 3x3
    for (int stack = 0; stack < 3; ++stack) {
        for (int i = 0; i < 3; ++i) {
            int c1 = stack * 3 + i;
            int c2 = stack * 3 + rand() % 3;
            for (int r = 0; r < BOARD_SIZE; ++r) {
                int temp = game->solution[r][c1];
                game->solution[r][c1] = game->solution[r][c2];
                game->solution[r][c2] = temp;
            }
        }
    }

    copy_grid(game->board, game->solution);

    // Remove células para criar um puzzle de nível intermediário (dando mais dicas, ~40 dicas)
    int cells_to_remove = 41;
    while (cells_to_remove > 0) {
        int r = rand() % BOARD_SIZE;
        int c = rand() % BOARD_SIZE;
        if (game->board[r][c] != 0) {
            game->board[r][c] = 0;
            cells_to_remove--;
        }
    }

    game->selected_row = 0;
    game->selected_col = 0;
    game->mistakes = 0;
    game->finished = false;

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            game->fixed[row][col] = game->board[row][col] != 0;
        }
    }
}

static bool is_completed(const GameState *game)
{
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (game->board[row][col] != game->solution[row][col]) {
                return false;
            }
        }
    }
    return true;
}

static bool has_conflict(const GameState *game, int row, int col)
{
    int value = game->board[row][col];
    int start_row = (row / 3) * 3;
    int start_col = (col / 3) * 3;

    if (value == 0) {
        return false;
    }

    for (int current = 0; current < BOARD_SIZE; ++current) {
        if (current != col && game->board[row][current] == value) {
            return true;
        }
        if (current != row && game->board[current][col] == value) {
            return true;
        }
    }

    for (int r = start_row; r < start_row + 3; ++r) {
        for (int c = start_col; c < start_col + 3; ++c) {
            if ((r != row || c != col) && game->board[r][c] == value) {
                return true;
            }
        }
    }

    return false;
}

static void draw_box_border(int top, int left, int height, int width)
{
    mvaddch(top, left, ACS_ULCORNER);
    mvaddch(top, left + width, ACS_URCORNER);
    mvaddch(top + height, left, ACS_LLCORNER);
    mvaddch(top + height, left + width, ACS_LRCORNER);

    for (int x = left + 1; x < left + width; ++x) {
        mvaddch(top, x, ACS_HLINE);
        mvaddch(top + height, x, ACS_HLINE);
    }

    for (int y = top + 1; y < top + height; ++y) {
        mvaddch(y, left, ACS_VLINE);
        mvaddch(y, left + width, ACS_VLINE);
    }
}

static void draw_grid(int top, int left)
{
    int width = BOARD_SIZE * CELL_WIDTH;
    int height = BOARD_SIZE * CELL_HEIGHT;

    draw_box_border(top, left, height, width);

    for (int row = 1; row < BOARD_SIZE; ++row) {
        int y = top + row * CELL_HEIGHT;
        chtype horizontal = (row % 3 == 0) ? ACS_HLINE | A_BOLD : ACS_HLINE;

        for (int x = left + 1; x < left + width; ++x) {
            mvaddch(y, x, horizontal);
        }
    }

    for (int col = 1; col < BOARD_SIZE; ++col) {
        int x = left + col * CELL_WIDTH;
        chtype vertical = (col % 3 == 0) ? ACS_VLINE | A_BOLD : ACS_VLINE;

        for (int y = top + 1; y < top + height; ++y) {
            mvaddch(y, x, vertical);
        }
    }

    for (int row = 1; row < BOARD_SIZE; ++row) {
        for (int col = 1; col < BOARD_SIZE; ++col) {
            int y = top + row * CELL_HEIGHT;
            int x = left + col * CELL_WIDTH;
            bool thick_row = (row % 3 == 0);
            bool thick_col = (col % 3 == 0);

            mvaddch(y, x, (thick_row && thick_col) ? (ACS_PLUS | A_BOLD) : ACS_PLUS);
        }
    }
}

static void draw_board(const GameState *game)
{
    const int top = 2;
    const int left = 4;

    clear();
    mvprintw(0, 4, "Sudoku em C com PDCurses");
    mvprintw(1, 4, "Setas movem | 1-9 preenchem | 0/Backspace apaga | N novo | R reinicia | Q sai");

    draw_grid(top, left);

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            int value = game->board[row][col];
            int y = top + row * CELL_HEIGHT + 1;
            int x = left + col * CELL_WIDTH + 2;
            bool selected = row == game->selected_row && col == game->selected_col;
            bool fixed = game->fixed[row][col];
            bool conflict = has_conflict(game, row, col);
            int color = 1;

            if (fixed) {
                color = 2;
            } else if (conflict) {
                color = 6;
            } else if (value != 0) {
                color = 3;
            }

            if (selected) {
                attron(COLOR_PAIR(4) | A_BOLD);
                mvaddch(y, x - 1, ' ');
                mvaddch(y, x, value == 0 ? '.' : '0' + value);
                mvaddch(y, x + 1, ' ');
                attroff(COLOR_PAIR(4) | A_BOLD);
                continue;
            }

            attron(COLOR_PAIR(color) | (fixed ? A_BOLD : A_NORMAL));
            mvaddch(y, x, value == 0 ? '.' : '0' + value);
            attroff(COLOR_PAIR(color) | (fixed ? A_BOLD : A_NORMAL));
        }
    }

    mvprintw(5, 45, "Erros: %d", game->mistakes);

    if (game->finished) {
        attron(COLOR_PAIR(5) | A_BOLD);
        mvprintw(7, 45, "Parabens! Sudoku concluido.");
        attroff(COLOR_PAIR(5) | A_BOLD);
    } else {
        mvprintw(7, 45, "Preencha todo o tabuleiro.");
    }

    mvprintw(10, 45, "Legenda:");
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(11, 45, "Azul");
    attroff(COLOR_PAIR(2) | A_BOLD);
    mvprintw(11, 51, "= pista fixa");

    attron(COLOR_PAIR(3));
    mvprintw(12, 45, "Amarelo");
    attroff(COLOR_PAIR(3));
    mvprintw(12, 54, "= valor digitado");

    attron(COLOR_PAIR(6));
    mvprintw(13, 45, "Vermelho");
    attroff(COLOR_PAIR(6));
    mvprintw(13, 55, "= conflito");

    mvprintw(16, 45, "Objetivo:");
    mvprintw(17, 45, "Cada linha, coluna e bloco 3x3");
    mvprintw(18, 45, "deve conter os numeros de 1 a 9.");

    refresh();
}

static void reset_current_puzzle(GameState *game)
{
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (!game->fixed[row][col]) {
                game->board[row][col] = 0;
            }
        }
    }

    game->mistakes = 0;
    game->finished = false;
}

static void handle_digit(GameState *game, int digit)
{
    int row = game->selected_row;
    int col = game->selected_col;

    if (game->fixed[row][col] || game->finished) {
        return;
    }

    game->board[row][col] = digit;

    if (digit != 0 && digit != game->solution[row][col]) {
        game->mistakes++;
    }

    if (is_completed(game)) {
        game->finished = true;
    }
}

static void move_selection(GameState *game, int key)
{
    switch (key) {
        case KEY_UP:
            game->selected_row = (game->selected_row + BOARD_SIZE - 1) % BOARD_SIZE;
            break;
        case KEY_DOWN:
            game->selected_row = (game->selected_row + 1) % BOARD_SIZE;
            break;
        case KEY_LEFT:
            game->selected_col = (game->selected_col + BOARD_SIZE - 1) % BOARD_SIZE;
            break;
        case KEY_RIGHT:
            game->selected_col = (game->selected_col + 1) % BOARD_SIZE;
            break;
    }
}

void run_sudoku(void)
{
    GameState game;
    int key;

    srand((unsigned int)time(NULL));

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    init_colors();
    start_new_game(&game);

    while (true) {
        draw_board(&game);
        key = getch();

        if (key == 'q' || key == 'Q' || key == 27) {
            break;
        }

        if (key == 'n' || key == 'N') {
            start_new_game(&game);
            continue;
        }

        if (key == 'r' || key == 'R') {
            reset_current_puzzle(&game);
            continue;
        }

        if (key >= '1' && key <= '9') {
            handle_digit(&game, key - '0');
            continue;
        }

        if (key == '0' || key == KEY_BACKSPACE || key == 8 || key == 127 || key == KEY_DC) {
            handle_digit(&game, 0);
            continue;
        }

        move_selection(&game, key);
    }

    endwin();
}