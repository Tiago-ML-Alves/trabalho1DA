#include "FlowNetwork.h"

#include <ranges>

FlowNetwork::FlowNetwork(const std::map<int, Submission>& submissions, const std::map<int, Reviewer>& reviewers, const Parameters& parameters, int generateAssignments)
{
    parameters_ = parameters;
    generateAssignments_ = generateAssignments;
    buildNetwork(submissions, reviewers);
}

void FlowNetwork::buildNetwork(const std::map<int, Submission>& submissions, const std::map<int, Reviewer>& reviewers)
{
    NodeInfo source = {0, VertexType::SOURCE};
    NodeInfo sink = {0, VertexType::SINK};
    g.addVertex(source);
    g.addVertex(sink);
    for (const auto& submission : submissions | std::views::values)
    {
        NodeInfo sub = {submission.id, VertexType::SUBMISSION};
        g.addVertex(sub);
        g.addEdge(source, sub, parameters_.minReviewsPerSubmission);
    }
    for (const auto& reviewer : reviewers | std::views::values)
    {
        NodeInfo rev = {reviewer.id, VertexType::REVIEWER};
        g.addVertex(rev);
        g.addEdge(rev, sink, parameters_.maxReviewsPerReviewer);
    }
    for (const auto& rev : reviewers | std::views::values)
    {
        for (const auto& sub : submissions | std::views::values)
        {
            if (topicsMatch(sub, rev))
            {
                g.addEdge({sub.id, VertexType::SUBMISSION}, {rev.id, VertexType::REVIEWER}, 1);
            }
        }
    }
}

bool FlowNetwork::topicsMatch(const Submission& sub, const Reviewer& rev)
{

    switch (generateAssignments_)
    {
    case (1):
        if (sub.primaryTopic == rev.primaryExpertise) return true;
        break;
    case (2):
        if (sub.primaryTopic == rev.primaryExpertise || sub.secondaryTopic == rev.primaryExpertise) return true;
        break;
    case (3):
        if (sub.primaryTopic == rev.primaryExpertise
            || sub.secondaryTopic == rev.primaryExpertise
            || (sub.primaryTopic == rev.secondaryExpertise)
            || (rev.secondaryExpertise != -1 && sub.secondaryTopic == rev.secondaryExpertise)
        )
            return true;
        break;
    default: return false;
    }
    return false;
}

/*
g.addEdge({sub.id, VertexType::SUBMISSION}, {rev.id, VertexType::REVIEWER}, 1);
*/