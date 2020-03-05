#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <cmath>

using namespace std;

// Each function will return the number of correct predictions that it
// makes and the total of number branches in the correct format

// Each input will be the line of the trace, with format 
// *PC* *OUTCOME* *BRANCH TARGET*

// Return CORRECT: NUMBER OF CORRECT PREDICTIONS FROM EACH BRANCH
// Predictor #1: Always Taken
// Read in entire file as input, because line doesn't matter each prediction will be same
string always_taken(char* file){
	// Get file input
	ifstream infile(file);
	// Total branches
	int count;
	// Total correct predictions
	int total;
	// PC address
	unsigned long long PC;
	// Decision
	string decision;
	// Send to
	string line;
	while(getline(infile, line)){
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
string never_taken(char* file){
	// Get file input
	ifstream infile(file);
	// Total branches
	int count;
	// Total correct predictions
	int total;
	// PC address
	unsigned long long PC;
	// Decision
	string decision;
	// Send to
	string line;
	while(getline(infile, line)){
		stringstream s(line);
		s >> std::hex >> PC >> decision;
		// if
		if(decision == "NT"){
			correct++;
		}
		total++;
	}
	return (to_string(correct) + "," + to_string(total));
}


// Predictor #3: Bimodal Predictor with Single Bit of History
// Must hard-code in table size -> will do in main fcn
// Assume initial state of all prediction counters is Taken ("T")
// input: test to compare with, unsigned long PC, size of table
int bimodal_single_bit(int table_size, char *file){
	// Create and initialize prediction table
	std::vector<string> table;
	for(int i = 0; i < table_size; i++){
		// Initialize all values to Taken
		table.push_back("T");
	}
	string decision;
	string line;
	int count;
	int total;
	unsigned long long PC;

	while(getline(infile, line)){
		stringstream s(line);
		s >> std::hex >> addr >> decision;
		int index = addr % table_size;
		int table_index = table[index];
		if(table[index] == "T"){
			if(decision == "T"){ // match
				correct++;
			}
			else{
				table[index] == 0;
			}
		}
		if(table[index] == "NT"){
			if(decision == "NT"){ // match
				correct++;
			}
			else{
				table[index] = 1;
			}
		}
		count++;
	}
	return (to_string(correct) + "," + to_string(total) + ";");
}
