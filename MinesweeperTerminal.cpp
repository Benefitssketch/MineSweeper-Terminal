#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>


#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __linux__
#include <unistd.h>
#endif

void ClearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

#define ANSI_RESET_ALL      "\x1b[0m"
#define ANSI_BOLD           "\x1b[1m"
#define ANSI_UNDERLINE      "\x1b[4m"
#define ANSI_COLOR_RED      "\x1b[31m"
#define asterisk            42

int a;
std::string str;
char HtP;
int zeror;
char zeroc;
bool play = true;
char proceed;
int permanent_placeholder;
int MINES;
int board_size;
int board_length = 0;
int difficulty_selection;
int guess_type;
int useable_flags;
bool game_end = 0;
bool formatting_guess = 0;
bool victory = 0;

char column_container;
int row_container;

std::vector<std::vector<int>> board;
std::vector<std::vector<char>> obfuscated_board; //(board_length, std::vector<char>(board_length))
std::vector<std::vector<int>> flag_buffer;

std::vector<std::vector<int>> zeroes;

// functions
std::vector<std::vector<int>> generate_mines(std::vector<std::vector<int>> sub_board);
std::vector<std::vector<int>> complete_board(std::vector<std::vector<int>> sub_board);

void print_board();
void print_ob_board();
void create_cheat_sheet();
bool flag_mine_overlap();

void ClearLine() {printf("\x1b[1F"); printf("\x1b[2K");}
void Clear() {while (getchar() != '\n');}

// The name has no descriptive meaning, it's just a memorable phrase
// that includes the word zero :)
void Zero_sum_game(int row, char col);
void Zero_iterations(int row, char col);
void Inf_iterations(int row, char col);


void game_start() {
    Zero_sum_game(zeror, zeroc);
    while (game_end == false) {

        print_ob_board();
        printf("\nrow: ");
        scanf("%d", &row_container);


        if (row_container < 1 || row_container > board_length) {
            Clear();
            printf("That row position does not exist. Please select a number (1-%d) from the rows above.\nPress " ANSI_BOLD"(Enter)" ANSI_RESET_ALL" to continue.", board_length);
            scanf("%c", &permanent_placeholder);
            continue; }


        Clear();
        printf("column: "); scanf("%c", &column_container);
        if (column_container < 90) {column_container -= 65;}
        else {column_container -= 97;}

        if (int(column_container) < 0 || int(column_container) > board_length) {
            Clear();
            printf("That column position does not exist. Please select a character (A-%c) from the columns above.\nPress " ANSI_BOLD"(Enter)" ANSI_RESET_ALL" to continue.", board_length + 64);
            scanf("%c", &permanent_placeholder);
            continue; }

        row_container--;
        printf("action: ");
        scanf("%d", &guess_type);
        Clear();

        switch (guess_type) {
        //place flag
        case 2: {
            if (obfuscated_board[row_container][column_container] > 42) {printf("that space has already been revealed. "); scanf("%c", &permanent_placeholder); break; }
            else if (obfuscated_board[row_container][column_container] == 33) {printf("a flag has already been placed in that position. "); scanf("%c", &permanent_placeholder); break; }
            else { obfuscated_board[row_container][column_container] = 33;

            flag_buffer.insert(flag_buffer.begin(), std::vector<int>(board_length * board_size, 0));
            --useable_flags; }

            break;
        }

        //reveal space
        case 1: {
            if (obfuscated_board[row_container][column_container] == 33) useable_flags++;
            if (board[row_container][column_container] == 9) {
                print_board();
                printf("\nYou failed! Press " ANSI_BOLD"(Enter)" ANSI_RESET_ALL" to play again. ");
                scanf("%c", &permanent_placeholder);
                game_end = true;
                break;
            } else {obfuscated_board[row_container][column_container] = board[row_container][column_container]+48; }
        }

        default: break;
        printf("%d", flag_buffer.size());

        Clear();
        ClearScreen();
        }

        Zero_iterations(row_container, column_container);
        if (useable_flags == 0) {
            if (flag_mine_overlap()) {
                print_ob_board();
                printf("\nYou Won! Press " ANSI_BOLD"(Enter)" ANSI_RESET_ALL" to play again.");
                Clear();
                scanf("%c", permanent_placeholder);
                game_end = true;
                break;
            }
        }
    }
}


