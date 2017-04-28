#include "Parser.h"
#include <string>
#include <map>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <iterator>

using namespace std;

const string DOLLAR_SIGN = "$";

string start_symbol;
std::map<string, bool> all_strings;
map <string,vector<string> > productions;
set<string> non_terminals_with_epsilon; // changed
map<string,set<string>> first;          // changed
map <std::pair<std::string, string>, string> production_table;
map<string, vector<string>> new_productions ;
bool lr ;

Parser::Parser()
{
    //ctor
}

// --------------------------------------- ASMAA ------------------------------------- //


void Parser::eliminate_left_rec()
{
    map<string, vector<string>>::iterator it ;
    eliminate_dir_left_rec(*productions.begin(), *productions.begin());

    for(it = productions.begin() ; it != productions.end() ; it++)
    {

        map<string, vector<string>>::iterator it2 ;
        pair<string, vector<string>> tup ;
        //to accumulate substitution
        set<string> pros ;
        for(int i = 0 ; i < it->second.size() ; i++)
            pros.insert(it->second[i]);
        lr = false ;
        //cout << it-> first << " : " << new_productions.size() << endl;
        for(it2 = new_productions.begin() ; it2 != new_productions.end() ; it2++)
        {

            set<string> pros2  ;
            set<string>:: iterator it3 ;
            for(it3 = pros.begin() ; it3 != pros.end() ; it3++)
            {
                for(int i = 0 ; i < it2->second.size() ; i++)
                {
                    if(it2->first[it2->first.length()-1] != '_')
                    {
                        pros2.insert(replace_all(*it3, it2->first, it2->second[i])) ;
                    }
                }
            }
            if(pros2.size() != 0)pros = pros2 ;
            //cout << " \{" << pros.size() << "\}" << endl;

        }


        vector<string> temp2 ;
        set<string>::iterator it3 ;
        for(it3 = pros.begin() ; it3 != pros.end() ; it3++)
        {
            temp2.push_back(*it3);
            // cout <<it->first << " : " << *it3 << endl;
        }
        //cout << " \{" << temp2.size() << "\}" << endl;
        eliminate_dir_left_rec(std::make_pair(it->first, temp2),std::make_pair(it->first, it->second));
    }
    productions = new_productions ;
    map<string, vector<string> >:: iterator it5 ;
    for(it5 = productions.begin() ; it5 != productions.end() ; it5++)
    {
        for(int i = 0 ; i < it5->second.size() ; i++)
        {
            cout << it5->first << " : " << it5->second[i] << endl;
        }
    }
    cout << "-----------------------------------------------------\n";
}

void Parser::eliminate_dir_left_rec(pair<string, vector<string>> tup, pair<string, vector<string>> tup1)
{
    /* cout << "logged : " << tup.first ;
     for(int i = 0 ; i < tup.second.size() ; i++){
         cout << " " << tup.second[i] << endl ;
     }*/
    string nonterminal = tup.first ;
    vector<string> products = tup.second ;
    bool arr[products.size()];

    bool left_rec = false;
    for(int i = 0 ; i < products.size() ; i++)
    {
        vector<string> temp = split(products[i], " ");
        if(nonterminal.compare(temp[0]) == 0)
        {
            arr[i] = 1 ;
            left_rec = true ;
        }
        else
        {
            arr[i] = 0 ;
        }
    }

    if(!left_rec)
    {
        new_productions.insert(tup1);
    }
    else
    {
        lr = true ;
        string A_ = nonterminal + "_";
        vector<string> a_prod, aprod;
        for(int i = 0 ; i < products.size() ; i++)
        {
            if(arr[i] == 0)
            {
                aprod.push_back(products[i] + " " + A_) ;
            }
            else
            {
                string temp = products[i];
                temp = remove_first(temp, nonterminal);
                temp += " " + A_ ;
                a_prod.push_back(temp);
            }
        }
        a_prod.push_back(EPSILON) ;
        new_productions.insert(make_pair(A_, a_prod));
        new_productions.insert(make_pair(nonterminal, aprod));

        /*    for(int k = 0 ; k < a_prod.size() ; k++){
                cout << A_ << " : " << a_prod[k] << endl ;
            }
            for(int l = 0 ; l < aprod.size() ; l++){
                cout << nonterminal << " : " << aprod[l] << endl ;
            }*/

    }
}

