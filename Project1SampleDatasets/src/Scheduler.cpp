#include "Scheduler.h"
#include "EdmondsKarp.h"

Scheduler::Scheduler(FlowNetwork& flowNetwork, const std::map<int, Submission>& submissions, const std::map<int, Reviewer>& reviewers)
	: flowNetwork_(flowNetwork), submissions_(submissions), reviewers_(reviewers), successful_(true)
{

	edmondsKarp(&flowNetwork.getGraph(), flowNetwork.getSource(), flowNetwork.getSink());
	extractAssignments();
}

void Scheduler::extractAssignments()
{
	Graph<NodeInfo>& graph = flowNetwork_.getGraph();
	int neededMinReviews = flowNetwork_.getParameters().minReviewsPerSubmission;
	for (Vertex<NodeInfo>* v : graph.getVertexSet())
	{
		if (v->getInfo().type == VertexType::SUBMISSION)
		{
			int reviewersAssigned = 0;
			int subID = v->getInfo().id;
			for (Edge<NodeInfo>* e : v->getAdj())
			{
				int revID = e->getDest()->getInfo().id;
				if (e->getDest()->getInfo().type == VertexType::REVIEWER && e->getFlow() == 1)
				{
					reviewersAssigned++;
					int matchingTopic = flowNetwork_.matchingTopic(submissions_.at(subID), reviewers_.at(revID));
					assignments_.push_back({subID, revID, matchingTopic});
				}
			}
			if (reviewersAssigned < neededMinReviews)
			{
				successful_ = false;
				missingReviews_.push_back({subID, submissions_.at(subID).primaryTopic, neededMinReviews - reviewersAssigned});
			}
		}
	}
}

std::vector<Assignment> Scheduler::getAssignments() const
{
	return assignments_;
}

bool Scheduler::wasSuccessful() const
{
	return successful_;
}

std::vector<FailedAssignment> Scheduler::getMissingReviews() const
{
	return missingReviews_;
}


