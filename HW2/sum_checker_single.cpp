#include <iostream>
#include <cstdlib>
#include <string>
#include <cstdio>



using namespace std;



#define PUZZLE_SIZE 9



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



int main(int argc, char* argv[]) {
	bool rv = true; // flag to check answer
    // input the sudoku file
    SetPuzzle("test.txt");
    
    // Get the sum
    int answer = 0;
    for (int col = 1; col < 10; col++)
        answer += puzzle[1][col];

    // Check every row
    for (int row = 2; row < 10; row++) {
        int sum = 0;
        for (int col = 1; col < 10; col++)
            sum += puzzle[row][col];
        if (answer != sum) {
            rv = false;
            break;
        }
    }

    // Check every column
    if (rv) {
        for (int col = 1; col < 10; col++) {
            int sum = 0;
            for (int row = 1; row < 10; row++)
                sum += puzzle[row][col];
            if (answer != sum) {
                rv = false;
                break;
            }
        }
    }

    // Check every subgrid
    if (rv) {
        for (int row = 1; row < 8; row += 3) {
            for (int col = 1; col < 8; col += 3) {
                int sum = 0;

                for (int i = row; i < row + 3; i++)
                    for (int j = col; j < col + 3; j++)
                        sum += puzzle[i][j];

                if (answer != sum) {
                    rv = false;
                    break;
                }
            }

            if (!rv) break;
        }
    }
    
	if (rv)
		cout << "Successful :)" << endl;
	else
		cout << "Must check again! :(" << endl;

	return 0;
}