vector<string> Parser::split_string(string stream_)
{
    /*
        std::istringstream buffer(stream_);
        std::vector<std::string> ret((std::istream_iterator<std::string>(buffer)),
                                     std::istream_iterator<std::string>());
        return ret;*/

    vector<string>strings;
    istringstream f(stream_);
    string s;
    while (getline(f, s, ' '))
    {

        strings.push_back(s);
    }

    return strings;
}

int Parser::get_largest_sub(string str1, string str2 )
{
/// str1 =she is nice , str2 = she is good
/// count should be 7

    int count_ = 0;
    vector<string> str_vector1, str_vector2 ;
    str_vector1 = split_string(str1);
    str_vector2 = split_string(str2);
    int limit ;
    if(str_vector1.size() < str_vector2.size())
        //limit is the small ine
        limit = str_vector1.size();
    else
        limit = str_vector2.size();
    for(int i=0 ; i< limit; i++)
    {
        if(str_vector1[i] == str_vector2[i])
        {
            count_+=str_vector1[i].size();
            count_++;
        }
        else
            break;

    }
    /* int limit = str1.size();
     if(str2.size() < str1.size())
         limit = str2.size();
     for(int i=0 ; i<limit ; i++)
     {
         if (str1.at(i) == str2.at(i))
             count_++;
         else
             break;
     }*/
    //cout << count_;
    return count_;
}
vector <string> Parser::free_of_spaces(vector<string> free)
{
    vector<string>  returned_vect;
    // add space to last string in the vector
    if(free.size() > 0)
    {
        free[free.size()-1] = free[free.size()-1]+ " ";
        for(auto f:free)
        {
            string final_="";
            std::istringstream buffer(f);
            std::vector<std::string> ret((std::istream_iterator<std::string>(buffer)),
                                         std::istream_iterator<std::string>());
            for(int i=0; i<ret.size() ; i++)
                final_+=ret[i]+" ";
            returned_vect.push_back(final_);
        }


    }
    return returned_vect;
}


