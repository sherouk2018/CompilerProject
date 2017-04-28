#include "Analyzer.h"
#include <sstream>
#include <regex>
#include <algorithm>
#include <utility>
#include <set>

const string GENERAL = "general";

struct move_step_dfa DFA::get_next_states(int state,string input)
{
    struct move_step_dfa next;
    next.is_acceptance =0;
    next.priority =  3;
    string one_input = input ;
    std::pair<int,std::string> entry;
    pair <int, string >end_type;
    entry.first = state;
    vector<int>::iterator myIntVectorIterator;
    /*for(int index = 0 ; index < definitions.size() +1; index++){
      int  found = 0 ;
      if(index== 0 ){
      one_input = input ;
      found = 1 ;
      }
      else{
       string one_input=  definitions[index-1];
       regex pattern ("["+ one_input +"]");
       if(regex_match(input , pattern))
       {
          found = 1;
       }
       }
        if(found==1){
         entry.second =one_input ;
           vector <int> closure_v = NFA_automata.transition_table[entry];

           for(int index =0 ; index<closure_v.size() ; index++)
           {
               int state = closure_v.at(index) ;
               if(nfa_acceptance_states.find(state) != nfa_acceptance_states.end())
               {
                   end_type = nfa_acceptance_states.find(state)->second;
                   if (end_type.first < next.priority)
                   {
                       next.priority = end_type.first ;
                       next.is_acceptance = 1;
                       next.acceptance_type = end_type.second;
                   }
               }
           }
           copy(closure_v.begin(),closure_v.end(),std::inserter(next.equivalent_states,next.equivalent_states.end()));
       }
       }
    */

    for(auto one_input : inputs)
    {
        if(input.compare(EPSILON) == 0 )
        {
            one_input = EPSILON;
        }
        if((input.compare(one_input) == 0  ||input.compare(EPSILON) == 0  )
                ||(input.size() == 1 && (( (int)input[0] >= 97 && (int)input[0] <=122) || ((int)input[0] >= 65 && (int)input[0] <= 90) )&& regex_match(one_input,regex("(a-z\s*\\|\s*A-Z)|(A_Z*\\|\s*a-z)")  ))
                || ((input.size() == 1 &&  (int)input[0] >= 48 && (int)input[0] <= 57 ) && regex_match(one_input,regex("0-9"))))
        {

            entry.second =one_input ;
            vector <int> closure_v = NFA_automata.transition_table[entry];

            for(int index =0 ; index<closure_v.size() ; index++)
            {
                int state = closure_v.at(index) ;
                if(nfa_acceptance_states.find(state) != nfa_acceptance_states.end())
                {
                    end_type = nfa_acceptance_states.find(state)->second;
                    if (end_type.first < next.priority)
                    {
                        next.priority = end_type.first ;
                        next.is_acceptance = 1;
                        next.acceptance_type = end_type.second;
                    }
                }
            }
            copy(closure_v.begin(),closure_v.end(),std::inserter(next.equivalent_states,next.equivalent_states.end()));

        }
    }
    return next;
}

struct move_step_dfa  DFA::get_epsolin_closure(set <int>states)
{
    struct move_step_dfa next;
    next.is_acceptance =0;
    next.priority = 3 ;
    struct move_step_dfa temp;
    std::stack<int> stack_closure;
    int state_temp ;
    set <int> epsolin_closure ;

    for (auto state:  states)
    {
        temp = get_next_states(state,EPSILON);
        if(temp.equivalent_states.size() != 0)
        {
            set<int> temp_next_state = temp.equivalent_states ;
            epsolin_closure.insert(temp_next_state.begin(), temp_next_state.end());
        }

        if(temp.priority < next.priority)
        {
            next.priority = temp.priority;
            next.acceptance_type = temp.acceptance_type;
            next.is_acceptance =1;
        }
    }

    set<int>::const_iterator pos;
    if(epsolin_closure.size() != 0)
    {
        for (auto state: epsolin_closure)
        {
            stack_closure.push(state);
        }
    }

