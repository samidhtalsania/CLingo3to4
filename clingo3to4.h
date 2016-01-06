#include "constants.h"

class clingo3to4
{
public:
	int convert(const int argc, const char *argv[]);
	
private:
	
	int convert_file(const char* argv[],bool stdout);
	int convert_stdin(const char* argv[]);
	int match_normal_rule(std::string& output, const std::string& input);
	int match_counting_literal_rule(std::string& output, const std::string& input);
	int match_hide_base_rule(std::string& output, const std::string& input);
	int match_rule(std::string& output, const std::string& input);
	std::string get_file_contents(const char *filename);
	
};

