#include "clingo3to4.h"
#include "domain.h"

#define NEGATION ":-"

namespace io = boost::iostreams; 
namespace po = boost::program_options; 

int clingo3to4::convert(const int argc,const char *argv[]){	
	// int parse_op = clingo3to4::parse_input_args(argc,argv);

	po::options_description desc("Options");
	desc.add_options()
		("help,h","Produce help message")
		("stdin,s","Use this option to read from stdin. This is the default.")
		("stdout,o","Use this option to output to stdout when reading from file.")
		("incremental,i","Use this option to output iclingo compatible file.")
		("file,f",po::value<std::string>(),"Use this option to read from file");
	
	try
	{
		po::positional_options_description p;
		p.add("file",-1);
		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
		po::notify(vm);

		
		if (vm.count("help"))
		{
			std::cout<< desc << std::endl;
		}

		if(vm.count("incremental")){
			clingo3to4::set_incremental(true);
		}

		setup_clauses();

		if (vm.count("stdin"))
		{
			clingo3to4::convert_stdin(argv);	
		}

		else if (vm.count("file"))
		{
			std::string filename = vm["file"].as<std::string>();
			if (vm.count("stdout")){
				clingo3to4::convert_file(argv,true,filename);	
			}
			else{
				clingo3to4::convert_file(argv,false,filename);
			}
		}
		
		// If no input arguments are given it defaults to stdin
		else{
			clingo3to4::convert_stdin(argv);
		}




		
	}

	catch(po::error& e) 
    { 
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
		std::cerr << desc << std::endl; 
		return -1; 
    } 
	
	catch(std::exception& e) 
	{ 
		std::cerr << "Unhandled Exception reached the top of main: " 
		          << e.what() << ", application will now exit" << std::endl; 
		return -1; 
	} 
	return 0;
}

int clingo3to4::convert_stdin(const char *argv[]){
	std::string str; 
	while (std::getline(std::cin, str)) 
	{
		if(!str.empty())
        {

			//process str. Str can be a single line ex q(1,2).q(1,3). need to separate these commands process them individually
        	std::vector<std::string> ind_commands;

        	if(str.compare(0,1,COMMENT) == 0)
			{
				std::cout << str.append(NEWLINE);
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
					std::cout << output;
					#ifdef DEBUG
						std::cout<<output<<std::endl;
					#endif
				}
			}
        }
	}
}

int clingo3to4::convert_file(const char *argv[],bool stdout,std::string filename){
	const char *input_file_name;
	// const char *output_file_name = "output.l";
	
	
	input_file_name = filename.c_str();
	


	std::ifstream file(input_file_name, std::ios_base::in | std::ios_base::binary);
	// std::ofstream outfile;


	char *output_file_name = (char *) malloc (sizeof(char*) * (strlen(argv[1]) + strlen(OUTPUT_EXTN)));
	strcpy(output_file_name,input_file_name);
	strcat(output_file_name,OUTPUT_EXTN);
	std::ofstream outfile(output_file_name);	
	
	if(!outfile.is_open() && !stdout)
	{
		std::cerr << "Couldn't open output.l \n";
		// delete outfile;
		return -1;
	}

    if(file)
    {
	    try 
		{
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
						if(!stdout)
							outfile << str.append(NEWLINE);
						else
							std::cout << str.append(NEWLINE);
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
        					if(!stdout)
        						outfile << output;
        					else
        						std::cout << output;
        					#ifdef DEBUG
        						std::cout<<output<<std::endl;
        					#endif
        				}

            			
        				
            		}
	            }

	        }
	    }
	    catch(std::exception& e) {
	         std::cout << e.what() << '\n';
	         // if(!stdout)
	         // 	delete outfile;
	         return -1;
	    }
	}
	else
	{
		std::cerr << "File could not be opened!\n";
		std::cerr << "Type clingo3to4 -h for help."<<"\n";	
	}
	// if(!stdout)
	// {
	// 	delete outfile;
	// }
   	return 0;
}