    while(!stack_closure.empty())
    {
        int state_temp;
        state_temp = stack_closure.top();
        stack_closure.pop();
        temp = get_next_states(state_temp,EPSILON);

        if(temp.priority < next.priority)
        {
            next.priority = temp.priority;
            next.acceptance_type = temp.acceptance_type;
            next.is_acceptance =1;
        }
        if(temp.equivalent_states.size() != 0)
        {
            for (auto state:  temp.equivalent_states)
            {
                if( epsolin_closure.find(state) == epsolin_closure.end())
                {
                    stack_closure.push(state);
                }
            }
        }

        if(temp.equivalent_states.size()!=0)
        {
            set<int> temp_next_state = temp.equivalent_states ;
            epsolin_closure.insert(temp_next_state.begin(), temp_next_state.end());
        }
    }
    if(epsolin_closure.size() != 0)
        next.equivalent_states = epsolin_closure ;
    return next;
}

struct move_step_dfa DFA::move_step(set <int> initial_states, string input)
{
    struct move_step_dfa next;
    next.is_acceptance =0;
    next.priority = 3;
    struct move_step_dfa temp;
    std::set <int> next_states;
    if(!initial_states.empty())
    {
        set <int> epsolin_closure;

        for (auto state : initial_states)
        {
            temp = get_next_states(state,input);
            if(temp.equivalent_states.size() !=0)
            {
                set<int> temp_next_state = temp.equivalent_states ;
                next_states.insert(temp_next_state.begin(),temp_next_state.end());
            }

            if(temp.priority < next.priority)
            {
                next.priority = temp.priority;
                next.acceptance_type = temp.acceptance_type;
                next.is_acceptance =1;
            }

        }
        temp= get_epsolin_closure(next_states);
        if(temp.equivalent_states.size() !=0)
        {
            set<int> temp_next_state = temp.equivalent_states;
            next_states.insert(temp_next_state.begin(),temp_next_state.end());
        }
        if(temp.priority < next.priority)
        {
            next.priority = temp.priority;
            next.acceptance_type = temp.acceptance_type;
            next.is_acceptance =1;
        }
        next.equivalent_states = next_states ;
    }
    return next;
}

std::map<std::pair <int, string>, struct dfa_states > DFA::dfa(int s1)
{
    std::copy(inputs.begin(), inputs.end(), std::back_inserter(possible_inputs));
    std::map  <set<int>,int > look_up_table_by_set ;
    std::map <int, set<int>> look_up_table_by_int ;
    vector<int> states ;
    int new_states_number = 0 ;
    set <int> temp ;
    temp.insert(s1);
    struct move_step_dfa temp_state = move_step(temp, EPSILON) ;
    temp_state.equivalent_states.insert(s1) ;
    look_up_table_by_int.insert(std::make_pair(new_states_number, temp_state.equivalent_states)) ;
    look_up_table_by_set.insert(std::make_pair(temp_state.equivalent_states, new_states_number)) ;
    states.push_back(new_states_number);

    ///Mayar
    vector <int> temp_vect ;
    temp_vect.push_back(new_states_number) ;
    groups_mapping.insert(std::make_pair(GENERAL, temp_vect));
    ///Mayar
    new_states_number++;

    int set_number = 0 ;
    for(set_number = 0 ; set_number < states.size() ; set_number++)
    {
        std::set<int> current_set = look_up_table_by_int.at(set_number) ;
        for(string input : possible_inputs)
        {
            struct move_step_dfa my_struct = move_step(current_set, input) ;
            /// state was not previously used
            if(look_up_table_by_set.find(my_struct.equivalent_states) == look_up_table_by_set.end())
            {
                look_up_table_by_int.insert(std::make_pair(new_states_number, my_struct.equivalent_states)) ;
                look_up_table_by_set.insert(std::make_pair(my_struct.equivalent_states, new_states_number)) ;
                states.push_back(new_states_number);
                struct dfa_states new_dfa_state ;
                new_dfa_state.acceptance_type = my_struct.acceptance_type ;
                new_dfa_state.is_acceptance = my_struct.is_acceptance ;
                new_dfa_state.priority = my_struct.priority ;
                new_dfa_state.state_num = new_states_number ;
                dfa_table.insert(std::make_pair(std::make_pair(set_number, input), new_dfa_state));
                ///Mayar
                if(new_dfa_state.is_acceptance)
                {
                    if(groups_mapping.find(my_struct.acceptance_type) == groups_mapping.end())
                    {
                        vector <int> new_vect ;
                        new_vect.push_back(new_states_number) ;
                        groups_mapping.insert(std::make_pair(my_struct.acceptance_type, new_vect));
                    }
                    else
                    {
                        if (std::find(groups_mapping[new_dfa_state.acceptance_type].begin(), groups_mapping[new_dfa_state.acceptance_type].end(), new_states_number)
                                == groups_mapping[new_dfa_state.acceptance_type].end())
                            groups_mapping[new_dfa_state.acceptance_type].push_back(new_states_number);
                        // find_in_vector( groups_mapping[new_dfa_state.acceptance_type], new_states_number);
                        // groups_mapping[new_dfa_state.acceptance_type] .push_back(new_states_number);
                    }
                }
                else
                {
                    if (std::find(groups_mapping[GENERAL].begin(), groups_mapping[GENERAL].end(), new_states_number) == groups_mapping[GENERAL].end())
                        groups_mapping[GENERAL].push_back(new_states_number);
                    //find_in_vector( groups_mapping[GENERAL], new_states_number);
                }
                // groups_mapping[GENERAL] .push_back(new_states_number);
                ///Mayar
                new_states_number++;
            }
            /// state was previously used
            else
            {
                struct dfa_states new_dfa_state ;
                new_dfa_state.acceptance_type = my_struct.acceptance_type ;
                new_dfa_state.is_acceptance = my_struct.is_acceptance ;
                new_dfa_state.priority = my_struct.priority ;
                new_dfa_state.state_num = look_up_table_by_set.at(my_struct.equivalent_states) ;
                dfa_table.insert(std::make_pair(std::make_pair(set_number, input)
                                                , new_dfa_state));
            }
        }
    }
    return get_minimal_dfa(dfa_table);
}

