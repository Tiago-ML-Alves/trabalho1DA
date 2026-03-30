/**
 * @file main.cpp
 * @brief Entry point for the Review Assignment Tool.
 *
 * Two modes:
 *  - Interactive: text menu.
 *  - Batch:       ./review_tool -b input.csv output.csv
 *
 * All computation lives in Parser, FlowNetwork, Scheduler, Output.
 * This file only wires them together.
 */

#include <iostream>
#include <iomanip>
#include <limits>
#include <memory>
#include <map>
#include <string>
#include <filesystem>

#include "Types.h"
#include "Parser.h"
#include "FlowNetwork.h"
#include "Scheduler.h"
#include "Output.h"

// ─── helpers ─────────────────────────────────────────────────────────────────

/**
 * @brief Truncate a string to @p max chars, appending "..." if cut.
 * @complexity O(n)
 */
static std::string truncate(const std::string& s, std::size_t max)
{
    if (s.size() <= max) return s;
    if (max <= 3) return s.substr(0, max);
    return s.substr(0, max - 3) + "...";
}

std::string filenameOnly(const std::string& path) {
    return std::filesystem::path(path).filename().string();
}

/** @brief Wait for the user to press Enter. */
static void pauseExecution()
{
    std::cout << "\n  Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}


// ─── display ─────────────────────────────────────────────────────────────────

/**
 * @brief Print loaded submissions, reviewers, parameters and control to stdout.
 * @complexity O(S + R)
 */
static void displayData(const std::map<int, Submission>& subs,
                        const std::map<int, Reviewer>&   revs,
                        const Parameters& p,
                        const Control&    c)
{
    std::cout << "\n  SUBMISSIONS (" << subs.size() << ")\n"
              << "" << std::string(54, '-') << "\n";
    for (const auto& kv : subs)
    {
        const Submission& s = kv.second;
        std::cout << "  " << std::setw(4)  << std::right << s.id
                  << "  " << std::setw(30) << std::left  << truncate(s.title, 30)
                  << "  topic " << s.primaryTopic;
        if (s.secondaryTopic != -1) std::cout << "/" << s.secondaryTopic;
        std::cout << "\n";
    }

    std::cout << "\n  REVIEWERS (" << revs.size() << ")\n"
              << "" << std::string(44, '-') << "\n";
    for (const auto& kv : revs)
    {
        const Reviewer& r = kv.second;
        std::cout << "  " << std::setw(4)  << std::right << r.id
                  << "  " << std::setw(22) << std::left  << truncate(r.name, 22)
                  << "  expertise " << r.primaryExpertise;
        if (r.secondaryExpertise != -1) std::cout << "/" << r.secondaryExpertise;
        std::cout << "\n";
    }

    std::cout << "\n  PARAMETERS\n"
              << "  minReviewsPerSubmission  " << p.minReviewsPerSubmission  << "\n"
              << "  maxReviewsPerReviewer    " << p.maxReviewsPerReviewer    << "\n"
              << "  CONTROL\n"
              << "  generateAssignments      " << c.generateAssignments << "\n"
              << "  riskAnalysis             " << c.riskAnalysis        << "\n"
              << "  outputFileName           " << c.outputFileName      << "\n";
}

/**
 * @brief Print assignment results to stdout.
 * @complexity O(A)
 */
static void displayResults(const Scheduler& sched)
{
    if (sched.wasSuccessful())
    {
        std::vector<Assignment> asgn = sched.getAssignments();
        std::cout << "\n  " << asgn.size() << " assignment(s) made.\n"
                  << "" << std::setw(12) << "Submission"
                  << "" << std::setw(12) << "Reviewer"
                  << "" << std::setw(12) << "Topic\n"
                  << "" << std::string(36, '-') << "\n";
        for (std::size_t i = 0; i < asgn.size(); ++i)
            std::cout << "" << std::setw(5) << asgn[i].submissionID
                      << "" << std::setw(15) << asgn[i].reviewerID
                      << "" << std::setw(13) << asgn[i].matchedTopic << "\n";
    }
    else
    {
        std::cout << "Assignment incomplete:\n";
        std::vector<FailedAssignment> missing = sched.getMissingReviews();
        for (std::size_t i = 0; i < missing.size(); ++i)
            std::cout << "Submission " << missing[i].id
                      << " missing "   << missing[i].missingReviews
                      << " review(s) (domain " << missing[i].primaryDomain << ")\n";
    }
}

// ─── batch mode ───────────────────────────────────────────────────────────────

/**
 * @brief Parse → assign → risk analysis → write. No user interaction.
 * @return true on success.
 * @complexity O(V·E²)
 */
static bool runBatch(const std::string& inputFile, const std::string& outputFile)
{
    std::cerr << "[batch] " << inputFile << " -> " << outputFile << "\n";

    std::map<int, Submission> subs;
    std::map<int, Reviewer>   revs;
    Parameters p = {};
    Control    c = {};
    c.outputFileName = outputFile;

    Parser::parse(inputFile, subs, revs, p, c);

    if (subs.empty())
    {
        std::cerr << "[batch] error: no submissions loaded\n";
        return false;
    }

    if (p.minReviewsPerSubmission == 0 || p.maxReviewsPerReviewer == 0)
    {
        std::cerr << "[batch] error: invalid parameters\n";
        return false;
    }

    c.outputFileName = outputFile; // CLI arg overrides value from file

    FlowNetwork net(subs, revs, p, c.generateAssignments);
    Scheduler   sched(net, subs, revs);

    if (c.riskAnalysis != 0)
        sched.runRiskAnalysis(c.riskAnalysis);

    Output::write(sched, c);
    std::cerr << "[batch] done.\n";
    return true;
}

// ─── interactive mode ─────────────────────────────────────────────────────────

/**
 * @brief Text-menu loop.
 *
 * Options:
 *  1  Load input file
 *  2  Display loaded data
 *  3  Run assignment
 *  4  Run risk analysis
 *  5  Save output
 *  0  Exit
 *
 * FlowNetwork must outlive Scheduler (Scheduler stores a reference to it),
 * so both are kept as unique_ptrs in the same scope and reset together.
 *
 * @complexity O(1) per iteration, excluding pipeline calls.
 */
static void runInteractive()
{
    std::map<int, Submission> subs;
    std::map<int, Reviewer> revs;
    Parameters p = {};
    Control c = {};
    c.outputFileName = "output.csv";
    bool loaded = false;
    std::string path;

    // Both reset together whenever a new file is loaded.
    std::unique_ptr<FlowNetwork> net;
    std::unique_ptr<Scheduler>   sched;

    int opt = -1;
    do
    {
        std::cout << "\n" << std::string(50, '-')
                  << "\n  REVIEW ASSIGNMENT TOOL"
                  << "\n  File: " << (loaded ? filenameOnly(path) : "(none)")
                  << "\n" << std::string(50, '-')
                  << "\n  1  Load input file"
                  << "\n  2  Display loaded data"
                  << "\n  3  Run assignment"
                  << "\n  4  Run risk analysis"
                  << "\n  5  Save output"
                  << "\n  0  Exit"
                  << "\n" << std::string(50, '-')
                  << "\n  Option: ";

        if (!(std::cin >> opt))
        {
            std::cin.clear();
            opt = -1;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (opt)
        {
        // 1 ── load ──────────────────────────────────────────────────────────
        case 1:
        {
            std::string file;
            std::cout << "  Input file name (Relative Path): ";
            std::getline(std::cin, file);
            path = file;


            std::map<int, Submission> ns;
            std::map<int, Reviewer> nr;
            Parameters np = {};
            Control nc = {};
            nc.outputFileName = "output.csv";

            Parser::parse(file, ns, nr, np, nc);

            if (ns.empty())
            {
                std::cerr << "  error: no submissions loaded\n";
                loaded = false;
                break;
            }

            subs = ns; revs = nr; p = np; c = nc;
            loaded = true;
            sched.reset();
            net.reset();

            std::cout << "  Loaded " << subs.size() << " submission(s), "
                      << revs.size() << " reviewer(s).\n";
            pauseExecution();
            break;
        }

        // 2 ── display ────────────────────────────────────────────────────────
        case 2:
            if (!loaded) std::cout << "  Load a file first (option 1).\n";
            else displayData(subs, revs, p, c);
            pauseExecution(); break;

        // 3 ── assign ─────────────────────────────────────────────────────────
        case 3:
            if (!loaded) std::cout << "  Load a file first (option 1).\n";
            else {
                net = std::unique_ptr<FlowNetwork>(new FlowNetwork(subs, revs, p, c.generateAssignments));
                sched = std::unique_ptr<Scheduler>(new Scheduler(*net, subs, revs));
                displayResults(*sched);
            }
            pauseExecution();
            break;


        // 4 ── risk analysis ──────────────────────────────────────────────────
        case 4:
            if (!sched)
            {
                std::cout << " Run the assignment first (option 3).\n";
                pauseExecution(); break;
            }
            std::cout << "  RiskAnalysis is " << c.riskAnalysis << " in the file.\n" << "" << std::string(54, '-') <<
                    "\n";

            sched->runRiskAnalysis(c.riskAnalysis);

            if (sched->getRiskyReviewers().empty()) std::cout << "  No risky reviewers found.\n";
            else
            {
                std::cout << "  Risky reviewer IDs: ";
                for (auto rev : sched->getRiskyReviewers())
                    std::cout << rev << " ";
                std::cout << "\n";
            }
            pauseExecution();
            break;

        // 5 ── save ───────────────────────────────────────────────────────────
        case 5:
        {
            if (!sched)
            {
                std::cout << "  Nothing to save yet — run assignment first.\n";
                pauseExecution(); break;
            }
            std::string fname;
            std::cout << "  Output folder (Relative Path): ";
            std::string oldname = c.outputFileName;
            std::getline(std::cin, fname);
            if (!fname.empty()) c.outputFileName = fname + c.outputFileName;
            Output::write(*sched, c);
            std::cout << "  Saved to \"" << c.outputFileName << "\"\n";
            c.outputFileName = oldname;
            pauseExecution();
            break;
        }

        case 0:
            std::cout << "  Goodbye!\n";
            break;

        default:
            std::cout << "   Invalid option.\n";
            pauseExecution();
        }
    }
    while (opt != 0);
}

// ─── entry point ──────────────────────────────────────────────────────────────

/**
 * @brief Program entry point.
 *
 * Usage:
 *  ./review_tool                        — interactive mode
 *  ./review_tool -b input.csv out.csv   — batch mode
 */

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        runInteractive();
        return 0;
    }
    if (argc >= 4 && std::string(argv[1]) == "-b")
    {
        return runBatch(argv[2], argv[3]) ? 0 : 1;
    }
    std::cerr << "Usage:\n"
              << "" << argv[0] << "\n"
              << "" << argv[0] << " -b input.csv output.csv\n";
    return 1;
}
