//Implements some utility functions.

#ifndef PROJECT1_UTIILS_H
#define PROJECT1_UTIILS_H

#include <algorithm>
#include <string>

/**
 *
 * @param string1
 * @return
 */
std::string trim(const std::string& string1)
{
    if (string1.empty()) return "";

    auto firstPos = string1.find_first_not_of(" \r\n\t"); // ← also trim \r!
    if (firstPos == std::string::npos) return "";

    auto lastPos = string1.find_last_not_of(" \r\n\t");   // ← also trim \r!
    std::string trimmed = string1.substr(firstPos, lastPos - firstPos + 1);

    if (trimmed.front() == '"' && trimmed.back() == '"')
    {
        trimmed = trimmed.substr(1, trimmed.size() - 2);
        firstPos = trimmed.find_first_not_of(" \r\n\t");
        if (firstPos == std::string::npos) return "";
        lastPos = trimmed.find_last_not_of(" \r\n\t");
        trimmed = trimmed.substr(firstPos, lastPos - firstPos + 1);
    }
    return trimmed;
}
#endif //PROJECT1_UTIILS_H