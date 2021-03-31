#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <unistd.h>



using namespace std;



#define NUMBER_OF_THREADS 27 /* Hint */
#define PUZZLE_SIZE 9



int answers[NUMBER_OF_THREADS];



/* example puzzle */
int puzzle[PUZZLE_SIZE+1][PUZZLE_SIZE+1] = {
            {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
            {-1,0,0,0,0,0,0,0,0,0},
            {-1,0,0,0,0,0,0,0,0,0},
            {-1,0,0,0,0,0,0,0,0,0},
            {-1,0,0,0,0,0,0,0,0,0},
            {-1,0,0,0,0,0,0,0,0,0},
            {-1,0,0,0,0,0,0,0,0,0},
            {-1,0,0,0,0,0,0,0,0,0},
            {-1,0,0,0,0,0,0,0,0,0},
            {-1,0,0,0,0,0,0,0,0,0}
        };



/* data structure for passing data to threads */
typedef struct
{
    int thread_number;
    int row;
    int col;
} parameters;



/* print puzzle */ 
#define LINE "======================"
#define COLLINE "\n___________________________________"
#define NAME "||  SUM   CHECKER  ||"
void print_grid(int grid[10][10]) {
    int i, j;
    cout << LINE << endl;
    cout << NAME << endl;
    cout << LINE << endl;

    for (i = 1; i < 10; i++) {
        for (j = 1; j < 10; j++) {
            cout<< "|" << grid[i][j] << " |";
        }
        cout << COLLINE << endl;
    }
    cout << endl;
}



// read file to check sudoku
void SetPuzzle(string filename) {
    FILE *file = fopen(filename.c_str(), "r");
    int ch, num;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            while (((ch = getc(file)) != EOF)) {
                if (ch == EOF) break;
                if (ch == ',') break;
                if (ch == '\n') break;
                ungetc(ch, file);
                fscanf(file, "%d", &num);
                if (num != -1)
                    puzzle[i][j] = num;            
            } 
        }
    }
    print_grid(puzzle);
    return;
}



void* child(void* data) {
    parameters* args = (parameters*) data;

    int sum = 0;
    if (args->thread_number < 9) { // Check row
        for (int col = 1; col < 10; col++)
            sum += puzzle[args->row][col];
    } else if (args->thread_number < 18) { // Check column
        for (int row = 1; row < 10; row++)
            sum += puzzle[row][args->col];
    } else { // Check subgrid
        for (int i = args->row; i < args->row + 3; i++)
            for (int j = args->col; j < args->col + 3; j++)
                sum += puzzle[i][j];
    }
    answers[args->thread_number] = sum;
    sleep(5);

    pthread_exit(NULL);
}



int main(int argc, char* argv[]) {
    bool rv = true; // flag to check answer
    // input the sudoku file
    SetPuzzle("test.txt");
    
    parameters args[NUMBER_OF_THREADS];
    pthread_t t[NUMBER_OF_THREADS];
    for (int i = 0; i < 27; i++) { // Create threads
        args[i].thread_number = i;

        if (i < 9) { // Check row
            args[i].row = i + 1;
        } else if (i < 18) { // Check column
            args[i].col = i - 8;
        } else { // Check subgrid
            switch (i) {
                case 18: args[i].row = 1; args[i].col = 1; break;
                case 19: args[i].row = 1; args[i].col = 4; break;
                case 20: args[i].row = 1; args[i].col = 7; break;
                case 21: args[i].row = 4; args[i].col = 1; break;
                case 22: args[i].row = 4; args[i].col = 4; break;
                case 23: args[i].row = 4; args[i].col = 7; break;
                case 24: args[i].row = 7; args[i].col = 1; break;
                case 25: args[i].row = 7; args[i].col = 4; break;
                case 26: args[i].row = 7; args[i].col = 7; break;
            }
        }

        pthread_create(&(t[i]), NULL, child, &(args[i]));
    }

    // Wait for all child thread finished and check the answers
    pthread_join(t[0], NULL);
    int answer = answers[0];
    for (int i = 1; i < 27; i++) {
        pthread_join(t[i], NULL);
        if (answer != answers[i])
            rv = false;
    }
    
    if (rv)
        cout << "Successful :)" << endl;
    else
        cout << "Must check again! :(" << endl;

    return 0;
}

