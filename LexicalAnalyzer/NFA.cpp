#include "Analyzer.h"
#include <sstream>
#include <regex>
#include <algorithm>
#include <utility>
#include <set>

NFA :: NFA(void)
{
    new_state_no = 0 ;
    buildDefintion("\\L=EPSILON");
}

void NFA::file_read(std::string file)
{
    std::ifstream lexicalRules(file);
    string str;
    while (std::getline(lexicalRules,str))
    {
        std::regex  keyWord ("\\{.+\\}");
        std::regex  regExp (".+\\s*:\\s*.+");
        std::regex  regDef(".+\\s*=\\s*.+");
        std::regex  punc ("\\[(.+)\\]");

        //Check if KeyWord between {}
        //{if}
        if(regex_match(str,keyWord))
        {
            buildKeyword(str);
        }

        //Check if Regualar Expression
        //id : letter(letter|digdit)*
        else if(regex_match(str,regExp))
        {
            buildExpression(str);
        }

        //Check if Regualar Defintiton
        //digit = 0-9
        else if(regex_match(str,regDef))
        {
            buildDefintion(str);
        }

        //Check if Regualar Punctuation between []
        //[; , \( \) { }]
        else if(regex_match(str,punc))
        {
            buildPunctuation(str);
        }
    }
}

void NFA::buildKeyword(std::string st1)
{
    st1 = st1.substr(1, st1.length()-2) ;
    std::string::iterator end_pos = std::remove(st1.begin(), st1.end(), '\\');
    st1.erase(end_pos, st1.end());
    vector <string> vector_of_keywords = countWords(st1, ' ');
    for(int i =0 ; i<vector_of_keywords.size() ; i++)
    {

        struct automata temp ;
        temp = concate_chars(vector_of_keywords[i]) ;
        temp.priority  = 1 ;
        temp.type = 'k' ;
        automata_table.insert(std::make_pair(vector_of_keywords[i], temp));
    }
}

void NFA::buildPunctuation(std::string st1)
{
    st1 = st1.substr(1, st1.length()-2) ;
    //cout<< st1 <<'\n';
    std::string::iterator end_pos = std::remove(st1.begin(), st1.end(), '\\');
    st1.erase(end_pos, st1.end());
    vector <string> vector_of_keywords = countWords(st1, ' ');

    for(int i =0 ; i<vector_of_keywords.size() ; i++)
    {
        struct automata temp ;
        temp = concate_chars(vector_of_keywords[i]) ;

        temp.priority  = 2 ;
        temp.type = 'p' ;
        automata_table.insert(std::make_pair(vector_of_keywords[i], temp));
    }
}

struct automata NFA::concate_chars(std::string st6)
{
    std::string::iterator end_pos = std::remove(st6.begin(), st6.end(), '\\');
    st6.erase(end_pos, st6.end());
    return  concate_chars_helper(st6)  ;
}

struct automata NFA::concate_chars_helper(std::string st6)
{
    if(st6.length()== 1)
    {
        inputs.insert(st6);
        struct automata newDef ;
        newDef.state = new_state_no++ ;
        std::vector<int> tempVect ;
        tempVect.push_back(new_state_no++);
        newDef.transition_table.insert(std::make_pair(std::make_pair(newDef.state, st6), tempVect));
        newDef.acceptanceStates.push_back(new_state_no-1);

        return newDef ;
    }
    else
    {
        return concatinate (concate_chars_helper(st6.substr(0, 1)), concate_chars_helper(st6.substr(1, st6.length()-1)));
    }
}
/**
 * dealing with : + *
 * 1-def = 0-9

 * def = ((0-9)+   >> not handled
 * s1(0-9) -> s2
 * s2(0-9) -> s2
 * s1(epsl)
 * 3-def = letter(letter|digit)
 * (escape char) \+
 */
