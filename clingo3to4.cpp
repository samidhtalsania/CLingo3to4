#include "clingo3to4.h"
#include "domain.h"


namespace io = boost::iostreams; 


int clingo3to4::convert(const char *argv[])
{
	const char *input_file_name = argv[1];
	const char *output_file_name = "output.l";
	
	std::ifstream file(input_file_name, std::ios_base::in | std::ios_base::binary);
	std::ofstream outfile(output_file_name);

	if(!outfile.is_open())
	{
		std::cerr << "Couldn't open output.l \n";
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

	            	if(str.compare(0,1,COMMENT) == 0)
					{
						outfile << str.append(NEWLINE);
						continue;
					}
	            	
	            	// First you need to check if it is a range statement.
	            	// Example. number(1..8) should not be split at the innner dots.
	            	// One way to do this is convert the first pair of dots to ",," do the split then reconvert it back to ".."
	            	std::size_t found = str.find("..");
	            	
	            	if(found != std::string::npos)
	            	{
	            		boost::replace_all(str, "..", ",,");
	            	}

            		boost::split(ind_commands,str,boost::is_any_of(DOT));

            		for (int i = 0; i < ind_commands.size(); ++i)
            		{
            			/* code */
            			boost::trim_if(ind_commands[i], boost::is_any_of(WHITESPACE)); 
            			
            			std::string input_temp(ind_commands[i]);
            			std::string output(input_temp); 
						
						match_rule(output,input_temp);
            			domain::remove_domain_variables(output,output);
						output.append(DOT).append(NEWLINE);
						
						//Reversing the above operation
            			if(found != std::string::npos)
            			{
            				boost::replace_all(output, ",,", "..");
            			}

            			// Dirty hack to remove all outputs with just a dot. need to figure out why

            			if (output.size() > 2)
        				{
        					outfile << output;
        					#ifdef DEBUG
        						std::cout<<output<<std::endl;
        					#endif
        				}

            			
        				
            		}
	            }

	        }
	    }
	    catch(...) {
	         std::cout << "Bad File" << '\n';
	         return -1;
	    }
	}
	else
	{
		std::cerr << "File could not be opened!\n";
		std::cerr << "Usage: clingo3to4 <input file>"<<"\n";	
	}
   return 0;

}

int clingo3to4::match_normal_rule(std::string& output, const std::string& input)
{
	boost::regex expr("([ a-zA-Z0-9\\(\\)]+)(:-){1}([ ,.a-zA-Z0-9\\(\\)]+)"); 
	if(boost::regex_match(input,expr))
	{
		boost::regex comma_expr("(,)");
		std::string semi_colon_expr(";");
		boost::regex_replace(output,comma_expr,semi_colon_expr);
		
		return 1;
	}
	else
		return 0;
}

//need to optimize this
int clingo3to4::match_counting_literal_rule(std::string& output, const std::string& input)
{
	boost::regex expr("([0-9 ]*) ([{]){1} ([A-Za-z0-9\\(\\),:<>= ]+) ([}]){1} ([0-9 ]*)"); 
	


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

		
		return 1;
	}
	else
		return 0;
}



/*
Clingo 4 does not support #hide and #base clauses
*/

int clingo3to4::match_hide_base_rule(std::string& output, const std::string& input)
{
	output = input;

	//c++ true = 1 false = 0
	std::string hide("#hide");
	std::string base("#base");
	std::vector<std::string> clauses;
	clauses.push_back(hide);
	clauses.push_back(base);

	for (int i = 0; i < clauses.size(); ++i)
	{
		if (output.find(clauses.at(i)) != std::string::npos)
		{
			output.insert(0, COMMENT);
			return 1;
		}
	}

	return 0;
}

//Every string will match one of these rules. If it wont match the string is written as it is.
//Every string after its transformation also needs to be checked for domain variables.
//If it contains domain variables then in that case those variables need to be removed.
int clingo3to4::match_rule(std::string& output, const std::string& input)
{
	if (match_hide_base_rule(output,input))
	{
		#ifdef DEBUG
			std::cout<<"hide rule matched"<<std::endl;
		#endif
		return 1;
	}
	else if (match_counting_literal_rule(output,input))
	{
		#ifdef DEBUG
			std::cout<<"counting rule matched"<<std::endl;
		#endif
		return 1;
	}
	else if (match_normal_rule(output,input))
	{
		#ifdef DEBUG
			std::cout<<"normal rule matched"<<std::endl;
		#endif
		return 1;
	}
	else if (domain::match_domain_rule(output,input))
	{
		#ifdef DEBUG
			std::cout<<"domain rule matched"<<std::endl;
		#endif
		return 1;
	}
	else
	{
		#ifdef DEBUG
			std::cout<<"No rule matched"<<std::endl;
		#endif
		return 0;
	}
}


std::string clingo3to4::get_file_contents(const char *filename)
{
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in)
  {
    return(std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));
  }
  throw(errno);
}