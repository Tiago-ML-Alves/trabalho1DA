#include "Parser.h"
#include <fstream>
#include <iostream>

void Parser::parse(const std::string& filename, std::map<int, VertexType> papers, std::map<int, VertexType> reviewers, Parameters& parameters, Control& control)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "could not open " << filename << "!" << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line))
    {
        std::cout << line;
    }

    file.close();
}