void NFA::buildDefintion(std::string st3)
{
    struct automata newDef ;
    string def ;
    string rhs ;
    newDef.state = new_state_no++ ;
    newDef.type = 'd' ;
    std::string::iterator end_pos = std::remove(st3.begin(), st3.end(), ' ');
    st3.erase(end_pos, st3.end());

    std::istringstream str(st3);
    // first split gets def name
    std::getline(str, def, '=') ;
    // second
    std::getline(str, rhs, '=') ;
    //std::regex not_letter  (".*((\\*) | (\\|)| (\\s)+ | (\\() | (\\))).*");
    if(contains_def(rhs))
    {
        replace_(st3, "=", ":");
        struct automata temp ;
        temp =  buildExpression(st3);
        temp.type = 'd';
        std::map  <std::string, struct automata >::iterator automata_iter = automata_table.find(def);
        if(automata_iter != automata_table.end())
        {
            automata_iter->second = temp;
        }
        return ;
    }
    else
    {
        definitions.push_back(rhs);
        if(rhs.compare(EPSILON) != 0)inputs.insert(rhs);
        std::vector<int> tempVect ;
        tempVect.push_back(new_state_no++);
        newDef.transition_table.insert(std::make_pair(std::make_pair(newDef.state, rhs), tempVect));
        newDef.acceptanceStates.push_back(new_state_no-1);
        automata_table.insert(std::make_pair(def, newDef));
    }
}

// s.th. like 0-9 is not handled
struct automata NFA::buildExpression(std::string st4)
{
    struct automata newEx ;
    string exp ;
    string rhs ;

    // std::string::iterator end_pos = std::remove(st4.begin(), st4.end(), ' ');
    // st4.erase(end_pos, st4.end());
    std::istringstream str(st4);
    // first split gets def name
    std::getline(str, exp, ':') ;
    // second
    std::getline(str, rhs, ':') ;
    replace_(rhs, "(", " (");
    newEx = rec_manipulator(rhs);

    std::string::iterator end_pos = std::remove(exp.begin(), exp.end(), ' ');
    exp.erase(end_pos, exp.end());
    newEx.type = 'e' ;
    newEx.priority = 2 ;
    automata_table.insert(std::make_pair(exp, newEx));
    return newEx ;
}

struct automata NFA::rec_manipulator(std::string rhs)
{
    struct automata tempAutomata ;
    bool i_have_an_aut = false ;
    string temp ;

    if(!contains(rhs))
    {
        temp  = rhs ;
        std::map<string,struct automata>::iterator it;
        it = automata_table.find(temp);
        //base case
        if (it != automata_table.end())
        {
            struct automata temp1, base = it->second;
            map <int, int> new_states ;
            temp1.priority = base.priority ;
            temp1.type = base.type ;


            // fill a table with states and their corresponding new states
            map<std::pair <int, string>, vector <int>>::iterator it1;
            for (it1 = base.transition_table.begin(); it1 != base.transition_table.end(); it1++)
            {
                if(look_in_table(it1->first.first,new_states) == -1)
                {
                    new_states.insert(std::make_pair(it1->first.first, new_state_no++));
                }
                std::pair<int, string> p = std::make_pair(look_in_table(it1->first.first,new_states),it1->first.second );

                vector<int> v ;
                for(int i = 0 ; i < (it1->second).size() ; i++)
                {
                    if(look_in_table((it1->second)[i],new_states) == -1)
                    {
                        new_states.insert(std::make_pair((it1->second)[i], new_state_no++));
                    }
                    v.push_back(look_in_table((it1->second)[i],new_states));
                }

                temp1.transition_table.insert(std::make_pair(p,v));
            }

            // set start state
            temp1.state = look_in_table(base.state, new_states);

            // set acceptance states
            for(int i = 0 ; i < base.acceptanceStates.size() ; i++)
            {
                temp1.acceptanceStates.push_back(look_in_table(base.acceptanceStates[i], new_states));
            }

            return temp1 ;
        }
        /*one or more than one char*/
        else
        {
            if(rhs[0] == '\\' && rhs[1]=='L')
            {
                tempAutomata.state = new_state_no ++;
                tempAutomata.acceptanceStates.push_back(new_state_no++);
                tempAutomata.transition_table.insert(std::make_pair(std::make_pair(new_state_no-2, EPSILON),tempAutomata.acceptanceStates));
                tempAutomata.type = 'L';
                tempAutomata.priority = 2 ;
                return tempAutomata ;
            }
            return concate_chars(rhs) ;
        }
    }

