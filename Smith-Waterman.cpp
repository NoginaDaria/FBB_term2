/*
Smith-Waterman algorythm implementation

was inspired by https://github.com/ngopal/SimpleSmithWatermanCPP/blob/master/SmithWaterman.cpp
this one didn't work and I had to rewrite it, because it had problems (big problems) with indexes and memory allocation
perspectives: divide gap penalty to gap opening penalty and gap extension penalty (upd 4th may: done)
+ update: user can set parameters

SOS: alignments are calculated not bad, but the matrix coefficients are all non-positive, which argues with the idea of this algorythm. 
Have no idea how to fix it and not to break anything. Other word I have, but it creates some "anti-alignments"

input: program will ask you for two sequences and for parameters if you agree
output: score and alignments

hope it works nicely
*/


#include <iostream>
#include <string>

using namespace std;

const signed char BLOSUM[][25] = { // the blosum 62 scoring matrix, taken from https://sosal.kr/625
 { 4,-1,-2,-2, 0,-1,-1, 0,-2,-1,-1,-1,-1,-2,-1, 1, 0,-3,-2, 0,-2,-1,-1,-1,-4},
 {-1, 5, 0,-2,-3, 1, 0,-2, 0,-3,-2, 2,-1,-3,-2,-1,-1,-3,-2,-3,-1,-2, 0,-1,-4},
 {-2, 0, 6, 1,-3, 0, 0, 0, 1,-3,-3, 0,-2,-3,-2, 1, 0,-4,-2,-3, 4,-3, 0,-1,-4},
 {-2,-2, 1, 6,-3, 0, 2,-1,-1,-3,-4,-1,-3,-3,-1, 0,-1,-4,-3,-3, 4,-3, 1,-1,-4},
 { 0,-3,-3,-3, 9,-3,-4,-3,-3,-1,-1,-3,-1,-2,-3,-1,-1,-2,-2,-1,-3,-1,-3,-1,-4},
 {-1, 1, 0, 0,-3, 5, 2,-2, 0,-3,-2, 1, 0,-3,-1, 0,-1,-2,-1,-2, 0,-2, 4,-1,-4},
 {-1, 0, 0, 2,-4, 2, 5,-2, 0,-3,-3, 1,-2,-3,-1, 0,-1,-3,-2,-2, 1,-3, 4,-1,-4},
 { 0,-2, 0,-1,-3,-2,-2, 6,-2,-4,-4,-2,-3,-3,-2, 0,-2,-2,-3,-3,-1,-4,-2,-1,-4},
 {-2, 0, 1,-1,-3,-0, 0,-2, 8,-3,-3,-1,-2,-1,-2,-1,-2,-2, 2,-3, 0,-3, 0,-1,-4},
 {-1,-3,-3,-3,-1,-3,-3,-4,-3, 4, 2,-3, 1, 0,-3,-2,-1,-3,-1, 3,-3, 3,-3,-1,-4},
 {-1,-2,-3,-4,-1,-2,-3,-4,-3, 2, 4,-2, 2, 0,-3,-2,-1,-2,-1, 1,-4, 3,-3,-1,-4},
 {-1, 2, 0,-1,-3, 1, 1,-2,-1,-3,-2, 5,-1,-3,-1, 0,-1,-3,-2,-2, 0,-3, 1,-1,-4},
 {-1,-1,-2,-3,-1, 0,-2,-3,-2, 1, 2,-1, 5, 0,-2,-1,-1,-1,-1, 1,-3, 2,-1,-1,-4},
 {-2,-3,-3,-3,-2,-3,-3,-3,-1, 0, 0,-3, 0, 6,-4,-2,-2, 1, 3,-1,-3, 0,-3,-1,-4},
 {-1,-2,-2,-1,-3,-1,-1,-2,-2,-3,-3,-1,-2,-4, 7,-1,-1,-4,-3,-2,-2,-3,-1,-1,-4},
 { 1,-1, 1, 0,-1, 0, 0, 0,-1,-2,-2, 0,-1,-2,-1, 4, 1,-3,-2,-2, 0,-2, 0,-1,-4},
 { 0,-1, 0,-1,-1,-1,-1,-2,-2,-1,-1,-1,-1,-2,-1, 1, 5,-2,-2, 0,-1,-1,-1,-1,-4},
 {-3,-3,-4,-4,-2,-2,-3,-2,-2,-3,-2,-3,-1, 1,-4,-3,-2,11, 2,-3,-4,-2,-2,-1,-4},
 {-2,-2,-2,-3,-2,-1,-2,-3, 2,-1,-1,-2,-1, 3,-3,-2,-2, 2, 7,-1,-3,-1,-2,-1,-4},
 { 0,-3,-3,-3,-1,-2,-2,-3,-3, 3, 1,-2, 1,-1,-2,-2, 0,-3,-1, 4,-3, 2,-2,-1,-4},
 {-2,-1, 4, 4,-3, 0, 1,-1, 0,-3,-4, 0,-3,-3,-2, 0,-1,-4,-3,-3, 4,-3, 0,-1,-4},
 {-1,-2,-3,-3,-1,-2,-3,-4,-3, 3, 3,-3, 2, 0,-3,-2,-1,-2,-1, 2,-3, 3,-3,-1,-4},
 {-1, 0, 0, 1,-3, 4, 4,-2, 0,-3,-3, 1,-1,-3,-1, 0,-1,-2,-2,-2, 0,-3, 4,-1,-4},
 {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-4},
 {-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4, 1}
};



