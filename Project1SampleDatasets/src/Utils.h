//Implements some utility functions.

#ifndef PROJECT1_UTIILS_H
#define PROJECT1_UTIILS_H

#include <algorithm>
#include <string>

std::string trim(const std::string& string1)
{
    if (string1.empty()) return "";
    int firstPos = string1.find_first_not_of(" ");
    int lastPos = string1.find_last_not_of(" ");
    std::string trimmed = string1.substr(firstPos, lastPos - firstPos + 1);
    if (trimmed.front() == '"' && trimmed.back() == '"')
    {
        trimmed = trimmed.substr(1, trimmed.size() - 2);
        firstPos = trimmed.find_first_not_of(" ");
        lastPos = trimmed.find_last_not_of(" ");
        trimmed = trimmed.substr(firstPos, lastPos - firstPos + 1);
    }
    return trimmed;
}
#endif //PROJECT1_UTIILS_H