    else        //not a base case
    {
        std::string str_concated = "";

        for(int i = 0; i<rhs.length() ; i++)
        {
            if (rhs[i] ==' ')
            {
                if(str_concated.length() == 0 )
                {
                    while(rhs[i] ==' ')
                        i++ ;
                }
                else
                {
                    while(rhs[i] ==' ')
                        i++ ;
                    if(!i_have_an_aut)
                    {
                        tempAutomata = rec_manipulator(str_concated);
                    }
                    else
                    {
                        //cout << str_concated << endl ;
                        tempAutomata = concatinate(tempAutomata, rec_manipulator(str_concated));
                    }
                    str_concated = "" ;
                    i_have_an_aut = true;
                }
            }
            if(i==rhs.length())break ;
            if (rhs[i] =='+')
            {

                if(!i_have_an_aut)
                {
                    if(rhs[i-1] != '\\')tempAutomata = positiveClosure(rec_manipulator(str_concated));
                    else tempAutomata = concate_chars("+");
                }
                else
                {
                    if(rhs[i-1] != '\\')tempAutomata = concatinate(tempAutomata, positiveClosure(rec_manipulator(str_concated)));
                    else tempAutomata = concatinate(tempAutomata, concate_chars("+"));
                }
                str_concated = "" ;
                i_have_an_aut = true;
            }
            else if(rhs[i] =='*')
            {
                //if(rhs[i-1]=='\\')
                if(!i_have_an_aut)
                {
                    if(rhs[i-1] != '\\')tempAutomata = kleeneClosure(rec_manipulator(str_concated));
                    else tempAutomata = concate_chars("*");
                }
                else
                {
                    if(rhs[i-1] != '\\')tempAutomata = concatinate(tempAutomata, kleeneClosure(rec_manipulator(str_concated)));
                    else tempAutomata = concatinate(tempAutomata, concate_chars("*"));
                }
                str_concated = "" ;
                i_have_an_aut = true;
            }
            else if (rhs[i] =='|')
            {
                if(str_concated.length() != 0)
                {
                    tempAutomata = rec_manipulator(str_concated);
                    str_concated = "";
                }
                list<automata > autos ;
                autos.push_back(tempAutomata);
                autos.push_back(rec_manipulator(rhs.substr(i+1, rhs.length()+1-i)));
                return oring(autos, false) ;
            }
            else if (rhs[i] =='(')
            {
                i++;
                while(rhs[i] != ')')
                {
                    str_concated += rhs[i++];
                    //cout << str_concated << endl ;
                }
                struct automata temp2 ;
                if (rhs[i+1]=='*')
                {
                    temp2 =  kleeneClosure(rec_manipulator(str_concated));
                    i++;
                }
                else if (rhs[i+1]=='+')
                {
                    temp2 = positiveClosure(rec_manipulator(str_concated));
                    i++;
                }
                else temp2 = rec_manipulator(str_concated);

                if(i_have_an_aut)
                {
                    tempAutomata = concatinate(tempAutomata, temp2);
                }
                else
                {
                    tempAutomata = temp2 ;
                }

                str_concated = "" ;
                i_have_an_aut = true ;
            }
            else
            {
                str_concated = str_concated+rhs[i] ;
                //   cout << "my concatenated string ===========" << str_concated << endl;
            }
        }
        if(str_concated.length() != 0)
        {
            if(i_have_an_aut)
            {
                string k = "k";
                if(str_concated[0]==' ')k = "h";
//                    cout << "if i have an aut -----------------\n" << str_concated.length() << endl;
                return concatinate(tempAutomata, rec_manipulator(str_concated));
            }
            else
            {
                tempAutomata = rec_manipulator(str_concated) ;
            }
        }
        return tempAutomata ;
        //}
    }
    // search in automata map if found get the automata
}

