/*Pseudo code

1 Read file line by line //done
2 Every line that is read, split the line by delimiter(".") and put the result in a std::string array. //done
3 Match every std::string in array with one of the rules. If a particular rule matches, change output accordingly.
4 While matching the rules if the rule matched is the domain one, push the variable in domain in a struct which has 2 values(func, variable) and comment out that line.
5 While matching the rule, everywhere the domain variable appears, replace the domain variable with a local variable.
6 repeat till all lines are taken care of.
7 at max should require one parse throught he file. 


compiling command : g++ -I /usr/include/boost f2lp_wrapper.cpp -o regex_test -L /usr/lib/x86_64-linux-gnu/ -lboost_regex
*/

#include <fstream>
#include <streambuf>
#include <string>
#include <cerrno>
#include <iostream>
#include <vector>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/iostreams/filtering_stream.hpp>


#define DEBUG 0
#define COMMENT "%"


namespace io = boost::iostreams; 

std::string match_ground_term_rule(const std::string input);

int match_normal_rule(std::string& output, const std::string& input)
{
	boost::regex expr("([ a-zA-Z0-9\\(\\)]+)(:-){1}([ ,.a-zA-Z0-9\\(\\)]+)"); 
	if(boost::regex_match(input,expr))
	{
		boost::regex comma_expr("(,)");
		std::string semi_colon_expr(";");
		std::cout<<boost::regex_replace(output,comma_expr,semi_colon_expr);
		return 1;
	}
	else
		return 0;
}

//need to optimize this
int match_counting_literal_rule(std::string& output, const std::string& input)
{
	boost::regex expr("([0-9 ]*) ([{]){1} ([A-Za-z0-9\\(\\),:<>= ]+) ([}]){1} ([0-9 ]*)"); 
	std::cout<<boost::regex_match(input,expr)<<std::endl;

	output = input;

	if(boost::regex_match(output,expr))
	{
		
		//std::string new_string(s);
		int count = 0;
		int inside_paren = 0;//1 is true in c++, 0 is false
		
		//2 loops
		// first loop changes all the commas separating two choice rules to semi colons
		
		//first loop
		for (size_t i = 0; i < output.length() ; ++i)
		{
			if(output[i] == '(')
			{
				inside_paren = 1;
				continue;
			}

			if(output[i] == ')')
			{
				inside_paren = 0;
				continue;
			}

			if (output[i] == ',' && inside_paren == 0)
			{
				output[i] = ';';
				continue;
			}
		}

		#ifdef DEBUG
		std::cout<<"output after first rule\n";
		std::cout<<output<<std::endl;
		#endif

		//second loop changes all colons except the first colon to commas
		// 1 { q(X,Y): p(X): p(Y): X < Y, q(X,X): p(X)   } 1 
		for (size_t i = 0; i < output.length(); ++i)
		{
			if (output[i] == ':')
			{
			
				if(count != 0)
				{
					output[i] = ','	;
				}

				count++ ;
				continue;
			}

			if(output[i] == ';')
			{
				count = 0;
				continue;
			}
		}

		#ifdef DEBUG
		std::cout<<"output after second rule\n";
		std::cout<<output<<std::endl;
		#endif
		
		return 1;
	}
	else
		return 0;
}

std::string match_conditional_literal_rule(const std::string input);
std::string match_domain_rule(const std::string input);
int match_hide_rule(std::string& output, const std::string& input)
{
	boost::regex expr("(#hide)"); 
	// std::cout<<boost::regex_match(s,expr)<<std::endl;
	
	#ifdef DEBUG
		//c++ true = 1 false = 0
		std::cout<<boost::regex_match(output,expr)<<std::endl;
	#endif


	if(boost::regex_match(output,expr))
	{
		output.insert(0,COMMENT);
		
		#ifdef DEBUG
			std::cout<<output<<std::endl;
		#endif
		
		return 1;
	}
	else
		return 0;
}

int match_rule(std::string& output, const std::string& input)
{

	return 1;
}



struct domain_line
{
	std::string function_name;
	std::string function_variable;
};

std::string get_file_contents(const char *filename)
{
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in)
  {
    return(std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));
  }
  throw(errno);
}



int main(int argc, char const *argv[])
{

	const char *input_file_name = "input.txt";
	const char *output_file_name = "output.txt";
	
	std::ifstream file(input_file_name, std::ios_base::in | std::ios_base::binary);
	std::ofstream outfile(output_file_name);

	if(!outfile.is_open())
	{
		std::cerr << "Couldn't open output.txt \n";
		return -1;
	}

    if(file)
    {
	    try {

	        io::filtering_istream in;
	        
	        in.push(file);
	        for(std::string str; std::getline(in, str); )
	        {
	            //str has all the lines
	            if(!str.empty())
	            {
	            	//process str. Str can be a single line ex q(1,2).q(1,3). need to separate these commands process them individually
	            	std::vector<std::string> ind_commands;
	            	
            		boost::split(ind_commands,str,boost::is_any_of("."));

            		for (int i = 0; i < ind_commands.size(); ++i)
            		{
            			/* code */
            			boost::trim_if(ind_commands[i], boost::is_any_of("\t \n")); 
            			std::string output;
            			std::string input_temp(ind_commands[i]); 
            			if(match_rule(output,input_temp))
            			{
            				continue;
            			}
            		}

	            	//output str to file
	            	// outfile << str << std::endl;
	            }

	        }
	    }
	    catch(...) {
	         std::cout << "Bad File" << '\n';
	    }
	}
	else
	{
		std::cerr << "File could not be opened!\n";
		std::cerr << "Error Code: "<< strerror(errno)<<"\n";	
	}
   
	return 0;
}