void Parser::left_factoring()
{

//Let the map be filled in vector
    for(auto p : productions)
        productions_temp.push_back(std::make_pair(p.first,free_of_spaces(p.second)));

    for(auto p : productions_temp)
    {
        cout << p.first << " --->";
        for(auto p1: p.second )
            cout << p1 << " -- ";
        cout << endl ;
    }
    cout << endl<<endl<<endl;


    int main_counter = 0 ;
    for(int prod=0 ; prod < productions_temp.size() ; prod++)
    {
        string str = productions_temp[prod].first;
        vector<string> vector_ = productions_temp[prod].second;
        for(int i=0 ; i<productions_temp[prod].second.size() ; i++ )
        {
            int max_=  0;
            // cout << "-----------------" << productions_temp[prod].second[i] <<endl;
            vector<int> array_;
            bool found_sub = false;
            vector <string> new_productions;
            for(int j=i+1 ; j<productions_temp[prod].second.size() ; j++)
            {
                array_.push_back(get_largest_sub(productions_temp[prod].second[i], productions_temp[prod].second[j]));

                if (array_[j-i-1]> max_)
                {
                    // found at least one match
                    found_sub = true;
                    max_=array_[j-i-1];


                }
                //cout << endl << endl;
            }
            //---------------------------------------------------

            if(found_sub)
            {

                string temp_string = productions_temp[prod].second[i];
                temp_string= temp_string.substr(max_,temp_string.size()- max_ );
                if(temp_string == "")
                    temp_string=EPSILON;
                new_productions.push_back(temp_string);
                //cout << array_.size() <<endl;
                for(int k=0 ; k< array_.size() ; k++)
                {
                    //cout << max_ <<endl;
                    if(array_[k] == max_)
                    {

                        string str1 = productions_temp[prod].second[k+i+1];
                        //cout << str1 <<endl;
                        str1= str1.substr(max_,str1.size()- max_ );
                        if(str1 == "")
                            str1=EPSILON;
                        new_productions.push_back(str1);
                    }

                }
                // new_production was filled with at least one string

                string str2 = str;           // A* or A** ...
                string str3 = productions_temp[prod].second[i];   // to construct commmon substring
                str3 = str3.substr(0,max_);
                for(int u=0 ; u< main_counter+1 ; u++)
                    str2+="*";    // A* or A** ...
                main_counter++;
                str3+=str2;
                productions_temp.push_back(make_pair(str2, new_productions));
                int count_me= 0;
                for(int k=0 ; k< array_.size() ; k++)
                {

                    if(array_[k] == max_)
                    {
                        //cout << " earased "<<productions_temp[prod].second[k+i+1-count_me]<<endl;;
                        productions_temp[prod].second.erase(productions_temp[prod].second.begin()+k+i+1-count_me);
                        count_me++;
                    }
                }
                productions_temp[prod].second.erase(productions_temp[prod].second.begin()+i);
                productions_temp[prod].second.push_back(str3);
                i--;


            }

        }
    }

    productions.clear();
    for(auto p : productions_temp)
        productions.insert(std::make_pair(p.first,p.second));
    //cout << productions.size();
    vector< map<string, vector<string> >:: iterator > iterators ;
    map<string, vector<string> >:: iterator it5 ;
    for(it5 = productions.begin() ; it5 != productions.end() ; it5++)
    {
        bool found = false ;
        map<string, vector<string> >:: iterator it6 ;

        for(it6 = productions.begin() ; it6 != productions.end() ; it6++)
        {
            for(int i = 0 ; i < it6->second.size() ; i++)
            {
                vector<string> temp = splitter(it6->second[i]);

                if (find(temp.begin(), temp.end(), it5->first)  != temp.end())
                {
                    found = true ;
                }
            }
        }
        if(!found && (it5 -> first !=start_symbol))iterators.push_back(it5);
    }

    for(int i = 0 ; i < iterators.size() ; i++)
    {
        productions.erase(iterators[i]);
    }

    for(it5 = productions.begin() ; it5 != productions.end() ; it5++)
    {
        for(int i = 0 ; i < it5->second.size() ; i++)
        {
            cout << it5->first << " ::: " << it5->second[i] << endl;
        }
    }
}


void Parser::file_Reader(string file)
{
    std::ifstream lexicalRules(file);
    string str, rhs, lhs;
    std::map<string, bool>::iterator it1 ;
    bool first = true ;
    while (std::getline(lexicalRules,str))
    {
        vector<string> temp;
        //insert new production rule
        if(str[0] == '#')
        {
            str = str.substr(1,str.length()-1);
            temp = split(str, "::=");
            vector<string> t = split(temp[0], " ");
            rhs = t[0];
            lhs = temp[1];
            if(first)start_symbol = rhs ;
            first = false ;
            vector<string> lhss = split(temp[1], "|");
            productions.insert(std::make_pair(rhs, lhss));

            it1 = all_strings.find(rhs);
            if(it1 == all_strings.end())
            {
                all_strings.insert(std::make_pair(rhs, false));
            }
        }
        //append to the last production
        else
        {
            std::map<string, vector<string>>::iterator it = productions.find(rhs);
            vector<string> lhss = split(str, "|") ;
            lhs = str ;
            if (it != productions.end())
            {
                it->second.insert((it->second).end(), lhss.begin(), lhss.end());
            }
        }
        //find terminals
        vector<string> temp1 = find_terminals(lhs);
        for(int i = 0 ; i  < temp1.size() ; i++)
        {
            it1 = all_strings.find(temp1[i]);
            if(it1 == all_strings.end())
            {
                all_strings.insert(std::make_pair(temp1[i], true));
            }
        }
    }

    std::map<string, vector<string>>::iterator it ;
    for(it = productions.begin() ; it != productions.end() ; it++)
    {
        std::vector<string>::iterator it2 ;
        for(it2 = it->second.begin() ; it2 != it->second.end() ; it2++)
        {
            *it2 = remove_char(*it2, '\'');
        }
    }

}

