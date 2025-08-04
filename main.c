#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

typedef struct {
    char name[50];
    char symbol;
    int wins;
} Player;

typedef struct {
    int player1_wins;
    int player2_wins;
    int draws;
} Scoreboard;

char board[3][3];

#define MAX_PLAYERS 100

Player leaderboard[MAX_PLAYERS];
int leaderboard_count = 0;

void clear_screen();
void reset_board();
void print_board(Player* p1, Player* p2);
void print_game_screen(Player* p1, Player* p2);
void trim_newline(char* str);
int is_full();
char check_winner();
int human_turn(Player* player);
int get_valid_menu_choice(int min, int max);
int get_move_input_with_quit();
void computer_turn(int difficulty, Player* computer);
void get_player_names(Player* p1, Player* p2, int mode);
void save_game_result(Player* p1, Player* p2, const char* winner);
void show_game_history();
int evaluate_board();
int minimax(int depth, int isMaximizing);
int find_best_move();
void update_leaderboard(Player* p);
void settings_menu(Player* p1, Player* p2);

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void reset_board() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            board[i][j] = ' ';
}

void trim_newline(char* str) {
    str[strcspn(str, "\n")] = '\0';
}

void print_board(Player* p1, Player* p2) {
    printf("\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            char c = board[i][j];
            if (c == p1->symbol)
                printf(" \033[1;34m%c\033[0m ", c);
            else if (c == p2->symbol)
                printf(" \033[1;31m%c\033[0m ", c);
            else
                printf(" \033[1m%c\033[0m ", c);
            if (j < 2) printf("|");
        }
        printf("\n");
        if (i < 2) printf("---|---|---\n");
    }
    printf("\n");
}

void print_game_screen(Player* p1, Player* p2) {
    print_board(p1, p2);
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

int human_turn(Player* player) {
    printf("%s's turn (%c)\n", player->name, player->symbol);
    int move = get_move_input_with_quit();
    if (move == -1) return 1;
    int row = move / 3;
    int col = move % 3;
    board[row][col] = player->symbol;
    return 0;
}

char check_winner() {
    for (int i = 0; i < 3; i++) {
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return board[i][0];
        if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i])
            return board[0][i];
    }
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return board[0][0];
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0])
        return board[0][2];
    return ' ';
}

int is_full() {
    for (int i = 0; i < 9; i++)
        if (board[i / 3][i % 3] == ' ') return 0;
    return 1;
}

int evaluate_board() {
    char winner = check_winner();
    if (winner == 'O') return 10;
    if (winner == 'X') return -10;
    return 0;
}

int minimax(int depth, int isMaximizing) {
    int score = evaluate_board();
    if (score == 10 || score == -10) return score;
    if (is_full()) return 0;

    if (isMaximizing) {
        int best = -1000;
        for (int i = 0; i < 9; i++) {
            int r = i / 3, c = i % 3;
            if (board[r][c] == ' ') {
                board[r][c] = 'O';
                int val = minimax(depth + 1, 0);
                best = best > val ? best : val;
                board[r][c] = ' ';
            }
        }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < 9; i++) {
            int r = i / 3, c = i % 3;
            if (board[r][c] == ' ') {
                board[r][c] = 'X';
                int val = minimax(depth + 1, 1);
                best = best < val ? best : val;
                board[r][c] = ' ';
            }
        }
        return best;
    }
}

int find_best_move() {
    int best_val = -1000, best_move = -1;
    for (int i = 0; i < 9; i++) {
        int r = i / 3, c = i % 3;
        if (board[r][c] == ' ') {
            board[r][c] = 'O';
            int move_val = minimax(0, 0);
            board[r][c] = ' ';
            if (move_val > best_val) {
                best_val = move_val;
                best_move = i;
            }
        }
    }
    return best_move;
}

void computer_turn(int difficulty, Player* computer) {
    int move;
    if (difficulty == 1) {
        int empty_cells[9], count = 0;
        for (int i = 0; i < 9; i++) {
            int r = i / 3, c = i % 3;
            if (board[r][c] == ' ') empty_cells[count++] = i;
        }
        move = empty_cells[rand() % count];
    } else {
        move = find_best_move();
    }
    int r = move / 3, c = move % 3;
    board[r][c] = computer->symbol;
    printf("Computer - %d\n", move + 1);
}

void save_game_result(Player* p1, Player* p2, const char* winner) {
    FILE* f = fopen("game_history.txt", "a");
    if (!f) return;
    fprintf(f, "%s vs %s: %s\n", p1->name, p2->name, winner);
    fclose(f);
}

