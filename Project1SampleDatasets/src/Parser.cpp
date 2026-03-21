//Implementation of the Parser

#include "Parser.h"
#include <fstream>
#include <functional>
#include <sstream>
#include <iostream>
#include <vector>

#include "Utils.h"

void Parser::parse(const std::string& filename, std::map<int, Submission> submissions, std::map<int, Reviewer> reviewers, Parameters& parameters, Control& control)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "could not open " << filename << "!" << std::endl;
        return;
    }
    std::string line;
    Section currentSection = Section::NONE;
    while (std::getline(file, line))
    {
        if (line.empty()) continue;
        if (line == "#Submissions")
        {
            currentSection = Section::SUBMISSIONS;
            continue;
        }
        if (line == "#Reviewers")
        {
            currentSection = Section::REVIEWERS;
            continue;
        }
        if (line == "#Parameters"){
            currentSection = Section::PARAMETERS;
            continue;
        }
        if (line == "#Control")
        {
            currentSection = Section::CONTROL;
            continue;
        }
        if (line == "#")
        {
            currentSection = Section::NONE;
            continue;
        }
        if (line[0] == '#')
        {
            continue;
        }


        switch (currentSection)
        {
            case (Section::SUBMISSIONS):
                parseSubmissions(line, submissions);
                break;
            case (Section::REVIEWERS):
                parseReviewers(line,reviewers);
                break;
            case (Section::PARAMETERS):
                parseParameters(line, parameters);
                break;
            case (Section::CONTROL):
                parseControl(line, control);
                break;
            default:
                break;
        }
    }
    file.close();
}

void Parser::parseSubmissions(const std::string& line, std::map<int, Submission>& submissions)
{
    std::istringstream fullLine(line);
    std::string field;
    std::vector<std::string> fields;
    while (std::getline(fullLine, field, ','))
    {
        fields.push_back(trim(field));
    }

    if (fields.size() < 5)
    {
        std::cerr << "invalid line: less than 5 fields" << std::endl;
    }

    Submission sub;
    sub.id = std::stoi(fields[0]);
    sub.title = fields[1];
    sub.authorName = fields[2];
    sub.email = fields[3];
    sub.primaryTopic = std::stoi(fields[4]);
    sub.secondaryTopic = (fields.size() > 5 && !fields[5].empty()) ? std::stoi(fields[5]) : -1;
    if (submissions.contains(sub.id))
    {
        std::cerr << "invalid line: duplicate submission found" << std::endl;
        return;
    }
    submissions[sub.id] = sub;
}

void Parser::parseReviewers(const std::string& line, std::map<int, Reviewer>& reviewers)
{
    std::istringstream fullLine(line);
    std::string field;
    std::vector<std::string> fields;
    while (std::getline(fullLine, field, ','))
    {
        fields.push_back(trim(field));
    }

    if (fields.size() < 4)
    {
        std::cerr << "invalid line: less than 4 fields" << std::endl;
        return;
    }

    Reviewer rev;
    rev.id = std::stoi(fields[0]);
    rev.name = fields[1];
    rev.email = fields[2];
    rev.primaryExpertise = std::stoi(fields[3]);
    rev.secondaryExpertise = (fields.size() > 4 && !fields[4].empty()) ? std::stoi(fields[4]) : -1;
    if (reviewers.contains(rev.id))
    {
        std::cerr << "invalid line: duplicate review found" << std::endl;
        return;
    }
        reviewers[rev.id] = rev;
}

void Parser::parseParameters(const std::string& line, Parameters& parameters)
{
    std::istringstream fullLine(line);
    std::string field;
    std::vector<std::string> fields;
    while (std::getline(fullLine, field, ','))
    {
        fields.push_back(trim(field));
    }
    std::string name = fields[0];
    int value = std::stoi(fields[1]);

    std::map<std::string, std::function<void(int)>> paramMap = {
        {"MinReviewsPerSubmission",  [&](int v){ parameters.minReviewsPerSubmission = v; }},
        {"MaxReviewsPerReviewer",    [&](int v){ parameters.maxReviewsPerReviewer = v; }},
        {"PrimaryReviewerExpertise", [&](int v){ parameters.primaryReviewerExpertise = v; }},
        {"SecondaryReviewerExpertise",[&](int v){ parameters.secondaryReviewerExpertise = v; }},
        {"PrimarySubmissionDomain",  [&](int v){ parameters.primarySubmissionDomain = v; }},
        {"SecondarySubmissionDomain",[&](int v){ parameters.secondarySubmissionDomain = v; }}
    };

    if (paramMap.contains(name)) paramMap[name](value);
    else std::cerr << "invalid line: unrecognized parameter" << std::endl;
}

void Parser::parseControl(const std::string& line, Control& control)
{
    std::istringstream fullLine(line);
    std::string field;
    std::vector<std::string> fields;
    while (std::getline(fullLine, field, ','))
    {
        fields.push_back(trim(field));
    }
    std::string name = fields[0];
    std::string value = fields[1];

    std::map<std::string, std::function<void(std::string)>> controlMap = {
        {"GenerateAssignments",[&](std::string c){ control.generateAssignments = std::stoi(c); }},
        {"RiskAnalysis",  [&](std::string c){ control.riskAnalysis = std::stoi(c); }},
        {"OutputFileName",[&](std::string c){control.outputFileName = c; }}
    };

    if (controlMap.contains(name)) controlMap[name](value);
    else std::cerr << "invalid line: unrecognized parameter" << std::endl;
}


