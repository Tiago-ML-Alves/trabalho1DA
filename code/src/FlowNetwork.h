//
// Created by alex on 3/20/26.
//

/**
 * @file FlowNetwork.h
 * @brief Declaration of the FlowNetwork class.
 */

#ifndef PROJECT1_FLOWNETWORK_H
#define PROJECT1_FLOWNETWORK_H
#include "Graph.h"
#include "Types.h"
#include <map>

/**
 * @class FlowNetwork
 * @brief Builds a Max-Flow network from submissions and reviewers.
 *
 * Network topology:
 *   SOURCE ──(minReviews)──► [Submission] ──(1)──► [Reviewer] ──(maxReviews)──► SINK
 *
 * @see EdmondsKarp.h
 * @see Scheduler
 */

class FlowNetwork
{
private:
    int generateAssignments_;
    Graph<NodeInfo> graph_;
    Parameters parameters_;
    bool topicsMatch(const Submission& sub, const Reviewer& rev) const;
public:
    FlowNetwork(const std::map<int, Submission>& submissions, const std::map<int, Reviewer>& reviewers, const Parameters& parameters, int generateAssignments);
    Graph<NodeInfo>& getGraph();
    NodeInfo getSource() const;
    int matchingTopic(const Submission& sub, const Reviewer& rev) const;
    void buildNetwork(const std::map<int, Submission>& submissions, const std::map<int, Reviewer>& reviewers);
    NodeInfo getSink() const;
    Parameters getParameters() const;
};

#endif //PROJECT1_FLOWNETWORK_H