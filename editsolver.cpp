#include "editsolver.h"

#include <algorithm>
#include <sstream>

EditSolver::EditSolver() = default;

void EditSolver::solve(const std::string& src, const std::string& tgt)
{
    m_source = src;
    m_target = tgt;

    const int rows = static_cast<int>(m_source.size()) + 1;
    const int cols = static_cast<int>(m_target.size()) + 1;

    m_dp.assign(rows, std::vector<int>(cols, 0));
    m_decisions.assign(rows, std::vector<Decision>(cols, Decision::Base));
    m_fillTrace.clear();
    m_backtracePath.clear();
    m_operations.clear();

    for (int i = 0; i < rows; ++i) {
        m_dp[i][0] = i;
        m_decisions[i][0] = Decision::Base;
        m_fillTrace.push_back({i, 0, m_dp[i][0], Decision::Base});
    }

    for (int j = 1; j < cols; ++j) {
        m_dp[0][j] = j;
        m_decisions[0][j] = Decision::Base;
        m_fillTrace.push_back({0, j, m_dp[0][j], Decision::Base});
    }

    for (int i = 1; i < rows; ++i) {
        for (int j = 1; j < cols; ++j) {
            // DP step 1: when the current characters already match,
            // the best answer is the diagonal value with zero extra cost.
            if (m_source[static_cast<std::size_t>(i - 1)] == m_target[static_cast<std::size_t>(j - 1)]) {
                m_dp[i][j] = m_dp[i - 1][j - 1];
                m_decisions[i][j] = Decision::Match;
            } else {
                // DP step 2: delete the current source character and
                // reuse the best answer from the cell above.
                const int deleteCost = m_dp[i - 1][j] + 1;

                // DP step 3: insert the current target character and
                // reuse the best answer from the cell to the left.
                const int insertCost = m_dp[i][j - 1] + 1;

                // DP step 4: replace the current source character with
                // the current target character and reuse the diagonal cell.
                const int replaceCost = m_dp[i - 1][j - 1] + 1;

                // DP step 5: choose the cheapest edit operation.
                int bestCost = replaceCost;
                Decision bestDecision = Decision::Replace;

                if (insertCost < bestCost) {
                    bestCost = insertCost;
                    bestDecision = Decision::Insert;
                }

                if (deleteCost < bestCost) {
                    bestCost = deleteCost;
                    bestDecision = Decision::Delete;
                }

                m_dp[i][j] = bestCost;
                m_decisions[i][j] = bestDecision;
            }

            m_fillTrace.push_back({i, j, m_dp[i][j], m_decisions[i][j]});
        }
    }

    buildBacktrace();
    buildOperations();
}

std::vector<std::vector<int>> EditSolver::getDPTable() const
{
    return m_dp;
}

std::vector<std::vector<EditSolver::Decision>> EditSolver::getDecisionTable() const
{
    return m_decisions;
}

std::vector<EditSolver::CellTrace> EditSolver::getFillTrace() const
{
    return m_fillTrace;
}

std::vector<std::pair<int, int>> EditSolver::getBacktracePath() const
{
    return m_backtracePath;
}

std::vector<EditSolver::Operation> EditSolver::getOperations() const
{
    return m_operations;
}

int EditSolver::getEditDistance() const
{
    if (m_dp.empty() || m_dp.back().empty()) {
        return 0;
    }

    return m_dp.back().back();
}

double EditSolver::getSimilarityPercent() const
{
    const int maxLength = static_cast<int>(std::max(m_source.size(), m_target.size()));
    if (maxLength == 0) {
        return 100.0;
    }

    const double score = static_cast<double>(maxLength - getEditDistance()) / static_cast<double>(maxLength);
    return std::max(0.0, score * 100.0);
}

std::vector<std::string> EditSolver::getSuggestions(const std::vector<std::string>& dict) const
{
    std::vector<std::string> words;
    const auto suggestions = getSuggestionDetails(dict);
    for (const Suggestion& item : suggestions) {
        words.push_back(item.word);
    }
    return words;
}

std::vector<EditSolver::Suggestion> EditSolver::getSuggestionDetails(const std::vector<std::string>& dict) const
{
    std::vector<Suggestion> ranked;
    ranked.reserve(dict.size());

    for (const std::string& word : dict) {
        ranked.push_back({word, computeDistance(m_source, word)});
    }

    std::stable_sort(ranked.begin(), ranked.end(), [](const Suggestion& left, const Suggestion& right) {
        if (left.distance != right.distance) {
            return left.distance < right.distance;
        }
        return left.word < right.word;
    });

    if (ranked.size() > 3) {
        ranked.resize(3);
    }

    return ranked;
}

