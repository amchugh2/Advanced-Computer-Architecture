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
		//cout << behavior << endl;
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
	// Open file for reading 
	// Initialize vars
	unsigned long long addr;
	string behavior, line;
	unsigned long long target;
	int correct, total;
	correct = 0;
	total = 0;
	int index;
	string prediction;
	int last_bits;

	ifstream infile(file);

	string table[table_size];
	for(int i = 0; i < table_size; i++){
		table[i] = "T";
	}

	// get index
	while(getline(infile,line)){
		stringstream s(line);
		s >> std::hex >> addr >> behavior >> std::hex >> target;
		
		// FIXME: saying behavior is always NT
		last_bits = (addr & ((1 << ((int)log2(table_size))) - 1));
		if(behavior == "T"){
			if(table[last_bits % table_size] == "T"){
				correct++;
			}
			// only going here
			if(table[last_bits % table_size] == "NT"){
				table[last_bits % table_size] = "T";
			}
		}
		else if(behavior == "NT"){
			if(table[last_bits % table_size] == "NT"){
			//	cout << "got to NT correct" << endl;
				correct++;
			}
			if(table[last_bits % table_size] == "T"){
			//	cout << "got to NT incorrect" << endl;
				table[last_bits % table_size] = "NT";
			}
		}
		total++;
	}
	string str = to_string(correct) + "," + to_string(total) + ";";
	return str;
}
// Predictor 4: Binomial Double Bit
string bimodal_double_bit(int table_size, char *file){
	// Open file for reading 
	// Initialize vars
	unsigned long long addr;
	string behavior, line;
	unsigned long long target;
	int correct, total;
	correct = 0;
	total = 0;
	int index;
	int prediction;
	int last_bits;

	ifstream infile(file);

	int table[table_size];
	for(int i = 0; i < table_size; i++){
		table[i] = 3;
	}

	// get index
	while(getline(infile,line)){
		stringstream s(line);
		s >> std::hex >> addr >> behavior >> std::hex >> target;

		// FIXME: saying behavior is always NT
		last_bits = (addr & ((1 << ((int)log2(table_size))) - 1));
		// strongly or weakly not taken
		if(table[last_bits % table_size] == 11 || table[last_bits % table_size] == 10){
			if(behavior == 4){
				correct++;
			}

		total++;
	}
	string str = to_string(correct) + "," + to_string(total) + ";";
	return str;
}

int main(int argc, char *argv[]){
	//vector<int> test_vals  = {16, 32, 128, 256, 512, 1024, 2048};
	cout << "Always Taken" << endl;
	cout << always_taken(argv[1]) << endl;
	cout << "Never Taken" << endl;
	cout << never_taken(argv[1]) << endl;
	cout << "Bimodal Single Bit" << endl;
	cout << bimodal_single_bit(16, argv[1]) << endl;
	cout << bimodal_single_bit(32, argv[1]) << endl;
	cout << bimodal_single_bit(128, argv[1]) << endl;
	cout << bimodal_single_bit(256, argv[1]) << endl;
	cout << bimodal_single_bit(512, argv[1]) << endl;
	cout << bimodal_single_bit(1024, argv[1]) << endl;
	cout << bimodal_single_bit(2048, argv[1]) << endl;
}
