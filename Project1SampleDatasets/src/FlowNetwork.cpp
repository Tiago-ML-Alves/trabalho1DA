#include "FlowNetwork.h"

#include <ranges>

FlowNetwork::FlowNetwork(const std::map<int, Submission>& submissions, const std::map<int, Reviewer>& reviewers, const Parameters& parameters, int generateAssignments)
    : parameters_(parameters)
{
    generateAssignments_ = generateAssignments;
    buildNetwork(submissions, reviewers);
}

void FlowNetwork::buildNetwork(const std::map<int, Submission>& submissions, const std::map<int, Reviewer>& reviewers)
{
    NodeInfo source = FlowNetwork::getSource();
    NodeInfo sink = FlowNetwork::getSink();
    graph_.addVertex(source);
    graph_.addVertex(sink);
    for (const auto& submission : submissions | std::views::values)
    {
        NodeInfo sub = {submission.id, VertexType::SUBMISSION};
        graph_.addVertex(sub);
        graph_.addEdge(source, sub, parameters_.minReviewsPerSubmission);
    }
    for (const auto& reviewer : reviewers | std::views::values)
    {
        NodeInfo rev = {reviewer.id, VertexType::REVIEWER};
        graph_.addVertex(rev);
        graph_.addEdge(rev, sink, parameters_.maxReviewsPerReviewer);
    }
    for (const auto& rev : reviewers | std::views::values)
    {
        for (const auto& sub : submissions | std::views::values)
        {
            if (topicsMatch(sub, rev))
            {
                graph_.addEdge({sub.id, VertexType::SUBMISSION}, {rev.id, VertexType::REVIEWER}, 1);
            }
        }
    }
}

bool FlowNetwork::topicsMatch(const Submission& sub, const Reviewer& rev) const
{
    switch (generateAssignments_)
    {
    case(0):
    case (1):
        if (sub.primaryTopic == rev.primaryExpertise) return true;
        break;
    case (2):
        if (sub.primaryTopic == rev.primaryExpertise || sub.secondaryTopic == rev.primaryExpertise) return true;
        break;
    case (3):
        if (sub.primaryTopic == rev.primaryExpertise
            || sub.secondaryTopic == rev.primaryExpertise
            || (rev.secondaryExpertise != -1 && sub.primaryTopic == rev.secondaryExpertise)
            || (rev.secondaryExpertise != -1 && sub.secondaryTopic == rev.secondaryExpertise)
        )
            return true;
        break;
    default: return false;

    }
    return false;
}

int FlowNetwork::matchingTopic(const Submission& sub, const Reviewer& rev) const
{
    switch (generateAssignments_)
    {
    case(0):
    case (1):
        if (sub.primaryTopic == rev.primaryExpertise) return sub.primaryTopic;
        break;
    case (2):
        if (sub.primaryTopic == rev.primaryExpertise)
            return sub.primaryTopic;
        if (sub.secondaryTopic == rev.primaryExpertise)
            return sub.secondaryTopic;
        break;
    case (3):
        if (sub.primaryTopic == rev.primaryExpertise)
            return sub.primaryTopic;
        if (sub.secondaryTopic == rev.primaryExpertise)
            return sub.secondaryTopic;
        if (sub.primaryTopic == rev.secondaryExpertise)
            return sub.primaryTopic;
        if (sub.secondaryTopic != -1 && rev.secondaryExpertise != -1 && sub.secondaryTopic == rev.secondaryExpertise)
            return sub.secondaryTopic;
        break;
    default: return -1;
    }
    return -1;
}

Graph<NodeInfo>& FlowNetwork::getGraph()
{
    return graph_;
}

NodeInfo FlowNetwork::getSink() const
{
    return {0, VertexType::SINK};
}

NodeInfo FlowNetwork::getSource() const
{
    return {0, VertexType::SOURCE};
}

Parameters FlowNetwork::getParameters() const
{
    return parameters_;
}
