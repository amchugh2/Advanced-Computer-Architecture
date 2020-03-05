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
	int count = 0;
	// Total correct predictions
	int correct = 0;
	// Idk
	unsigned long long address;
	while(getline(infile, line)){
		stringstream s(line);

}

/*
// Predictor #2: Never Taken
// Same concept as AT - > Do not need to calculate anything because always NT
int never_taken(std::vector<std::string> s){
	int correct = 0;
	for(int i = 0; i < s.size(); i++){
		if(s[i] == "NT"){
			correct++;
		}
	}
	return correct;
}

// Predictor #3: Bimodal Predictor with Single Bit of History
// Must hard-code in table size -> will do in main fcn
// Assume initial state of all prediction counters is Taken ("T")
// input: test to compare with, unsigned long PC, size of table
int bimodal_single_bit(std::vector<std::string> test, vector<unsigned long> pc, int size){
	int correct = 0;
	// Create and initialize prediction table
	std::vector<string> table;
	for(int i = 0; i < size; i++){
		// Initialize all values to Taken
		table.push_back("T"); 
	}
	// Loop through PC
	for(int i = 0; i < s.size(); i++){
		// Index into history depending on table size
		int index = pc[i]%size;
		if(test[i] == pc[i]){
			correct++;
		}
		else{ // Change for next call
			test[i] = pc[i];
		}
		
	}
	return correct;
}

*/
