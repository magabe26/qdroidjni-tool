#include "utils.h"
#include <regex>

bool matchPattern(char c, std::string pattern){
    std::regex rgx(pattern);
    std::cmatch match;

    char cv[2] ; cv[0] = c; cv[1] ='\0';
    const char * pc = cv;

    bool hasMatch = false;
    if(std::regex_search(pc,match,rgx)){
        hasMatch = (match.size() > 0);
    }

    return hasMatch;
}

