/**
 * @file Parser.cpp
 * @brief Implementation of the Parser class.
 */

#include "Parser.h"
#include "Utils.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <vector>

// ---------------------------------------------------------------------------
// Public – top-level parse
// ---------------------------------------------------------------------------

/**
 * @details
 * Iterates through every line of the file.  Lines starting with `#` switch the
 * active section; all other non-empty lines are forwarded to the matching helper.
 * Unknown `#`-prefixed lines (that are not recognised section headers) are
 * silently skipped so that comments inside the file are allowed.
 *
 * @note The `submissions` and `reviewers` parameters are passed **by reference**
 *       so that inserted entries are visible to the caller.  (Previous version
 *       passed them by value — a bug that caused all parsed data to be lost.)
 */

void Parser::parse(const std::string& filename,
                   std::map<int, Submission>& submissions,
                   std::map<int, Reviewer>&   reviewers,
                   Parameters& parameters,
                   Control&    control)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "\n  error: could not open \"" << filename << "\"" << std::endl;
        return;
    }

    std::string line;
    Section currentSection = Section::NONE;

    while (std::getline(file, line))
    {
        // Strip Windows-style CR if present
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (line.empty()) continue;

        if (line == "#Submissions") { currentSection = Section::SUBMISSIONS; continue; }
        if (line == "#Reviewers")   { currentSection = Section::REVIEWERS;   continue; }
        if (line == "#Parameters")  { currentSection = Section::PARAMETERS;  continue; }
        if (line == "#Control")     { currentSection = Section::CONTROL;     continue; }
        if (line == "#")            { currentSection = Section::NONE;        continue; }
        if (line[0] == '#')         { /* comment or unknown section header */  continue; }

        switch (currentSection)
        {
            case Section::SUBMISSIONS: parseSubmissions(line, submissions); break;
            case Section::REVIEWERS:   parseReviewers(line, reviewers);     break;
            case Section::PARAMETERS:  parseParameters(line, parameters);   break;
            case Section::CONTROL:     parseControl(line, control);         break;
            default: break;
        }
    }

    file.close();
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

/**
 * @brief Parses a single CSV line from the #Submissions section.
 * @param line    Raw CSV line to parse.
 * @param submissions Map to insert the parsed Submission into.
 * @complexity O(F) where F is the number of comma-separated fields.
 */

void Parser::parseSubmissions(const std::string& line,
                              std::map<int, Submission>& submissions)
{
    std::istringstream ss(line);
    std::string field;
    std::vector<std::string> fields;

    while (std::getline(ss, field, ','))
        fields.push_back(trim(field));

    if (fields.size() < 5)
    {
        std::cerr << "error (submissions): expected ≥5 fields, got "
                  << fields.size() << " in line: " << line << std::endl;
        return;
    }

    Submission sub;
    try
    {
        sub.id           = std::stoi(fields[0]);
        sub.title        = fields[1];
        sub.authorName   = fields[2];
        sub.email        = fields[3];
        sub.primaryTopic = std::stoi(fields[4]);
        sub.secondaryTopic = (fields.size() > 5 && !fields[5].empty())
                             ? std::stoi(fields[5]) : -1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "  error (submissions): could not parse line: "
                  << line << " (" << e.what() << ")" << std::endl;
        return;
    }

    if (submissions.count(sub.id))
    {
        std::cerr << "error (submissions): duplicate ID " << sub.id << std::endl;
        return;
    }

    submissions[sub.id] = sub;
}

/**
 * @brief Parses a single CSV line from the #Reviewers section.
 * @param line     Raw CSV line to parse.
 * @param reviewers Map to insert the parsed Reviewer into.
 * @complexity O(F)
 */

