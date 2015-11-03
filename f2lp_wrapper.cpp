/*Pseudo code

1 Read file line by line //done
2 Every line that is read, split the line by delimiter(".") and put the result in a std::string array. //done
3 Match every std::string in array with one of the rules. If a particular rule matches, change output accordingly.
4 While matching the rules if the rule matched is the domain one, push the variable in domain in a struct which has 2 values(func, variable) and comment out that line.
5 While matching the rule, everywhere the domain variable appears, replace the domain variable with a local variable.
6 repeat till all lines are taken care of.
7 at max should require one parse throught he file. 

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


std::string match_ground_term_rule(const std::string input);
std::string match_normal_rule(const std::string input);
std::string match_counting_literal_rule(const std::string input);
std::string match_conditional_literal_rule(const std::string input);
std::string match_domain_rule(const std::string input);
std::string match_hide_rule(const std::string input);

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
	// std::string file_content = get_file_contents("input.txt");
	
	// std::vector<std::string> nSeparatedStrings;
	

	// //boost::trim_if(file_content, boost::is_any_of("\n"));
	// boost::split(nSeparatedStrings,file_content,boost::is_any_of("\n"),boost::token_compress_on);

	// //operate on each line
	// int count= 0 ;

	// for (size_t i = 0; i < nSeparatedStrings.size(); i++)
 //    {
 //    	std::vector<std::string> dotSeparatedStrings;
 //    	boost::split(dotSeparatedStrings,nSeparatedStrings[i],boost::is_any_of("."));
    	
 //    	for (size_t j = 0; j < dotSeparatedStrings.size(); j++)
 //    	{
	// 		//use dotSeparatedStrings[j] for the rest of the program to make changes
 //    	}	
 //    }

	std::ifstream file("input.txt", std::ios_base::in | std::ios_base::binary);
    if(file)
    {
	    try {

	        boost::iostreams::filtering_istream in;
	        
	        in.push(file);
	        for(std::string str; std::getline(in, str); )
	        {
	            std::cout << "Processed line " << str << '\n';
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


