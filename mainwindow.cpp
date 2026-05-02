#include "mainwindow.h"

#include <QApplication>
#include <QAbstractButton>
#include <QAbstractItemView>
#include <QButtonGroup>
#include <QComboBox>
#include <QFrame>
#include <QFont>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPalette>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea> // LAYOUT FIX
#include <QSlider>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

#include <algorithm>

namespace {
constexpr const char* kBackground = "#121826";
constexpr const char* kBackgroundAlt = "#0D1117";
constexpr const char* kPanel = "#1E293B";
constexpr const char* kPanelRaised = "#243247";
constexpr const char* kBorder = "#334155";
constexpr const char* kText = "#E5E7EB";
constexpr const char* kMutedText = "#94A3B8";
constexpr const char* kAccent = "#38BDF8";
constexpr const char* kSuccess = "#22C55E";
constexpr const char* kWarning = "#F59E0B";
constexpr const char* kDistance = "#FACC15";
constexpr const char* kError = "#EF4444";
constexpr const char* kErrorHover = "#DC2626";
constexpr const char* kCurrent = "#F59E0B";
constexpr const char* kReplace = "#EF4444";
constexpr const char* kInsert = "#38BDF8";
constexpr const char* kMatch = "#22C55E";
constexpr const char* kBacktrack = "#FACC15";
constexpr const char* kDelete = "#A855F7";
constexpr const char* kBase = "#64748B";

QColor contrastingTextColor(const QColor& background)
{
    return background.lightnessF() > 0.62 ? QColor("#0F172A") : QColor(kText);
}
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Smart Spell Checker & Text Similarity Analyser");
    setMinimumSize(1400, 900); // LAYOUT FIX
    setFont(QFont("Segoe UI", 10));

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::onAnimationStep);

    buildUi();
    applyDarkTheme();
    updateModeUi();
}

void MainWindow::buildUi()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    auto* mainLayout = new QVBoxLayout(m_centralWidget); // LAYOUT FIX
    mainLayout->setContentsMargins(16, 16, 16, 16); // LAYOUT FIX
    mainLayout->setSpacing(8); // LAYOUT FIX

    auto* heroFrame = new QFrame(this);
    heroFrame->setObjectName("heroFrame");
    heroFrame->setFixedHeight(190); // LAYOUT FIX
    auto* heroLayout = new QHBoxLayout(heroFrame); // LAYOUT FIX
    heroLayout->setContentsMargins(20, 18, 20, 18); // LAYOUT FIX
    heroLayout->setSpacing(20); // LAYOUT FIX

    auto* titleBlock = new QVBoxLayout; // LAYOUT FIX
    titleBlock->setSpacing(10); // LAYOUT FIX

    auto* headerLabel = new QLabel("Smart Spell Checker & Text Similarity Analyser", this);
    headerLabel->setObjectName("headerLabel");

    auto* subtitleLabel = new QLabel(
        "Visualise how Edit Distance transforms spellings, DNA sequences, and code tokens step by step.",
        this);
    subtitleLabel->setObjectName("subtitleLabel");
    subtitleLabel->setWordWrap(true);

    m_statusLabel = new QLabel("Ready to animate the DP grid. Choose a preset or enter your own source and target strings.", this);
    m_statusLabel->setObjectName("statusPill");
    m_statusLabel->setWordWrap(true);

    titleBlock->addWidget(headerLabel);
    titleBlock->addWidget(subtitleLabel);
    titleBlock->addStretch(1); // LAYOUT FIX
    titleBlock->addWidget(m_statusLabel);

    m_summaryLabel = new QLabel(this);
    m_summaryLabel->setObjectName("summaryCard");
    m_summaryLabel->setFixedWidth(300); // LAYOUT FIX
    m_summaryLabel->setMinimumHeight(150); // LAYOUT FIX
    m_summaryLabel->setWordWrap(true);
    m_summaryLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft); // LAYOUT FIX

    heroLayout->addLayout(titleBlock, 1); // LAYOUT FIX
    heroLayout->addWidget(m_summaryLabel, 0, Qt::AlignTop); // LAYOUT FIX

    buildInputPanel();
    buildDPPanel();
    buildOperationsPanel();
    buildSuggestionsPanel();

    auto* leftColumnContainer = new QWidget(this); // LAYOUT FIX
    auto* leftColumnLayout = new QVBoxLayout(leftColumnContainer); // LAYOUT FIX
    leftColumnLayout->setContentsMargins(0, 0, 0, 0); // LAYOUT FIX
    leftColumnLayout->setSpacing(0); // LAYOUT FIX
    leftColumnLayout->addWidget(m_dpGroup); // LAYOUT FIX

    auto* leftScrollArea = new QScrollArea(this); // LAYOUT FIX
    leftScrollArea->setWidgetResizable(true); // LAYOUT FIX
    leftScrollArea->setFrameShape(QFrame::NoFrame); // LAYOUT FIX
    leftScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded); // LAYOUT FIX
    leftScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded); // LAYOUT FIX
    leftScrollArea->setWidget(leftColumnContainer); // LAYOUT FIX
    leftScrollArea->setMinimumWidth(480); // LAYOUT FIX

    auto* rightColumn = new QWidget(this); // LAYOUT FIX
    auto* rightColumnLayout = new QVBoxLayout(rightColumn); // LAYOUT FIX
    rightColumnLayout->setContentsMargins(0, 0, 0, 0); // LAYOUT FIX
    rightColumnLayout->setSpacing(8); // LAYOUT FIX
    rightColumnLayout->addWidget(m_operationsGroup, 3); // LAYOUT FIX
    rightColumnLayout->addWidget(m_suggestionsGroup, 2); // LAYOUT FIX
    rightColumn->setMinimumWidth(700); // LAYOUT FIX

    auto* workspaceSplitter = new QSplitter(Qt::Horizontal, this); // LAYOUT FIX
    workspaceSplitter->setChildrenCollapsible(false); // LAYOUT FIX
    workspaceSplitter->setHandleWidth(10); // LAYOUT FIX
    workspaceSplitter->addWidget(leftScrollArea); // LAYOUT FIX
    workspaceSplitter->addWidget(rightColumn); // LAYOUT FIX
    workspaceSplitter->setSizes({620, 780}); // LAYOUT FIX
    workspaceSplitter->setStretchFactor(0, 4); // LAYOUT FIX
    workspaceSplitter->setStretchFactor(1, 5); // LAYOUT FIX

    mainLayout->addWidget(heroFrame);
    mainLayout->addWidget(m_inputGroup);
    mainLayout->addWidget(workspaceSplitter, 1); // LAYOUT FIX

    resetViews();
}

