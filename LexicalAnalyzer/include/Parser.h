#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED
#include "Analyzer.h"
#include "Matcher.h"
#include <iostream>
#include <bits/stdc++.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <regex>
#include <map>

using namespace std;

extern string start_symbol;
extern map<string,bool> all_strings;
extern map<string,vector<string>> productions; // Key will be terminal value will hold the productions of that terminal
extern set<string> non_terminals_with_epsilon;
extern map<string,set<string>> first;
extern map<pair<string,string>,string> production_table; ///Main table that matcher will use
extern map<string, vector<string>> new_productions ;
extern bool lr ;

class Parser
{
public:
    //constructor
    Parser();

    ///Sherouk
    map <string,set<string>> follows;
    void form_follow ();
    void rec_follow(std::string);
    vector<string>splitter (string);
    bool check_coming_nonterminal_has_epsilon_production(vector<string>,ptrdiff_t);
    void add_to_followers(string, set <string>);

    vector <pair <string, vector <string>>> productions_temp;
    vector<string> split_string(string stream_);
    int get_largest_sub(string, string);
    vector <string> free_of_spaces(vector<string> );
    void left_factoring();
    void print();

    ///Mayar
    bool is_terminal(string str);
    void formulate_first_for_all_non_terminals();
    bool formulate_first_for_current_non_terminal(string non_terminal);
    bool is_first_calculated(string non_terminal);
    set<string> get_first_of_current_production(string non_terminal, string production, bool first_of_follow);
    vector<string> get_splitted_production(string production);
    void update_production_table(string non_terminal, string terminal, string production);
    // NOT_USED
    bool contains(string non_terminal, string production);

    /// Asmaa
    void file_Reader(string file);
    //BONUS
    void eliminate_left_rec();
    void eliminate_dir_left_rec(pair<string, vector<string>>, pair<string, vector<string>>);
    pair<string , vector<string> > left_factoring(pair<string , vector<string> >);


    //Helpers
    string remove_char(string src, char x);
    string remove_spaces_from_terminal(string src);
    string remove_first(string to_split, string split_upon);
    vector<string> split(string to_split, string split_upon);
    vector<string> find_terminals(string lhs);
    string replace_all(string to_split, string to_replace, string replace_with);
};

class ParserMatcher
{
public:
    ParserMatcher();
   bool get_productions(string token);
   vector<string>output;
    void print_productions();
};

#endif // PASER_H_INCLUDED
