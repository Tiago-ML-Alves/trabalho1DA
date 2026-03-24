#ifndef PROJECT1_SCHEDULER_H
#define PROJECT1_SCHEDULER_H
#include <set>

#include "Types.h"
#include "FlowNetwork.h"

class Scheduler
{
public:
    Scheduler (FlowNetwork& flowNetwork, const std::map<int, Submission>& submissions, const std::map<int, Reviewer>& reviewers);
    void runRiskAnalysis (int k);
    std::vector<Assignment> getAssignments() const;
    std::set<int> getRiskyReviewers() const;
    std::vector<FailedAssignment> getMissingReviews() const;
    bool wasSuccessful() const;
private:
    FlowNetwork& flowNetwork_;
    std::map<int, Submission> submissions_;
    std::map <int, Reviewer> reviewers_;
    bool successful_;
    std::vector<Assignment> assignments_;
    std::vector<FailedAssignment> missingReviews_;
    std::set<int> riskyReviewers_;
    int countAssignedReviewers(Vertex<NodeInfo>* v) const;
    void extractAssignments();
    bool checkSuccessful();
};
#endif //PROJECT1_SCHEDULER_H