//auto penalty
double gap_opening_penalty = 10;
double gap_extension_penalty = 0.5;
double gap_new_penalty = gap_opening_penalty + gap_extension_penalty;

double max(double x, double y)
{
	return x > y ? x : y;
};

double max(double x, double y, double z)
{
	return x > y ? (z = (x > z ? x : z)) : (z = (y > z ? y : z));
};


int main()
{
	string seq1, seq2;
	std::cout << "Input first sequence " << endl;
	cin >> seq1;
	std::cout << "Input second sequence " << endl;
	cin >> seq2;
	
	//allowing user to set parameters
	cout << "Do you want to use auto parameters? [y,n]" << endl;
	string answer;
	cin >> answer;
	if (answer == "n") {
		cout << "Gap opening penalty[10.0]: ";
		cin >> gap_opening_penalty;
		cout << endl << "Gap extension penalty[0.5]: ";
		cin >> gap_extension_penalty;
		cout << endl;
	}
	else if (answer == "y") {
		cout << "Continued with auto parameters" << endl;
	}
	else {
		cout << "Sorry, I didn't understand, continued with auto parameters" << endl;
	}

	//set seq lengths
	int seq1_len = seq1.length();
	int seq2_len = seq2.length();

	//set matrix
	//my compiler has problems with dynamic array size so the code will be twisted
	
	//the main matrix
	double **matrix = (double **)malloc((seq1_len + 1) * sizeof(double *));
	for (int i = 0; i < (seq1_len + 1); i++)
		matrix[i] = (double *)malloc((seq2_len + 1) * sizeof(double));
	
	//the 0-column will be filled with big non-positive number 
	double **I_i = (double **)malloc((seq1_len + 1) * sizeof(double *));
	for (int i = 0; i < (seq1_len + 1); i++)
		I_i[i] = (double *)malloc((seq2_len + 1) * sizeof(double));

	//the 0-row will be filled with big non-positive number
	double **I_j = (double **)malloc((seq1_len + 1) * sizeof(double *));
	for (int i = 0; i < (seq1_len + 1); i++)
		I_j[i] = (double *)malloc((seq2_len + 1) * sizeof(double));

	//just big number to be the biggest
	double big_big_double = -60000;
	   	 
	//setting matrixes 
	for (int i = 1; i < (seq1_len + 1); i++) {
		I_i[i][0] = big_big_double;
		matrix[i][0] = I_j[i][0] = -gap_opening_penalty - i * gap_extension_penalty;
	}

	for (int j = 1; j < (seq2_len + 1); j++) {
		I_j[0][j] = big_big_double;
		matrix[0][j] = I_i[0][j] = -gap_opening_penalty - j * gap_extension_penalty;
	}


	//calculating matrix with affine gap penalty
	for (int i = 1; i < (seq1_len + 1); i++) {
		for (int j = 1; j < (seq2_len + 1); j++) {
			I_i[i][j] =
				max(I_i[i][j - 1] - gap_extension_penalty, matrix[i][j - 1] - gap_new_penalty);
			I_j[i][j] =
				max(I_j[i - 1][j] - gap_extension_penalty, matrix[i - 1][j] - gap_new_penalty);
			matrix[i][j] =
				max(
					matrix[i - 1][j - 1] + BLOSUM[seq1[i - 1] - 'A'][seq2[j - 1] - 'A']
					/* + here should be BLOSUM coefficient*/,
					I_i[i][j], I_j[i][j]
				);
		}
	}

	//backtracking

	int i = seq1_len, j = seq2_len;

	while (i > 0 || j > 0) {
		if (matrix[i][j] == I_i[i][j]) {
			seq1.insert(i, 1, '-'); //insert(position, number of symbols, symbol)
			j--;
		}
		else if (matrix[i][j] == I_j[i][j]) {
			seq2.insert(j, 1, '-');
			i--;
		}
		else {
			i--, j--;
		}
	}

	//print result
	cout << "Score: " << matrix[seq1_len][seq2_len] << endl << endl << seq1 << endl << seq2 << endl;

	return 0;
}