void show_game_history() {
    FILE* f = fopen("game_history.txt", "r");
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

void get_player_names(Player* p1, Player* p2, int mode) {
    printf("\nEnter name for Player 1 (%c): ", p1->symbol);
    fgets(p1->name, sizeof(p1->name), stdin);
    trim_newline(p1->name);
    if (strlen(p1->name) == 0) strcpy(p1->name, "Player 1");

    if (mode == 2) {
        printf("Enter name for Player 2 (%c): ", p2->symbol);
        fgets(p2->name, sizeof(p2->name), stdin);
        trim_newline(p2->name);
        if (strlen(p2->name) == 0) strcpy(p2->name, "Player 2");
    } else {
        strcpy(p2->name, "Computer");
    }
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

void update_leaderboard(Player* p) {
    if (strcmp(p->name, "Computer") == 0) return;
    for (int i = 0; i < leaderboard_count; i++) {
        if (strcmp(leaderboard[i].name, p->name) == 0) {
            leaderboard[i].wins++;
            return;
        }
    }
    if (leaderboard_count < MAX_PLAYERS) {
        strcpy(leaderboard[leaderboard_count].name, p->name);
        leaderboard[leaderboard_count].wins = 1;
        leaderboard_count++;
    }
}

void settings_menu(Player* p1, Player* p2) {
    clear_screen();
    printf("\n\033[1;36mSETTINGS\033[0m\n\n");
    printf("Symbol Customization!\n\n");
    printf("Current symbols:\n");
    printf("Player 1: %c\n", p1->symbol);
    printf("Player 2: %c\n\n", p2->symbol);

    char input[10];
    printf("Do you want to change symbols? (y/n): ");
    if (!fgets(input, sizeof(input), stdin)) return;
    trim_newline(input);
    if (input[0] != 'y' && input[0] != 'Y') return;

    while (1) {
        printf("\nEnter symbol for Player 1: ");
        if (!fgets(input, sizeof(input), stdin)) return;
        trim_newline(input);
        if (strlen(input) != 1 || input[0] == ' ') {
            printf("Invalid symbol. Try again.\n");
            continue;
        }
        if (input[0] == p2->symbol) {
            printf("Symbol already used by Player 2. Choose another.\n");
            continue;
        }
        p1->symbol = input[0];
        break;
    }

    while (1) {
        printf("Enter symbol for Player 2: ");
        if (!fgets(input, sizeof(input), stdin)) return;
        trim_newline(input);
        if (strlen(input) != 1 || input[0] == ' ') {
            printf("Invalid symbol. Try again.\n");
            continue;
        }
        if (input[0] == p1->symbol) {
            printf("Symbol already used by Player 1. Choose another.\n");
            continue;
        }
        p2->symbol = input[0];
        break;
    }

    printf("\nSymbols updated successfully!\n\nPress ENTER to continue...");
    getchar();
}

int main() {
    srand(time(NULL));
    char again[10];
    int menu, mode, difficulty;
    Player p1 = {.symbol = 'X'};
    Player p2 = {.symbol = 'O'};

    while (1) {
        clear_screen();
        printf("\n\033[1;32mTIC TAC TOE\033[0m\n\n");
        printf("1. Play Game\n2. Game History\n3. Game Guide\n4. Settings\n5. Exit\n\nChoice: ");
        menu = get_valid_menu_choice(1, 5);

        if (menu == 1) {
            Scoreboard scoreboard = {0, 0, 0};
            printf("\nSelect mode:\n1. Player vs Computer\n2. Two Player\n\nChoice: ");
            mode = get_valid_menu_choice(1, 2);

            if (mode == 1) {
                printf("\nSelect difficulty:\n1. Easy\n2. Hard\n\nChoice: ");
                difficulty = get_valid_menu_choice(1, 2);
            }

            get_player_names(&p1, &p2, mode);

            do {
                reset_board();
                int quit_game = 0, turn = 1;
                char winner = ' ';
                clear_screen();
                print_game_screen(&p1, &p2);

                while (1) {
                    if (mode == 1) {
                        if (turn % 2 == 1)
                            quit_game = human_turn(&p1);
                        else
                            computer_turn(difficulty, &p2);
                    } else {
                        Player* current = (turn % 2 == 1) ? &p1 : &p2;
                        quit_game = human_turn(current);
                    }

                    if (quit_game) break;
                    clear_screen();
                    print_game_screen(&p1, &p2);
                    winner = check_winner();
                    if (winner != ' ' || is_full()) break;
                    turn++;
                }

                if (!quit_game) {
                    if (winner == p1.symbol) {
                        printf("\033[1;32m%s wins!\033[0m\n", p1.name);
                        save_game_result(&p1, &p2, p1.name);
                        scoreboard.player1_wins++;
                        update_leaderboard(&p1);
                    } else if (winner == p2.symbol) {
                        printf("\033[1;32m%s wins!\033[0m\n", p2.name);
                        save_game_result(&p1, &p2, p2.name);
                        scoreboard.player2_wins++;
                        update_leaderboard(&p2);
                    } else {
                        printf("\033[1;33mIt's a draw!\033[0m\n");
                        save_game_result(&p1, &p2, "Draw");
                        scoreboard.draws++;
                    }

                    printf("\n\033[1;36mSCOREBOARD\033[0m\n");
                    printf("%s: %d\n", p1.name, scoreboard.player1_wins);
                    printf("%s: %d\n", p2.name, scoreboard.player2_wins);
                    printf("Draws: %d\n", scoreboard.draws);

                    printf("\nPlay again? (y/n): ");
                    fgets(again, sizeof(again), stdin);
                    trim_newline(again);
                    if (again[0] != 'y' && again[0] != 'Y') break;
                } else {
                    break;
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
            settings_menu(&p1, &p2);
            continue;
        }

        if (menu == 5) {
            printf("\nThanks for playing!\n");
            break;
        }
    }

    return 0;
}