struct automata NFA::kleeneClosure(automata a1)
{
    int overall_start = new_state_no++;
    int overall_end = new_state_no++;
    struct automata kleene_automata;

    kleene_automata.state = overall_start;
    kleene_automata.acceptanceStates = a1.acceptanceStates;
    kleene_automata.transition_table.insert(a1.transition_table.begin(), a1.transition_table.end());

    vector<int> start_transition;
    start_transition.push_back(a1.state);
    start_transition.push_back(overall_end);
    kleene_automata.transition_table.insert(std::make_pair(std::make_pair(overall_start,EPSILON), start_transition));

    // add the EPSILON TRANSITION of the acceptance states of a1 to the new overall_end & the old start
    for(std::vector<int>::size_type i = 0; i != kleene_automata.acceptanceStates.size(); i++)
    {
        vector<int> next_states;
        next_states.push_back(overall_end);
        next_states.push_back(a1.state);

        map <pair<int, string>, vector <int>>::iterator it = kleene_automata.transition_table.find(std::make_pair(kleene_automata.acceptanceStates[i], EPSILON));
        if(it != kleene_automata.transition_table.end())
        {
            it->second.insert(it->second.end(), next_states.begin(), next_states.end());
        }
        else
        {
            kleene_automata.transition_table.insert(std::make_pair(std::make_pair(kleene_automata.acceptanceStates[i], EPSILON), next_states));
        }
    }

    vector<int> acceptance;
    acceptance.push_back(overall_end);
    kleene_automata.acceptanceStates = acceptance;

    return kleene_automata;
}

struct automata NFA::positiveClosure(automata a2)
{
    struct automata positive_automata;

    positive_automata.state = a2.state;
    positive_automata.acceptanceStates = a2.acceptanceStates;
    positive_automata.transition_table.insert(a2.transition_table.begin(), a2.transition_table.end());

    for(std::vector<int>::size_type i = 0; i != positive_automata.acceptanceStates.size(); i++)
    {
        vector<int> next_states;
        next_states.push_back(a2.state);

        map <pair<int, string>, vector <int>>::iterator it = positive_automata.transition_table.find(std::make_pair(positive_automata.acceptanceStates[i], EPSILON));
        if(it != positive_automata.transition_table.end())
        {
            it->second.insert(it->second.end(), next_states.begin(), next_states.end());
        }
        else
        {
            positive_automata.transition_table.insert(std::make_pair(std::make_pair(positive_automata.acceptanceStates[i], EPSILON), next_states));
        }
    }

    return positive_automata;
}

struct automata NFA::oring (list<automata> automatas, bool NFA)
{
    int mutual_start = new_state_no++;
    int mutual_end = new_state_no++;
    struct automata ored_automata;
    vector<int> mutual_start_trans;

    ored_automata.state = mutual_start;
    for (std::list<automata>::iterator it = automatas.begin(); it != automatas.end(); ++it)
    {
        struct automata sub_automata = *it;
        mutual_start_trans.push_back(sub_automata.state);
        ored_automata.transition_table.insert(sub_automata.transition_table.begin(), sub_automata.transition_table.end());
        if(!NFA)
        {
            vector<int> next_states;
            next_states.push_back(mutual_end);
            // loop over the acceptance states for the same automata
            for(std::vector<int>::size_type i = 0; i != sub_automata.acceptanceStates.size(); i++)
            {
                map <pair<int, string>, vector <int>>::iterator it = ored_automata.transition_table.find(std::make_pair(sub_automata.acceptanceStates[i], EPSILON));
                if(it != ored_automata.transition_table.end())
                {
                    it->second.insert(it->second.end(), next_states.begin(), next_states.end());
                }
                else
                {
                    ored_automata.transition_table.insert(std::make_pair(std::make_pair(sub_automata.acceptanceStates[i], EPSILON), next_states));
                }
            }
            ored_automata.acceptanceStates = next_states;
        }
    }

