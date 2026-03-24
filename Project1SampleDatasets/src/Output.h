#ifndef PROJECT1_OUTPUT_H
#define PROJECT1_OUTPUT_H
#include "Scheduler.h"

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