//------------------------Helpers---------------------------//

string Parser::remove_first(string to_split, string split_upon)
{
    int i = 0 ;
    int last_match = 0, start_of_match = 0, match_indx = -1;
    for(int i = 0 ; i < to_split.length() ; i++)
    {
        if(to_split[i] == split_upon[match_indx+1])
        {
            match_indx++;
            if(match_indx == 0)start_of_match = i ;
            if(match_indx == split_upon.length()-1)
            {
                if(i == to_split.length()-1 || to_split[i+1] == ' ')
                {
                    to_split = to_split.substr(0, start_of_match) + to_split.substr(i+1, to_split.length()-i-1);
                    break ;
                }
            }
        }
        else
        {
            match_indx = -1 ;
        }
    }
    return to_split ;
}

string Parser::remove_char(string src, char x)
{
    for(int i = 0 ; i < src.length() ; i++)
    {
        if(src[i] == x)
        {
            string temp = src ;
            src = src.substr(0, i);
            src += temp.substr(i+1, temp.length()-i-1);
            i--;
        }
    }
    return src ;
}

vector<string> Parser::split(string to_split, string split_upon)
{
    int last_match = 0, start_of_match = 0, match_indx = -1;
    vector<string> output ;

    for(int i = 0 ; i < to_split.length() ; i++)
    {
        if(to_split[i] == split_upon[match_indx+1])
        {
            match_indx++;
            if(match_indx == 0)start_of_match = i ;
            if(match_indx == split_upon.length()-1)
            {
                string temp = to_split.substr(last_match, start_of_match-last_match) ;
                if(temp.length() != 0)output.push_back(temp);
                last_match = start_of_match+ split_upon.length();
                match_indx = -1;
            }
        }
        else
            match_indx = -1 ;
    }
    string temp = to_split.substr(last_match, to_split.length()-last_match) ;
    if(temp.length() != 0)output.push_back(temp);

    return output;
}

string Parser::replace_all(string to_split, string to_replace, string replace_with)
{
    int  start_of_match = 0, match_indx = -1;
    string temp = to_split;

    for(int i = 0 ; i < temp.length() ; i++)
    {
        if(temp[i] == to_replace[match_indx+1])
        {
            match_indx++;
            if(match_indx == 0)start_of_match = i ;
            if(match_indx == to_replace.length()-1)
            {
                if((i == temp.length()-1 || temp[i+1] == ' ') && (i == to_replace.length()-1 || temp[i-to_replace.length()]==' '))
                {
                    string x = temp.substr(0, start_of_match) ;
                    x = temp.substr(i+1, temp.length()-i-1); ;
                    temp = temp.substr(0, start_of_match) + replace_with +
                           temp.substr(i+1, temp.length()-i-1);
                    match_indx = -1;
                    i = start_of_match + replace_with.length()-1 ;
                }
            }
        }
        else
            match_indx = -1 ;
    }
    return temp;
}

vector<string> Parser::find_terminals(string lhs)
{
    vector<string> output ;
    int start = -1 ;
    for(int i = 0 ; i < lhs.length() ; i++)
    {
        if(lhs[i]=='\'')
        {
            if(start != -1)
            {
                output.push_back(lhs.substr(start+1, i-start-1));
                start = -1;
            }
            else
            {
                start = i ;
            }
        }
    }
    return output ;
}
// --------------------------------------- MAYAR ------------------------------------- //

