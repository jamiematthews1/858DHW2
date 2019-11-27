#include <iostream>
using namespace std;
#include <vector>
#include <math.h>
#include <fstream>
#include <string>
#include "bf.h"
#include <time.h>

bf::bf() { }

// Since c++ only has one string hash function, we prepend a variety of chars
// in order to create our k different hash functions
int bf::get_hash(string to_hash, int hash_num, int mod_num) {
    char char1 = (char) hash_num%95 + 32;
    char char2 = (char) hash_num/95 + 32;
    char char3 = (char) hash_num/95/95 + 32;
    return hashfxn(string(1,char3) + string(1,char2) + string(1,char1) + to_hash) % mod_num;
}

void bf::build(string input_file, string output_file, double fpr, int num_keys) {
    int num_bits = ceil(-num_keys * log(fpr) / pow(log(2),2));
    int num_hashes = ceil(num_bits/num_keys * log(2));

    vector<bool> filter(num_bits,0);
    ifstream input(input_file);
    string curr_key;
    //insert each key into the bloom filter
    int count = 0;
    while (getline(input, curr_key)) {
        count = count +1;
        for (int curr_hash=0; curr_hash < num_hashes; curr_hash++) {
            filter[get_hash(curr_key, curr_hash, num_bits)] = true;
        }
    }

    //write bloom filter to file
    ofstream output(output_file);
    output << num_hashes << "\n";
    for (int i=0; i<num_bits; i++) {
        output << (int) filter[i];
    }
    output << "\n";
}

bool bf::check_query(vector<bool> *filter, string key, int num_hashes, int num_bits) {
    bool found = true;
    for (int curr_hash=0; curr_hash < num_hashes; curr_hash++) {
        if ((*filter)[get_hash(key, curr_hash, num_bits)] == false) {
            found = false;
        }
    }
    return found;
}

int bf::query(string input_file, string query_file) {
    //read how many hashes were used
    ifstream input(input_file);
    string line;
    getline(input, line);
    int num_hashes = stoi(line);
    int count_found = 0;

    //read bloom filter data from file
    getline(input, line);
    int num_bits = line.size();

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
        if (check_query(&filter, curr_key, num_hashes, num_bits)) {
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
    bf bloomfilter;
    bloomfilter = bf();
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
