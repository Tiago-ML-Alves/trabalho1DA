#ifndef PROJECT1_TYPES_H
#define PROJECT1_TYPES_H

enum class VertexType;

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

struct NodeInfo
{
    int id;
    VertexType type;

    bool operator==(const NodeInfo& other) const
    {
        return id == other.id && type == other.type;
    }
};
enum class VertexType
{
    SOURCE,
    PAPER,
    REVIEWER,
    SINK
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