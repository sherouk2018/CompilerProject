#include "Analyzer.h"
#include "Matcher.h"
#include <iostream>
#include <fstream>
#include <map>
#include "Parser.h"

using namespace std;

Matcher::Matcher()
{
    //ctor
}

Matcher::~Matcher()
{
    //dtor
}

vector<string> Matcher::countWords(string myString, char x)
{

    string str_new ="";
    vector<string> temp_vect ;
    for (int i=0; i<myString.length(); i++)
    {
        if (myString[i] == x || myString[i] == '\t')
        {
            if(str_new.length()!=0)
            {
                temp_vect.push_back(str_new);
            }
            str_new = "";
            bool minuss = false ;
            while(myString[i] == x || myString[i] == '\t')
            {
                i++;
                minuss = true ;
            }
            if(minuss)i--;
        }
        else
        {
            str_new= str_new + myString[i] ;
        }
    }
    if(str_new.length()!=0)
    {
        temp_vect.push_back(str_new);
    }
    return temp_vect ;
}


vector<pair<string, string>> Matcher::read_java_file(string file_name, map<std::pair<int, string>, struct dfa_states> dfa)
{
    ParserMatcher parser_matcher ;
    ifstream fin(file_name);
    //vector<pair<string, string> > tokens ;

    //getting start state :
    map<pair<int, string>, struct dfa_states>::iterator it;
    int state, start_state =0 ;
    state = start_state ;
    int current_statr_pt  ;
    int last_acc = -1 ;
    string last_type = "trash";

    string line ;
    // int number_of_lines = 0;

    while (getline(fin, line))
    {
        vector<string> words = countWords(line, ' ');
        int j = 0 ;
        for(int i = 0 ; i<words.size() ; i++)
        {
            string concatinated = "" ;
            struct dfa_states next ;
            int l = 0 ;
            j = current_statr_pt = 0 ;
            //  cout << "word : " << words[i] << "\n" ;
            while(j < words[i].length())
            {
                //  cout << "1. j: " << j << "\n";
                //cout << "1.state:" << state<<" char : " << words[i].substr(j, 1)<< "\n";
                it = dfa.find(std::make_pair(state, words[i].substr(j, 1)));
                // there is a next state;
                if(it != dfa.end())
                {
                    next = it-> second ;
                    state = next.state_num ;
                    //    cout << "1. to state : " << state<<"\n" ;
                    if(next.is_acceptance)
                    {
                        // cout << "1. acceppted\n";
                        last_acc = j ;
                        last_type = next.acceptance_type ;
                    }
                    concatinated += words[i][j] ;
                    //cout << "concatinated : " << concatinated << endl ;
                    l++;
                    j++ ;
                }
                // no next state
                else
                {
                    bool found = false ;
                    for(int k = 0 ; k < definitions.size() ; k++)
                    {
                        string input =  definitions[k];
                        regex pattern ("["+ input +"]");
                        if(regex_match(words[i].substr(j, 1),pattern))
                        {
                            found = true ;
                            it = dfa.find(std::make_pair(state, definitions[k]));
                            next = it-> second ;
                            state = next.state_num ;
                            //  cout << "4. to state : " << state<<"\n" ;
                            if(next.is_acceptance)
                            {
                                //    cout << "4. acceppted\n";
                                last_acc = j ;
                                last_type = next.acceptance_type ;
                            }
                            concatinated += words[i][j] ;
                            //cout << "4. concatinated : " << concatinated << endl ;
                            l++;
                            j++ ;
                            break ;
                        }
                    }
                    if(!found)
                    {
                        //cout << "2. \n" ;
                        if(last_acc != -1)
                        {
                            //  cout << "2. splitting  last_acc:------------------------" << last_acc<<"\n";
                            tokens.push_back(std::make_pair(last_type, concatinated.substr(0, last_acc+1-current_statr_pt)));
                            parser_matcher.get_productions(last_type);
                            j = last_acc + 1 ;
                            current_statr_pt = j ;
                        }
                        else
                        {
                            //cout << "2. no acceptnce found;\n";
                            // cout << "ERROR" << endl ;
                            tokens.push_back(std::make_pair("ERROR", words[i].substr(current_statr_pt, j-current_statr_pt+1)));
                            parser_matcher.get_productions("ERROR");
                            j = current_statr_pt + 1;
                            current_statr_pt = j ;
                            if(j == words[i].length())break;
                        }
                        concatinated = "" ;
                        last_acc = -1 ;
                        l = 0 ;
                        state = 0 ;
                        //cout << "2. state restarted\n";
                    }
                }
                if(j == words[i].length())
                {
                    // this was a whole token
                    //cout << "3. end of word\n";
                    if(last_acc == j-1)
                    {
                        //  cout << "3. whole word accepted and put\n";
                        tokens.push_back(std::make_pair(last_type, concatinated));
                        parser_matcher.get_productions(last_type);
                    }
                    //token will be splitted
                    else
                    {
                        //if you met an acceptance state
                        if(last_acc != -1)
                        {
                            //                            cout << "55. splitting  last_acc:------------------------" << last_acc<<"\n";
                            tokens.push_back(std::make_pair(last_type, concatinated.substr(0, last_acc+1-current_statr_pt)));
                            parser_matcher.get_productions(last_type);
                            j = last_acc + 1 ;
                            current_statr_pt = j ;
                        }
                        else
                        {
                            // start a char advanced
                            // cout << "ERROR" << endl ;
                            tokens.push_back(std::make_pair("ERROR", words[i].substr(current_statr_pt, j-current_statr_pt+1)));
                            parser_matcher.get_productions(last_type);
                            j = current_statr_pt+ 1;
                            current_statr_pt = j ;
                        }
                    }
                    concatinated = "" ;
                    last_acc = -1 ;
                    l = 0 ;
                    state = start_state ;
                }
            }
        }
    }
    if(parser_matcher.get_productions("$"))
    {
        parser_matcher.output.push_back("ACCEPTED WITH ERROR");

    }
    else
    {
        parser_matcher.output.push_back("ACCEPTED");
    }

    parser_matcher.print_productions();
    ofstream sym, output_file;
    sym.open("Symbol Table.txt");
    output_file.open("test_code_output_file.txt");

    for(int i = 0 ; i < tokens.size(); i++)
    {
        if(tokens[i].first.compare("id") == 0)
        {
            sym << tokens[i].second << "\t\tid" << endl;
        }
        cout << tokens[i].first << " : " << tokens[i].second << endl ;
        output_file << tokens[i].first << " : " << tokens[i].second << endl ;
    }
    sym.close();
    output_file.close();
    return tokens;
}
