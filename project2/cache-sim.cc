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
	/* Set Associative Steps:
	 * PRIORITY QUEUE: higher cache blocks are the least recently used
	 * 1. Create cache structure with appropriate amount of ways and sets
	 * 2. Initialize the cache structure
	 * 3. Loop through each entry in the trace
	 * 	a. If the tags match and the bit for the entry is valid, then it is a cache HIT
	 * 		if the way isn't the one we just accessed, then increment it's value
	 * 		otherwise reset the lru value to 0
	 * 	b. If the valid bit is 0, then it is a cache MISS
	 * 		insert the data into the caches
	 * 		repeat: if the way isn't the one that we just accessed, then increment it's value
	 * 4. Take care of LRU
	 * 	else if(i ==(ways-i)). If the way is the highest way, and hasn't been accessed as either ah it or a miss in this cycle
	 * 		assign it as the highest LRU value (least recently used)
	 * 		assign index as the cache's least recently used index
	 * 		for each of the other ways
	 * 		if the LRU value is higher, then it is the least recently used way
	 * 		assign it as the new least recently used cache block
	 * 		when done with for loop:
	 * 			assign the previous LRU cache block as 0 (because now it was used)
	 * 			increment each of the other ways in the set, because now they are less recently used
	 * 			put the value in the least recently used cache block	
	 *
	 *
	 */
	int hits = 0;
	
	// create cache structure	
	unsigned long sets = 512/ways;
	struct Cache cache[sets][ways];
	
	// initialize cache
	for (int i = 0; i < sets; i++){
		for (int j = 0; j < ways; j++){
			cache[i][j].valid = 0;
			cache[i][j].tag = 0;
			cache[i][j].index = 0;
			cache[i][j].lru = 0;
		}
	}
	// for each entry in entries
	for (entry e: entries){
		// assign access
		unsigned long tag = e.addr >> 5;
		unsigned long index = tag % sets;
		
		// for each way
		for (int i = 0;	i < ways; i++){
			// if the data is valid for the set and the tags match, hit
			if (cache[index][i].valid && cache[index][i].tag == tag){
				//cache hit!!
				hits++;
				// if the way isn't recently used then increment it's value
				for (int j = 0; j < ways; j++){
					if (j != i) cache[index][j].lru = cache[index][j].lru + 1;
					// otherwise reset to 0
					else cache[index][j].lru = 0;
				}
				break;
			}
			// miss: data isn't valid
			else if (!cache[index][i].valid){
				// insert data into cache
				cache[index][i].tag = tag;
				cache[index][i].valid = 1;
				// for the ways that weren't use, increment their LRU value
				for (int j = 0; j < ways; j++){
					if (j != i) cache[index][j].lru = cache[index][j].lru + 1;
					else cache[index][j].lru = 0;
				}
			}
			// LRU associativity
			else if (i == (ways-1)){ // if the highest way (and it hasn't been used as a hit or a miss)
				// assign it as the highest LRU value (it is the least recently used way)
				int high = cache[index][i].lru;
				// assign high index as this cache's i
				int highIndex = i;
				// for each of the other ways
				for (int j = 0; j < ways; j++){
					// if the LRU value is higher then assign it as the LRU
					if (cache[index][j].lru > high){
						high = cache[index][j].lru;
						highIndex = j;
					}
				}
				// reset lru of LEAST RECENTLY USED cache block to 0
				cache[index][highIndex].lru = 0;
				// for all of the other ways
				for (int j = 0; j < ways; j++){
					// increment the rest of the ways because now they are less recently used than the previous least
					// recently used
					if (j != highIndex) cache[index][j].lru = cache[index][j].lru + 1;
					else cache[index][j].lru = 0;
				}
				// put the value in the least recently used block
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

// still not working BRUHHH
int HCFullyAssociative(vector<entry> entries){
	/*
	* Hot - Cold Fully Associative Cache Steps
	 * 1. Create cache with appropriate ways/sets
	 * 2. Initialize cache
	 * 3. Create hc cache and intialize to all 0s
	 * 4. Loop through each entry in the set
	 * 5. Create 
	 * 	tag_found: if tag found in cache (used for HC later on)
	 * 	hit_index: index of the last cache block that was a hit
	 * 6. If tag matches and valid bit is 1, then HIT
	 * 	increment hits
	 * 	set the index to the current block
	 * 	set the tag_found to true
	 * 	BREAK
	 * 7. If tag_found is true
	 * 	new_index = hit_index + ways
	 * 	while the new index is greater than zero
	 * 	if the hit is the right child:
	 * 		the new index is the child of the right child
	 * 	if the hit is the left child
	 * 		the new index is the child of the left child
	 *
	 * 	
	 *
	int hits = 0;
	unsigned long ways = 16384/32;
	// create cache structure
	struct Cache cache[ways];

	// initialize values
	for(int i = 0; i < ways; i++){
		cache[i].tag = 0;
		cache[i].valid = 0;
		cache[i].index = 0;
	}

	// create and initialize hc to all 0s
	int hc[ways - 1];
	for(int i = 0; i < (ways-1); i++){
		hc[i] = 0;
	}

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
*/

int noAllocationSA(vector<entry> entries, int ways){
	/* No Allocation on Write Miss Set Associative Cache
	 * - If a store isn misses into the cache, then the missing line is NOT written into the cache, but
	 *   instead directly to memory
	 * 1. Set up cache same way as SA
	 * 2. Iterate though entries
	 * 3. If e.instr is "S" (store), DO NOTHING WITH THE MISS (we can't write to 'memory' in this program)
	 * 4. If e.instr is "L" (load), DO THE SAME THING AS SA LRU
	 */
	
	int hits = 0;
	unsigned long sets = 512/ways;
	struct Cache cache[sets][ways];
	
	// initialize cache same way as set associative
	for(int i = 0; i < sets; i++){
		for(int j = 0; j < ways; j++){
			cache[i][j].valid = 0;
			cache[i][j].tag = 0;
			cache[i][j].index = 0;
			cache[i][j].lru = 0;
		}
	}

	for(entry e : entries){
		unsigned long tag = e.addr >> 5;
		unsigned long index = tag % sets;
		if(e.instr == "S"){
			for(int i = 0; i < ways; i++){
				if(cache[index][i].valid && cache[index][i].tag == tag){ // hit
					hits+=1;
					for(int j = 0; j < ways; j++){
						if(j !=i) cache[index][j].lru = cache[index][j].lru + 1;
						else cache[index][j].lru = 0;
					}
					break;
				}
			}
		}
		else { // LOAD
		for(int i = 0; i < ways; i++){
			if (cache[index][i].valid && cache[index][i].tag == tag){
				//cache hit!!
				hits++;
				// if the way isn't recently used then increment it's value
				for (int j = 0; j < ways; j++){
					if (j != i) cache[index][j].lru = cache[index][j].lru + 1;
					// otherwise reset to 0
					else cache[index][j].lru = 0;
				}
				break;
			}
			// miss: data isn't valid
			else if (!cache[index][i].valid){
				// insert data into cache
				cache[index][i].tag = tag;
				cache[index][i].valid = 1;
				// for the ways that weren't use, increment their LRU value
				for (int j = 0; j < ways; j++){
					if (j != i) cache[index][j].lru = cache[index][j].lru + 1;
					else cache[index][j].lru = 0;
				}
			}
			// LRU associativity
			else if (i == (ways-1)){ // if the highest way (and it hasn't been used as a hit or a miss)
				// assign it as the highest LRU value (it is the least recently used way)
				int high = cache[index][i].lru;
				// assign high index as this cache's i
				int highIndex = i;
				// for each of the other ways
				for (int j = 0; j < ways; j++){
					// if the LRU value is higher then assign it as the LRU
					if (cache[index][j].lru > high){
						high = cache[index][j].lru;
						highIndex = j;
					}
				}
				// reset lru of LEAST RECENTLY USED cache block to 0
				cache[index][highIndex].lru = 0;
				// for all of the other ways
				for (int j = 0; j < ways; j++){
					// increment the rest of the ways because now they are less recently used than the previous least
					// recently used
					if (j != highIndex) cache[index][j].lru = cache[index][j].lru + 1;
					else cache[index][j].lru = 0;
				}
				// put the value in the least recently used block
				cache[index][highIndex].tag = tag;
				cache[index][highIndex].valid = 1;
				}
			}
		}
	}
	return hits;
}

int NLPrefetchSA(vector<entry> entries, int ways){
	/* Set - Associative Cache with Next-line Prefetch
	 * 1. Do regular SA with LRU
	 * 2. Prefetch the next line
	 * 	same process as above just with next line
	 */
	int hits = 0;
	
	// create cache structure	
	unsigned long sets = 512/ways;
	struct Cache cache2[sets][ways];
	
	// initialize cache
	for (int i = 0; i < sets; i++){
		for (int j = 0; j < ways; j++){
			cache2[i][j].valid = 0;
			cache2[i][j].tag = 0;
			cache2[i][j].index = 0;
			cache2[i][j].lru = 0;
		}
	}

	// iterate through entries
	for(entry e : entries){
		unsigned long tag = e.addr >> 5;
		unsigned long index = tag % sets;
		for(int i = 0; i < ways; i++){
			if (cache2[index][i].valid && cache2[index][i].tag == tag){
				//cache hit
				hits++;
				// if the way isn't recently used then increment it's value
				for (int j = 0; j < ways; j++){
					if (j != i) cache2[index][j].lru = cache2[index][j].lru + 1;
					// otherwise reset to 0
					else cache2[index][j].lru = 0;
				}
				break;
			}
			// miss: data isn't valid
			else if (!cache2[index][i].valid){
				// insert data into cache
				cache2[index][i].tag = tag;
				cache2[index][i].valid = 1;
				// for the ways that weren't use, increment their LRU value
				for (int j = 0; j < ways; j++){
					if (j != i) cache2[index][j].lru = cache2[index][j].lru + 1;
					else cache2[index][j].lru = 0;
				}
			}
			// LRU associativity
			else if (i == (ways-1)){ // if the highest way (and it hasn't been used as a hit or a miss)
				// assign it as the highest LRU value (it is the least recently used way)
				int high = cache2[index][i].lru;
				// assign high index as this cache's i
				int highIndex = i;
				// for each of the other ways
				for (int j = 0; j < ways; j++){
					// if the LRU value is higher then assign it as the LRU
					if (cache2[index][j].lru > high){
						high = cache2[index][j].lru;
						highIndex = j;
					}
				}
				// reset lru of LEAST RECENTLY USED cache block to 0
				cache2[index][highIndex].lru = 0;
				// for all of the other ways
				for (int j = 0; j < ways; j++){
					// increment the rest of the ways because now they are less recently used than the previous least
					// recently used
					if (j != highIndex) cache2[index][j].lru = cache2[index][j].lru + 1;
					else cache2[index][j].lru = 0;
				}
				// put the value in the least recently used block
				cache2[index][highIndex].tag = tag;
				cache2[index][highIndex].valid = 1;
				}
			}
		// PREFETCH
		// increment index and tag
		index = (index + 1) % sets;
		tag++;
		for(int i = 0; i < ways; i++){
			if (cache2[index][i].valid && cache2[index][i].tag == tag){
				//cache hit
				// if the way isn't recently used then increment it's value
				for (int j = 0; j < ways; j++){
					if (j != i) cache2[index][j].lru = cache2[index][j].lru + 1;
					// otherwise reset to 0
					else cache2[index][j].lru = 0;
				}
				break;
			}
			// miss: data isn't valid
			else if (!cache2[index][i].valid){
				// insert data into cache
				cache2[index][i].tag = tag;
				cache2[index][i].valid = 1;
				// for the ways that weren't use, increment their LRU value
				for (int j = 0; j < ways; j++){
					if (j != i) cache2[index][j].lru = cache2[index][j].lru + 1;
					else cache2[index][j].lru = 0;
				}
			}
			// LRU associativity
			else if (i == (ways-1)){ // if the highest way (and it hasn't been used as a hit or a miss)
				// assign it as the highest LRU value (it is the least recently used way)
				int high = cache2[index][i].lru;
				// assign high index as this cache's i
				int highIndex = i;
				// for each of the other ways
				for (int j = 0; j < ways; j++){
					// if the LRU value is higher then assign it as the LRU
					if (cache2[index][j].lru > high){
						high = cache2[index][j].lru;
						highIndex = j;
					}
				}
				// reset lru of LEAST RECENTLY USED cache block to 0
				cache2[index][highIndex].lru = 0;
				// for all of the other ways
				for (int j = 0; j < ways; j++){
					// increment the rest of the ways because now they are less recently used than the previous least
					// recently used
					if (j != highIndex) cache2[index][j].lru = cache2[index][j].lru + 1;
					else cache2[index][j].lru = 0;
				}
				// put the value in the least recently used block
				cache2[index][highIndex].tag = tag;
				cache2[index][highIndex].valid = 1;
				}
			}
		}
	return hits;
}

int missPrefetchSA(vector<entry> entries, int ways){
	int hits = 0;
	bool is_hit = false;

	unsigned long sets = 512/ways;
	struct Cache cache[sets][ways];

	for(int i = 0; i < sets; i++){
		for(int j = 0; j < ways; j++){
			cache[i][j].valid = 0;
			cache[i][j].tag = 0;
			cache[i][j].index = 0;
			cache[i][j].lru = 0;
		}
	}

	for(entry e : entries){
		unsigned long tag = e.addr >> 5;
		unsigned long index = tag % sets;
		for(int i = 0; i < ways; i++){
			if(cache[index][i].valid && cache[index][i].tag == tag){
				hits++;
				is_hit = true;
				for (int j = 0; j < ways; j++){
					if (j != i) cache[index][j].lru = cache[index][j].lru + 1;
					// otherwise reset to 0
					else cache[index][j].lru = 0;
				}
				break;
			}
			// miss: data isn't valid
			else if (!cache[index][i].valid){
				is_hit = false;
				// insert data into cache
				cache[index][i].tag = tag;
				cache[index][i].valid = 1;
				// for the ways that weren't use, increment their LRU value
				for (int j = 0; j < ways; j++){
					if (j != i) cache[index][j].lru = cache[index][j].lru + 1;
					else cache[index][j].lru = 0;
				}
			}
			// LRU associativity
			else if (i == (ways-1)){ // if the highest way (and it hasn't been used as a hit or a miss)
				// assign it as the highest LRU value (it is the least recently used way)
				is_hit = false;
				int high = cache[index][i].lru;
				// assign high index as this cache's i
				int highIndex = i;
				// for each of the other ways
				for (int j = 0; j < ways; j++){
					// if the LRU value is higher then assign it as the LRU
					if (cache[index][j].lru > high){
						high = cache[index][j].lru;
						highIndex = j;
					}
				}
				// reset lru of LEAST RECENTLY USED cache block to 0
				cache[index][highIndex].lru = 0;
				// for all of the other ways
				for (int j = 0; j < ways; j++){
					// increment the rest of the ways because now they are less recently used than the previous least
					// recently used
					if (j != highIndex) cache[index][j].lru = cache[index][j].lru + 1;
					else cache[index][j].lru = 0;
				}
				// put the value in the least recently used block
				cache[index][highIndex].tag = tag;
				cache[index][highIndex].valid = 1;
			}	
		}
	       	// if it's a miss -> PREFETCH
		if(is_hit == false){
			int updated_index = (index + 1) % sets;
			tag++;
		// for each way
			for (int i = 0;	i < ways; i++){
			// if the data is valid for the set and the tags match, hit
				if (cache[updated_index][i].tag == tag){
				// if the way isn't recently used then increment it's value
					for (int j = 0; j < ways; j++){
						if (j != i) cache[updated_index][j].lru = cache[updated_index][j].lru + 1;
					// otherwise reset to 0
						else cache[updated_index][j].lru = 0;
					}
					break;
				}
				// miss: data isn't valid
				else if (!cache[updated_index][i].valid){
					// insert data into cache
					cache[updated_index][i].tag = tag;
					cache[updated_index][i].valid = 1;
					// for the ways that weren't use, increment their LRU value
					for (int j = 0; j < ways; j++){
						if (j != i) cache[updated_index][j].lru = cache[updated_index][j].lru + 1;
						else cache[updated_index][j].lru = 0;
					}
				}
				// LRU associativity
				else if (i == (ways-1)){ // if the highest way (and it hasn't been used as a hit or a miss)
					// assign it as the highest LRU value (it is the least recently used way)
					int high = cache[updated_index][i].lru;
					// assign high index as this cache's i
					int highIndex = i;
					// for each of the other ways
					for (int j = 0; j < ways; j++){
						// if the LRU value is higher then assign it as the LRU
						if (cache[updated_index][j].lru > high){
							high = cache[updated_index][j].lru;
							highIndex = j;
						}
					}
					// reset lru of LEAST RECENTLY USED cache block to 0
					cache[updated_index][highIndex].lru = 0;
					// for all of the other ways
					for (int j = 0; j < ways; j++){
						// increment the rest of the ways because now they are less recently used than the previous least
						// recently used
						if (j != highIndex) cache[updated_index][j].lru = cache[updated_index][j].lru + 1;
						else cache[updated_index][j].lru = 0;
					}
					// put the value in the least recently used block
					cache[updated_index][highIndex].tag = tag;
					cache[updated_index][highIndex].valid = 1;
				}
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
	// order:
	// 1. direct mapped
	// 2. set associaitve
	// 3. fully associative LRU
	// 4. fully associative hot cold
	// 5. associative without store allocation
	// 6. associative cache next line prefetch
	// 7. associative cache next line prefetch misses
	
	// Direct Mapped
	outfile << directMapped(entries, 1024) << "," << entries.size() << "; ";
	outfile << directMapped(entries, 4096) << "," << entries.size() << "; ";
	outfile << directMapped(entries, 16384) << "," << entries.size() << "; ";
	outfile << directMapped(entries, 32768) << "," << entries.size() << "; " << endl;

	// Set-Associative
	outfile << setAssociative(entries, 2) << "," << entries.size() << "; ";
	outfile << setAssociative(entries, 4) << "," << entries.size() << "; ";
	outfile << setAssociative(entries, 8) << "," << entries.size() << "; ";
	outfile << setAssociative(entries, 16) << "," << entries.size() << "; " << endl;

	// LRU Full-Associative
	outfile << LRUFullyAssociative(entries) << "," << entries.size() << "; " << endl;

	// HC Fully-Associative NOT WORKING
	outfile << HCFullyAssociative(entries) << ";" << entries.size() << "; " << endl;

	// Set- Associative Cache no Allocation on a Write Miss
	outfile << noAllocationSA(entries, 2) << "," << entries.size() << "; ";
	outfile << noAllocationSA(entries, 4) << "," << entries.size() << "; ";
	outfile << noAllocationSA(entries, 8) << "," << entries.size() << "; ";
	outfile << noAllocationSA(entries, 16) << "," << entries.size() << "; " << endl;

	outfile << NLPrefetchSA(entries, 2) << "," << entries.size() << "; ";
	outfile << NLPrefetchSA(entries, 4) << "," << entries.size() << "; ";
	outfile << NLPrefetchSA(entries, 8) << "," << entries.size() << "; ";
	outfile << NLPrefetchSA(entries, 16) << "," << entries.size() << "; " << endl;


	outfile << missPrefetchSA(entries, 2) << "," << entries.size() << "; ";
	outfile << missPrefetchSA(entries, 4) << "," << entries.size() << "; ";
	outfile << missPrefetchSA(entries, 8) << "," << entries.size() << "; ";
	outfile << missPrefetchSA(entries, 16) << "," << entries.size() << "; " << endl;


	return 0;
}

