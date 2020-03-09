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
	unsigned long long addr;
	string behavior, line;
	unsigned long long target;
	int correct, total;
	correct = 0;
	total = 0;
	ifstream infile(file);
	
	while(getline(infile,line)){
		stringstream s(line);
		s >> std::hex >> addr >> behavior >> std::hex >> target;
		if(behavior == "T"){
			correct = correct + 1;
		}
	total = total + 1;
	}
	string str = to_string(correct) + "," + to_string(total) + ";\n";
	return str;
}

// Predictor #2: Never Taken
// Same concept as AT - > Do not need to calculate anything because always NT
string never_taken(char *file){
	unsigned long long addr;
	string behavior, line;
	unsigned long long target;
	int correct, total;
	correct = 0;
	total = 0;
	ifstream infile(file);
	
	while(getline(infile,line)){
		stringstream s(line);
		s >> std::hex >> addr >> behavior >> std::hex >> target;
		if(behavior == "NT"){
			correct++;
		}
	total++;
	}
	string str = to_string(correct) + "," + to_string(total) + ";\n";
	return str;
}


// Predictor #3: Bimodal Predictor with Single Bit of History
// Must hard-code in table size -> will be passed on as parameter
// Assume initial state of all prediction counters is Taken ("T")
// input: test to compare with, unsigned long PC, size of table
string bimodal_single_bit(int table_size, char *file){
	// initialize vars
	ifstream infile(file);
	unsigned long long addr;
	unsigned long long target;
	string behavior, line;
	int correct;
	int total;
	int index;
	
	string prediction;
	vector<string> predictions;
	for(int i = 0; i < table_size; i++){
		predictions.push_back("T");
	}
	
	while(getline(infile, line)){
		stringstream s(line);
		s >> std::hex >> addr >> behavior >> target;
		int last_bits = (addr & (( 1 << (int)log2(table_size))) - 1);
		prediction = predictions[last_bits % table_size];
		if(prediction == "T"){
			if(behavior == "T"){
				correct++;
			}
			else{
				predictions[last_bits % table_size] = "NT";
			}
		}
		else if(prediction == "NT"){
			if(behavior == "NT"){
				correct++;
			}
			else{
				predictions[last_bits % table_size] = "T";
		}
		total++;
	}

	/*
	// initialize prediction table
	bool table[table_size];
	for(int i = 0; i < table_size; i++){
		table[i] == true;
	}

	// get index
	while(getline(infile, line)){
		stringstream s(line);
		s >> std::hex >> addr >> behavior >> target;
		index = addr % table_size;
		if(behavior == "T"){
			if(table[index] == true){
				correct++;
			}
			else if(table[index] == false){
				table[index] = true;
			}
		}
		if(behavior == "NT"){
			if(table[index] == false) {
				correct++;
			}
			else if(table[index] == true){
				table[index] = false;
			}
			
		}
		total++;
	}
	*/

	string str = to_string(correct) + "," + to_string(total) + ";";
	return str;
}

int main(int argc, char *argv[]){
	cout << always_taken(argv[1]) << endl;
	cout << never_taken(argv[1]) << endl;
	cout << bimodal_single_bit(16, argv[1]) << endl;
}
