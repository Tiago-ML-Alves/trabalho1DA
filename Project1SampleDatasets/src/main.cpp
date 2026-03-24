#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <unordered_set>

using namespace std;

// ── Data structures ──────────────────────────────────────────────────────────

struct Submission {
    int id;
    string title;
    string authors;
    string email;
    int primaryTopic;
    int secondaryTopic;   // 0 if absent
};

struct Reviewer {
    int id;
    string name;
    string email;
    int primaryExpertise;
    int secondaryExpertise;  // 0 if absent
};

struct Parameters {
    int minReviewsPerSubmission;
    int maxReviewsPerReviewer;
    int primaryReviewerExpertise;
    int secondaryReviewerExpertise;
    int primarySubmissionDomain;
    int secondarySubmissionDomain;
};

struct Control {
    int    generateAssignments;
    int    riskAnalysis;
    string outputFileName;
};

// ── Helpers ───────────────────────────────────────────────────────────────────

// Safely truncate a string for display, appending "..." only when actually cut.
static string truncate(const string& s, size_t maxLen) {
    if (s.size() <= maxLen) return s;
    if (maxLen <= 3)        return s.substr(0, maxLen);
    return s.substr(0, maxLen - 3) + "...";
}

// Try to parse an int; throw a descriptive exception on failure.
static int safeStoi(const string& s, const string& context) {
    if (s.empty())
        throw invalid_argument("Empty value for field: " + context);
    try {
        size_t pos;
        int v = stoi(s, &pos);
        if (pos != s.size())
            throw invalid_argument("Non-numeric characters in field: " + context);
        return v;
    } catch (const invalid_argument&) {
        throw invalid_argument("Invalid integer for field: " + context + " (got \"" + s + "\")");
    } catch (const out_of_range&) {
        throw out_of_range("Integer out of range for field: " + context + " (got \"" + s + "\")");
    }
}

// ── Main class ────────────────────────────────────────────────────────────────

class ReviewAssignmentTool {
private:
    vector<Submission> submissions;
    vector<Reviewer>   reviewers;
    Parameters         params;
    Control            control;
    string             currentInputFile;
    bool               dataLoaded;

    // ── UI helpers ────────────────────────────────────────────────────────────

    void printHeader() const {
        cout << "\n" << string(60, '=') << "\n"
             << "     SCIENTIFIC CONFERENCE REVIEW ASSIGNMENT TOOL\n"
             << string(60, '=') << "\n";
    }

    void printMenu() const {
        cout << "\nMAIN MENU:\n" << string(40, '-') << "\n"
             << "1. Load input file\n"
             << "2. Display loaded data\n"
             << "3. Run review assignment\n"
             << "4. Run risk analysis\n"
             << "5. Configure parameters\n"
             << "6. Save output\n"
             << "7. Run batch mode\n"
             << "0. Exit\n"
             << string(40, '-') << "\n"
             << "Option: ";
    }

