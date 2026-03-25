#include "Output.h"
#include <fstream>

void Output::write(const Scheduler& scheduler, const Control& control)
{
    std::ofstream file(control.outputFileName);
    if (!file.is_open())
    {
        std::cerr << "error: could not open output file" << std::endl;
        return;
    }
    if (control.generateAssignments != 0)
    {
        if (scheduler.wasSuccessful())
        {
            writeAssignments(scheduler.getAssignments(), file);
        }
        else
        {
            writeFailedAssignments(scheduler.getMissingReviews(), file);
        }
    }
    if (control.riskAnalysis != 0)
    {
        writeRiskAnalysis(scheduler.getRiskyReviewers(), control, file);
    }
    file.close();
}

void Output::writeAssignments(std::vector<Assignment> assignments, std::ofstream& file)
{
    std::sort(assignments.begin(), assignments.end(), [](const Assignment& a, const Assignment& b) {return a.submissionID < b.submissionID;});
    file << "#SubmissionId,ReviewerId,Match" << std::endl;
    for (const Assignment& assignment : assignments)
    {
        file << assignment.submissionID << ", " << assignment.reviewerID <<  ", " << assignment.matchedTopic << std::endl;
    }
    std::sort(assignments.begin(), assignments.end(), [](const Assignment& a, const Assignment& b) {return a.reviewerID < b.reviewerID;});
    file << "#ReviewerId,SubmissionId,Match" << std::endl;
    for (const Assignment& assignment : assignments)
    {
        file << assignment.reviewerID << ", " << assignment.submissionID <<  ", " << assignment.matchedTopic << std::endl;
    }
    file << "#Total: " << assignments.size() << std::endl;
}

void Output::writeFailedAssignments(std::vector<FailedAssignment> failedAssignments, std::ofstream& file)
{
    std::sort(failedAssignments.begin(), failedAssignments.end(), [](const FailedAssignment& a, const FailedAssignment& b) {return a.id < b.id;});
    file << "#SubmissionId,Domain,MissingReviews" << std::endl;
    for (const FailedAssignment& assignment : failedAssignments)
    {
        file << assignment.id << ", " << assignment.primaryDomain << ", " << assignment.missingReviews << std::endl;
    }
}

void Output::writeRiskAnalysis(const std::set<int>& riskyReviewers, const Control& control, std::ofstream& file)
{
    bool first = true;
    file << "#Risk Analysis: " << control.riskAnalysis << std::endl;
    if (riskyReviewers.empty()) return;
    for (const int& reviewer : riskyReviewers)
    {
        if (!first) file << ", ";
        file << reviewer;
        first = false;
    }
    file << std::endl;
}

