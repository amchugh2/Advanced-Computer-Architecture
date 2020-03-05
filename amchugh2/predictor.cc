#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <cmath>
#include <sstream>
#include <vector>

using namespace std;

// Each function will return the number of correct predictions that it
// makes and the total of number branches in the correct format

// Each input will be the line of the trace, with format 
// *PC* *OUTCOME* *BRANCH TARGET*

// Return CORRECT: NUMBER OF CORRECT PREDICTIONS FROM EACH BRANCH
// Predictor #1: Always Taken
// Read in entire file as input, because line doesn't matter each prediction will be same
string always_taken(char* file){
	ifstream infile(file);
	int correct = 0;
	int total = 0;
	unsigned long long PC;
	string decision;
	string line;

	while(getline(infile,line)){
		stringstream s(line);
		s >> std::hex >> PC >> decision;
		// if
		if(decision == "T"){
			correct++;
		}
		total++;
	}
	return (to_string(correct) + "," + to_string(total));
}

// Predictor #2: Never Taken
// Same concept as AT - > Do not need to calculate anything because always NT
string never_taken(char *file){
	// Get file input
	ifstream infile(file);
	// Total branches
	int correct = 0;
	// Total correct predictions
	int total = 0;
	// PC address
	unsigned long long PC;
	// Decision
	string decision;
	// Send to
	string line;
	while(infile.good()){
		getline(infile, line);
		stringstream s(line);
		s >> std::hex >> PC >> decision >> std::hex;
		// if
		if(decision == "NT"){
			correct++;
		}
		total++;
	}
	return (to_string(correct) + "," + to_string(total));
}


// Predictor #3: Bimodal Predictor with Single Bit of History
// Must hard-code in table size -> will be passed on as parameter
// Assume initial state of all prediction counters is Taken ("T")
// input: test to compare with, unsigned long PC, size of table
string bimodal_single_bit(int table_size, char *file){
	// Get file
	ifstream infile(file);
	// Create and initialize prediction table
	int table[table_size];

	for(int i = 0; i < table_size; i++){
		// Initialize all values to Taken
		table[i] = 1;
	}
	string decision;
	string line;
	int correct;
	int total;
	unsigned long long PC;

	while(getline(infile, line)){
		stringstream s(line);
		s >> std::hex >> PC >> decision;
		int index = PC % table_size;
		int table_index = table[index];
		if(table[index] == 1){
			if(decision == "T"){ // match
				correct++;
			}
			else{
				table[index] == 0;
			}
		}
		if(table[index] == 0){
			if(decision == "NT"){ // match
				correct++;
			}
			else{
				table[index] = 1;
			}
		}
		total++;
	}
	
	string str = to_string(correct) + "," + to_string(total) + ";";
	return str;
}

int main(int argc, char *argv[]){
	cout << always_taken(argv[0]) << endl;
}
