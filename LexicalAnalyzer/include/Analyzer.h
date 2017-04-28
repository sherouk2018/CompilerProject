#ifndef REGEX_H_INCLUDED
#define REGEX_H_INCLUDED
#include <iostream>
#include <bits/stdc++.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <regex>
#include <map>

using namespace std;

const string EPSILON = "\\L";

extern std::map  <std::string, struct automata > automata_table ;
extern std::map<int,std::pair<int, string>> nfa_acceptance_states;
extern struct automata NFA_automata ;
extern set<string> inputs ;
extern vector<string> possible_inputs ;
extern vector<string> definitions ;

struct automata
{
    int state ;
    map <std::pair<int, string>, vector <int>> transition_table ;
    vector <int> acceptanceStates ;
    int priority ;
    char type ;
};

struct dfa_states
{
    int state_num;
    int priority;
    std::string acceptance_type;
    bool is_acceptance;
};

struct move_step_dfa
{
    set<int> equivalent_states;
    std::string acceptance_type;
    bool is_acceptance;
    int priority;
};

class NFA
{
public :
    NFA();
    int new_state_no ;
    void file_read(std::string file);
    void  buildKeyword(std::string st1);
    void buildPunctuation(std::string st2);
    void buildDefintion(std::string st3);
    struct automata buildExpression(std::string st4);
    struct automata rec_manipulator(std::string st5);
    struct automata kleeneClosure(struct automata a1 );
    struct automata positiveClosure(struct automata a2);
    struct automata epslonAutomata();
    // it's taking string as some chars has to be sent with there escape chars
    struct automata charAutomata(std::string ch);
    struct automata oring (list<automata> automatas, bool NFA);
    struct automata concatinate (struct automata a3,struct automata a4);

    //Helpers
    bool replace_(std::string& str, const std::string& from, const std::string& to);
    bool contains(string container);
    int look_in_table(int x, map<int, int>new_values);
    struct automata concate_chars(std::string st6);
    struct automata concate_chars_helper(std::string st6);
    bool contains_def(string container);
    void print_trans_table(struct automata aut);
    vector<string> countWords(string myString, char x);
    void oring_for_NFA_automata () ;
};

class DFA
{
public:
    std::map<std::pair <int, string>, struct dfa_states > dfa_table;
    struct move_step_dfa get_next_states(int state,string input);
    struct move_step_dfa get_epsolin_closure(set <int>states);
    struct move_step_dfa move_step(set <int> initial_states, string input);
    std::map<std::pair <int, string>, struct dfa_states > dfa(int s1);
    void dfa_print();
    map <string, vector<int>> groups_mapping ;
    map<pair<int, string>, struct dfa_states> get_minimal_dfa(map<pair<int, string>, struct dfa_states> dfa);
    vector<vector<int>> get_grouped_states(map<vector<int>,string> grouped_states, vector<int> tuple_states, map<pair<int, string>, struct dfa_states> dfa);
    map<pair<int, string>, struct dfa_states> form_minimized_dfa(map<pair<int, string>, struct dfa_states> dfa, map<vector<int>, string> grouped_states);
};

#endif // REGEX_H_INCLUDED