void MainWindow::buildInputPanel()
{
    m_inputGroup = new QGroupBox("Input Panel", this);
    m_inputGroup->setMinimumHeight(280); // LAYOUT FIX
    auto* layout = new QGridLayout(m_inputGroup); // LAYOUT FIX
    layout->setContentsMargins(16, 16, 16, 16); // LAYOUT FIX
    layout->setHorizontalSpacing(12); // LAYOUT FIX
    layout->setVerticalSpacing(12); // LAYOUT FIX
    layout->setColumnStretch(1, 1); // LAYOUT FIX
    layout->setColumnStretch(2, 0); // LAYOUT FIX
    layout->setColumnStretch(3, 0); // LAYOUT FIX

    auto* sourceLabel = new QLabel("Source Word", m_inputGroup);
    auto* targetLabel = new QLabel("Target Word", m_inputGroup);
    auto* presetLabel = new QLabel("Preset Examples", m_inputGroup);
    auto* speedLabel = new QLabel("Animation Speed", m_inputGroup);
    auto* modeLabel = new QLabel("Mode", m_inputGroup);

    m_sourceEdit = new QLineEdit(m_inputGroup);
    m_sourceEdit->setMinimumHeight(36); // LAYOUT FIX
    m_sourceEdit->setFont(QFont("Segoe UI", 14)); // LAYOUT FIX

    m_targetEdit = new QLineEdit(m_inputGroup);
    m_targetEdit->setMinimumHeight(36); // LAYOUT FIX
    m_targetEdit->setFont(QFont("Segoe UI", 14)); // LAYOUT FIX

    m_presetCombo = new QComboBox(m_inputGroup);
    m_presetCombo->setMinimumHeight(32); // LAYOUT FIX
    m_presetCombo->addItem("Choose preset...");
    m_presetCombo->addItem("kitten -> sitting");
    m_presetCombo->addItem("recieve -> receive");
    m_presetCombo->addItem("GATTACA -> GCATGCU");

    m_speedSlider = new QSlider(Qt::Horizontal, m_inputGroup);
    m_speedSlider->setRange(1, 10);
    m_speedSlider->setValue(6);
    m_speedSlider->setMinimumHeight(40); // LAYOUT FIX
    m_speedValueLabel = new QLabel("6", m_inputGroup);
    m_speedValueLabel->setMinimumWidth(28); // LAYOUT FIX

    auto* speedRow = new QWidget(m_inputGroup); // LAYOUT FIX
    auto* speedRowLayout = new QHBoxLayout(speedRow); // LAYOUT FIX
    speedRowLayout->setContentsMargins(0, 0, 0, 0); // LAYOUT FIX
    speedRowLayout->setSpacing(12); // LAYOUT FIX
    speedRowLayout->addWidget(m_speedSlider, 1); // LAYOUT FIX
    speedRowLayout->addWidget(m_speedValueLabel, 0, Qt::AlignVCenter); // LAYOUT FIX

    m_modeGroup = new QButtonGroup(this);
    auto* spellButton = new QRadioButton("Spell Check", m_inputGroup);
    auto* dnaButton = new QRadioButton("DNA Analysis", m_inputGroup);
    auto* diffButton = new QRadioButton("Code Diff", m_inputGroup);
    spellButton->setChecked(true);
    spellButton->setMinimumHeight(40); // LAYOUT FIX
    dnaButton->setMinimumHeight(40); // LAYOUT FIX
    diffButton->setMinimumHeight(40); // LAYOUT FIX
    m_modeGroup->addButton(spellButton, static_cast<int>(AppMode::SpellCheck));
    m_modeGroup->addButton(dnaButton, static_cast<int>(AppMode::DNAAnalysis));
    m_modeGroup->addButton(diffButton, static_cast<int>(AppMode::CodeDiff));

    auto* modeLayout = new QHBoxLayout; // LAYOUT FIX
    modeLayout->setContentsMargins(0, 0, 0, 0); // LAYOUT FIX
    modeLayout->setSpacing(20); // LAYOUT FIX
    modeLayout->addWidget(spellButton); // LAYOUT FIX
    modeLayout->addWidget(dnaButton); // LAYOUT FIX
    modeLayout->addWidget(diffButton); // LAYOUT FIX
    modeLayout->addStretch(1); // LAYOUT FIX

    m_modeHintLabel = new QLabel(m_inputGroup);
    m_modeHintLabel->setObjectName("modeHintLabel");
    m_modeHintLabel->setWordWrap(true);

    m_solveButton = new QPushButton("Check Spelling", m_inputGroup);
    m_solveButton->setMinimumHeight(44); // LAYOUT FIX
    m_solveButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // LAYOUT FIX
    m_solveButton->setIconSize(QSize(20, 20));

    layout->addWidget(sourceLabel, 0, 0); // LAYOUT FIX
    layout->addWidget(m_sourceEdit, 0, 1, 1, 3); // LAYOUT FIX
    layout->addWidget(targetLabel, 1, 0); // LAYOUT FIX
    layout->addWidget(m_targetEdit, 1, 1, 1, 3); // LAYOUT FIX
    layout->addWidget(modeLabel, 2, 0); // LAYOUT FIX
    layout->addLayout(modeLayout, 2, 1, 1, 3); // LAYOUT FIX
    layout->addWidget(presetLabel, 3, 0); // LAYOUT FIX
    layout->addWidget(m_presetCombo, 3, 1, 1, 3); // LAYOUT FIX
    layout->addWidget(speedLabel, 4, 0); // LAYOUT FIX
    layout->addWidget(speedRow, 4, 1, 1, 3); // LAYOUT FIX
    layout->addWidget(m_modeHintLabel, 5, 0, 1, 4); // LAYOUT FIX
    layout->addWidget(m_solveButton, 6, 0, 1, 4); // LAYOUT FIX

    connect(m_solveButton, &QPushButton::clicked, this, &MainWindow::onSolveClicked);
    connect(m_presetCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::onPresetChanged);
    connect(m_speedSlider, &QSlider::valueChanged, this, &MainWindow::onSpeedChanged);
    connect(spellButton, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            updateModeUi();
        }
    });
    connect(dnaButton, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            updateModeUi();
        }
    });
    connect(diffButton, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            updateModeUi();
        }
    });
}

