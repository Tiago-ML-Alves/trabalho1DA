/**
* @file Output.cpp
 * @brief Implementation of the Output class.
 */

#include "Output.h"
#include <filesystem>
#include <fstream>

/**
 * @brief Writes all output sections to the CSV file specified in @p control.
 * @param scheduler Completed Scheduler containing results.
 * @param control   Control parameters (output filename and flags).
 * @complexity O(A log A)
 */

void Output::write(const Scheduler& scheduler, const Control& control)
{
    std::filesystem::path outputPath(control.outputFileName);
    if (outputPath.has_parent_path()) std::filesystem::create_directories(outputPath.parent_path());
    std::ofstream file(control.outputFileName);
    if (!file.is_open())
    {
        std::cerr << "error: could not open output file" << std::endl;
        return;
    }
    if (control.generateAssignments != 0)
    {
        writeAssignments(scheduler.getAssignments(), file);
        if (!scheduler.wasSuccessful())
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

/**
 * @brief Writes the assignment table (sorted by submission and by reviewer).
 * @param assignments Vector of assignments (by value for sorting).
 * @param file        Open output file stream.
 * @complexity O(A log A)
 */

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

/**
 * @brief Writes the failed-assignment section.
 * @param failedAssignments Failed assignments (by value for sorting).
 * @param file              Open output file stream.
 * @complexity O(F log F)
 */

void Output::writeFailedAssignments(std::vector<FailedAssignment> failedAssignments, std::ofstream& file)
{
    std::sort(failedAssignments.begin(), failedAssignments.end(), [](const FailedAssignment& a, const FailedAssignment& b) {return a.id < b.id;});
    file << "#SubmissionId,Domain,MissingReviews" << std::endl;
    for (const FailedAssignment& assignment : failedAssignments)
    {
        file << assignment.id << ", " << assignment.primaryDomain << ", " << assignment.missingReviews << std::endl;
    }
}

/**
 * @brief Writes the risk-analysis section.
 * @param riskyReviewers Sorted set of risky reviewer IDs.
 * @param control        Control parameters (provides the K value).
 * @param file           Open output file stream.
 * @complexity O(R)
 */

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

