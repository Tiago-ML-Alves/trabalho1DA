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
    static void parse(const std::string& filename, std::map<int, VertexType> papers, std::map<int, VertexType> reviewers,
                Parameters& parameters, Control& control);
};

#endif //PROJECT1_PARSER_H