/// Minimiztion

map<pair<int, string>, struct dfa_states> DFA::get_minimal_dfa(map<pair<int, string>, struct dfa_states> dfa)
{
    int id_count = 1;
    char group_id = 97;
    map<vector<int>, string> grouped_states;

    for(auto iter = groups_mapping.begin(); iter != groups_mapping.end(); iter++)
    {
        string str(id_count, group_id++);
        grouped_states.insert(std::make_pair(iter->second, str));
    }

    vector<vector<int>> ubdistributid_states;
    bool terminate = true;
    for(auto it = grouped_states.begin(); it != grouped_states.end(); )
    {
        if(it == grouped_states.begin())
        {
            terminate = false;
        }

        vector<int> current_tuple = it->first;
        if(current_tuple.size() > 1)
        {
            vector<vector<int>> distribution_result = get_grouped_states(grouped_states, current_tuple, dfa);
            if(distribution_result.size() == 1)
            {
                ++it;
                ubdistributid_states.push_back(current_tuple);
            }
            else
            {
                std::map<vector<int>, string>::iterator toErase = it;
                terminate = true;
                ++it;
                grouped_states.erase(toErase);
                for(std::vector<int>::size_type i = 0; i != distribution_result.size(); i++)
                {
                    string str(id_count, group_id++);
                    grouped_states.insert(std::make_pair(distribution_result[i], str));
                    if(group_id == 123)
                    {
                        group_id = 97;
                        id_count++;
                    }
                }
            }
        }
        else if (current_tuple.size() == 1)
        {
            ++it;
        }

        if(it == grouped_states.end() && terminate)
        {
            it = grouped_states.begin();
        }
    }
    //cout << "----" << grouped_states.size() <<  "----"  << endl;
    return form_minimized_dfa(dfa, grouped_states);
}

