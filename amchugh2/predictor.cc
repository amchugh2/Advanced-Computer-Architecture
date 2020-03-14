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

		last_bits = (addr & ((1 << ((int)log2(table_size))) - 1));
		// strongly or weakly not taken
		if(behavior == "T"){
			if(table[last_bits % table_size] == 0 || table[last_bits % table_size] == 1){
				table[last_bits % table_size]++;
			}
			else{
				table[last_bits % table_size] = 3;
				correct++;
			}
		}
		else if(behavior == "NT"){
			if(table[last_bits % table_size] == 3 || table[last_bits % table_size] == 2){
				table[last_bits % table_size]--;
			}
			else{
				table[last_bits % table_size] = 0;
				correct++;
			}
		}
		total++;
	}
	string str = to_string(correct) + "," + to_string(total) + ";";
	return str;
}

// Predictor 5: Gshare -> PC XOR'd with GHR to generate index. Fix table size at 2048. 
string gshare(unsigned int GHL, char *file){
	// initialize variables
	int correct;
	int total;
	// fixed table size at 2048
	int table_size = 2048;
	int table[table_size];
	// initialize to strongly taken
	for(int i = 0; i < table_size; i++){
		table[i] = 3;
	}

	unsigned long long addr;
	unsigned long long target;
	string behavior;
	string line;
	int prediction;
	int last_bits;
	correct = 0;
	total = 0;
	unsigned int GHR = 0;
	
	ifstream infile(file);

	while(getline(infile,line)){
		stringstream s(line);
		s >> std::hex >> addr >> behavior >> std::hex >> target;
		int index = ((GHR ^ addr) % table_size);
		if(behavior == "T"){
			if(table[index] == 0 || table[index] == 1){
				table[index]++;
			}
			else{
				table[index] = 3;
				correct++;
			}

			GHR = (((1 << (GHL)) - 1)) & ((GHR << 1) + 1);
		}
		else if(behavior == "NT"){
			if(table[index] == 3 || table[index] == 2){
				table[index]--;
			}
			else{
				table[index] = 0;
				correct++;
			}
			GHR = (((1 <<(GHL)) - 1)) & (GHR << 1);
		}
	total++;
	}
	string str = to_string(correct) + "," + to_string(total) + ";";
	return str;
}

string tournament(char* file){
	// initialize tables
	int table_size = 2048;
	int bimodal[table_size];
	int gshare[table_size];
	int selector[table_size];
	int correct;
	int GHR = 0;
	bool bimodal_correct;
	bool gshare_correct;
	int index;
	int gshare_index;
	int bimodal_index;
	int total;
	string line;
	unsigned long long addr;
	unsigned long long target;
	string behavior;

	// initialize tables 
	for(int i = 0; i < table_size; i++){
		// strongly taken
		bimodal[i] = 3;
		gshare[i] = 3;
		// prefer gshare
		selector[i] = 0;
	}


	// loop through all the branch instructions
	ifstream infile(file);
	while(getline(infile,line)){
		stringstream s(line);
		s >> std::hex >> addr >> behavior >> std::hex >> target;
		bimodal_correct = false;
		gshare_correct = false;
		index = (addr & ((1 << 11) - 1)) % 2048;
		gshare_index = (index ^ GHR) % 2048;
		// CHECK PREDICTOR THEN CHECK GSHARE
		// bimodal prediction: taken
		if(bimodal[index] == 3 || bimodal[index] == 2){
			// correct prediction
			if(behavior == "T"){
				bimodal[index] = 3;
				bimodal_correct = true;
			} // incorrect prediction
			else{
				bimodal[index]-=1;
			}
		}
		// bimodal prediction: not taken
		else{
			// incorrect prediction
			if(behavior == "NT"){
					bimodal[index] = 0;
					bimodal_correct = true;
				}
		       	// correct prediction
			else if(behavior == "T"){
				bimodal[index]+=1;
			}
		}

		// gshare prediction: taken
		if(gshare[gshare_index] == 2 || gshare[gshare_index] == 3){
			if(behavior == "T"){ // correct
				gshare[gshare_index] = 3;
				gshare_correct = true;
				GHR = ((GHR << 1) | 1) & ((1 << (11)) - 1);
			}
			else if(behavior == "NT"){ // incorrect
				gshare[gshare_correct]-=1;
				GHR = (GHR << 1) & ((1 << (11)) - 1);

			}
		} // prediction is not taken
		else{
			if(behavior == "T"){ // incorrect
				gshare[gshare_index]+=1;
				GHR = ((GHR << 1) | 1) & ((1 << (11)) - 1);

			}
			else if(behavior == "NT"){ // correct
				gshare[gshare_index] = 0;
				gshare_correct = true;
				GHR = (GHR << 1) & ((1 << (11)) - 1);
			}
		}
		// selector
		if((gshare_correct == true) && (bimodal_correct == true)){
			correct++;
			continue;
		}
		else if((gshare_correct == false) &&( bimodal_correct == false)){
			continue;
		}

		// if selector is 0 or 1 prefer gshare
		if(selector[index] == 0 || selector[index] == 1){
			if(gshare_correct == true){ // correct prediction
				selector[index] = 0;
				correct++;
			}
			else if(gshare_correct == false){ // incorrect prediction
				selector[index]+=1;
			}
		}
		else{
			if(bimodal_correct == true){ // correct prediction
				selector[index] = 3;
				correct++;
			}
			else if(bimodal_correct == false){
				selector[index]-=1;
			}
		}
		total++;
	}
	string str = to_string(correct) + "," + to_string(total) + ";";
	return str;
}		

int main(int argc, char *argv[]){
	//vector<int> test_vals  = {16, 32, 128, 256, 512, 1024, 2048};
	/*
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
		
	cout << "Bimodal Double Bit" << endl;
	cout << bimodal_double_bit(16, argv[1]) << endl;
	cout << bimodal_double_bit(32, argv[1]) << endl;
	cout << bimodal_double_bit(128, argv[1]) << endl;
	cout << bimodal_double_bit(256, argv[1]) << endl;
	cout << bimodal_double_bit(512, argv[1]) << endl;
	cout << bimodal_double_bit(1024, argv[1]) << endl;
	cout << bimodal_double_bit(2048, argv[1]) << endl;
	
	cout << "GShare" << endl;
	cout << gshare(3, argv[1]) << endl;
	cout << gshare(4, argv[1]) << endl;
	cout << gshare(5, argv[1]) << endl;
	cout << gshare(6, argv[1]) << endl;
	cout << gshare(7, argv[1]) << endl;
	cout << gshare(8, argv[1]) << endl;
	cout << gshare(9, argv[1]) << endl;
	cout << gshare(10, argv[1]) << endl;
	cout << gshare(11, argv[1]) << endl;
	*/

	cout << "Tournament" << endl;
	cout << tournament(argv[1]) << endl;
}
