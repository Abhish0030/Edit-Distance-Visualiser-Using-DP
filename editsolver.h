#ifndef EDITSOLVER_H
#define EDITSOLVER_H

#include <string>
#include <vector>

class EditSolver {
public:
    enum class Decision {
        Base,
        Match,
        Replace,
        Insert,
        Delete
    };

    struct Operation {
        Decision type = Decision::Base;
        int position = 0;
        char fromChar = '\0';
        char toChar = '\0';
        int cost = 0;
        std::string description;
        std::string snapshot;
    };

    struct Suggestion {
        std::string word;
        int distance = 0;
    };

    struct CellTrace {
        int row = 0;
        int col = 0;
        int value = 0;
        Decision decision = Decision::Base;
    };

    EditSolver();

    void solve(const std::string& src, const std::string& tgt);
    std::vector<std::vector<int>> getDPTable() const;
    std::vector<std::vector<Decision>> getDecisionTable() const;
    std::vector<CellTrace> getFillTrace() const;
    std::vector<std::pair<int, int>> getBacktracePath() const;
    std::vector<Operation> getOperations() const;
    int getEditDistance() const;
    double getSimilarityPercent() const;
    std::vector<std::string> getSuggestions(const std::vector<std::string>& dict) const;
    std::vector<Suggestion> getSuggestionDetails(const std::vector<std::string>& dict) const;

private:
    static int computeDistance(const std::string& left, const std::string& right);
    void buildOperations();
    void buildBacktrace();

    std::string m_source;
    std::string m_target;
    std::vector<std::vector<int>> m_dp;
    std::vector<std::vector<Decision>> m_decisions;
    std::vector<CellTrace> m_fillTrace;
    std::vector<std::pair<int, int>> m_backtracePath;
    std::vector<Operation> m_operations;
};

#endif // EDITSOLVER_H
