#include <fstream>
#include <streambuf>
#include <string>
#include <cerrno>
#include <iostream>
#include <vector>
#include <algorithm>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/program_options.hpp>

// #define DEBUG 0
#define COMMENT "%"
#define PAREN_OPEN "("
#define PAREN_CLOSE ")"
#define COMMA ","
#define COLON_DASH ":-" 
#define DOT "."
#define DOTCHAR '.'
#define WHITESPACE "\t \n"
#define HASHTAG "#"
#define NEWLINE "\n"
#define OUTPUT_EXTN ".cl\0" 

namespace ba = boost::algorithm;