int main() {
    while (play) {
    srand(static_cast<unsigned int>(time(nullptr)));

    ClearScreen();
    printf(ANSI_BOLD"Minesweeper!" ANSI_RESET_ALL);
    printf("\nPress " ANSI_BOLD"(Enter)" ANSI_RESET_ALL " to proceed or type " ANSI_BOLD"(h)" ANSI_RESET_ALL " for help\n");
    scanf("%c", &proceed);
    ClearLine();

    //Help the user
    if (proceed == 'h' || proceed == 'H') {
        ClearScreen();
        Clear();


        if (FILE *H2P = fopen("HowtoPlay.txt", "r")) {
            while ((HtP = fgetc(H2P)) != EOF) {
                printf("%c", HtP);
            }
            fclose(H2P);
        }
        else {
            printf("HowtoPlay.txt was not found... ");
            printf("\nPress " ANSI_BOLD"(Enter)" ANSI_RESET_ALL" to continue: ");
            scanf("%c", &permanent_placeholder);
            main();
        }
        printf(ANSI_BOLD"\n(Scroll Up to see all usage info)" ANSI_RESET_ALL);
        printf("\nPress " ANSI_BOLD"(Enter)" ANSI_RESET_ALL" to continue: ");
        scanf("%c", &permanent_placeholder);
        main();
    }

    selection:
    printf("Select your difficulty:\n(1) Easy\n" "(2) Medium\n" "(3) Hard\n");
    scanf("%d", &difficulty_selection);


    //Gamemodes
    switch(difficulty_selection){
        case 1: printf("Selected Difficulty: Easy\n"); MINES = 10; board_length = 9; board_size = 81; break;
        case 2: printf("Selected Difficulty: Medium\n"); MINES = 40; board_length = 16; board_size = 256; break;
        case 3: printf("Selected Difficulty: Hard\n"); MINES = 99; board_length = 26; board_size = 676; break;
        default: printf("Please select a difficulty from the given list\n"); goto selection; break;
    }

    zeroes.assign(board_length, std::vector<int>(board_length, 1));
    useable_flags = MINES;
    //std::vector<std::vector<char>> obfuscated_board(board_length, std::vector<char>(board_length));

    obfuscated_board.assign(board_length, std::vector<char>(board_length, 42));
    ZeroRow:
    Clear();
    print_ob_board();
    printf("\nrow: ");
    scanf("%d", &zeror);
    zeror--;

    if (zeror < 0 || zeror > board_length) {
        ClearLine();
        printf("That row position does not exist. Please select a number (1-%d) from the rows above.\nPress " ANSI_BOLD"(Enter)" ANSI_RESET_ALL" to continue: ", board_length);
        scanf("%c", &permanent_placeholder);
        goto ZeroRow;
    }

    Clear();

    printf("column: ");
    scanf("%c", &zeroc);
    if (zeroc < 90) {zeroc -= 65;}
    else {zeroc -= 97;}

    if (int(zeroc) < 0 || int(zeroc) > board_length) {
        Clear();
        ClearLine();
        printf("That column position does not exist. Please select a character (A-%c) from the columns above.\nPress " ANSI_BOLD"(Enter)" ANSI_RESET_ALL" to continue.", board_length + 64);
        scanf("%c", &permanent_placeholder);
        goto ZeroRow;
    }
    Clear();

    board = generate_mines(board);
    board = complete_board(board);
    Zero_iterations(zeror, zeroc);

    create_cheat_sheet();
    int board_size = board.size();

    game_end = false;
    game_start();
    }
    return 0;
}


bool flag_mine_overlap() {
    for (int r = 0; r<board_length; r++) {
        for (int c = 0; c<board_length; c++) {
            if (obfuscated_board[r][c] == '!') {
                if (board[r][c] == 9) continue;
                else return 0;
            }
        }
    }
    return true;
}

void Zero_iterations(int row, char col) {
    if (board[row][col] == 0) {
        Zero_sum_game(row, col);
        zeroes[row][col] = 0;
        for (int r = 0; r<board_length; r++) {
            for (int c = 0; c<board_length; c++) {
                if (obfuscated_board[r][c] == 48 && zeroes[r][c] != 0) {
                    Zero_sum_game(r, c);
                    zeroes[r][c] = 0;
                    if (c > 0) c--;
                    r = 0;
                }
            }
        }
    }
}

void Inf_iterations(int row, char col) {
    if (board[row][col] == 0) {
        Zero_sum_game(row, col);
        zeroes[row][col] = 0;
        for (int r = 0; r<board_length; r++) {
            for (int c = 0; c<board_length; c++) {
                if (obfuscated_board[r][c] == 48 && zeroes[r][c] != 0) {
                    Zero_sum_game(r, c);
                    zeroes[r][c] = 0;
                    if (c > 0) c--;
                    r = 0;
                }
            }
        }
    }
}

//reveals all the neighboring cells around a selected cell
void Zero_sum_game(int row, char col) {
    if (row < board_length-1) {
        if (col > 0 && obfuscated_board[row+1][col-1] == 42)obfuscated_board[row+1][col-1] = board[row+1][col-1]+48;
        if (obfuscated_board[row+1][col] == 42) obfuscated_board[row+1][col] = board[row+1][col]+48;
        if (col < board_length && obfuscated_board[row+1][col+1] == 42) obfuscated_board[row+1][col+1] = board[row+1][col+1]+48;
    }

    if (col > 0 && obfuscated_board[row][col-1] == 42) obfuscated_board[row][col-1] = board[row][col-1]+48;
    if (col < board_length-1 && obfuscated_board[row][col+1] == 42) obfuscated_board[row][col+1] = board[row][col+1]+48;

    if (row > 0) {
        if (col > 0 && obfuscated_board[row-1][col-1] == 42) obfuscated_board[row-1][col-1] = board[row-1][col-1]+48;
        if (obfuscated_board[row-1][col] == 42) obfuscated_board[row-1][col] = board[row-1][col]+48;
        if (col < board_length-1 && obfuscated_board[row-1][col+1] == 42) obfuscated_board[row-1][col+1] = board[row-1][col+1]+48;
    }
}


