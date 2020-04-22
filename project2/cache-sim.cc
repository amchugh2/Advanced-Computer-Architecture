#include <stddef.h>
#include <stdlib.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <cmath>
#include <sstream>
#include <vector>

using namespace std;

// create entry struct
struct entry { 
	string instr; // instruction
	unsigned long long addr;
};

// cache entry struct
struct Cache {
	int valid;
	unsigned long tag;
	unsigned long index;
	int lru;
};

int directMapped(vector<entry> entries, int cacheSize){
	int hits = 0;
	// assume each cache line size is 32 bytes
	unsigned long cacheLines = cacheSize / 32;
	// create cache structure
	struct Cache cache[cacheLines];

	// initialize cache
	for(int i = 0; i < cacheLines; i++){
		cache[i].valid = 0;
		cache[i].tag = 0;
		cache[i].index = 0;
	}

	// loop through each of the entries
	for(entry e : entries){
		unsigned long tag = e.addr >> 5;
		// index into cache
		unsigned long index = tag % cacheLines;
		// if valid bit is 1 and tags match: HIT
		if(cache[index].valid == 1 && cache[index].tag == tag){
			hits+=1;
		}
		else { // miss
			// update cache, put entry in
			cache[index].valid = 1;
			cache[index].tag = tag;
		}
	}
	return hits;
}

int setAssociative(vector<entry> entries, int ways) {
	int hits = 0;
	
	unsigned long sets = 512/ways;
	struct Cache cache[sets][ways];
	
	for (int i = 0; i < sets; i++){
		for (int j = 0; j < ways; j++){
			cache[i][j].valid = 0;
			cache[i][j].tag = 0;
			cache[i][j].index = 0;
			cache[i][j].lru = 0;
		}
	}
//check for hits
	for (entry e: entries){
		unsigned long tag = e.addr >> 5;
		unsigned long index = tag % sets;

		for (int i = 0; i < ways; i++){
			if (cache[index][i].valid && cache[index][i].tag == tag){
				//cache hit!!
				hits++;
				for (int j = 0; j < ways; j++){
					if (j != i) cache[index][j].lru = cache[index][j].lru + 1;
					else cache[index][j].lru = 0;
				}
				break;
			}
			else if (!cache[index][i].valid){
				cache[index][i].tag = tag;
				cache[index][i].valid = 1;
				for (int j = 0; j < ways; j++){
					if (j != i) cache[index][j].lru = cache[index][j].lru + 1;
					else cache[index][j].lru = 0;
				}
			}
			else if (i == (ways-1)){
				int high = cache[index][i].lru;
				int highIndex = i;
				for (int j = 0; j < ways; j++){
					if (cache[index][j].lru > high){
						high = cache[index][j].lru;
						highIndex = j;
					}
				}
				cache[index][highIndex].lru = 0;
				for (int j = 0; j < ways; j++){
					if (j != highIndex) cache[index][j].lru = cache[index][j].lru + 1;
					else cache[index][j].lru = 0;
				}
				cache[index][highIndex].tag = tag;
				cache[index][highIndex].valid = 1;
			}
		}
	}
	return hits;
}