    void waitForKeyPress() {
        cout << "\nPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    // ── Parsing ───────────────────────────────────────────────────────────────

/*
    bool parseSubmissionLine(const string& line) {
        stringstream ss(line);
        string field;
        Submission sub;

        if (!getline(ss, field, ','))
            throw invalid_argument("Missing submission ID");
        sub.id = safeStoi(field, "submission ID");

        if (!getline(ss, field, ','))
            throw invalid_argument("Missing submission title");
        sub.title = field;

        if (!getline(ss, field, ','))
            throw invalid_argument("Missing submission authors");
        sub.authors = field;

        if (!getline(ss, field, ','))
            throw invalid_argument("Missing submission email");
        sub.email = field;

        if (!getline(ss, field, ','))
            throw invalid_argument("Missing submission primary topic");
        sub.primaryTopic = safeStoi(field, "submission primary topic");

        // Secondary topic is optional
        sub.secondaryTopic = 0;
        if (getline(ss, field, ',') && !field.empty())
            sub.secondaryTopic = safeStoi(field, "submission secondary topic");

        submissions.push_back(sub);
        return true;
    }

    bool parseReviewerLine(const string& line) {
        stringstream ss(line);
        string field;
        Reviewer rev;

        if (!getline(ss, field, ','))
            throw invalid_argument("Missing reviewer ID");
        rev.id = safeStoi(field, "reviewer ID");

        if (!getline(ss, field, ','))
            throw invalid_argument("Missing reviewer name");
        rev.name = field;

        if (!getline(ss, field, ','))
            throw invalid_argument("Missing reviewer email");
        rev.email = field;

        if (!getline(ss, field, ','))
            throw invalid_argument("Missing reviewer primary expertise");
        rev.primaryExpertise = safeStoi(field, "reviewer primary expertise");

        // Secondary expertise is optional
        rev.secondaryExpertise = 0;
        if (getline(ss, field, ',') && !field.empty())
            rev.secondaryExpertise = safeStoi(field, "reviewer secondary expertise");

        reviewers.push_back(rev);
        return true;
    }



    bool parseParametersLine(const string& line) {
        stringstream ss(line);
        string param, value;

        if (!getline(ss, param, ',') || !getline(ss, value, ','))
            throw invalid_argument("Malformed parameter line: \"" + line + "\"");

        if      (param == "MinReviewsPerSubmission")   params.minReviewsPerSubmission   = safeStoi(value, param);
        else if (param == "MaxReviewsPerReviewer")     params.maxReviewsPerReviewer     = safeStoi(value, param);
        else if (param == "PrimaryReviewerExpertise")  params.primaryReviewerExpertise  = safeStoi(value, param);
        else if (param == "SecondaryReviewerExpertise")params.secondaryReviewerExpertise= safeStoi(value, param);
        else if (param == "PrimarySubmissionDomain")   params.primarySubmissionDomain   = safeStoi(value, param);
        else if (param == "SecondarySubmissionDomain") params.secondarySubmissionDomain = safeStoi(value, param);
        else
            cerr << "WARNING: Unknown parameter \"" << param << "\" – ignored.\n";

        return true;
    }

    bool parseControlLine(const string& line) {
        stringstream ss(line);
        string param, value;

        if (!getline(ss, param, ',') || !getline(ss, value, ','))
            throw invalid_argument("Malformed control line: \"" + line + "\"");

        if (param == "GenerateAssignments") {
            control.generateAssignments = safeStoi(value, param);
        } else if (param == "RiskAnalysis") {
            control.riskAnalysis = safeStoi(value, param);
        } else if (param == "OutputFileName") {
            value.erase(remove(value.begin(), value.end(), '"'), value.end());
            control.outputFileName = value;
        } else {
            cerr << "WARNING: Unknown control key \"" << param << "\" – ignored.\n";
        }

        return true;
    }
*/
    // ── Validation ────────────────────────────────────────────────────────────

    bool validateData() const {
        bool valid = true;

        // Duplicate submission IDs
        unordered_set<int> subIds;
        for (const auto& s : submissions) {
            if (s.id <= 0) {
                cerr << "ERROR: Submission ID must be positive (got " << s.id << ")\n";
                valid = false;
            }
            if (!subIds.insert(s.id).second) {
                cerr << "ERROR: Duplicate submission ID: " << s.id << "\n";
                valid = false;
            }
        }

        // Duplicate reviewer IDs
        unordered_set<int> revIds;
        for (const auto& r : reviewers) {
            if (r.id <= 0) {
                cerr << "ERROR: Reviewer ID must be positive (got " << r.id << ")\n";
                valid = false;
            }
            if (!revIds.insert(r.id).second) {
                cerr << "ERROR: Duplicate reviewer ID: " << r.id << "\n";
                valid = false;
            }
        }

        if (params.minReviewsPerSubmission <= 0) {
            cerr << "ERROR: MinReviewsPerSubmission must be positive\n";
            valid = false;
        }
        if (params.maxReviewsPerReviewer <= 0) {
            cerr << "ERROR: MaxReviewsPerReviewer must be positive\n";
            valid = false;
        }

        return valid;
    }

public:
    // ── Constructor ───────────────────────────────────────────────────────────

    ReviewAssignmentTool() : dataLoaded(false) {
        params.minReviewsPerSubmission  = 3;
        params.maxReviewsPerReviewer    = 5;
        params.primaryReviewerExpertise = 1;
        params.secondaryReviewerExpertise = 0;
        params.primarySubmissionDomain  = 1;
        params.secondarySubmissionDomain = 1;

        control.generateAssignments = 0;
        control.riskAnalysis        = 0;
        control.outputFileName      = "output.csv";
    }

    // ── File loading ──────────────────────────────────────────────────────────

    bool readInputFile(const string& filename) {
        // Clear any previously loaded data before reading a new file
        submissions.clear();
        reviewers.clear();

        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "ERROR: Could not open file: " << filename << "\n";
            return false;
        }

        cout << "Loading file: " << filename << " ...\n";

        string line, section;
        int  lineNum   = 0;
        bool hasErrors = false;

        while (getline(file, line)) {
            ++lineNum;
            if (line.empty()) continue;

            if (line[0] == '#') {
                section = line;
                continue;
            }

            try {
                if      (section.find("#Submissions") != string::npos) { if (!parseSubmissionLine(line))  hasErrors = true; }
                else if (section.find("#Reviewers")   != string::npos) { if (!parseReviewerLine(line))    hasErrors = true; }
                else if (section.find("#Parameters")  != string::npos) { if (!parseParametersLine(line))  hasErrors = true; }
                else if (section.find("#Control")     != string::npos) { if (!parseControlLine(line))     hasErrors = true; }
            } catch (const exception& e) {
                cerr << "ERROR at line " << lineNum << ": " << e.what() << "\n";
                hasErrors = true;
            }
        }

        file.close();

        if (!hasErrors && validateData()) {
            dataLoaded       = true;
            currentInputFile = filename;
            cout << "File loaded successfully!\n"
                 << "  - " << submissions.size() << " submissions\n"
                 << "  - " << reviewers.size()   << " reviewers\n";
            return true;
        }

        cerr << "ERROR: File contains invalid data\n";
        dataLoaded = false;
        return false;
    }

