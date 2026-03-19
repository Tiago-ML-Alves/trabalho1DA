//
// Created by alex on 3/18/26.
//

#ifndef PROJECT1_PARSER_H
#define PROJECT1_PARSER_H
#include <string>
#include <map>
#include "Types.h"

class Parser
{
public:
    static void parse(const std::string& filename, std::map<int, Submission> submissions, std::map<int, Reviewer> reviewers,
                Parameters& parameters, Control& control);
    static void parseSubmissions(const std::string& line, std::map<int, Submission>& submissions);
    static void parseReviewers(const std::string& line, std::map<int, Reviewer>& reviewers);
    static void parseParameters(const std::string& line, Parameters& parameters);
    static void parseControl(const std::string& line, Control& control);
};

#endif //PROJECT1_PARSER_H