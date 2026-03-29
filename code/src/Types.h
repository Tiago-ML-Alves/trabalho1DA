/**
* @file Types.h
 * @brief Shared data types, structs and enumerations used across the project.
 */

#ifndef PROJECT1_TYPES_H
#define PROJECT1_TYPES_H
#include <string>

enum class VertexType;

/**
 * @struct Assignment
 * @brief Represents a successful reviewer-to-submission assignment.
 */

struct Assignment
{
    int submissionID;
    int reviewerID;
    int matchedTopic;
};

/**
 * @struct FailedAssignment
 * @brief Represents a submission that could not be fully assigned.
 *
 * Stores the submission ID, its primary domain, and how many reviews are still missing.
 */

struct FailedAssignment
{
    int id;
    int primaryDomain;
    int missingReviews;
};

/**
 * @struct Submission
 * @brief A scientific paper submitted to the conference.
 *
 * Each submission has a unique numeric ID, metadata (title, authors, e-mail)
 * and one mandatory primary topic plus an optional secondary topic (-1 if absent).
 */

struct Submission
{
    int id;
    std::string title;
    std::string authorName;
    std::string email;
    int primaryTopic;
    int secondaryTopic;
};

/**
 * @struct Reviewer
 * @brief A conference reviewer with one or two areas of expertise.
 * @note secondaryExpertise is -1 when the reviewer has only one area.
 */

struct Reviewer
{
    int id;
    std::string name;
    std::string email;
    int primaryExpertise;
    int secondaryExpertise;
};

/**
 * @struct Parameters
 * @brief Numeric parameters read from the #Parameters section of the input CSV.
 */

struct Parameters
{
    int maxReviewsPerReviewer;
    int minReviewsPerSubmission;
    int primaryReviewerExpertise;
    int secondaryReviewerExpertise;
    int primarySubmissionDomain;
    int secondarySubmissionDomain;
};

/**
 * @struct Control
 * @brief Runtime control flags read from the #Control section of the input CSV.
 */

struct Control
{
    int generateAssignments;
    int riskAnalysis;
    std::string outputFileName = "output.csv";
};

/**
 * @enum VertexType
 * @brief Identifies the role of a vertex inside the flow network.
 */

enum class VertexType
{
    SOURCE,
    SUBMISSION,
    REVIEWER,
    SINK
};

/**
 * @struct NodeInfo
 * @brief Information stored at each vertex of the flow-network graph.
 */

struct NodeInfo
{
    int id;
    VertexType type;

    bool operator==(const NodeInfo& other) const
    {
        return id == other.id && type == other.type;
    }
};

/**
 * @enum Section
 * @brief Tracks which CSV section the parser is currently reading.
 */

enum class Section
{
    NONE,
    SUBMISSIONS,
    REVIEWERS,
    PARAMETERS,
    CONTROL
};


#endif