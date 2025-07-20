#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HUMAN 1
#define AI 2

char board[3][3];
int turn_count = 1;
int last_move = -1;
int AI_last_move = -1;
int mode = 1;

char player1_name[50] = "Player 1";
char player2_name[50] = "Player 2";

void print_board();
void print_board_guide();
void human_turn(int player);
void ai_turn();
int check_winner();
int is_full();
void reset_board();
int get_best_move_minimax();
int minimax(int is_ai);
void get_player_names();
void trim_newline(char *str);

int main() {
    char again;

    while (1) {
        printf("\n\nTIC TAC TOE\n\n");
        printf("Select mode:\n1. Single Player (vs Computer)\n2. Two Player\n\nChoice: ");
        if (scanf("%d", &mode) != 1) {
            while (getchar() != '\n');
            printf("Invalid input! Defaulting to Single Player mode.\n");
            mode = 1;
        }
        getchar();

        if (mode == 2) {
            get_player_names();
        }

        do {
            int winner;
            turn_count = 1;
            reset_board();
            print_board_guide();
            print_board();

            while (1) {
                switch (mode) {
                    case 1:
                        if (turn_count % 2 == 1)
                            human_turn(HUMAN);
                        else
                            ai_turn();
                        break;

                    case 2: {
                        int current_player = (turn_count % 2 == 1) ? HUMAN : AI;
                        human_turn(current_player);
                        break;
                    }

                    default:
                        printf("Invalid mode selected. Exiting.\n");
                        return 1;
                }

                print_board();

                winner = check_winner();
                if (winner != 0 || is_full()) {
                    break;
                }

                turn_count++;
            }

            if (mode == 1) {
                if (winner == HUMAN)
                    printf("\nYou win!\n");
                else if (winner == AI)
                    printf("\nComputer wins!\n");
                else
                    printf("\nIt's a draw!\n");
            } else {
                if (winner == HUMAN)
                    printf("\n%s wins!\n", player1_name);
                else if (winner == AI)
                    printf("\n%s wins!\n", player2_name);
                else
                    printf("\nIt's a draw!\n");
            }

            printf("\nPlay again in the same mode? (y/n): ");
            scanf(" %c", &again);
            getchar();

        } while (again == 'y' || again == 'Y');
    }

    return 0;
}

void reset_board() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            board[i][j] = ' ';
    last_move = -1;
    AI_last_move = -1;
}

void print_board() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    printf("\n");
    for (int i = 0; i < 3; i++) {
        printf(" %c | %c | %c \n", board[i][0], board[i][1], board[i][2]);
        if (i < 2) printf("---|---|---\n");
    }
    printf("\n");
}

void print_board_guide() {
    printf("\nBoard positions:\n\n");
    printf(" 1 | 2 | 3\n");
    printf("---|---|---\n");
    printf(" 4 | 5 | 6\n");
    printf("---|---|---\n");
    printf(" 7 | 8 | 9\n\n");
}

void human_turn(int player) {
    int move;
    while (1) {
        if (mode == 2) {
            printf("%s (%c), enter your move (1-9): ",
                   (player == HUMAN ? player1_name : player2_name),
                   (player == HUMAN ? 'X' : 'O'));
        } else {
            printf("Player %d (%c), enter your move (1-9): ",
                   player, (player == HUMAN ? 'X' : 'O'));
        }

        if (scanf("%d", &move) != 1) {
            while (getchar() != '\n');
            printf("Invalid input! Enter a number between 1-9.\n");
            continue;
        }

        move--;
        int row = move / 3;
        int col = move % 3;

        if (move >= 0 && move < 9 && board[row][col] == ' ') {
            board[row][col] = (player == HUMAN) ? 'X' : 'O';
            last_move = move;
            break;
        } else {
            printf("Invalid move! Try again.\n");
        }
    }
}

void ai_turn() {
    int move = get_best_move_minimax();

    if (move == -1) {
        fprintf(stderr, "Warning: No valid move found.\n");
        return;
    }

    int row = move / 3;
    int col = move % 3;
    board[row][col] = 'O';
    AI_last_move = move;
    printf("Computer chose: %d\n", move + 1);
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
            if (board[i][j] == ' ')
                return 0;
    return 1;
}

int get_best_move_minimax() {
    int best_score = -1000;
    int best_move = -1;

    for (int i = 0; i < 9; i++) {
        int row = i / 3, col = i % 3;
        if (board[row][col] == ' ') {
            board[row][col] = 'O';
            int score = minimax(0);
            board[row][col] = ' ';
            if (score > best_score) {
                best_score = score;
                best_move = i;
            }
        }
    }
    return best_move;
}

int minimax(int is_ai) {
    int winner = check_winner();
    if (winner == AI) return 10;
    if (winner == HUMAN) return -10;
    if (is_full()) return 0;

    int best_score = is_ai ? -1000 : 1000;

    for (int i = 0; i < 9; i++) {
        int row = i / 3, col = i % 3;
        if (board[row][col] == ' ') {
            board[row][col] = is_ai ? 'O' : 'X';
            int score = minimax(!is_ai);
            board[row][col] = ' ';
            if (is_ai)
                best_score = (score > best_score) ? score : best_score;
            else
                best_score = (score < best_score) ? score : best_score;
        }
    }

    return best_score;
}

void get_player_names() {
    printf("Enter name for Player 1 (X), or press ENTER to skip: ");
    fgets(player1_name, sizeof(player1_name), stdin);
    trim_newline(player1_name);
    if (strlen(player1_name) == 0)
        strcpy(player1_name, "Player 1");

    printf("Enter name for Player 2 (O), or press ENTER to skip: ");
    fgets(player2_name, sizeof(player2_name), stdin);
    trim_newline(player2_name);
    if (strlen(player2_name) == 0)
        strcpy(player2_name, "Player 2");
}

void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
        str[len - 1] = '\0';
}