void MainWindow::buildDPPanel()
{
    m_dpGroup = new QGroupBox("DP Table Visualisation", this);
    auto* layout = new QVBoxLayout(m_dpGroup); // LAYOUT FIX
    layout->setContentsMargins(16, 18, 16, 16); // LAYOUT FIX
    layout->setSpacing(12); // LAYOUT FIX

    auto* legendLabel = new QLabel(
        "<span style='color:#ff9f1c;'>● Current</span> &nbsp;&nbsp;"
        "<span style='color:#06d6a0;'>● Match</span> &nbsp;&nbsp;"
        "<span style='color:#e94560;'>● Replace</span> &nbsp;&nbsp;"
        "<span style='color:#00b4d8;'>● Insert</span> &nbsp;&nbsp;"
        "<span style='color:#9d4edd;'>● Delete</span> &nbsp;&nbsp;"
        "<span style='color:#ffd166;'>● Backtrace</span> &nbsp;&nbsp;"
        "<span style='color:#9aa0b4;'>● Base</span>",
        m_dpGroup);
    legendLabel->setObjectName("legendLabel");

    m_dpTable = new QTableWidget(m_dpGroup);
    m_dpTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_dpTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_dpTable->setAlternatingRowColors(true);
    m_dpTable->setMinimumHeight(450); // LAYOUT FIX
    m_dpTable->setFont(QFont("Consolas", 13, QFont::Bold)); // LAYOUT FIX
    m_dpTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_dpTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_dpTable->verticalHeader()->setDefaultSectionSize(42); // LAYOUT FIX
    m_dpTable->horizontalHeader()->setDefaultSectionSize(42); // LAYOUT FIX
    m_dpTable->verticalHeader()->setMinimumSectionSize(42); // LAYOUT FIX
    m_dpTable->horizontalHeader()->setMinimumSectionSize(42); // LAYOUT FIX
    m_dpTable->verticalHeader()->setFixedWidth(50); // LAYOUT FIX
    m_dpTable->horizontalHeader()->setFixedHeight(30); // LAYOUT FIX

    auto* tableHost = new QWidget(m_dpGroup); // LAYOUT FIX
    auto* tableHostLayout = new QVBoxLayout(tableHost); // LAYOUT FIX
    tableHostLayout->setContentsMargins(0, 0, 0, 0); // LAYOUT FIX
    tableHostLayout->setSpacing(0); // LAYOUT FIX
    tableHostLayout->addWidget(m_dpTable); // LAYOUT FIX

    auto* tableScrollArea = new QScrollArea(m_dpGroup); // LAYOUT FIX
    tableScrollArea->setWidgetResizable(true); // LAYOUT FIX
    tableScrollArea->setFrameShape(QFrame::NoFrame); // LAYOUT FIX
    tableScrollArea->setWidget(tableHost); // LAYOUT FIX

    layout->addWidget(legendLabel);
    layout->addWidget(tableScrollArea, 1); // LAYOUT FIX
}

