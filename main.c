#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define HUMAN 1
#define AI 2

typedef struct {
    char name[50];
    char symbol;
} Player;

char board[3][3];

void clear_screen();
void reset_board(char b[3][3]);
void print_board();
void print_game_screen();
void trim_newline(char *str);
int is_full();
int check_winner();
int human_turn(Player *player);
int get_valid_menu_choice(int min, int max);
int get_move_input_with_quit();
void ai_turn();
int minimax(int is_ai);
void get_player_names(Player *p1, Player *p2);
void save_game_result(Player *p1, Player *p2, const char *winner);
void show_game_history();

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void reset_board(char b[3][3]) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            b[i][j] = ' ';
}

void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
        str[len - 1] = '\0';
}

void print_board() {
    printf("\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            char c = board[i][j];
            if (c == 'X')
                printf(" \033[1;34m%c\033[0m ", c);
            else if (c == 'O')
                printf(" \033[1;31m%c\033[0m ", c);
            else
                printf(" \033[1m%c\033[0m ", c);
            if (j < 2) printf("\033[1m|\033[0m");
        }
        printf("\n");
        if (i < 2) printf("\033[1m---|---|---\033[0m\n");
    }
    printf("\n");
}

void print_game_screen() {
    print_board();
}

int get_move_input_with_quit() {
    char input[20];
    int move;
    while (1) {
        printf("Enter your move (1-9): ");
        if (!fgets(input, sizeof(input), stdin)) return -1;
        trim_newline(input);
        if (strcmp(input, "quit") == 0) return -1;
        if (sscanf(input, "%d", &move) != 1 || move < 1 || move > 9) {
            printf("Invalid input! Try again.\n");
            continue;
        }
        int row = (move - 1) / 3;
        int col = (move - 1) % 3;
        if (board[row][col] != ' ') {
            printf("Cell already taken! Try again.\n");
            continue;
        }
        return move - 1;
    }
}

int human_turn(Player *player) {
    printf("%s's turn (%c)\n", player->name, player->symbol);
    int move = get_move_input_with_quit();
    if (move == -1) return 1;
    int row = move / 3;
    int col = move % 3;
    board[row][col] = player->symbol;
    return 0;
}

int check_winner() {
    char lines[8][3] = {
        {board[0][0], board[0][1], board[0][2]},
        {board[1][0], board[1][1], board[1][2]},
        {board[2][0], board[2][1], board[2][2]},
        {board[0][0], board[1][0], board[2][0]},
        {board[0][1], board[1][1], board[2][1]},
        {board[0][2], board[1][2], board[2][2]},
        {board[0][0], board[1][1], board[2][2]},
        {board[0][2], board[1][1], board[2][0]},
    };
    
    for (int i = 0; i < 8; i++)
        if (lines[i][0] != ' ' && lines[i][0] == lines[i][1] && lines[i][1] == lines[i][2])
            return (lines[i][0] == 'X') ? HUMAN : AI;
    return 0;
}

int is_full() {
    for (int i = 0; i < 9; i++)
        if (board[i / 3][i % 3] == ' ') return 0;
    return 1;
}

int minimax(int is_ai) {
    int winner = check_winner();
    if (winner == AI) return 10;
    if (winner == HUMAN) return -10;
    if (is_full()) return 0;

    int best_score = is_ai ? -1000 : 1000;
    for (int i = 0; i < 9; i++) {
        int r = i / 3, c = i % 3;
        if (board[r][c] == ' ') {
            board[r][c] = is_ai ? 'O' : 'X';
            int score = minimax(!is_ai);
            board[r][c] = ' ';
            if (is_ai) best_score = (score > best_score) ? score : best_score;
            else best_score = (score < best_score) ? score : best_score;
        }
    }
    return best_score;
}

void ai_turn() {
    int best_score = -1000, best_move = -1;
    for (int i = 0; i < 9; i++) {
        int r = i / 3, c = i % 3;
        if (board[r][c] == ' ') {
            board[r][c] = 'O';
            int score = minimax(0);
            board[r][c] = ' ';
            if (score > best_score) {
                best_score = score;
                best_move = i;
            }
        }
    }
    if (best_move != -1) {
        int r = best_move / 3, c = best_move % 3;
        board[r][c] = 'O';
    }
}

void save_game_result(Player *p1, Player *p2, const char *winner) {
    FILE *f = fopen("game_history.txt", "a");
    if (f) {
        fprintf(f, "%s vs %s: Winner - %s\n", p1->name, p2->name, winner);
        fclose(f);
    }
}

