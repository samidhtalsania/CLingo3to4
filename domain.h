#include "constants.h"



class domain
{
public:
	static std::unordered_map<std::string,std::string> domain_list;
	static std::set<std::string> search_domain_variables(const std::string& input);
	static int isDomain(const std::string& input);
	static void get_domain_variables(const std::string& input);
	static int match_domain_rule(std::string& output, const std::string& input);
	static void remove_domain_variables(std::string& output,const std::string& input);
};