bool Parser::is_terminal(string str)
{
    map<string,bool>::iterator it;
    it = all_strings.find(str);
    return (it != all_strings.end()) ? it->second : false;
}

bool Parser::is_first_calculated(string non_terminal)
{
    map<string,set<string>>::iterator it;
    it = first.find(non_terminal);
    return (it != first.end());
}

void Parser::formulate_first_for_all_non_terminals()
{
    for (std::map<string,vector<string> >::iterator it = productions.begin(); it != productions.end(); ++it)
    {
        // check if the first is calculated or not
        if(!is_first_calculated(it->first))
        {
            formulate_first_for_current_non_terminal(it->first);
        }
    }

    cout << "//------------------- FIRST ------------------//\n" << endl;
    for (std::map<string,set<string> >::iterator it = first.begin(); it != first.end(); ++it)
    {
        cout << "<< " << it->first << " >>" << endl;
        for(auto x : it->second)
        {
            cout << x << " & ";
        }
        cout << endl;
    }
    cout << "\n\n\n";
    cout << "//------------------- Non-Terminals with EPSILON ------------------//\n" << endl;
    for(auto x : non_terminals_with_epsilon)
    {
        cout << x << " & ";
    }
}

bool Parser::formulate_first_for_current_non_terminal(string non_terminal)
{
    std::map<string,vector<string> >::iterator it = productions.find(non_terminal);
    vector<string> value = it->second;
    set<string> overall_non_terminal_first;
    bool epsilon_production = false;

    for(int i = 0; i < value.size(); i++)
    {
        value[i] = remove_spaces_from_terminal(value[i]);

        // compare the value[i] with epsilon
        if(value[i].compare(EPSILON) == 0)
        {
            //cout << "Hey, I'm" << value[i] <<  "SIZE = " << value[i].size() << "But " << EPSILON << "SIZE = "  << EPSILON.size() << endl;
            epsilon_production = true;
            non_terminals_with_epsilon.insert(it->first);
            overall_non_terminal_first.insert(EPSILON);
        }
        else
        {
            set<string> cur_first = get_first_of_current_production(it->first, value[i], false);

            // add First to production table
            for(auto str : cur_first)
            {
                update_production_table(non_terminal, str, value[i]);
            }

            overall_non_terminal_first.insert(cur_first.begin(), cur_first.end());
        }
    }
    // fill the table && first
    first.insert(std::make_pair(non_terminal, overall_non_terminal_first));
    return epsilon_production;
}

set<string> Parser::get_first_of_current_production(string non_terminal, string production, bool first_of_follow)
{
    vector<string> splitted_production = get_splitted_production(production);
    // cout << non_terminal << " -- >> " << production << endl;
    set<string> production_first;

    if(is_terminal(splitted_production[0]))
    {
        production_first.insert(splitted_production[0]);
        return production_first;
    }

    bool terminate = true;
    for(int i = 0; i < splitted_production.size() && terminate; i++)
    {
        terminate = false;
        if(!is_first_calculated(splitted_production[i]))
        {
            terminate = formulate_first_for_current_non_terminal(splitted_production[i]);

        }
        else
        {
            set<string>::iterator iter = non_terminals_with_epsilon.find(splitted_production[i]);
            terminate = (iter == non_terminals_with_epsilon.end()) ? false: true;
        }
        map<string,set<string>>::iterator it;
        it = first.find(splitted_production[i]);
        production_first.insert(it->second.begin(), it->second.end());
    }

    if(terminate && !first_of_follow)
    {
        non_terminals_with_epsilon.insert(non_terminal);
    }
    return production_first;
}

vector<string> Parser::get_splitted_production(string production)
{
    string buf;
    stringstream ss(production);
    vector<string> tokens;
    while (ss >> buf)
        tokens.push_back(buf);
    return tokens;
}

