#ifndef PROJECT1_SCHEDULER_H
#define PROJECT1_SCHEDULER_H
#include "Types.h"
#include "FlowNetwork.h"

class Scheduler
{
public:
    Scheduler (FlowNetwork& flowNetwork, const std::map<int, Submission>& submissions, const std::map<int, Reviewer>& reviewers);
    void runRiskAnalysis ();
    std::vector<Assignment> getAssignments() const;
    std::vector<FailedAssignment> getMissingReviews() const;
    bool wasSuccessful() const;
private:
    FlowNetwork& flowNetwork_;
    std::map<int, Submission> submissions_;
    std::map <int, Reviewer> reviewers_;
    bool successful_;
    std::vector<Assignment> assignments_;
    std::vector<FailedAssignment> missingReviews_;
    void extractAssignments();
};
#endif //PROJECT1_SCHEDULER_H