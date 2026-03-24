#ifndef PROJECT1_TYPES_H
#define PROJECT1_TYPES_H
#include <string>

enum class VertexType;

struct Assignment
{
    int submissionID;
    int reviewerID;
    int matchedTopic;
};
struct FailedAssignment
{
    int id;
    int primaryDomain;
    int missingReviews;
};
struct Submission
{
    int id;
    std::string title;
    std::string authorName;
    std::string email;
    int primaryTopic;
    int secondaryTopic;
};

struct Reviewer
{
    int id;
    std::string name;
    std::string email;
    int primaryExpertise;
    int secondaryExpertise;
};

struct Parameters
{
    int maxReviewsPerReviewer;
    int minReviewsPerSubmission;
    int primaryReviewerExpertise;
    int secondaryReviewerExpertise;
    int primarySubmissionDomain;
    int secondarySubmissionDomain;
};

struct Control
{
    int generateAssignments;
    int riskAnalysis;
    std::string outputFileName = "output.csv";
};

enum class VertexType
{
    SOURCE,
    SUBMISSION,
    REVIEWER,
    SINK
};
struct NodeInfo
{
    int id;
    VertexType type;

    bool operator==(const NodeInfo& other) const
    {
        return id == other.id && type == other.type;
    }
};

enum class Section
{
    NONE,
    SUBMISSIONS,
    REVIEWERS,
    PARAMETERS,
    CONTROL
};


#endif