    // ── Display ───────────────────────────────────────────────────────────────

    void displayData() const {
        if (!dataLoaded) {
            cout << "No data loaded. Please load a file first.\n";
            return;
        }

        cout << "\nCURRENT DATA:\n" << string(40, '-') << "\n";

        // Submissions
        cout << "\nSUBMISSIONS (" << submissions.size() << "):\n"
             << setw(5) << "ID" << " | " << setw(30) << left << "Title" << " | Topics\n"
             << string(50, '-') << "\n";
        for (const auto& sub : submissions) {
            cout << setw(5) << right << sub.id << " | "
                 << setw(30) << left << truncate(sub.title, 30) << " | "
                 << sub.primaryTopic;
            if (sub.secondaryTopic > 0) cout << ", " << sub.secondaryTopic;
            cout << "\n";
        }

        // Reviewers
        cout << "\nREVIEWERS (" << reviewers.size() << "):\n"
             << setw(5) << "ID" << " | " << setw(20) << left << "Name" << " | Expertise\n"
             << string(40, '-') << "\n";
        for (const auto& rev : reviewers) {
            cout << setw(5) << right << rev.id << " | "
                 << setw(20) << left << truncate(rev.name, 20) << " | "
                 << rev.primaryExpertise;
            if (rev.secondaryExpertise > 0) cout << ", " << rev.secondaryExpertise;
            cout << "\n";
        }

        // Parameters
        cout << "\nPARAMETERS:\n"
             << "  MinReviewsPerSubmission:   " << params.minReviewsPerSubmission   << "\n"
             << "  MaxReviewsPerReviewer:     " << params.maxReviewsPerReviewer     << "\n"
             << "  PrimaryReviewerExpertise:  " << params.primaryReviewerExpertise  << "\n"
             << "  SecondaryReviewerExpertise:" << params.secondaryReviewerExpertise << "\n"
             << "  PrimarySubmissionDomain:   " << params.primarySubmissionDomain   << "\n"
             << "  SecondarySubmissionDomain: " << params.secondarySubmissionDomain  << "\n";

        // Control
        cout << "\nCONTROL SETTINGS:\n"
             << "  GenerateAssignments: " << control.generateAssignments << "\n"
             << "  RiskAnalysis:        " << control.riskAnalysis        << "\n"
             << "  OutputFileName:      " << control.outputFileName      << "\n";
    }

    // ── Parameter configuration ───────────────────────────────────────────────

