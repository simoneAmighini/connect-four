// LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// BOOLEAN TYPE DEFINITION
typedef enum{false, true} bool;

// OUTPUT COLOR DEFINITIONS: remember to put END_COL after using a color in printf()!
#define END_COL "\x1b[0m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define BLUE    "\x1b[34m"

// GAME SPECIFICATIONS: values > 9 cause problems (at least with text formatting)
#define ROWS         6
#define COLUMNS      7
/* Game data is stored in a 2*ROWS*COLUMNS matrix:
--> the matrix ROWS*COLUMNS represents the game
    table
--> each player has its own table (so there are
    2 levels) */
char* symbols[] = {"\x1b[34mO\x1b[0m", "\x1b[31mX\x1b[0m"};

// FUNCTION DECLARATIONS
void cls();
void pause();
unsigned short getInput();
void p2pPlay();
void pcPlay();
void startMenu();
void initializeMatrix(unsigned short matrix[2][ROWS][COLUMNS]);
void printTable(unsigned short matrix[2][ROWS][COLUMNS]);
unsigned short askPlayer(unsigned short player, unsigned short matrix[2][ROWS][COLUMNS]);
void updateMatrix(unsigned short column, unsigned short player, unsigned short matrix[2][ROWS][COLUMNS]);
bool winCheck(unsigned short player, unsigned short matrix[2][ROWS][COLUMNS], unsigned short column);
void printWinner(unsigned short player);
void printTie();

// MAIN
int main(int argc, char **argv) {
    cls();
    if (argc == 2) {
        if(strcmp(argv[1], "--p2pPlay") == 0) p2pPlay();
        else if(strcmp(argv[1], "--pcPlay") == 0) pcPlay();
        else startMenu();
    }
    else startMenu();
    cls();
    return 0;
}

// FUNCTIONS
void cls() {
    system("cls");
}

void pause() {
    system("pause");
}

unsigned short getInput() {
    /* inspired by https://stackoverflow.com/questions/50096953/c-integer-safe-input
    This function handles string inputs: it returns 0 (number's value) in case of wrong input. */
    unsigned short number = 0;
    scanf("%hu", &number);
    if(getchar() == '\n') return number; // if the character after the typed number is ENTER then the input is correct
    while(getchar() != '\n'); // buffer management to avoid errors
    return 0;
}

void startMenu() {
    unsigned short menuChoice;
    printf(GREEN "+---------------------------+\n" END_COL);
    printf(GREEN "| " END_COL "C O N N E C T   F O U R ! " GREEN "|\n" END_COL);
    printf(GREEN "+---------------------------+\n" END_COL);
    printf("[Tip: you can also pass '--p2pPlay' or '--pcPlay' as arguments when you call connect-four in the terminal.]\n");
    printf("___________________________________________________________________________________________________________\n\n");
    printf("Type...\n");
    printf("+-----------------------------+\n");
    printf("| " BLUE "1"END_COL " for PLAYER vs PLAYER mode |\n");
    printf("+-----------------------------+\n");
    printf("+-----------------------------+\n");
    printf("| " RED "2" END_COL " for PLAYER vs PC mode     |\n");
    printf("+-----------------------------+\n");
    printf("+-----------------------------+\n");
    printf("| Other key to EXIT           |\n");
    printf("+-----------------------------+\n\n");
    printf("> Answer: ");
    menuChoice = getInput();
    if(menuChoice == 1) p2pPlay();
    else if(menuChoice == 2) pcPlay();
    else exit(0);
}

void p2pPlay() {
    unsigned short currentPlayer = 1, matrix[2][ROWS][COLUMNS], latestChoice, counter = 0;
    bool win;
    cls();
    initializeMatrix(matrix);
    printTable(matrix);
    do {
        if(currentPlayer == 0) currentPlayer++; // current player update: player 0 always plays the first round.
        else currentPlayer--;
        latestChoice = askPlayer(currentPlayer, matrix);
        updateMatrix(latestChoice, currentPlayer, matrix);
        printTable(matrix);
        counter++;
        win = winCheck(currentPlayer, matrix, latestChoice);
    } while(!win && counter < ROWS*COLUMNS);
    if(win) printWinner(currentPlayer);
    else printTie();
    pause();
}

void pcPlay() { 
    cls();
    printf(RED "Not implemented yet...\n\n" END_COL);
    pause();
}

void initializeMatrix(unsigned short matrix[2][ROWS][COLUMNS]) {
    for(int k = 0; k < 2; k++) {
        for(int i = 0; i < ROWS; i++) {
            for(int j = 0; j < COLUMNS; j++) {
                matrix[k][i][j] = 0;
            }
        }
    }
}

