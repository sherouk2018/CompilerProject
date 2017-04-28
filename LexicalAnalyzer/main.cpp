#include <iostream>
#include <regex>
#include "Analyzer.h"
#include "Matcher.h"
#include "Parser.h"


using namespace std;

std::map  <std::string, struct automata > automata_table;
std::map<int,std::pair<int, string>> nfa_acceptance_states;
struct automata NFA_automata;
set<string> inputs;
vector<string> possible_inputs;
vector<string> definitions;

int main()
{
    Parser p;
    p.file_Reader("cfg.txt");
    p.eliminate_left_rec();
    p.left_factoring();
    p.formulate_first_for_all_non_terminals();
    p.form_follow();
    NFA obj;
    DFA inst;
    Matcher match;
    obj.file_read("Lexical_Rules.txt");
    obj.oring_for_NFA_automata() ;
    match.read_java_file("test_code.txt",inst.dfa(NFA_automata.state));
    return 0;
}