int clingo3to4::match_normal_rule(std::string& output, const std::string& input){
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
int clingo3to4::match_counting_literal_rule(std::string& output, const std::string& input){
	// 1{c_a_1_howmany(GVAR_item_1,o_none,t-1),c_a_1_buy(GVAR_item_1,o_false,t-1)}1
	boost::regex expr("([0-9 ]*)([{]){1}([A-Za-z0-9\\(\\),:<>=_\\+\\-\\*\\/ ]+)([}]){1}([0-9 ]*)"); 
	std::string::const_iterator start, end; 
	start = input.begin(); 
	end = input.end();
	boost::match_results<std::string::const_iterator> what;

	boost::match_flag_type flags = boost::match_default;

	std::string prefix("");
	std::string str("");
	std::string suffix("");
	if(boost::regex_search(start,end,what,expr,flags))
	{
		prefix = what.prefix();
		str = what[0];
		// start = what[0].second;
	}
	else
	{
		//if it cannot find anything that matches our regex we exit
		return 0;
	}
	
	suffix = what.suffix();
	
	output = str;

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
				inside_paren += 1;
				continue;
			}

			if(output[i] == ')')
			{
				inside_paren -= 1;
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

		output = prefix + output + suffix;		
		return 1;
	}
	else
		return 0;
}

/*
Clingo 4 does not support #hide clause
*/

int clingo3to4::match_clause_rule(std::string& output, const std::string& input){
	output = input;

	for (int i = 0; i < clauses.size(); ++i)
	{
		if (output.find(clauses.at(i)) != std::string::npos)
		{
			//If it is in incremental mode then dont comment out base,cumulative and volatile
			if(get_incremental())
			{	
				if(clauses.at(i) == IBASE)
				{
					set_current_scope(BASE);
					boost::replace_all(output,IBASE,PRGBASE);

				}
				else if(clauses.at(i) == ICUMULATIVE)
				{
					
					set_current_scope(CUMULATIVE);
					std::string cum_var_string(std::string(PAREN_OPEN) + input.at(input.length()-1) + PAREN_CLOSE);
					boost::algorithm::replace_all(output,input,PRGCUMULATIVE + cum_var_string);
					
				}
				else if(clauses.at(i) == IVOLATILE)
				{
					set_current_scope(VOLATILE);
					std::string vol_var_string(std::string(PAREN_OPEN) + input.at(input.length()-1) + PAREN_CLOSE);
					clingo3to4::set_volatile_query(std::string(1,input.at(input.length()-1)));
					boost::algorithm::replace_all(output,input,PRGVOLATILE + vol_var_string);
				}
			}
			if(clauses.at(i) == HIDE)
			{
				//It is #hide. Comment it.
				#ifdef DEBUG
					std::cout<<"Hide rule matched"<<std::endl;
				#endif
				output.insert(0, COMMENT);	
			}
			return 1;
		}
	}

	return 0;
}


int clingo3to4::match_volatile_rule(std::string& output, const std::string& input){
	output = input;
	if(boost::algorithm::starts_with(input,NEGATION) && get_scope() == VOLATILE){
		output.append(COMMA)
			 .append(std::string(PRGQUERY) + PAREN_OPEN + clingo3to4::get_volatile_query() + PAREN_CLOSE);
		return 1;
	}
	return 0;
}	


void clingo3to4::setup_clauses(){
	clauses.push_back(std::string("#hide"));
	if(get_incremental())
	{
		clauses.push_back(std::string(IBASE));
		clauses.push_back(std::string(ICUMULATIVE));
		clauses.push_back(std::string(IVOLATILE));
	}
}


//Every string will match one of these rules. If it wont match the string is written as it is.
//Every string after its transformation also needs to be checked for domain variables.
//If it contains domain variables then in that case those variables need to be removed.
int clingo3to4::match_rule(std::string& output, const std::string& input){
	if (match_clause_rule(output,input))
	{
		#ifdef DEBUG
			std::cout<<"Clause rule matched"<<std::endl;
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
	else if (clingo3to4::match_volatile_rule(output,input))
	{
		#ifdef DEBUG
			std::cout<<"volatile rule matched"<<std::endl;
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


std::string clingo3to4::get_file_contents(const char *filename){
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in)
  {
    return(std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));
  }
  throw(errno);
}

void clingo3to4::set_incremental(bool is_incremental){
	is_incremental = is_incremental;
}

bool clingo3to4::get_incremental(){
	return is_incremental;
}

void clingo3to4::set_current_scope(IncrementalScope scope){
	current_scope = scope; 
}

clingo3to4::IncrementalScope clingo3to4::get_scope(){
	return current_scope;
}

void clingo3to4::set_volatile_query(const std::string& str){
	vol_var_string = str;
}

std::string clingo3to4::get_volatile_query(){
	return vol_var_string;
}