int EditSolver::computeDistance(const std::string& left, const std::string& right)
{
    const int rows = static_cast<int>(left.size()) + 1;
    const int cols = static_cast<int>(right.size()) + 1;
    std::vector<std::vector<int>> local(rows, std::vector<int>(cols, 0));

    for (int i = 0; i < rows; ++i) {
        local[i][0] = i;
    }

    for (int j = 0; j < cols; ++j) {
        local[0][j] = j;
    }

    for (int i = 1; i < rows; ++i) {
        for (int j = 1; j < cols; ++j) {
            if (left[static_cast<std::size_t>(i - 1)] == right[static_cast<std::size_t>(j - 1)]) {
                local[i][j] = local[i - 1][j - 1];
            } else {
                local[i][j] = 1 + std::min({local[i - 1][j], local[i][j - 1], local[i - 1][j - 1]});
            }
        }
    }

    return local.back().back();
}

void EditSolver::buildBacktrace()
{
    int i = static_cast<int>(m_source.size());
    int j = static_cast<int>(m_target.size());

    m_backtracePath.push_back({i, j});

    while (i > 0 || j > 0) {
        if (i == 0) {
            --j;
        } else if (j == 0) {
            --i;
        } else {
            const Decision decision = m_decisions[i][j];
            if (decision == Decision::Match || decision == Decision::Replace) {
                --i;
                --j;
            } else if (decision == Decision::Insert) {
                --j;
            } else {
                --i;
            }
        }

        m_backtracePath.push_back({i, j});
    }

    std::reverse(m_backtracePath.begin(), m_backtracePath.end());
}

void EditSolver::buildOperations()
{
    struct PathStep {
        Decision type = Decision::Base;
        char fromChar = '\0';
        char toChar = '\0';
    };

    std::vector<PathStep> steps;

    for (std::size_t index = 1; index < m_backtracePath.size(); ++index) {
        const auto previous = m_backtracePath[index - 1];
        const auto current = m_backtracePath[index];
        const int i0 = previous.first;
        const int j0 = previous.second;
        const int i1 = current.first;
        const int j1 = current.second;

        if (i1 == i0 + 1 && j1 == j0 + 1) {
            if (m_source[static_cast<std::size_t>(i0)] == m_target[static_cast<std::size_t>(j0)]) {
                steps.push_back({Decision::Match, m_source[static_cast<std::size_t>(i0)], m_target[static_cast<std::size_t>(j0)]});
            } else {
                steps.push_back({Decision::Replace, m_source[static_cast<std::size_t>(i0)], m_target[static_cast<std::size_t>(j0)]});
            }
        } else if (i1 == i0 && j1 == j0 + 1) {
            steps.push_back({Decision::Insert, '\0', m_target[static_cast<std::size_t>(j0)]});
        } else if (i1 == i0 + 1 && j1 == j0) {
            steps.push_back({Decision::Delete, m_source[static_cast<std::size_t>(i0)], '\0'});
        }
    }

    std::string working = m_source;
    int cursor = 0;
    int stepNumber = 1;

    for (const PathStep& step : steps) {
        if (step.type == Decision::Match) {
            ++cursor;
            continue;
        }

        Operation op;
        op.type = step.type;
        op.position = cursor;
        op.fromChar = step.fromChar;
        op.toChar = step.toChar;
        op.cost = 1;

        std::ostringstream stream;
        if (step.type == Decision::Replace) {
            if (cursor >= 0 && cursor < static_cast<int>(working.size())) {
                working[static_cast<std::size_t>(cursor)] = step.toChar;
            }
            stream << "Step " << stepNumber << ": Replace '" << step.fromChar
                   << "' with '" << step.toChar << "' at position " << (cursor + 1)
                   << " (cost +1)";
            ++cursor;
        } else if (step.type == Decision::Insert) {
            working.insert(working.begin() + cursor, step.toChar);
            stream << "Step " << stepNumber << ": Insert '" << step.toChar
                   << "' at position " << (cursor + 1) << " (cost +1)";
            ++cursor;
        } else if (step.type == Decision::Delete) {
            if (cursor >= 0 && cursor < static_cast<int>(working.size())) {
                working.erase(working.begin() + cursor);
            }
            stream << "Step " << stepNumber << ": Delete '" << step.fromChar
                   << "' at position " << (cursor + 1) << " (cost +1)";
        }

        op.description = stream.str();
        op.snapshot = working;
        m_operations.push_back(op);
        ++stepNumber;
    }
}