vector<vector<int>> DFA::get_grouped_states(map<vector<int>, string> grouped_states, vector<int> tuple_states,map<pair<int, string>, struct dfa_states> dfa)
{
    vector<vector<int>> groups;
    map<string, vector<int>> versa_map;
    //Loop over states that get fro grouped_states [one set]
    for(int index0 = 0; index0 < tuple_states.size(); index0++)
    {
        int state = tuple_states.at(index0);
        int next_state;
        string output ="";

        //Loop over all possible inputs to get next state
        for(int index1=0 ; index1 <possible_inputs.size() ; index1++)
        {
            string input = possible_inputs.at(index1);
            pair<int,string> entry;
            entry.first = state;
            entry.second = input;
            next_state = dfa[entry].state_num;
            //Looop over all keys to get the group of the next state
            for ( auto entry_map : grouped_states )
            {
                vector <int> key = entry_map.first;
                if(find(key.begin(),key.end(),next_state) != key.end())
                {
                    output += grouped_states[key];
                    break;
                }
            }
        }
        // Check if the conatinated groups was added to map por not
        //If exist update the list of it by adding the new state
        //Else create a new vector and add this state to it
        vector <int > temp ;
        if (versa_map.find(output) != versa_map.end())
        {
            temp = versa_map [output];
        }
        temp.push_back(state);
        versa_map[output]=temp;
    }
    //Iterate thoriugh the map to  get all the values(vector of int) to an vector
    for(auto entry : versa_map)
    {
        groups.push_back(entry.second);
    }
    return groups;
}

map<pair<int, string>, struct dfa_states> DFA::form_minimized_dfa(map<pair<int, string>, struct dfa_states> dfa, map<vector<int>, string> grouped_states)
{
    ofstream myfile;
    myfile.open ("groups.txt");
    for(auto it = grouped_states.begin(); it != grouped_states.end(); ++it)
    {
        auto vec_it = it->first.begin();
        vec_it++;
        myfile << "START " << endl;
        while(vec_it != it->first.end())
        {
            int duplicate_state = vec_it[0];
            myfile << "*****  "   << duplicate_state << endl;
            for(auto iter = dfa.begin(); iter != dfa.end(); iter++)
            {
                // remove the duplicate state itself from the key
                if(iter->first.first == duplicate_state)
                {
                    myfile << "state num " << duplicate_state << " is removed" << endl;
                    map<pair<int, string>, struct dfa_states>::iterator toErase = iter;
                    dfa.erase(toErase);
                }

                // remove the duplicate state
                if (iter->second.state_num == duplicate_state)
                {
                    iter->second.state_num = it->first.at(0);
                }
            }
            vec_it++;
        }
        myfile << "\n\n\n\n\n";
    }
    return dfa;
}

void DFA:: dfa_print()
{
    std::map <std::pair<int, string>, struct dfa_states> ::iterator it2 ;

    for(it2 =dfa_table.begin(); it2!=dfa_table.end(); ++it2)
    {
        //if(it2->second.is_acceptance)
        //   cout<<"from " << it2 ->first.first <<"  to  "<< it2->second.state_num<<" , type = " << it2->first.second  << " acc_type:  "<<it2->second.acceptance_type << "    \n" ;

    }
    cout<<"_____________________________________________________________________________________________after minm \n\n\n" ;
    set<int> temp_set ;
    std::map <std::pair<int, string>, struct dfa_states> ::iterator it ;
    std::map <std::pair<int, string>, struct dfa_states>  map_min =    get_minimal_dfa(dfa_table) ;
    ofstream myfile;
    myfile.open ("minimization.txt");
    for(it = map_min.begin();  it != map_min.end(); ++it)
    {
        temp_set.insert(it->first.first);
        myfile << "from " << it ->first.first <<"  to  "<< it->second.state_num<<" , type = " << it->first.second  << " acc_type:  "<<it->second.acceptance_type << " _____   \n" ;
    }
    myfile.close();
//   cout << temp_set.size()<<"________________safaaaa \n " ;
    // cout<<"_____________________________________________________________________________________________ groups_mapping \n\n\n" ;
    std::map <string, vector<int>> ::iterator it1 ;
    ofstream myfile1;
    myfile1.open ("dfa.txt");
    for(it1 =groups_mapping.begin(); it1!=groups_mapping.end(); ++it1)
    {
        myfile1 << it1->first << "-----------------> ";
        for(auto i : it1->second)
            myfile1 << i <<" ";
        myfile1 << "  \n";
    }
    myfile1.close();
}


