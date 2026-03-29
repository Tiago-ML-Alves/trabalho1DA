/**
* @file Output.h
 * @brief Declaration of the Output class.
 */

#ifndef PROJECT1_OUTPUT_H
#define PROJECT1_OUTPUT_H
#include "Scheduler.h"

/**
 * @class Output
 * @brief Writes assignment results to a CSV output file.
 *
 * Generates up to three sections: successful assignments (by submission and by
 * reviewer), failed assignments, and risk analysis results.
 */

class Output
{
public:
    static void write(const Scheduler& scheduler, const Control& control);
private:
    static void writeAssignments(std::vector<Assignment> assignments, std::ofstream& file);
    static void writeFailedAssignments(std::vector<FailedAssignment> failedAssignments, std::ofstream& file);
    static void writeRiskAnalysis(const std::set<int>& riskyReviewers, const Control& control, std::ofstream& file);
};
#endif //PROJECT1_OUTPUT_H