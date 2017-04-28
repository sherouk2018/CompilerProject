#include "Parser.h"
#include <string>
#include <map>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <iterator>
#include<vector>
#include "Analyzer.h"
using namespace std;
stack <string>production_stack ;
vector<string>output;
bool error;

ParserMatcher::ParserMatcher()
{
    error = 0;
    production_stack.push("$");
    production_stack.push(start_symbol);
}

bool ParserMatcher::get_productions(string token)
{

    string str;
    string stack_top;
    string producticon ;
    pair<string,string >entry;
    while(!production_stack.empty() )
    {
        vector <string> temp ;
        stack_top = production_stack.top();
        production_stack.pop();
        //Non Terminal
        if (!all_strings[stack_top]  &&  stack_top.compare("$")!=0  )
        {

            if(all_strings[token] || token.compare("$")==0)
            {

                entry.first =stack_top;
                entry.second =token;
                producticon = production_table[entry];

                /*     cout << stack_top << " satckTop " << endl;
                     cout << token << " input" << endl;
                     if(!producticon.empty() )
                         cout << producticon << "--->> production" << endl<<endl;*/

                if(!producticon.empty() )
                {



                    str = producticon;
                    std::istringstream iss(str);
                    for(std::string str; iss >> str; )
                        temp.push_back(str);


                    if(temp.at(0).compare(EPSILON)!=0 &&temp.at(0).compare("sync")!=0 )
                    {
                        output.push_back(stack_top + "  ->>  " +producticon);
                        str = temp.at(0);
                        for(int i = temp.size() -1 ; i >= 0 ; i -- )
                        {
                            str = temp.at(i);
                            cout <<temp.at(i)<< "pushtostack"<<endl;
                            production_stack.push(temp.at(i));
                        }
                    }

                    else if(temp.at(0).compare("sync")==0)
                    {
                        output.push_back("ERROR SYNCH"+stack_top + "  ->>   SYNC");
                        error = 1 ;
                    }
                    else if(temp.at(0).compare(EPSILON)==0)
                    {
                        output.push_back(stack_top + "  ->>  " + temp.at(0));
                    }

                }
                ////////////////////////////////////////////////////////////////Error should be hanled
                else
                {
                    output.push_back("ERROR NO PRODUCTION");
                    production_stack.push(stack_top);
                    error = 1;
                    return error ;
                }

            }

        }
        //Terminal
        else  if(all_strings[stack_top] ||  stack_top.compare("$")==0)

        {
            /*cout << stack_top << "satckTop " << endl;
            cout << input.at(index).first << "input" << endl;*/
            if(stack_top.compare(token)==0 )
            {
                output.push_back("Match : "+ stack_top);
                cout << "Match : "+ stack_top + ", " <<token<< endl;
                return error ;
            }

            else
            {
                output.push_back("ERROR MISSINNG IN STACK : "+ stack_top+"&Input   "+ token);
                error = 1;
                cout << "ERROR MISSINNG IN STACK : "+ stack_top +" &Input  "+ token<< endl;
            }
        }

        //    }
    }

    return error ;
}


void ParserMatcher::print_productions()
{

    cout <<"\\----------------------------- PRODUCTIONS-------------------------------\\" << endl;
    for(int i = 0; i < output.size(); i++)
    {
        cout << "=>> " << output[i] << endl;
    }
}
