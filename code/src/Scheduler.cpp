/**
* @file Scheduler.cpp
 * @brief Implementation of the Scheduler class.
 */

#include "Scheduler.h"
#include "EdmondsKarp.h"

/**
 * @brief Constructs the Scheduler, runs Max-Flow and extracts assignments.
 * @param flowNetwork Reference to the pre-built FlowNetwork (must outlive this object).
 * @param submissions Map of submission ID → Submission.
 * @param reviewers   Map of reviewer ID → Reviewer.
 * @complexity O(V·E²) — dominated by Edmonds–Karp.
 */

Scheduler::Scheduler(FlowNetwork& flowNetwork, const std::map<int, Submission>& submissions, const std::map<int, Reviewer>& reviewers)
	: flowNetwork_(flowNetwork), submissions_(submissions), reviewers_(reviewers), successful_(true)
{
	resetFlow(&flowNetwork.getGraph());
	edmondsKarp(&flowNetwork.getGraph(), flowNetwork.getSource(), flowNetwork.getSink());
	extractAssignments();
}

/**
 * @brief Traverses the flow graph and populates assignments_ and missingReviews_.
 * @complexity O(S · R)
 */

void Scheduler::extractAssignments()
{
	Graph<NodeInfo>& graph = flowNetwork_.getGraph();
	int neededMinReviews = flowNetwork_.getParameters().minReviewsPerSubmission;
	for (Vertex<NodeInfo>* v : graph.getVertexSet())
	{
		if (v->getInfo().type == VertexType::SUBMISSION)
		{
			int assignedReviewers = countAssignedReviewers(v);
			int subID = v->getInfo().id;
			for (Edge<NodeInfo>* e : v->getAdj())
			{
				int revID = e->getDest()->getInfo().id;
				if (e->getDest()->getInfo().type == VertexType::REVIEWER && e->getFlow() == 1)
				{
					int matchingTopic = flowNetwork_.matchingTopic(submissions_.at(subID), reviewers_.at(revID));
					assignments_.push_back({subID, revID, matchingTopic});
				}
			}
			if (assignedReviewers < neededMinReviews)
			{
				missingReviews_.push_back({subID, submissions_.at(subID).primaryTopic, neededMinReviews - assignedReviewers});
			}
		}
	}
	successful_ = checkSuccessful();
}

/**
 * @brief Counts how many reviewers were assigned to vertex @p v.
 * @param v Submission vertex.
 * @return Number of edges to REVIEWER vertices with flow == 1.
 * @complexity O(deg(v))
 */

int Scheduler::countAssignedReviewers(Vertex<NodeInfo>* v) const
{
	int count = 0;
	for (Edge<NodeInfo>* e : v->getAdj())
		if (e->getDest()->getInfo().type == VertexType::REVIEWER && e->getFlow() == 1)
			count++;
	return count;
}

/**
 * @brief Checks whether every submission has at least minReviewsPerSubmission assigned reviewers.
 * @return true if all submissions are fully covered, false otherwise.
 * @complexity O(S · R)
 */

bool Scheduler::checkSuccessful()
{
	Graph<NodeInfo>& graph = flowNetwork_.getGraph();
	int neededMinReviews = flowNetwork_.getParameters().minReviewsPerSubmission;
	for (Vertex<NodeInfo>* v : graph.getVertexSet())
	{
		if (v->getInfo().type == VertexType::SUBMISSION)
		{
			if (countAssignedReviewers(v) < neededMinReviews)
			{
				return false;
			}
		}
	}
	return true;
}

/**
 * @brief Simulates reviewer removals to identify critical reviewers.
 * @param k Risk level: 0 = disabled; 1 = single-reviewer removal analysis.
 * @complexity O(R·V·E²) for k == 1.
 */

void Scheduler::runRiskAnalysis(int k){

	if (k == 0) return;
	if (k == 1)
	{
		Graph<NodeInfo>& graph = flowNetwork_.getGraph();
		for (Vertex<NodeInfo>* v : graph.getVertexSet())
		{
			if (v->getInfo().type == VertexType::REVIEWER)
			{
				Edge<NodeInfo>* reviewerToSink = nullptr;
				for (Edge<NodeInfo>* e : v->getAdj())
				{
					if (e->getDest()->getInfo().type == VertexType::SINK)
					{
						reviewerToSink = e;
						break;
					}
				}
				if (reviewerToSink == nullptr) continue;
				double originalWeight = reviewerToSink->getWeight();
				resetFlow(&graph);
				reviewerToSink->setFlow(originalWeight);
				edmondsKarp(&graph, flowNetwork_.getSource(), flowNetwork_.getSink());
				if (!checkSuccessful())
				{
					riskyReviewers_.insert(v->getInfo().id);
				}
				reviewerToSink->setFlow(0);
			}
		}
		resetFlow(&graph);
		edmondsKarp(&graph, flowNetwork_.getSource(), flowNetwork_.getSink());
	}
}

/**
 * @brief Returns the list of successful reviewer–submission assignments.
 * @return Vector of Assignment structs.
 */

std::vector<Assignment> Scheduler::getAssignments() const
{
	return assignments_;
}

/**
 * @brief Returns the set of reviewer IDs whose removal causes assignment failure.
 * @return Set of risky reviewer IDs.
 */

std::set<int> Scheduler::getRiskyReviewers() const
{
	return riskyReviewers_;
}

/**
 * @brief Returns whether all submissions were fully assigned.
 * @return true if successful, false if any submission is under-reviewed.
 */

bool Scheduler::wasSuccessful() const
{
	return successful_;
}

/**
 * @brief Returns the list of submissions that could not be fully assigned.
 * @return Vector of FailedAssignment structs.
 */

std::vector<FailedAssignment> Scheduler::getMissingReviews() const
{
	return missingReviews_;
}