void MainWindow::buildOperationsPanel()
{
    m_operationsGroup = new QGroupBox("Operations Panel", this);
    auto* layout = new QVBoxLayout(m_operationsGroup); // LAYOUT FIX
    layout->setContentsMargins(12, 12, 12, 12); // LAYOUT FIX
    layout->setSpacing(12); // LAYOUT FIX

    m_operationsList = new QListWidget(m_operationsGroup);
    m_operationsList->setMinimumHeight(220); // LAYOUT FIX

    m_transformView = new QTextEdit(m_operationsGroup);
    m_transformView->setReadOnly(true);
    m_transformView->setMinimumHeight(170); // LAYOUT FIX

    auto* operationsLabel = new QLabel("Edit Operations in Order", m_operationsGroup);
    operationsLabel->setObjectName("sectionLabel");
    auto* transformLabel = new QLabel("Word Transformation View", m_operationsGroup);
    transformLabel->setObjectName("sectionLabel");

    auto* operationsPane = new QWidget(m_operationsGroup); // LAYOUT FIX
    auto* operationsPaneLayout = new QVBoxLayout(operationsPane); // LAYOUT FIX
    operationsPaneLayout->setContentsMargins(0, 0, 0, 0); // LAYOUT FIX
    operationsPaneLayout->setSpacing(10); // LAYOUT FIX
    operationsPaneLayout->addWidget(operationsLabel); // LAYOUT FIX
    operationsPaneLayout->addWidget(m_operationsList); // LAYOUT FIX

    auto* transformPane = new QWidget(m_operationsGroup); // LAYOUT FIX
    auto* transformPaneLayout = new QVBoxLayout(transformPane); // LAYOUT FIX
    transformPaneLayout->setContentsMargins(0, 0, 0, 0); // LAYOUT FIX
    transformPaneLayout->setSpacing(10); // LAYOUT FIX
    transformPaneLayout->addWidget(transformLabel); // LAYOUT FIX
    transformPaneLayout->addWidget(m_transformView); // LAYOUT FIX

    auto* operationsSplitter = new QSplitter(Qt::Vertical, m_operationsGroup); // LAYOUT FIX
    operationsSplitter->setChildrenCollapsible(false); // LAYOUT FIX
    operationsSplitter->setHandleWidth(8); // LAYOUT FIX
    operationsSplitter->addWidget(operationsPane); // LAYOUT FIX
    operationsSplitter->addWidget(transformPane); // LAYOUT FIX
    operationsSplitter->setStretchFactor(0, 3); // LAYOUT FIX
    operationsSplitter->setStretchFactor(1, 2); // LAYOUT FIX
    operationsSplitter->setSizes({300, 220}); // LAYOUT FIX

    layout->addWidget(operationsSplitter, 1); // LAYOUT FIX
}

void MainWindow::buildSuggestionsPanel()
{
    m_suggestionsGroup = new QGroupBox("Backtrace Panel", this); // LAYOUT FIX
    auto* layout = new QVBoxLayout(m_suggestionsGroup); // LAYOUT FIX
    layout->setContentsMargins(16, 18, 16, 16); // LAYOUT FIX
    layout->setSpacing(12); // LAYOUT FIX

    m_backtraceList = new QListWidget(m_suggestionsGroup); // LAYOUT FIX
    m_backtraceList->setMinimumHeight(220); // LAYOUT FIX
    m_backtraceList->setWordWrap(true); // LAYOUT FIX

    auto* suggestionsLabel = new QLabel("Optimal Backtrace Steps", m_suggestionsGroup); // LAYOUT FIX
    suggestionsLabel->setObjectName("sectionLabel"); // LAYOUT FIX

    layout->addWidget(suggestionsLabel); // LAYOUT FIX
    layout->addWidget(m_backtraceList, 1); // LAYOUT FIX
}