    void configureParameters() {
        cout << "\nCONFIGURE PARAMETERS:\n" << string(40, '-') << "\n";

        auto promptInt = [](const string& prompt, int current) -> int {
            cout << prompt << " [" << current << "]: ";
            string input;
            getline(cin, input);
            if (input.empty()) return current;
            try {
                return safeStoi(input, prompt);
            } catch (const exception& e) {
                cerr << "WARNING: " << e.what() << " – keeping current value.\n";
                return current;
            }
        };

        params.minReviewsPerSubmission  = promptInt("MinReviewsPerSubmission",  params.minReviewsPerSubmission);
        params.maxReviewsPerReviewer    = promptInt("MaxReviewsPerReviewer",    params.maxReviewsPerReviewer);
        control.generateAssignments     = promptInt("GenerateAssignments (0/1/2/3)", control.generateAssignments);
        control.riskAnalysis            = promptInt("RiskAnalysis (0 or K)",    control.riskAnalysis);

        cout << "Parameters updated!\n";
    }

    // ── Batch mode ────────────────────────────────────────────────────────────

    bool runBatchMode(const string& inputFile, const string& outputFile) {
        cout << "Running in batch mode...\n"
             << "Input file:  " << inputFile  << "\n"
             << "Output file: " << outputFile << "\n";

        if (!readInputFile(inputFile)) {
            cerr << "Batch mode failed: could not load input file\n";
            return false;
        }

        // TODO: implement assignment logic (T2.x)
        cout << "Assignment logic will be implemented in T2 tasks\n"
             << "Output would be written to: " << outputFile << "\n";

        return true;
    }

    // ── Interactive loop ──────────────────────────────────────────────────────

    void runInteractive() {
        int option;
        do {
            printHeader();
            cout << "Current file: " << (dataLoaded ? currentInputFile : "None") << "\n";
            printMenu();

            if (!(cin >> option)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                option = -1;
            } else {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }

            switch (option) {
                case 1: {
                    string filename;
                    cout << "Enter input filename: ";
                    getline(cin, filename);
                    readInputFile(filename);
                    waitForKeyPress();
                    break;
                }
                case 2:
                    displayData();
                    waitForKeyPress();
                    break;
                case 3:
                    if (dataLoaded) {
                        cout << "\nRunning review assignment (Mode " << control.generateAssignments << ")...\n";
                        // TODO: implement T2.1, T2.2, T2.4
                        cout << "Assignment logic will be implemented in T2 tasks\n";
                    } else {
                        cout << "Please load data first.\n";
                    }
                    waitForKeyPress();
                    break;
                case 4:
                    if (dataLoaded && control.riskAnalysis > 0) {
                        cout << "\nRunning risk analysis (K=" << control.riskAnalysis << ")...\n";
                        // TODO: implement T2.2, T2.3
                        cout << "Risk analysis will be implemented in T2 tasks\n";
                    } else {
                        cout << "Please load data and set RiskAnalysis > 0 first.\n";
                    }
                    waitForKeyPress();
                    break;
                case 5:
                    configureParameters();
                    waitForKeyPress();
                    break;
                case 6:
                    if (dataLoaded) {
                        cout << "\nSaving output to: " << control.outputFileName << "\n";
                        // TODO: implement output writing
                        cout << "Output saving will be implemented in T2 tasks\n";
                    } else {
                        cout << "No data to save.\n";
                    }
                    waitForKeyPress();
                    break;
                case 7: {
                    string infile, outfile;
                    cout << "Enter input file for batch mode: ";
                    getline(cin, infile);
                    cout << "Enter output file: ";
                    getline(cin, outfile);
                    runBatchMode(infile, outfile);
                    waitForKeyPress();
                    break;
                }
                case 0:
                    cout << "Exiting... Goodbye!\n";
                    break;
                default:
                    cout << "Invalid option!\n";
                    waitForKeyPress();
            }
        } while (option != 0);
    }
};

// ── Entry point ───────────────────────────────────────────────────────────────

int main(int argc, char* argv[]) {
    ReviewAssignmentTool tool;

    if (argc >= 4 && string(argv[1]) == "-b") {
        cout << "Starting in BATCH mode\n";
        return tool.runBatchMode(argv[2], argv[3]) ? 0 : 1;
    } else if (argc > 1) {
        cerr << "Usage:\n"
             << "  Interactive: " << argv[0] << "\n"
             << "  Batch:       " << argv[0] << " -b input.csv output.csv\n";
        return 1;
    } else {
        cout << "Starting in INTERACTIVE mode\n";
        tool.runInteractive();
    }

    return 0;
}