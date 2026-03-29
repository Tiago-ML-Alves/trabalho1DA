/**
* @file FlowNetwork.cpp
 * @brief Implementation of the FlowNetwork class.
 */

#include "FlowNetwork.h"
#include <ranges>

/**
 * @brief Constructs the flow network and builds the graph immediately.
 * @param submissions Map of submission ID → Submission.
 * @param reviewers   Map of reviewer ID → Reviewer.
 * @param parameters  Problem parameters.
 * @param generateAssignments Topic-matching mode (1, 2 or 3).
 * @complexity O(S × R)
 */

FlowNetwork::FlowNetwork(const std::map<int, Submission>& submissions, const std::map<int, Reviewer>& reviewers, const Parameters& parameters, int generateAssignments)
    : parameters_(parameters)
{
    generateAssignments_ = generateAssignments;
    buildNetwork(submissions, reviewers);
}

/**
 * @brief Populates the graph with vertices and edges for the flow network.
 * @param submissions Map of submission ID → Submission.
 * @param reviewers   Map of reviewer ID → Reviewer.
 * @complexity O(S × R)
 */

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

/**
 * @brief Checks whether a submission–reviewer pair is topic-compatible.
 * @param sub Submission to check.
 * @param rev Reviewer to check.
 * @return true if compatible, false otherwise.
 * @complexity O(1)
 */

bool FlowNetwork::topicsMatch(const Submission& sub, const Reviewer& rev) const
{
    switch (generateAssignments_)
    {
    case (0):
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

/**
 * @brief Returns the matched topic ID for a compatible pair, or -1 if none.
 * @param sub Submission.
 * @param rev Reviewer.
 * @return Matched topic ID.
 * @complexity O(1)
 */

int FlowNetwork::matchingTopic(const Submission& sub, const Reviewer& rev) const
{
    switch (generateAssignments_)
    {
    case (0):
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

/**
 * @brief Returns a reference to the flow network graph.
 * @return Reference to the internal Graph<NodeInfo>.
 */

Graph<NodeInfo>& FlowNetwork::getGraph()
{
    return graph_;
}

/**
 * @brief Returns the NodeInfo identifying the sink vertex.
 * @return NodeInfo with type SINK and id 0.
 */

NodeInfo FlowNetwork::getSink() const
{
    return {0, VertexType::SINK};
}

/**
 * @brief Returns the NodeInfo identifying the source vertex.
 * @return NodeInfo with type SOURCE and id 0.
 */

NodeInfo FlowNetwork::getSource() const
{
    return {0, VertexType::SOURCE};
}

/**
 * @brief Returns the problem parameters used to build this network.
 * @return Copy of the Parameters struct.
 */

Parameters FlowNetwork::getParameters() const
{
    return parameters_;
}
