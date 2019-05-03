/*
Smith-Waterman algorythm implementation

was inspired by https://github.com/ngopal/SimpleSmithWatermanCPP/blob/master/SmithWaterman.cpp
this one didn't work and I had to rewrite it, because it had problems (big problems) with indexes and memory allocation
perspectives: divide gap penalty to gap opening penalty and gap extension penalty

input: program will ask you for two sequences
output: matrix, max value of the matrix and alignments

hope it works nicely
*/



#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

//I'm too lazy to make .h file
double ifequal(char a, char b);
int max_index(double array[], int length = 4);

double penalty = -4; //introducing auto penalty

//introducing some useful function which set us value for diagonal movement
double ifequal(char a, char b) 
{
	double result;
	a == b ? result = 1 : result = penalty;
	return result;
};

//take index of max element in array
int max_index(double array[], int length)
{
	double max = 0;
	int ind = 0;

	for (int i = 0; i < length; i++)
	{
		if (array[i] > max)
		{
			max = array[i];
			ind = i;
		}
	}
	return ind;
};


int main() 
	{
		string seq1, seq2;
		std::cout << "Input first sequence " << endl;
		cin >> seq1;
		std::cout << "Input second sequence " << endl;
		cin >> seq2;


		//set seq lengths
		int seq1_len = seq1.length();
		int seq2_len = seq2.length();

		//set matrix
		//my compiler has problems with dynamic array size so the code will be twisted

		double **matrix = (double **)malloc(seq1_len * sizeof(double *));
		for (int i = 0; i < seq1_len; i++)
			matrix[i] = (double *)malloc(seq2_len * sizeof(double));

		//set zero matrix
		for (int i = 0; i < seq1_len; i++)
		{
			for (int j = 0; j < seq2_len; j++)
			{
				matrix[i][j] = 0;
			}
		}

		//set traceback and some other cool things
		double traceback[4];

		double **I_i = (double **)malloc((seq1_len + 1) * sizeof(double *));
		for (int i = 0; i < (seq1_len + 1); i++)
			I_i[i] = (double *)malloc((seq2_len + 1) * sizeof(double));

		double **I_j = (double **)malloc((seq1_len + 1) * sizeof(double *));
		for (int i = 0; i < (seq1_len + 1); i++)
			I_j[i] = (double *)malloc((seq2_len + 1) * sizeof(double));

		//start populating matrix
		for (int i = 1; i < seq1_len; i++)
		{
			for (int j = 1; j < seq2_len; j++)
			{
				traceback[0] = matrix[i - 1][j - 1] + ifequal(seq1[i - 1], seq2[j - 1]); //match or mismatch
				traceback[1] = matrix[i - 1][j] + penalty; //deletion
				traceback[2] = matrix[i][j - 1] + penalty; //insertion
				traceback[3] = 0;
				int ind; //index of max element
				matrix[i][j] = *max_element(traceback, traceback + 4);
				ind = max_index(traceback, 4);
				switch (ind)
				{
				case 0:
					I_i[i][j] = i - 1;
					I_j[i][j] = j - 1;
					break;
				case 1:
					I_i[i][j] = i - 1;
					I_j[i][j] = j;
					break;
				case 2:
					I_i[i][j] = i;
					I_j[i][j] = j - 1;
					break;
				case 3:
					I_i[i][j] = i;
					I_j[i][j] = j;
					break;
				}
			}
		}

		// find the max score in the matrix
		double matrix_max = 0;
		int i_max = 0, j_max = 0;
		for (int i = 1; i < seq1_len;i++)
		{
			for (int j = 1; j < seq2_len;j++)
			{
				if (matrix[i][j] > matrix_max)
				{
					matrix_max = matrix[i][j];
					i_max = i;
					j_max = j;
				}
			}
		}

		std::cout << "Max score in the matrix is " << matrix_max << endl;

		for (int i = 0; i < seq1_len; i++) 
		{
			for (int j = 0; j < seq2_len; j++) {
				std::cout << matrix[i][j];
			}
			std::cout << endl;
		}

		// traceback

		int current_i = i_max, current_j = j_max;
		int next_i = I_i[current_i][current_j];
		int next_j = I_j[current_i][current_j];
		int tick = 0;

		char *consensus_1 = NULL;
		consensus_1 = (char *)malloc((seq1_len + seq2_len + 2) * sizeof(char));

		char *consensus_2 = NULL;
		consensus_2 = (char *)malloc((seq1_len + seq2_len + 2) * sizeof(char));


		while (((current_i != next_i) || (current_j != next_j)) && (next_j != 0) && (next_i != 0))
		{

			next_i == current_i ? consensus_1[tick] = '-' : consensus_1[tick] = seq1[current_i - 1];
			//deletion VS match/mismatch
			
			next_j == current_j ? consensus_2[tick] = '-' : consensus_2[tick] = seq2[current_j - 1]; 
		
			current_i = next_i;
			current_j = next_j;
			next_i = I_i[current_i][current_j];
			next_j = I_j[current_i][current_j];
			tick++;
		}

		//print the consensus sequences
		std::cout << endl << endl;
		std::cout << "Alignment:" << endl << endl;
		for (int i = 0;i < seq1_len;i++) { std::cout << seq1[i]; }; std::cout << "  and" << endl;
		for (int i = 0;i < seq2_len;i++) { std::cout << seq2[i]; }; std::cout << endl << endl;
		for (int i = tick - 1;i >= 0;i--) std::cout << consensus_1[i];
		std::cout << endl;
		for (int j = tick - 1;j >= 0;j--) std::cout << consensus_2[j];
		std::cout << endl;

		return 0;
}