string Parser::remove_spaces_from_terminal(string src)
{
    int i = 0 ;
    while(src[0]==' ')
    {
        src = src.substr(1, src.length()-1);
    }
    while(src[src.length() - 1]==' ')
    {
        src = src.substr(0, src.length()-1);
    }
    return src ;
}

/*
bool Parser::contains(string non_terminal, string production){
    string buf;
    stringstream ss(production);
    set<string> tokens;
    while (ss >> buf)
        tokens.insert(buf);
    set<string>::iterator it;
    it = tokens.find(non_terminal);
    return (it != tokens.end()) ? true : false;
}
*/




void Parser::form_follow ()
{
    for(auto non_terminal : non_terminals_with_epsilon)
    {
        std::map <string, set<string>> ::iterator it ;
        it = follows.find(non_terminal);
        if (it == follows.end())
            rec_follow(non_terminal);

        it = follows.find(non_terminal);
        //non_terminal contained in non_terminals_with_epsilon

        for(auto terminal: it->second)
        {
            update_production_table(non_terminal, terminal, EPSILON);
        }

    }


    for(auto non_terminal : productions)
    {
  //  cout << non_terminal.first << endl;
        std::map <string, set<string>> ::iterator it ;
        it = follows.find(non_terminal.first);
        if (it == follows.end())
            rec_follow(non_terminal.first);

    }

    print();

}