void MainWindow::applyDarkTheme()
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(kBackground));
    palette.setColor(QPalette::WindowText, QColor(kText));
    palette.setColor(QPalette::Base, QColor(kPanel));
    palette.setColor(QPalette::AlternateBase, QColor(kPanelRaised));
    palette.setColor(QPalette::Text, QColor(kText));
    palette.setColor(QPalette::Button, QColor(kPanel));
    palette.setColor(QPalette::ButtonText, QColor(kText));
    palette.setColor(QPalette::Highlight, QColor(kAccent));
    palette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    palette.setColor(QPalette::PlaceholderText, QColor(kMutedText));
    qApp->setPalette(palette);

    QString styleSheet = QStringLiteral(
        "QMainWindow, QWidget { background-color: ${BACKGROUND}; color: ${TEXT}; font-size: 14px; }"
        "QFrame#heroFrame { background-color: ${PANEL}; border: 1px solid ${BORDER}; border-radius: 18px; }"
        "QLabel { color: ${TEXT}; background: transparent; }"
        "#headerLabel { font-size: 30px; font-weight: 800; color: ${TEXT}; letter-spacing: 0.3px; }"
        "#subtitleLabel { color: ${MUTED}; font-size: 15px; }"
        "#statusPill { background-color: rgba(56, 189, 248, 0.12); border: 1px solid rgba(56, 189, 248, 0.45); border-radius: 12px; padding: 10px 14px; color: ${TEXT}; }"
        "#summaryCard { background-color: ${PANEL_RAISED}; border: 1px solid ${BORDER}; border-radius: 16px; padding: 16px; color: ${TEXT}; }"
        "QGroupBox { background-color: ${PANEL}; color: ${TEXT}; border: 1px solid ${BORDER}; border-radius: 16px; margin-top: 8px; padding: 26px 14px 14px 14px; font-weight: 700; }" // LAYOUT FIX
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 14px; top: 8px; padding: 0 8px; color: ${TEXT}; background-color: transparent; }" // LAYOUT FIX
        "QLineEdit, QComboBox, QTextEdit, QListWidget, QTableWidget { color: ${TEXT}; background-color: ${PANEL_RAISED}; border: 1px solid ${BORDER}; border-radius: 8px; padding: 6px; selection-background-color: ${ACCENT}; selection-color: #ffffff; }"
        "QLineEdit:focus, QComboBox:focus, QTextEdit:focus, QListWidget:focus, QTableWidget:focus { border: 1px solid ${ACCENT}; }"
        "QComboBox::drop-down { border: none; width: 28px; }"
        "QComboBox::down-arrow { image: none; border-left: 5px solid transparent; border-right: 5px solid transparent; border-top: 7px solid ${TEXT}; margin-right: 10px; }"
        "QComboBox QAbstractItemView { background-color: ${PANELRaised}; color: ${TEXT}; border: 1px solid ${BORDER}; selection-background-color: ${ACCENT}; selection-color: #ffffff; }"
        "QPushButton { background-color: ${ERROR}; color: #ffffff; border: none; border-radius: 8px; padding: 8px 16px; font-weight: 700; }"
        "QPushButton:hover { background-color: ${ERROR_HOVER}; }"
        "QPushButton:pressed { background-color: #B91C1C; }"
        "QHeaderView::section { background-color: ${BACKGROUND_ALT}; color: ${TEXT}; border: 1px solid ${BORDER}; padding: 4px; font-weight: 700; }" // LAYOUT FIX
        "QTableWidget { background-color: ${PANEL_RAISED}; color: ${TEXT}; gridline-color: ${BORDER}; alternate-background-color: ${PANEL}; font-size: 13px; }" // LAYOUT FIX
        "QTableCornerButton::section { background-color: ${BACKGROUND_ALT}; border: 1px solid ${BORDER}; }"
        "QTableWidget::item { color: ${TEXT}; padding: 4px; }"
        "QListWidget::item { color: ${TEXT}; padding: 6px 4px; border-radius: 6px; }"
        "QListWidget::item:selected { background-color: rgba(56, 189, 248, 0.20); color: ${TEXT}; }"
        "QTextEdit { font-family: Consolas, 'Cascadia Mono', monospace; }"
        "QSlider::groove:horizontal { height: 8px; background: ${BACKGROUND_ALT}; border-radius: 4px; }"
        "QSlider::sub-page:horizontal { background: ${ACCENT}; border-radius: 4px; }"
        "QSlider::add-page:horizontal { background: #0F172A; border-radius: 4px; }"
        "QSlider::handle:horizontal { background: ${ACCENT}; width: 18px; margin: -6px 0; border-radius: 9px; }"
        "QRadioButton { spacing: 10px; padding: 6px 0px 6px 0px; font-weight: 600; color: ${TEXT}; }" // LAYOUT FIX
        "QRadioButton::indicator { width: 18px; height: 18px; }"
        "QRadioButton::indicator:checked { background: ${ACCENT}; border: 2px solid ${ACCENT}; border-radius: 9px; }"
        "QRadioButton::indicator:unchecked { background: transparent; border: 2px solid ${BORDER}; border-radius: 9px; }"
        "#sectionLabel { color: #F8FAFC; font-size: 15px; font-weight: 700; padding-left: 2px; }" // LAYOUT FIX
        "#legendLabel { color: ${TEXT}; background-color: ${PANEL_RAISED}; border: 1px solid ${BORDER}; border-radius: 10px; padding: 8px 12px; }"
        "#modeHintLabel { background-color: rgba(245, 158, 11, 0.10); border: 1px solid rgba(245, 158, 11, 0.35); border-radius: 10px; padding: 10px 12px; color: #FDE68A; }");

    styleSheet.replace("${BACKGROUND}", kBackground);
    styleSheet.replace("${BACKGROUND_ALT}", kBackgroundAlt);
    styleSheet.replace("${PANEL}", kPanel);
    styleSheet.replace("${PANEL_RAISED}", kPanelRaised);
    styleSheet.replace("${PANELRaised}", kPanelRaised); // LAYOUT FIX
    styleSheet.replace("${BORDER}", kBorder);
    styleSheet.replace("${TEXT}", kText);
    styleSheet.replace("${MUTED}", kMutedText);
    styleSheet.replace("${ACCENT}", kAccent);
    styleSheet.replace("${ERROR}", kError);
    styleSheet.replace("${ERROR_HOVER}", kErrorHover);
    qApp->setStyleSheet(styleSheet);
}