void printTable(unsigned short matrix[2][ROWS][COLUMNS]) {
    cls();
    for(int i = ROWS-1; i >= 0; i--) {
        printf(GREEN "%d" END_COL " |", i+1);
        for(int j = 0; j < COLUMNS; j++) {
            if(matrix[0][i][j] == 1) printf(" %s |", symbols[0]);
            else if(matrix[1][i][j] == 1) printf(" %s |", symbols[1]);
            else printf("   |");
        }
        printf("\n--+");
        for(int j = 0; j < COLUMNS; j++) {
            printf("---+");
        }
        printf("\n");
    }
    printf("  |");
    for(int j = 0; j < COLUMNS; j++) {
        printf(GREEN " %d" END_COL " |", j+1);
    }
    printf("\n\n");
}

unsigned short askPlayer(unsigned short player, unsigned short matrix[2][ROWS][COLUMNS]) {
    unsigned short answer = 0;
    while(answer<1 || answer>COLUMNS || matrix[0][ROWS-1][answer-1] == 1 || matrix[1][ROWS-1][answer-1] == 1) {
        printf("> Player %u (%s) ==> ", player+1, symbols[player]);
        answer = getInput();
    }
    return answer-1;
}

void updateMatrix(unsigned short column, unsigned short player, unsigned short matrix[2][ROWS][COLUMNS]) {
    for(int i = 0; i<ROWS; i++) {
        if(matrix[0][i][column] == 0 && matrix[1][i][column] == 0) {
            matrix[player][i][column] = 1;
            break;
        }
    }
}

bool winCheck(unsigned short player, unsigned short matrix[2][ROWS][COLUMNS], unsigned short column) {
    // get the row of the latest round
    unsigned short row;
    for(int i = ROWS-1; i >= 0; i--) {
        if(matrix[player][i][column] == 1) {
            row = (unsigned short)i;
            break;
        }
    }
    
    // check method: if 4 elements are connected then their sum is equal to 4.
    unsigned short sum;
    int rowIndex, colIndex, counter;

    // diagonal 1 passes through the element inserted in the latest round with left-bottom to right-top direction
    rowIndex = row;
    colIndex = column;
    while(rowIndex > 0 && colIndex > 0) {
        rowIndex--;
        colIndex--;
    }
    while(rowIndex <= (ROWS - 4) && colIndex <= (COLUMNS - 4)) {
        counter = 0;
        sum = 0;
        while (counter < 4) {
            sum += matrix[player][rowIndex + counter][colIndex + counter];
            counter++;
        }
        if(sum == 4) {
            printf("DIAGONAL (left-bottom to right-top)\n");
            return true;
        }
        rowIndex++;
        colIndex++;
    }

    // diagonal 2 passes through the element inserted in the latest round with right-bottom to left-top direction
    rowIndex = row;
    colIndex = column;    
    while(rowIndex > 0 && colIndex < COLUMNS) {
        rowIndex--;
        colIndex++;
    }
    while(rowIndex <= (ROWS - 4) && colIndex >= (COLUMNS - 4)) {
        counter = 0;
        sum = 0;
        while (counter < 4) {
            sum += matrix[player][rowIndex + counter][colIndex - counter];
            counter++;
        }
        if(sum == 4) {
            printf("DIAGONAL (right-bottom to left-top)\n");
            return true;
        }
        rowIndex++;
        colIndex--;
    }

    // horizontal check
    for(colIndex = 0; colIndex <= (COLUMNS - 4); colIndex++) {
        counter = 0;
        sum = 0;
        while(counter < 4) {
            sum += matrix[player][row][colIndex + counter];
            counter++;
        }
        if(sum == 4) {
            printf("HORIZONTAL\n");
            return true;
        }
    }

    // vertical check
    for(rowIndex = 0; rowIndex <= (ROWS - 4); rowIndex++) {
        counter = 0;
        sum = 0;
        while(counter < 4) {
            sum += matrix[player][rowIndex + counter][column];
            counter++;
        }
        if(sum == 4) {
            printf("VERTICAL\n");
            return true;
        }
    }

    // if none of the checks returns true...
    return false;
}

void printWinner(unsigned short player) {
    printf("> PLAYER %d (%s) WON! WELL DONE! <\n\n", player+1, symbols[player]);
}

void printTie() {
    printf("> GAME ENDED IN A " GREEN "TIE" END_COL " <\n\n");
}