int LRUFullyAssociative(vector<entry> entries){
	int hits = 0;
	unsigned long ways = 16384/32;
	struct Cache cache[ways];
	
	for (int i = 0; i < ways; i++){
		cache[i].valid = 0;
		cache[i].tag = 0;
		cache[i].index = 0;
		cache[i].lru = 0;
	}
	
	for (entry e: entries){
		unsigned long tag = e.addr >> 5;
		for (int i = 0; i < ways; i++){
			if (cache[i].valid && cache[i].tag == tag){
				hits++;
				for (int j = 0; j < ways; j++){
					if (j != i){
						cache[j].lru = cache[j].lru + 1;
					}
					else {
						cache[j].lru = 0;
					}
				}
				break;
			}
			else if (!cache[i].valid){
				cache[i].tag = tag;
				cache[i].valid = 1;
				for (int j = 0; j < ways; j++){
					if (j != i) {
						cache[j].lru = cache[j].lru + 1;
					}
					else{
						cache[j].lru = 0;
					}
				}
			}
			
			else if (i == (ways-1)){
				int high = cache[i].lru;
				int highIndex = i;
				for (int j = 0; j < ways; j++){
					if (cache[j].lru > high){
						high = cache[j].lru;
						highIndex = j;
					}
				}
				cache[highIndex].lru = 0;
				for (int j = 0; j < ways; j++){
					if (j != highIndex) {
						cache[j].lru = cache[j].lru + 1;
					}
					else{ cache[j].lru = 0;
					}
				}
				cache[highIndex].tag = tag;
				cache[highIndex].valid = 1;
			}
		}
	}
	return hits;

}

int HCFullyAssociative(vector<entry> entries){
	int hits = 0;
	unsigned long ways = 16374/32;
	// create cache structure
	struct Cache cache[ways];

	// initialize values
	for(int i = 0; i < ways; i++){
		cache[i].tag = 0;
		cache[i].valid = 0;
		cache[i].index = 0;
	}

	// create and initialize hc to all 0s
	int hc[ways - 1] = {0};

	// loop
	for(entry e : entries){
		unsigned long tag = e.addr >> 5;
		bool tag_found = false;
		int hit_index = 0;

		for(int i = 0; i < ways; i++){
			// hit
			if(cache[i].valid && cache[i].tag == tag){
				hits+=1;
				hit_index = i;
				tag_found = true;
				// don't update
				break;
			}
		}

		int new_index = 0;
		if(tag_found == true){
			new_index = hit_index + ways;
			while(new_index >= 0){
				// right child
				if(new_index % 2 == 0){
					new_index = (new_index - 2) / 2;
					hc[new_index] = 1;
				}
				// left child
				else{
					new_index = (new_index - 1) / 2;
					hc[new_index] = 0;
				}
			}
		}

		// cache misses
		else {
			while(new_index < ways){
				// right child is cold
				if(hc[new_index] == 0){
					hc[new_index] = 1;
					new_index = (new_index * 2) + 2;
				}
				else{
					hc[new_index] = 0;
					new_index = (new_index * 2) + 1;
				}
			}

			int cold_index = new_index - ways;
			cache[cold_index].valid = 1;
			cache[cold_index].tag = tag;
		}
	}
	return hits;
}

int main(int argc, char *argv[]){
	
	// read input
	string input = argv[1];
	string output = argv[2];
	
	ifstream infile(input);
	ofstream outfile(output);

	string instr;
	unsigned long long addr;
	vector<entry> entries;

	while(infile >> instr >> std::hex >> addr) {
		entry e;
		e.instr = instr;
		e.addr = addr;
		entries.push_back(e);
	}
	
	/*
	// Direct Mapped
	outfile << directMapped(entries, 1024) << "," << entries.size() << "; ";
	outfile << directMapped(entries, 4096) << "," << entries.size() << "; ";
	outfile << directMapped(entries, 16384) << "," << entries.size() << "; ";
	outfile << directMapped(entries, 32768) << "," << entries.size() << "; ";	
	outfile << endl;

	// Set-Associative
	outfile << setAssociative(entries, 2) << "," << entries.size() << "; ";
	outfile << setAssociative(entries, 4) << "," << entries.size() << "; ";
	outfile << setAssociative(entries, 8) << "," << entries.size() << "; ";
	outfile << setAssociative(entries, 16) << "," << entries.size() << "; ";
	outfile << endl;

	// LRU Full-Associative
	outfile << LRUFullyAssociative(entries) << "," << entries.size() << "; ";
	return 0;
	*/

	// HC Fully-Associative 
	outfile << HCFullyAssociative(entries) << ";" << entries.size() << "; ";
	return 0;
}