void Parser::parseReviewers(const std::string& line,
                            std::map<int, Reviewer>& reviewers)
{
    std::istringstream ss(line);
    std::string field;
    std::vector<std::string> fields;

    while (std::getline(ss, field, ','))
        fields.push_back(trim(field));

    if (fields.size() < 4)
    {
        std::cerr << "error (reviewers): expected ≥4 fields, got "
                  << fields.size() << " in line: " << line << std::endl;
        return;
    }

    Reviewer rev;
    try
    {
        rev.id               = std::stoi(fields[0]);
        rev.name             = fields[1];
        rev.email            = fields[2];
        rev.primaryExpertise = std::stoi(fields[3]);
        rev.secondaryExpertise = (fields.size() > 4 && !fields[4].empty())
                                 ? std::stoi(fields[4]) : -1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "  error (reviewers): could not parse line: "
                  << line << " (" << e.what() << ")" << std::endl;
        return;
    }

    if (reviewers.count(rev.id))
    {
        std::cerr << "  error (reviewers): duplicate ID " << rev.id << std::endl;
        return;
    }

    reviewers[rev.id] = rev;
}

/**
 * @brief Parses a single key=value line from the #Parameters section.
 * @param line       Raw CSV line with format "KeyName, value".
 * @param parameters Parameters struct to update.
 * @complexity O(1) — fixed number of recognised keys.
 */

void Parser::parseParameters(const std::string& line, Parameters& parameters)
{
    std::istringstream ss(line);
    std::string field;
    std::vector<std::string> fields;

    while (std::getline(ss, field, ','))
        fields.push_back(trim(field));

    if (fields.size() < 2)
    {
        std::cerr << "  error (parameters): malformed line: " << line << std::endl;
        return;
    }

    const std::string& name = fields[0];
    int value = 0;
    try { value = std::stoi(fields[1]); }
    catch (const std::exception& e)
    {
        std::cerr << "  error (parameters): non-integer value for \""
                  << name << "\": " << e.what() << std::endl;
        return;
    }

    std::map<std::string, std::function<void(int)>> paramMap =
    {
        {"MinReviewsPerSubmission",   [&](int v){ parameters.minReviewsPerSubmission   = v; }},
        {"MaxReviewsPerReviewer",     [&](int v){ parameters.maxReviewsPerReviewer     = v; }},
        {"PrimaryReviewerExpertise",  [&](int v){ parameters.primaryReviewerExpertise  = v; }},
        {"SecondaryReviewerExpertise",[&](int v){ parameters.secondaryReviewerExpertise= v; }},
        {"PrimarySubmissionDomain",   [&](int v){ parameters.primarySubmissionDomain   = v; }},
        {"SecondarySubmissionDomain", [&](int v){ parameters.secondarySubmissionDomain = v; }}
    };

    if (paramMap.count(name))
        paramMap[name](value);
    else
        std::cerr << "  warning (parameters): unrecognised key \"" << name << "\"" << std::endl;
}

/**
 * @brief Parses a single key=value line from the #Control section.
 * @param line    Raw CSV line with format "KeyName, value".
 * @param control Control struct to update.
 * @complexity O(1) — fixed number of recognised keys.
 */

void Parser::parseControl(const std::string& line, Control& control)
{
    std::istringstream ss(line);
    std::string field;
    std::vector<std::string> fields;

    while (std::getline(ss, field, ','))
        fields.push_back(trim(field));

    if (fields.size() < 2)
    {
        std::cerr << "  error (control): malformed line: " << line << std::endl;
        return;
    }

    const std::string& name  = fields[0];
    const std::string& value = fields[1];

    std::map<std::string, std::function<void(const std::string&)>> controlMap =
    {
        {"GenerateAssignments", [&](const std::string& v){ control.generateAssignments = std::stoi(v); }},
        {"RiskAnalysis",        [&](const std::string& v){ control.riskAnalysis        = std::stoi(v); }},
        {"OutputFileName",      [&](const std::string& v){ control.outputFileName      = v;            }}
    };

    if (controlMap.count(name))
    {
        try { controlMap[name](value); }
        catch (const std::exception& e)
        {
            std::cerr << "  error (control): bad value for \""
                      << name << "\": " << e.what() << std::endl;
        }
    }
    else
    {
        std::cerr << "  warning (control): unrecognised key \"" << name << "\"" << std::endl;
    }
}
