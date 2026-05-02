#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "editsolver.h"

#include <QMainWindow>

#include <vector>

class QButtonGroup;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSlider;
class QTableWidget;
class QTextEdit;
class QTimer;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onSolveClicked();
    void onAnimationStep();
    void onPresetChanged(int index);
    void onSpeedChanged(int value);

private:
    enum class AppMode {
        SpellCheck,
        DNAAnalysis,
        CodeDiff
    };

    void buildUi();
    void buildInputPanel();
    void buildDPPanel();
    void buildOperationsPanel();
    void buildSuggestionsPanel();
    void applyDarkTheme();
    void updateModeUi();
    void resetViews();
    void prepareTable();
    void populateBaseHeaders();
    void colorCell(int row, int col, const QColor& color);
    void setCellText(int row, int col, int value);
    void displaySummary();
    void displayBacktrace();
    void displayOperations();
    void displaySuggestions();
    AppMode currentMode() const;
    QString decisionLabel(EditSolver::Decision decision) const;
    QColor decisionColor(EditSolver::Decision decision) const;
    std::vector<std::string> buildDictionary() const;

    QWidget* m_centralWidget = nullptr;
    QGroupBox* m_inputGroup = nullptr;
    QGroupBox* m_dpGroup = nullptr;
    QGroupBox* m_operationsGroup = nullptr;
    QGroupBox* m_suggestionsGroup = nullptr;

    QLineEdit* m_sourceEdit = nullptr;
    QLineEdit* m_targetEdit = nullptr;
    QComboBox* m_presetCombo = nullptr;
    QButtonGroup* m_modeGroup = nullptr;
    QPushButton* m_solveButton = nullptr;
    QSlider* m_speedSlider = nullptr;
    QLabel* m_speedValueLabel = nullptr;
    QLabel* m_statusLabel = nullptr;
    QLabel* m_summaryLabel = nullptr;
    QLabel* m_modeHintLabel = nullptr;
    QTableWidget* m_dpTable = nullptr;
    QListWidget* m_backtraceList = nullptr;
    QListWidget* m_operationsList = nullptr;
    QTextEdit* m_transformView = nullptr;
    QListWidget* m_suggestionsList = nullptr;

    QTimer* m_timer = nullptr;
    EditSolver m_solver;
    std::vector<EditSolver::CellTrace> m_fillTrace;
    int m_animationIndex = 0;
    int m_intervalMs = 120;
};

#endif // MAINWINDOW_H
