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
#define PAREN_OPEN "("
#define PAREN_CLOSE ")"
#define COMMA ","
#define COLON_DASH ":-" 


namespace io = boost::iostreams; 
namespace ba = boost::algorithm;

std::vector< std::pair<std::string,std::string> > domain_list;

std::set<std::string> search_domain_variables(const std::string& input)
{

	
	std::set<std::string> domains;

	
	// boost::regex expr("([A-Z]+)");
	boost::regex expr("([A-Z]+)([A-Za-z0-9]*)");

	boost::sregex_token_iterator iter(input.begin(), input.end(), expr, 0);
    boost::sregex_token_iterator end;

    for (; iter != end; ++iter)
    {
    	std::cout<<*iter<<std::endl;
		domains.insert(std::string(*iter));
   	}
	


	#ifdef DEBUG
	std::set<std::string>::iterator it;
	for (it = domains.begin(); it != domains.end() ; ++it)
	{
	 	/* code */
	 	std::cout<<*it<<std::endl;
	}
	
	#endif

	return domains;
}

int isDomain(const std::string& input)
{
	boost::regex expr("(#domain)([ ]+)([a-z]+)([(]){1}([A-Z]+)([A-Za-z0-9]*)([)]){1}");
	return regex_match(input,expr) ? 1 : 0 ;
}

std::pair<std::string,std::string> get_domain_variables(const std::string& input)
{
	std::string::const_iterator start, end; 
	start = input.begin(); 
	end = input.end();
	boost::match_results<std::string::const_iterator> what; 
	boost::regex expr("(#domain)([ ]+)([a-z]+)([(]){1}([A-Z]+[A-Za-z0-9]*)([)]){1}");
	boost::regex_match(start, end, what, expr);

	std::string variable(what[what.size()-2]);
	std::string identifier(what[what.size()-4]);
	std::pair <std::string,std::string> domain (variable,identifier);

	return domain;
}


/*
Matches all clauses that statrt with #domain. 
*/
int match_domain_rule(const std::string& input)
{
	//first check if the clause is a domain. If it is a domain clause, extract the identifier and variable from it and exit
	if(isDomain(input))
	{
		// The sentence is a domain. Extract Domin identfier and domain variable and put it in a list
		domain_list.push_back(get_domain_variables(input));
		return 1;
	}
	else
	{
		return 0;
	}
	//second check if the sentence has any variable in it that might be a domain variable. If it has then do appropriate operations to remove those.
}

/*
Method removes domain variables from clauses.
Call this method for every sentence.
All senstences need to be checked for domain variables
*/
void remove_domain_variables(std::string& output,const std::string& input)
{
	std::set<std::string> domains = search_domain_variables(input);
	output = input ;
	if(domains.size() > 0)
	{
		/* Found a domain in this input.

		strategy 1:
		input types:
		1-> {loc(X,Y)}. 

			Now although X and Y can both be a global 
			variables, there is a global variable associated 
			with X but not with Y. So we only process for X
			and assume that Y is a local variable.				

		*/

		/*For each global variable detected from a string and 
		append it to the end of the current clause. Match the 
		identifier of the global variable from the domain_list vector.*/

		/*This is the generated strnig that needs to be appended to 
		original string */
		std::string generated_string;
		std::set<std::string>::iterator i;
		for (i = domains.begin(); i != domains.end(); ++i)
		{
			
			for (int j = 0; j < domain_list.size(); ++j)
			{

				
				if ((*i).compare(domain_list.at(j).first) == 0)
				{
					
					generated_string.append(domain_list.at(j).second)
					.append(PAREN_OPEN)
					.append(domain_list.at(j).first)
					.append(PAREN_CLOSE)
					.append(COMMA);
					
				}
			}
		}

		/* The last clause will have a trailing comma, remove that*/
		ba::trim_right_if(generated_string,ba::is_any_of(COMMA));

		//detect if a string has ':-' in it
		//If it doesn't then append a ':-' to the clause and
		//then append the generated string
		if(input.find(":-") == std::string::npos)
		{
			output.append(COLON_DASH)
			.append(generated_string);
			#ifdef DEBUG
			std::cout<<"final string:"<<output<<std::endl;
			#endif
		}
		else
		{
			output.append(COMMA)
			.append(generated_string);
			#ifdef DEBUG
			std::cout<<"final string:"<<output<<std::endl;
			#endif
		}
	}
}

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


