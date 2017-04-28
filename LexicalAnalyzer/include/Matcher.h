#ifndef MATCHER_H
#define MATCHER_H
#include "Analyzer.h"
#include <string>
#include <map>
using namespace std;

class Matcher
{
    public:
        Matcher();
        virtual ~Matcher();
        vector<string> countWords(string myString, char x) ;
        vector<pair<string , string>> read_java_file(std::string file_name , map<pair<int , string> , struct dfa_states> dfa);
        vector<pair<string , string>> tokens;
    protected:

    private:
};

#endif // MATCHER_H
