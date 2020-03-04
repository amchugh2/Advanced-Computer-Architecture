#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <cmath>

// Return CORRECT: NUMBER OF CORRECT PREDICTIONS FROM EACH BRANCH
// Predictor #1: Always Taken
// Do not need to calculate anything because always T
int always_taken(td::vector<std::string> s){
	int correct = 0;
	for(int i = 0, i < s.size(); i++){
		// Check if correct
		if(s[i] == "T"){
			correct++;
		}
	}
	return correct;
}

// Predictor #2: Never Taken
// Same concept as AT - > Do not need to calculate anything because always NT
int never_taken(std::vector<std::string> s){
	int correct = 0;
	for(int i = 0; i < s.size(); i++){
		if(s[i] == "T"){
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

// Predictor #4: Bimodal Predictor with Double Bit of History