void MainWindow::updateModeUi()
{
    const AppMode mode = currentMode();

    if (mode == AppMode::SpellCheck) {
        m_solveButton->setText("Check Spelling");
        m_sourceEdit->setPlaceholderText("Enter misspelled word");
        m_targetEdit->setPlaceholderText("Enter correct word");
        m_modeHintLabel->setText("Spell mode compares natural-language words and ranks dictionary suggestions.");
    } else if (mode == AppMode::DNAAnalysis) {
        m_solveButton->setText("Analyse DNA");
        m_sourceEdit->setPlaceholderText("Enter DNA/RNA sequence");
        m_targetEdit->setPlaceholderText("Enter target sequence");
        m_modeHintLabel->setText("DNA mode compares biological sequences and highlights mutations.");
    } else {
        m_solveButton->setText("Compare Code Tokens");
        m_sourceEdit->setPlaceholderText("Enter source token/string");
        m_targetEdit->setPlaceholderText("Enter target token/string");
        m_modeHintLabel->setText("Code diff mode visualises token edits for refactors or typo fixes.");
    }
}

void MainWindow::resetViews()
{
    if (m_timer) {
        m_timer->stop();
    }
    m_fillTrace.clear();
    m_animationIndex = 0;
    m_dpTable->clear();
    m_dpTable->setRowCount(0);
    m_dpTable->setColumnCount(0);
    if (m_backtraceList) {
        m_backtraceList->clear();
    }
    m_operationsList->clear();
    m_transformView->clear();
    if (m_suggestionsList) {
        m_suggestionsList->clear();
    }
    m_summaryLabel->setText(
        "<div style='font-size:15px; margin: 4px 0;'>"
        "<div style='color:#cbd5e1; text-transform:uppercase; letter-spacing:1px; font-size:12px;'>Analysis Snapshot</div>"
        "<div style='margin-top:14px; font-size:24px; font-weight:800; color:#FACC15;'>Distance: -</div>"
        "<div style='margin-top:10px; font-size:15px; color:#cbd5e1;'>Similarity: <b style='color:#22C55E;'>-</b></div>" // LAYOUT FIX
        "<div style='margin-top:6px; font-size:15px; color:#cbd5e1;'>Operations: <b style='color:#ffffff;'>-</b></div>" // LAYOUT FIX
        "</div>");
}

void MainWindow::prepareTable()
{
    const QString src = m_sourceEdit->text();
    const QString tgt = m_targetEdit->text();

    m_dpTable->setRowCount(src.size() + 1);
    m_dpTable->setColumnCount(tgt.size() + 1);
    populateBaseHeaders();

    for (int row = 0; row < m_dpTable->rowCount(); ++row) {
        for (int col = 0; col < m_dpTable->columnCount(); ++col) {
            auto* item = new QTableWidgetItem("");
            item->setTextAlignment(Qt::AlignCenter);
            item->setForeground(QColor(kText));
            item->setBackground(QColor((row + col) % 2 == 0 ? kPanelRaised : kPanel));
            m_dpTable->setItem(row, col, item);
        }
    }
}

