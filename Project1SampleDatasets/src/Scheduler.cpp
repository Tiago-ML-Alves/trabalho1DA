#include "Scheduler.h"
#include "EdmondsKarp.h"

Scheduler::Scheduler(FlowNetwork& flowNetwork, const std::map<int, Submission>& submissions, const std::map<int, Reviewer>& reviewers)
	: flowNetwork_(flowNetwork), submissions_(submissions), reviewers_(reviewers), successful_(true)
{
	resetFlow(&flowNetwork.getGraph());
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
int Scheduler::countAssignedReviewers(Vertex<NodeInfo>* v) const
{
	int count = 0;
	for (Edge<NodeInfo>* e : v->getAdj())
		if (e->getDest()->getInfo().type == VertexType::REVIEWER && e->getFlow() == 1)
			count++;
	return count;
}

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
void Scheduler::runRiskAnalysis(int k)
{
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



std::vector<Assignment> Scheduler::getAssignments() const
{
	return assignments_;
}

std::set<int> Scheduler::getRiskyReviewers() const
{
	return riskyReviewers_;
}

bool Scheduler::wasSuccessful() const
{
	return successful_;
}

std::vector<FailedAssignment> Scheduler::getMissingReviews() const
{
	return missingReviews_;
}


