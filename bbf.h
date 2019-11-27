#include <vector>

class bbf {
public:
    bbf();
    void build(string, string, double, int);
    int query(string, string);
    int get_hash(string, int, int);
    bool check_query(vector<bool>*, string, int, int, int, int);

private:
    hash<string> hashfxn;
};
