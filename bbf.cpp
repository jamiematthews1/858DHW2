#include <iostream>
using namespace std;
#include <vector>
#include <math.h>
#include <fstream>
#include <string>
#include "bbf.h"

bbf::bbf() { }

// Since c++ only has one string hash function, we prepend a variety of chars
// in order to create our k different hash functions
int bbf::get_hash(string to_hash, int hash_num, int mod_num) {
    char char1 = (char) hash_num%95 + 32;
    char char2 = (char) hash_num/95 + 32;
    char char3 = (char) hash_num/95/95 + 32;
    return hashfxn(string(1,char3) + string(1,char2) + string(1,char1) + to_hash) % mod_num;
}

void bbf::build(string input_file, string output_file, double fpr, int num_keys) {
    int num_bits = ceil(-num_keys * log(fpr) / pow(log(2),2));
    int num_hashes = ceil(num_bits/num_keys * log(2));

    // increase num_bits so that all blocks are full
    int size_block = 512;
    int num_blocks = ceil((double)num_bits/size_block);
    num_bits = num_blocks * size_block;

    vector<bool> filter(num_bits,0);
    ifstream input(input_file);
    string curr_key;

    //insert each key into the bloom filter
    int block_num;
    while (getline(input, curr_key)) {
        block_num = get_hash(curr_key, 0, num_blocks);
        for (int curr_hash=1; curr_hash < num_hashes; curr_hash++) {
            filter[block_num*size_block + get_hash(curr_key, curr_hash, size_block)] = true;
        }
    }

    //write bloom filter to file
    ofstream output(output_file);
    output << num_hashes << "\n";
    output << size_block << "\n";

    for (int i=0; i<num_bits; i++) {
        output << (int) filter[i];
    }
    output << "\n";
}

bool bbf::check_query(vector<bool> *filter, string key, int num_hashes, int num_bits, int size_block, int num_blocks) {
    bool found = true;
    int block_num = get_hash(key, 0, num_blocks);
    for (int curr_hash=1; curr_hash < num_hashes; curr_hash++) {
        if ((*filter)[block_num*size_block + get_hash(key, curr_hash, size_block)] == false) {
            found = false;
        }
    }
    return found;
}

int bbf::query(string input_file, string query_file) {
    //read how many hashes were used
    ifstream input(input_file);
    string line;
    getline(input, line);
    int num_hashes = stoi(line);
    int count_found = 0;

    getline(input, line);
    int size_block = stoi(line);

    //read bloom filter data from file
    getline(input, line);
    int num_bits = line.size();
    int num_blocks = num_bits/size_block;

    //write bloom filter into vector bool
    vector<bool> filter(num_bits,0) ;
    for (int i=0; i<num_bits; i++) {
        filter[i] = (line[i] == '1');
    }

    //clock_t exp1 = clock();
    ifstream queries(query_file);
    string curr_key;
    while(getline(queries, curr_key)) {
        cout << curr_key << ":";
        if (check_query(&filter, curr_key, num_hashes, num_bits, size_block, num_blocks)) {
            cout << "Y\n";
            count_found = count_found+1;
        } else {
            cout << "N\n";
        }
    }
    //exp1 = clock() - exp1;
    //cout << "time: " << ((float)exp1)/CLOCKS_PER_SEC/1000 << "\n";

    return count_found;
}

int main(int argc, char** argv)
{
    bbf bloomfilter;
    bloomfilter = bbf();
    bool build;
    if (argc >= 2) {
        if (((string)"build").compare(argv[1]) == 0) {
            build = true;
        } else if (((string) argv[1]).compare("query") == 0) {
            build = false;
        } else {
            cout << "Unknown command. Please use either build or query.";
        }
    }

    char flag;
    string input_file;
    double fpr;
    int num_keys;
    string output_file;
    string query_file;
    for (int i=1; i < argc/2; i++) {
        flag = argv[2*i][1];
        switch(flag) {
            case 'k':
                input_file = argv[2*i+1];
                break;
            case 'f':
                fpr = stod(argv[2*i+1]);
                break;
            case 'n':
                num_keys = stoi(argv[2*i+1]);
                break;
            case 'o':
                output_file = (argv[2*i+1]);
                break;
            case 'i':
                input_file = (argv[2*i+1]);
                break;
            case 'q':
                query_file = (argv[2*i+1]);
                break;
            default:
                cout << "Error: unknown flag. Please use one of kfnoiq.";
        }
    }

    if (build) {
        bloomfilter.build(input_file, output_file, fpr, num_keys);
    } else {
        bloomfilter.query(input_file, query_file);
    }

}