void MainWindow::populateBaseHeaders()
{
    QStringList horizontalHeaders;
    horizontalHeaders << "∅";
    for (const QChar& ch : m_targetEdit->text()) {
        horizontalHeaders << ch;
    }

    QStringList verticalHeaders;
    verticalHeaders << "∅";
    for (const QChar& ch : m_sourceEdit->text()) {
        verticalHeaders << ch;
    }

    m_dpTable->setHorizontalHeaderLabels(horizontalHeaders);
    m_dpTable->setVerticalHeaderLabels(verticalHeaders);
}

void MainWindow::colorCell(int row, int col, const QColor& color)
{
    if (auto* item = m_dpTable->item(row, col)) {
        item->setBackground(color);
        item->setForeground(contrastingTextColor(color));
    }
}

void MainWindow::setCellText(int row, int col, int value)
{
    if (auto* item = m_dpTable->item(row, col)) {
        item->setText(QString::number(value));
    }
}

void MainWindow::displaySummary()
{
    const auto operations = m_solver.getOperations();
    m_summaryLabel->setText(QString(
        "<div style='font-size:15px; margin: 4px 0;'>"
        "<div style='color:#cbd5e1; text-transform:uppercase; letter-spacing:1px; font-size:12px;'>Analysis Snapshot</div>"
        "<div style='margin-top:14px; font-size:26px; font-weight:800; color:#FACC15;'>Distance: %1</div>"
        "<div style='margin-top:10px; font-size:15px; color:#cbd5e1;'>Similarity: <b style='color:#22C55E;'>%2%</b></div>" // LAYOUT FIX
        "<div style='margin-top:6px; font-size:15px; color:#cbd5e1;'>Operations: <b style='color:#ffffff;'>%3</b></div>" // LAYOUT FIX
        "<div style='margin-top:14px; color:#7DD3FC;'>%4 -> %5</div>"
        "</div>")
                                .arg(m_solver.getEditDistance())
                                .arg(QString::number(m_solver.getSimilarityPercent(), 'f', 2))
                                .arg(static_cast<int>(operations.size()))
                                .arg(m_sourceEdit->text().toHtmlEscaped())
                                .arg(m_targetEdit->text().toHtmlEscaped()));
}

void MainWindow::displayBacktrace()
{
    const auto path = m_solver.getBacktracePath();
    const auto decisions = m_solver.getDecisionTable();
    const QString src = m_sourceEdit->text();
    const QString tgt = m_targetEdit->text();

    for (const auto& cell : path) {
        colorCell(cell.first, cell.second, QColor(kBacktrack));
    }

    int stepCounter = 1;
    for (std::size_t index = 1; index < path.size(); ++index) {
        const auto previous = path[index - 1];
        const auto current = path[index];

        QString text;
        if (current.first == previous.first + 1 && current.second == previous.second + 1) {
            const QChar left = src[previous.first];
            const QChar right = tgt[previous.second];
            if (left == right) {
                text = QString("Step %1: Match '%2' with '%3' (cost +0)").arg(stepCounter).arg(left).arg(right);
            } else {
                text = QString("Step %1: Replace '%2' with '%3' (cost +1)").arg(stepCounter).arg(left).arg(right);
            }
        } else if (current.first == previous.first && current.second == previous.second + 1) {
            text = QString("Step %1: Insert '%2' (cost +1)").arg(stepCounter).arg(tgt[previous.second]);
        } else if (current.first == previous.first + 1 && current.second == previous.second) {
            text = QString("Step %1: Delete '%2' (cost +1)").arg(stepCounter).arg(src[previous.first]);
        }

        m_backtraceList->addItem(text);
        ++stepCounter;
    }

    Q_UNUSED(decisions);
}

void MainWindow::displayOperations()
{
    const auto operations = m_solver.getOperations();
    QString html;
    html += "<div style='font-family: Consolas, monospace;'>";
    html += QString("<p style='line-height:180%%;'><b>Start:</b> <span style='color:%1;'>%2</span></p>") // LAYOUT FIX
                .arg(kText, m_sourceEdit->text().toHtmlEscaped());

    if (operations.empty()) {
        m_operationsList->addItem("No edits required. The strings already match.");
        html += QString("<p style='line-height:180%%;'><b>Finish:</b> <span style='color:%1;'>%2</span></p>") // LAYOUT FIX
                    .arg(kMatch, m_targetEdit->text().toHtmlEscaped());
        html += "</div>";
        m_transformView->setHtml(html);
        return;
    }

    for (const EditSolver::Operation& op : operations) {
        m_operationsList->addItem(QString::fromStdString(op.description));

        QString accent = kReplace;
        if (op.type == EditSolver::Decision::Insert) {
            accent = kInsert;
        } else if (op.type == EditSolver::Decision::Delete) {
            accent = kDelete;
        }

        html += QString("<p style='line-height:180%%;'><b>%1</b><br/><span style='color:%2;'>%3</span></p>") // LAYOUT FIX
                    .arg(QString::fromStdString(op.description).toHtmlEscaped(),
                         accent,
                         QString::fromStdString(op.snapshot).toHtmlEscaped());
    }

    html += QString("<p style='line-height:180%%;'><b>Finish:</b> <span style='color:%1;'>%2</span></p>") // LAYOUT FIX
                .arg(kMatch, m_targetEdit->text().toHtmlEscaped());
    html += "</div>";
    m_transformView->setHtml(html);
}

