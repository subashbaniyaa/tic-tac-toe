#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define HUMAN 1
#define AI 2

char board[3][3];
int turn_count = 1;
int mode = 1;

char player1_name[50] = "Player 1 (X)";
char player2_name[50] = "Player 2 (O)";

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
}

int get_valid_menu_choice(int min, int max) {
    char input[20];
    int choice;
    while (1) {
        if (!fgets(input, sizeof(input), stdin)) continue;
        trim_newline(input);
        if (sscanf(input, "%d", &choice) != 1) {
            printf("Invalid input! Enter a number between %d and %d: ", min, max);
            continue;
        }
        if (choice < min || choice > max) {
            printf("Invalid choice! Enter a number between %d and %d: ", min, max);
            continue;
        }
        return choice;
    }
}

void reset_board(char b[3][3]) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            b[i][j] = ' ';
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
            if (j < 2)
                printf("\033[1m|\033[0m");
        }
        printf("\n");
        if (i < 2)
            printf("\033[1m---|---|---\033[0m\n");
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

int human_turn(int player) {
    int move;
    if (mode == 2) {
        char clean_name[50];
        strncpy(clean_name, (player == HUMAN) ? player1_name : player2_name, sizeof(clean_name));
        clean_name[sizeof(clean_name)-1] = '\0';
        char *p = strstr(clean_name, " (");
        if (p) *p = '\0';

        char symbol = (player == HUMAN) ? 'X' : 'O';
        printf("%s's turn (\033[1;%dm%c\033[0m)\n", clean_name, (symbol == 'X') ? 34 : 31, symbol);
    } else {
        printf("Your turn (\033[1;34mX\033[0m)\n");
    }
    move = get_move_input_with_quit();
    if (move == -1) return 1;
    int row = move / 3;
    int col = move % 3;
    board[row][col] = (player == HUMAN) ? 'X' : 'O';
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
    for (int i = 0; i < 8; i++) {
        if (lines[i][0] != ' ' && lines[i][0] == lines[i][1] && lines[i][1] == lines[i][2]) {
            return (lines[i][0] == 'X') ? HUMAN : AI;
        }
    }
    return 0;
}

int is_full() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] == ' ') return 0;
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
            if (is_ai) {
                if (score > best_score) best_score = score;
            } else {
                if (score < best_score) best_score = score;
            }
        }
    }
    return best_score;
}

void ai_turn() {
    int best_score = -1000;
    int best_move = -1;
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
        int r = best_move / 3;
        int c = best_move % 3;
        board[r][c] = 'O';
    }
}

void save_game_result(const char *winner) {
    FILE *file = fopen("game_history.txt", "a");
    if (!file) return;
    fprintf(file, "%s vs %s: Winner - %s\n", player1_name, player2_name, winner);
    fclose(file);
}

void show_game_history() {
    FILE *file = fopen("game_history.txt", "r");
    char line[200];
    if (!file) {
        printf("\nNo game history found.\n");
        return;
    }
    printf("\n\033[1;34m---------- GAME HISTORY ----------\033[0m\n");
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
}

void get_player_names() {
    printf("Enter name for Player 1 (X), or press ENTER to skip: ");
    if (!fgets(player1_name, sizeof(player1_name), stdin)) {
        strcpy(player1_name, "Player 1 (X)");
    }
    trim_newline(player1_name);
    if (strlen(player1_name) == 0) strcpy(player1_name, "Player 1 (X)");

    printf("Enter name for Player 2 (O), or press ENTER to skip: ");
    if (!fgets(player2_name, sizeof(player2_name), stdin)) {
        strcpy(player2_name, "Player 2 (O)");
    }
    trim_newline(player2_name);
    if (strlen(player2_name) == 0) strcpy(player2_name, "Player 2 (O)");
}

int main() {
    srand(time(NULL));
    char again_input[10];
    int menu_choice;

    while (1) {
        clear_screen();
        printf("\n\033[1;32mTIC TAC TOE\033[0m\n\n");
        printf("1. Play Game\n2. Game History\n3. Exit\n\nChoice: ");

        menu_choice = get_valid_menu_choice(1, 3);

        switch (menu_choice) {
            case 1:
                printf("\nSelect mode:\n1. Player vs Computer\n2. Two Player\n\nChoice: ");
                mode = get_valid_menu_choice(1, 2);

                if (mode == 2) {
                    get_player_names();
                }

                do {
                    int winner = 0, quit_flag = 0;
                    turn_count = 1;
                    reset_board(board);
                    clear_screen();
                    print_game_screen();

                    while (1) {
                        if (mode == 1) {
                            if (turn_count % 2 == 1)
                                quit_flag = human_turn(HUMAN);
                            else
                                ai_turn();
                        } else {
                            int current_player = (turn_count % 2 == 1) ? HUMAN : AI;
                            quit_flag = human_turn(current_player);
                        }

                        if (quit_flag) {
                            printf("\nReturning to main menu...\n");
                            break;
                        }

                        clear_screen();
                        print_game_screen();

                        winner = check_winner();
                        if (winner != 0 || is_full()) break;

                        turn_count++;
                    }

                    if (quit_flag) break;

                    if (mode == 1) {
                        if (winner == HUMAN) {
                            printf("\nYou win!\n");
                        } else if (winner == AI) {
                            printf("\nComputer wins!\n");
                        } else {
                            printf("\nIt's a draw!\n");
                        }
                    } else {
                        if (winner == HUMAN) {
                            printf("\n%s wins!\n", player1_name);
                            save_game_result(player1_name);
                        } else if (winner == AI) {
                            printf("\n%s wins!\n", player2_name);
                            save_game_result(player2_name);
                        } else {
                            printf("\nIt's a draw!\n");
                            save_game_result("Draw");
                        }
                    }

                    printf("\nPlay again in the same mode? (y/n): ");
                    if (!fgets(again_input, sizeof(again_input), stdin)) break;
                    trim_newline(again_input);

                } while (again_input[0] == 'y' || again_input[0] == 'Y');

                break;

            case 2:
                show_game_history();
                printf("\n\nPress ENTER to continue...");
                fgets(again_input, sizeof(again_input), stdin);
                break;

            case 3:
                printf("\nThanks for playing!\n");
                return 0;
        }
    }
    return 0;
}
