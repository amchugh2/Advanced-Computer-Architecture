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
struct cache {
	int valid;
	unsigned long tag;
	unsigned long index;
	int lru;
};

int directMapped(vector<entry> entries, int cacheSize){
	int hits = 0;
	// assume each cache line size is 32 bytes
	unsigned long long cacheLines = cacheSize / 32;
	// create cache structure
	struct cache cache[cacheLines];

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

int setAssociative(vector<entry> entries, int assoc) {
	int hits = 0;
	unsigned long cacheLines = 512/assoc;

	// create and initialize cache
	struct cache[cacheLines][associativity];
	for(int i = 0; i < cacheLines; i++){
		for(int j = 0; j < assoc; j++){
			cache[i][j].valid = 0;
			cache[i][j].tag = 0;
			cache[i][j].index = 0;
			cache[i][j].lru = 0;
		}
	}

	// loop through entries
	for(entry e : entries){
		// offset by 5
		unsigned long long tag = e.address >> 5;
		unsigned long index = tag % cacheLines;
		// loop thru sets
		for(int i = 0; i < assoc; i++){
			if(cache[index][i].valid == 1 && cache[index][i].tag = tag){
				hits+=1;
				// set as most recently used
				for(int j = 0; i < assoc; j++){
					// if not most recently used
					if(j != 1){
						cache[index][j].lru = 1;
					}
					break;
				}
				// if valid bit is 0
				else if(cache[index][i].valid == 0){
					// insert
					cache[index][i].tag = tag;
					cache[index][i].valid = 1;
					// set as most recently used
					for(int j = 0; j < assoc; j++){
						if(j != 1){
							cache[index][j].lru = 1;
						}
					}
				}
				else if(i == (assoc - 1)){
					int high = cache[index][i].lru;
					int highIndex = i;
					for(int j = 0; j < assoc; j++){
						if(cache[index][j].lru > high){
							high = cache[index][j].lru;
							highIndex = j;
						}
					}
					for(int j = 0; j < assoc; j++){
						if( j!= highIndex){
							cache[index][j].lru = 1;
						}
					}
					cache[index][highIndex].tag = tag;
					cache[index][highIndex].valid = 1;
				} 
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

	return 0;
}