void MainWindow::displaySuggestions()
{
    // Suggestions panel was replaced by the backtrace panel. // LAYOUT FIX
}

MainWindow::AppMode MainWindow::currentMode() const
{
    return static_cast<AppMode>(m_modeGroup->checkedId());
}

QString MainWindow::decisionLabel(EditSolver::Decision decision) const
{
    switch (decision) {
    case EditSolver::Decision::Match:
        return "Match";
    case EditSolver::Decision::Replace:
        return "Replace";
    case EditSolver::Decision::Insert:
        return "Insert";
    case EditSolver::Decision::Delete:
        return "Delete";
    case EditSolver::Decision::Base:
    default:
        return "Base";
    }
}

QColor MainWindow::decisionColor(EditSolver::Decision decision) const
{
    switch (decision) {
    case EditSolver::Decision::Match:
        return QColor(kMatch);
    case EditSolver::Decision::Replace:
        return QColor(kReplace);
    case EditSolver::Decision::Insert:
        return QColor(kInsert);
    case EditSolver::Decision::Delete:
        return QColor(kDelete);
    case EditSolver::Decision::Base:
    default:
        return QColor(kBase);
    }
}

std::vector<std::string> MainWindow::buildDictionary() const
{
    return {
        "receive", "relieve", "retrieve", "believe", "deceive",
        "kitten", "sitting", "written", "spelling", "distance",
        "analysis", "dynamic", "programming", "algorithm", "checker",
        "sequence", "mutation", "function", "variable", "compile"
    };
}

void MainWindow::onSolveClicked()
{
    const QString source = m_sourceEdit->text().trimmed();
    const QString target = m_targetEdit->text().trimmed();

    if (source.isEmpty() || target.isEmpty()) {
        m_statusLabel->setText("Both input fields are required before the solver can run.");
        return;
    }

    resetViews();
    prepareTable();

    m_solver.solve(source.toStdString(), target.toStdString());
    m_fillTrace = m_solver.getFillTrace();
    m_animationIndex = 0;

    displaySummary();
    m_statusLabel->setText(QString("Animating %1 DP cells for %2 mode...")
                               .arg(static_cast<int>(m_fillTrace.size()))
                               .arg(m_modeGroup->checkedButton()->text()));

    m_timer->start(m_intervalMs);
}

void MainWindow::onAnimationStep()
{
    if (m_animationIndex > 0 && m_animationIndex <= static_cast<int>(m_fillTrace.size())) {
        const EditSolver::CellTrace& previous = m_fillTrace[static_cast<std::size_t>(m_animationIndex - 1)];
        colorCell(previous.row, previous.col, decisionColor(previous.decision));
    }

    if (m_animationIndex >= static_cast<int>(m_fillTrace.size())) {
        m_timer->stop();
        displayBacktrace();
        displayOperations();
        displaySuggestions();
        m_statusLabel->setText("Animation complete. The optimal backtrace and edit sequence are now highlighted.");
        return;
    }

    const EditSolver::CellTrace& cell = m_fillTrace[static_cast<std::size_t>(m_animationIndex)];
    setCellText(cell.row, cell.col, cell.value);
    colorCell(cell.row, cell.col, QColor(kCurrent));

    m_statusLabel->setText(QString("Computing cell (%1, %2): %3 => %4")
                               .arg(cell.row)
                               .arg(cell.col)
                               .arg(decisionLabel(cell.decision))
                               .arg(cell.value));

    ++m_animationIndex;
}

void MainWindow::onPresetChanged(int index)
{
    if (index == 1) {
        m_sourceEdit->setText("kitten");
        m_targetEdit->setText("sitting");
    } else if (index == 2) {
        m_sourceEdit->setText("recieve");
        m_targetEdit->setText("receive");
    } else if (index == 3) {
        m_sourceEdit->setText("GATTACA");
        m_targetEdit->setText("GCATGCU");
    }
}

void MainWindow::onSpeedChanged(int value)
{
    m_speedValueLabel->setText(QString::number(value));
    m_intervalMs = std::max(25, 440 - (value * 40));
    if (m_timer && m_timer->isActive()) {
        m_timer->start(m_intervalMs);
    }
}