// this could bassically be recognised as 'int generate_mines(sub_board)',
// but it uses a 2D vector
std::vector<std::vector<int>> generate_mines(std::vector<std::vector<int>> sub_board) {
    sub_board = std::vector<std::vector<int>>(board_length, std::vector<int>(board_length, 0));
    sub_board[zeror][zeroc] = 11;
    obfuscated_board[zeror][zeroc] = 48;

    if (zeror < board_length-1) {
        if (zeroc > 0) sub_board[zeror+1][zeroc-1] = 11;
        sub_board[zeror+1][zeroc] = 11;
        if (zeroc < board_length) sub_board[zeror+1][zeroc+1] = 11;
    }

    if (zeroc > 0) sub_board[zeror][zeroc-1] = 11;
    if (zeroc < board_length-1) sub_board[zeror][zeroc+1] = 11;

    if (zeror > 0) {
        if (zeroc > 0) sub_board[zeror-1][zeroc-1] = 11;
        sub_board[zeror-1][zeroc] = 11;
        if (zeroc < board_length-1) sub_board[zeror-1][zeroc+1] = 11;
    }

    int placed = 0;
    while (placed < MINES) {
        int r = rand() % board_length;
        int c = rand() % board_length;
        if (sub_board[r][c] != 9 && sub_board[r][c] != 11) {
            sub_board[r][c] = 9;
            ++placed;
        }
    }

    if (zeror < board_length-1) {
        if (zeroc > 0) sub_board[zeror+1][zeroc-1] = 0;
        sub_board[zeror+1][zeroc] = 11;
        if (zeroc < board_length) sub_board[zeror+1][zeroc+1] = 0;
    }

    if (zeroc > 0) sub_board[zeror][zeroc-1] = 0;
    if (zeroc < board_length-1) sub_board[zeror][zeroc+1] = 0;

    if (zeror > 0) {
        if (zeroc > 0) sub_board[zeror-1][zeroc-1] = 0;
        sub_board[zeror-1][zeroc] = 0;
        if (zeroc < board_length-1) sub_board[zeror-1][zeroc+1] = 0;
    }
    return sub_board;
}


std::vector<std::vector<int>> complete_board(std::vector<std::vector<int>> sub_board) {
    int count_up;
    for (int i = 0; i < board_length; i++) {
        for (int j = 0; j < board_length; j++) {
            count_up = 0;

            if (sub_board[i][j] != 9) {
                if (i < board_length-1) {
                    if (j > 0) if (sub_board[i+1][j-1] == 9) count_up++;
                    if (sub_board[i+1][j] == 9) count_up++;
                    if (j < board_length) if (sub_board[i+1][j+1] == 9) count_up++;
                }

                if (j > 0) if (sub_board[i][j-1] == 9) count_up++;
                if (j < board_length-1) if (sub_board[i][j+1] == 9) count_up++;

                if (i > 0) {
                    if (j > 0) if (sub_board[i-1][j-1] == 9) count_up++;
                    if (sub_board[i-1][j] == 9) count_up++;
                    if (j < board_length-1) if (sub_board[i-1][j+1] == 9) count_up++;
                }
                sub_board[i][j] = count_up;
            }
        }
    }
    return sub_board;
}

void create_cheat_sheet() {
    FILE *fptr;
    fptr = fopen("cheatkey.txt", "w");
    fprintf(fptr, "   ");
    for (int c=65; c < (board_length + 65); c++) {fprintf(fptr, " %c", c);}
    fprintf(fptr, "\n");

    for (int i=0; i < board_length; i++) { fprintf(fptr, "\n"); fprintf(fptr, "%2d ", i+1);
    for (int j=0; j < board_length; j++) {
            fprintf(fptr, " %d", board[i][j]);
        }
    }
    fclose(fptr);
}

//print the board
void print_board() {
    ClearScreen();
    printf("    ");
    for (int c=65; c < (board_length + 65); c++) {printf("%c|", c);}
    printf("\n");

    for (int i=0; i < board_length; i++) { printf("\n"); printf("\x1b[4m%2d \x1b[0m", i+1);
    for (int j=0; j < board_length; j++) {
            if (board[i][j] == 9) printf(ANSI_COLOR_RED " %d" ANSI_RESET_ALL, board[i][j]);
            else printf(" %d", board[i][j]);
        }}
}

//print the obfuscated board, with the mines hidden
void print_ob_board() {
    ClearScreen();
    printf("     ");
    for (int c=65; c < (board_length + 65); c++) {printf("%c|", c);}
    printf("\b");
    printf("%*s %d", 5, "", useable_flags);
    printf("\n");

    for (int i=0; i < board_length; i++) { printf("\n"); printf(ANSI_UNDERLINE"%2d " ANSI_RESET_ALL " ", i+1);
    for (int j=0; j < board_length; j++) {
            printf(" %c", obfuscated_board[i][j]);
        }}
}
