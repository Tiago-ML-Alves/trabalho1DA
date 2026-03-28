/**
 * @file Parser.h
 * @brief Declaration of the Parser class.
 */
#ifndef PROJECT1_PARSER_H
#define PROJECT1_PARSER_H

#include <string>
#include <map>
#include "Types.h"

/**
 * @class Parser
 * @brief Static utility that reads and parses the CSV input file.
 */
class Parser
{
public:

    static void parse(const std::string& filename,
                      std::map<int, Submission>& submissions,
                      std::map<int, Reviewer>&   reviewers,
                      Parameters& parameters,
                      Control&    control);

    /** @brief Parse one line from the #Submissions section. @complexity O(1) */
    static void parseSubmissions(const std::string& line, std::map<int, Submission>& submissions);

    /** @brief Parse one line from the #Reviewers section. @complexity O(1) */
    static void parseReviewers(const std::string& line, std::map<int, Reviewer>& reviewers);

    /** @brief Parse one key,value line from the #Parameters section. @complexity O(1) */
    static void parseParameters(const std::string& line, Parameters& parameters);

    /** @brief Parse one key,value line from the #Control section. @complexity O(1) */
    static void parseControl(const std::string& line, Control& control);
};

#endif // PROJECT1_PARSER_H
