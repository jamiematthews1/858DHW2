#include <vector>

class bf {
public:
    bf();
    void build(string, string, double, int);
    int query(string, string);
    int get_hash(string, int, int);
    bool check_query(vector<bool>*, string, int, int);

private:
    //int num_bits;
    //int num_hashes;
    //int num_keys;
    //double fpr;
    hash<string> hashfxn;
};