void Parser::rec_follow(std::string non_terminal)
{
//cout << "22222222222222222"<<endl;
    //1- add $ to start_symbol
    if(non_terminal.compare(start_symbol)==0)
    {
        //cout <<"non_terminal "<< non_terminal << "1" <<endl;


        set<std::string> temp ;
        temp.insert(DOLLAR_SIGN);
        add_to_followers(non_terminal, temp);
        //follows.insert(std::make_pair(non_terminal, temp)) ;
    }
    //2-pass on all productions to find
    for(auto vector_ : productions)
    {
        for(auto production : vector_.second)
        {
            vector<string> temp = splitter(production);

            if (find(temp.begin(), temp.end(), non_terminal)  != temp.end())
            {

                // cout << " non_terminal " <<non_terminal <<"production"<<production<<endl;
                // Your non_terminal is found in that production
                vector<string> ret = splitter(production);
                //find the index in which non_terminal was found in
                ptrdiff_t pos = find(ret.begin(), ret.end(), non_terminal) - ret.begin();
                bool all_next_nonterminal_contains_epsilon = check_coming_nonterminal_has_epsilon_production(ret,pos);
//cout << "2 \n";

                if(pos != (ret.size()-1))
                {

                    if(is_terminal(ret[pos+1]))
                    {
                        //add this terminal to your follower set
                        set<string> terminal ;
                        terminal.insert(ret[pos+1]);
                        add_to_followers(non_terminal, terminal);
                        continue;
                    }
                    else
                    {
                        // Add first of nonterminal in pos+1 to your follower except epsilon
                        string str;
                        str += ret[pos+1];
                        if(ret.size()-1 >= pos+2)
                        {
                            for(int i = pos+2 ; i<ret.size() ; i++)
                            {
                                str+=" ";
                                str+=ret[i];
                                //set <string> temp_set = get_first_of_current_production();
                                //add_to_followers(non_terminal, first_itr->second);
                            }
                        }
                        set <string> temp_set = get_first_of_current_production(non_terminal, str,true);
                        add_to_followers(non_terminal, temp_set);
                    }

                }
                if((pos == (ret.size()-1)) || all_next_nonterminal_contains_epsilon)
                {
  //              cout << "rec " << ret[pos] <<endl;
                    // Add all followers of head to followers of that nonterminal
                    std::map <string, set<string>> ::iterator itretator ;
                    itretator = follows.find(vector_.first);
                    if (itretator == follows.end())
                    {
      //                   cout << "stat " <<vector_.first <<endl;
                        set<string> temp_set ;
                        follows.insert(std::make_pair(vector_.first, temp_set));
                        if(vector_.first.compare(ret[pos]) !=  0){
                            cout << vector_.first << " : " << ret[pos] << endl ;
                            rec_follow(vector_.first);
                        }
    //                    cout << "returned"<< endl;
                    }
                    add_to_followers(non_terminal,follows[vector_.first]);
                }
            }

        }
    }

    ///Add SYNC to production table

    if (std::find(non_terminals_with_epsilon.begin(), non_terminals_with_epsilon.end(), non_terminal)
            == non_terminals_with_epsilon.end())
    {
        map<string, set<string>> ::iterator my_itr = follows.find(non_terminal) ;
        for(auto follow_ : my_itr->second)
            update_production_table(non_terminal, follow_, "sync");
    }
}
//Helping funcs
vector<string> Parser::splitter (string input)
{
    std::istringstream buffer(input);
    std::vector<std::string> ret;

    std::copy(std::istream_iterator<std::string>(buffer),
              std::istream_iterator<std::string>(),
              std::back_inserter(ret));
    return ret;
}
bool Parser::check_coming_nonterminal_has_epsilon_production(vector<string>ret,ptrdiff_t pos)
{
    for(int i=pos+1 ; i<ret.size(); i++)
    {
        /*if(find(terminals.begin(), terminals.end(), ret[i])!= terminals.end())*/
        if(is_terminal(ret[i]))
            // this string was found in terminals vector
            return false;


        set<string> temp = first[ret[i]];
        if(find(temp.begin(), temp.end(), EPSILON)== temp.end())
        {
            //EPSILON was found in one of the coming non terminal
            return false ;
        }
    }
    return true ;
}
void Parser::add_to_followers(string non_terminal, set <string> vector_)
{
    ///NOTE : if the set contains EPSILON then you shouldn't add it

    vector_.erase (EPSILON);
    //vector_.erase( std::remove(vector_.begin(), vector_.end(), std::string(EPSILON)), vector_.end() );
    std::map <string, set<string>> ::iterator itretator ;
    itretator = follows.find(non_terminal);
    if (itretator != follows.end())
    {
        for(auto follow_ : vector_)
            itretator->second.insert(follow_);
    }
    else
    {
        set<string> temp_set ;
        for(auto follow_ : vector_)
            temp_set.insert(follow_);
        follows.insert(std::make_pair(non_terminal, temp_set));
    }

}

void Parser::update_production_table(string non_terminal, string terminal, string production)
{
    std::map<pair<string,string>,string> ::iterator iter = production_table.find(std::make_pair(non_terminal,terminal));
    if(iter == production_table.end())
    {
        production_table.insert(std::make_pair(std::make_pair(non_terminal,terminal),production));
    }
    else if(production != "sync")
    {
        cout <<"Terminal " <<terminal << " Non_terminal " <<non_terminal
              << " production "<<production<< " there was  " << iter->second<<"  Ambiguous Grammer" << endl;
    }

}

void Parser::print()
{
    cout << "\n\n\n//----------------------------- FOLLOW ---------------------------- //" << endl;
    for(auto follow : follows)
    {
        cout<< follow.first<<"->" ;
        for(auto f:follow.second)
        {
            cout << " " << f ;
        }
        cout <<"\n";
    }

    cout << "\n\n\n//----------------------------- Production_table ---------------------------- //" << endl;

    for(auto entry : production_table)
    {
        cout<< entry.first.first << " -> " << entry.first.second << " -> " << entry.second<< endl;

    }
    cout <<endl<<endl;
    /* vector <string > result =  get_production();
     for(int i=0 ; i < result.size() ; i++)
     {
         cout << result.at(i) << endl;
     }*/
}
//    ___________________________________________________________________________________