void show_game_history() {
    FILE *f = fopen("game_history.txt", "r");
    char line[100];
    clear_screen();
    printf("\n\033[1;36mGAME HISTORY\033[0m\n\n");
    if (!f) {
        printf("No game history found!\n");
    } else {
        while (fgets(line, sizeof(line), f)) printf("%s", line);
        fclose(f);
    }
    printf("\nPress ENTER to continue...");
    getchar();
}

void get_player_names(Player *p1, Player *p2) {
    printf("\nEnter name for Player 1 (X): ");
    fgets(p1->name, sizeof(p1->name), stdin);
    trim_newline(p1->name);
    if (strlen(p1->name) == 0) strcpy(p1->name, "Player 1");

    printf("Enter name for Player 2 (O): ");
    fgets(p2->name, sizeof(p2->name), stdin);
    trim_newline(p2->name);
    if (strlen(p2->name) == 0) strcpy(p2->name, "Player 2");

    p1->symbol = 'X';
    p2->symbol = 'O';
}

int get_valid_menu_choice(int min, int max) {
    char input[20];
    int choice;
    while (1) {
        if (!fgets(input, sizeof(input), stdin)) continue;
        trim_newline(input);
        if (sscanf(input, "%d", &choice) != 1 || choice < min || choice > max)
            printf("Invalid! Enter %d - %d: ", min, max);
        else
            return choice;
    }
}

int main() {
    srand(time(NULL));
    char again[10];
    int menu, mode;

    Player p1, p2;
    strcpy(p1.name, "Player 1");
    strcpy(p2.name, "Player 2");
    p1.symbol = 'X';
    p2.symbol = 'O';

    while (1) {
        clear_screen();
        printf("\n\033[1;32mTIC TAC TOE\033[0m\n\n");
        printf("1. Play Game\n2. Game History\n3. Game Guide\n4. Exit\n\nChoice: ");
        menu = get_valid_menu_choice(1, 4);

        if (menu == 1) {
            printf("\nSelect mode:\n1. Player vs Computer\n2. Two Player\n\nChoice: ");
            mode = get_valid_menu_choice(1, 2);
            if (mode == 2) get_player_names(&p1, &p2);

            do {
                reset_board(board);
                int winner = 0, quit = 0, turn = 1;
                clear_screen();
                print_game_screen();

                while (1) {
                    if (mode == 1) {
                        if (turn % 2 == 1)
                            quit = human_turn(&p1);
                        else
                            ai_turn();
                    } else {
                        Player *current = (turn % 2 == 1) ? &p1 : &p2;
                        quit = human_turn(current);
                    }

                    if (quit) break;

                    clear_screen();
                    print_game_screen();
                    winner = check_winner();
                    if (winner || is_full()) break;
                    turn++;
                }

                if (!quit) {
                    if (mode == 1) {
                        if (winner == HUMAN) printf("You win!\n");
                        else if (winner == AI) printf("Computer wins!\n");
                        else printf("It's a draw!\n");
                    } else {
                        if (winner == HUMAN) {
                            printf("%s wins!\n", p1.name);
                            save_game_result(&p1, &p2, p1.name);
                        } else if (winner == AI) {
                            printf("%s wins!\n", p2.name);
                            save_game_result(&p1, &p2, p2.name);
                        } else {
                            printf("It's a draw!\n");
                            save_game_result(&p1, &p2, "Draw");
                        }
                    }
                    printf("\nPlay again? (y/n): ");
                    fgets(again, sizeof(again), stdin);
                    trim_newline(again);
                    if (again[0] != 'y' && again[0] != 'Y') break;
                }
            } while (1);

            continue;
        }

        if (menu == 2) {
            show_game_history();
            continue;
        }

        if (menu == 3) {
            clear_screen();
            printf("\n\033[1;36mGAME GUIDE\033[0m\n\n");
            printf("1. Get 3 of your marks in a row to win.\n");
            printf("2. Type 'quit' to return to home.\n");

            printf("\nReference Board:\n\n");
            printf("  1 | 2 | 3\n");
            printf(" ---|---|---\n");
            printf("  4 | 5 | 6\n");
            printf(" ---|---|---\n");
            printf("  7 | 8 | 9\n");
            printf("\nPress ENTER to continue...");
            char temp[10];
            fgets(temp, sizeof(temp), stdin);
            continue;
        }

        if (menu == 4) {
            printf("\nThanks for playing!\n");
            break;
        }
    }

    return 0;
}