    ored_automata.transition_table.insert(std::make_pair(std::make_pair(mutual_start, EPSILON), mutual_start_trans));
    return ored_automata;
}

struct automata NFA::concatinate (automata a3, automata a4)
{
    struct automata concatinated_automata;
    concatinated_automata.state = a3.state;
    concatinated_automata.transition_table.insert(a3.transition_table.begin(), a3.transition_table.end());
    concatinated_automata.transition_table.insert(a4.transition_table.begin(), a4.transition_table.end());

    for(std::vector<int>::size_type i = 0; i != a3.acceptanceStates.size(); i++)
    {
        vector<int> next_states;
        next_states.push_back(a4.state);
        map <pair<int, string>, vector <int>>::iterator it = concatinated_automata.transition_table.find(std::make_pair(a3.acceptanceStates[i], EPSILON));
        if(it != concatinated_automata.transition_table.end())
        {
            it->second.insert(it->second.end(), next_states.begin(), next_states.end());
        }
        else
        {
            concatinated_automata.transition_table.insert(std::make_pair(std::make_pair(a3.acceptanceStates[i], EPSILON), next_states));
        }
    }
    concatinated_automata.acceptanceStates = a4.acceptanceStates;
    return concatinated_automata;
}

int NFA:: look_in_table(int x, map<int, int>new_values)
{
    map<int, int>::iterator it1;
    for (it1 = new_values.begin(); it1 != new_values.end(); it1 ++)
    {
        if(it1 -> first == x)return it1 -> second ;
    }
    return -1 ;
}

bool NFA::replace_(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

bool NFA::contains(string container)
{
    for(int  i = 0 ; i < container.length() ; i++)
    {
        if(container[i] == '+' |container[i] == '*' |container[i] == '(' |container[i] == ')' |container[i] == ' ' | container[i] == '|' )
        {
            return true ;
        }
    }
    return false ;
}

bool NFA::contains_def(string str)
{
    string temp = "";
    for(int i = 0 ; i<str.length() ; i++)
    {
        if(!contains(str.substr(i, 1)))
            temp = temp + str[i];
        else
        {
            std::map<string,struct automata>::iterator it;
            it = automata_table.find(temp);
            //base case
            if (it != automata_table.end())
                return true ;
            temp = "" ;
        }
    }
    return false ;
}

vector<string> NFA::countWords(string myString, char x)
{
    string str_new ="";
    vector<string> temp_vect ;
    for (int i=0; i<myString.length(); i++)
    {
        if (myString[i] == x)
        {
            if(str_new.length()!=0)
            {
                temp_vect.push_back(str_new);
            }
            str_new = "";
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

void NFA::oring_for_NFA_automata()
{
    list<automata> automatas ;
    for (std::map<string,automata>::iterator it=automata_table.begin(); it!=automata_table.end(); ++it)
    {
        if(it->second.type != 'd')
        {
            automatas.push_back(it->second) ;
            for(int i :it->second.acceptanceStates)
            {
                nfa_acceptance_states[i] = std::make_pair(it->second.priority,it->first ) ;
                // nfa_acceptance_states.insert(make_pair(it->second.acceptanceStates[i],
                // std::make_pair(it->second.priority,it->first )));
            }
        }
    }

    //cout<<"safaaaaaaaaaaaaaa" <<nfa_acceptance_states.size()<<'\n' ;
    std::map<int,std::pair<int, string> > ::iterator it;

    //cout << "Size of NFA   "<<nfa_acceptance_states.size() << endl ;
    for(it=nfa_acceptance_states.begin(); it!=nfa_acceptance_states.end(); ++it)
    {
        //cout << it ->first << " acceptance_point \n" ;
        //cout << it ->second.first << " priority \n" ;
        //cout << it ->second.second << " is the name of the automata \n";
    }
    NFA_automata = oring(automatas, true) ;
}
