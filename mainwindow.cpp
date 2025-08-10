#include "mainwindow.h"
#include "mousecontroller.h"
#include "macrorecorder.h"
#include <QApplication>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_mouseController(new MouseController(this))
    , m_macroRecorder(new MacroRecorder(m_mouseController, this))
    , m_centralWidget(nullptr)
    , m_statusTimer(new QTimer(this))
{
    setupUI();
    setupConnections();
    updateControls();
    loadStyleSheet();
    
    // Set window properties
    setWindowTitle("RCS - Record Control System v1.0");
    setMinimumSize(600, 500);
    resize(800, 600);
    
    // Start status updates
    m_statusTimer->start(100); // Update every 100ms
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(15, 15, 15, 15);
    
    // Recording Controls Group
    m_recordingGroup = new QGroupBox("Recording Controls", this);
    QGridLayout *recordLayout = new QGridLayout(m_recordingGroup);
    
    m_startRecordBtn = new QPushButton("Start Recording", this);
    m_startRecordBtn->setObjectName("primaryButton");
    m_stopRecordBtn = new QPushButton("Stop Recording", this);
    m_clearBtn = new QPushButton("Clear Macro", this);
    m_recordMouseMovement = new QCheckBox("Record Mouse Movement", this);
    m_recordMouseMovement->setChecked(true);
    
    recordLayout->addWidget(m_startRecordBtn, 0, 0);
    recordLayout->addWidget(m_stopRecordBtn, 0, 1);
    recordLayout->addWidget(m_clearBtn, 0, 2);
    recordLayout->addWidget(m_recordMouseMovement, 1, 0, 1, 3);
    
    // Playback Controls Group
    m_playbackGroup = new QGroupBox("Playback Controls", this);
    QGridLayout *playLayout = new QGridLayout(m_playbackGroup);
    
    m_playBtn = new QPushButton("Play Macro", this);
    m_playBtn->setObjectName("successButton");
    m_stopPlayBtn = new QPushButton("Stop Playback", this);
    
    QLabel *speedLabel = new QLabel("Speed:", this);
    m_playbackSpeed = new QDoubleSpinBox(this);
    m_playbackSpeed->setRange(0.1, 10.0);
    m_playbackSpeed->setValue(1.0);
    m_playbackSpeed->setSingleStep(0.1);
    m_playbackSpeed->setSuffix("x");
    
    m_playbackProgress = new QProgressBar(this);
    m_playbackProgress->setVisible(false);
    
    playLayout->addWidget(m_playBtn, 0, 0);
    playLayout->addWidget(m_stopPlayBtn, 0, 1);
    playLayout->addWidget(speedLabel, 0, 2);
    playLayout->addWidget(m_playbackSpeed, 0, 3);
    playLayout->addWidget(m_playbackProgress, 1, 0, 1, 4);
    
    // File Operations Group
    m_fileGroup = new QGroupBox("File Operations", this);
    QHBoxLayout *fileLayout = new QHBoxLayout(m_fileGroup);
    
    m_loadBtn = new QPushButton("Load Macro", this);
    m_saveBtn = new QPushButton("Save Macro", this);
    
    fileLayout->addWidget(m_loadBtn);
    fileLayout->addWidget(m_saveBtn);
    fileLayout->addStretch();
    
    // Information Display Group
    m_infoGroup = new QGroupBox("Information", this);
    QVBoxLayout *infoLayout = new QVBoxLayout(m_infoGroup);
    
    m_mousePositionLabel = new QLabel("Mouse Position: (0, 0)", this);
    m_eventCountLabel = new QLabel("Events: 0", this);
    m_statusLabel = new QLabel("Status: Ready", this);
    
    m_logOutput = new QTextEdit(this);
    m_logOutput->setMaximumHeight(120);
    m_logOutput->setReadOnly(true);
    m_logOutput->setObjectName("logOutput");
    
    infoLayout->addWidget(m_mousePositionLabel);
    infoLayout->addWidget(m_eventCountLabel);
    infoLayout->addWidget(m_statusLabel);
    infoLayout->addWidget(new QLabel("Log:", this));
    infoLayout->addWidget(m_logOutput);
    
    // Add all groups to main layout
    m_mainLayout->addWidget(m_recordingGroup);
    m_mainLayout->addWidget(m_playbackGroup);
    m_mainLayout->addWidget(m_fileGroup);
    m_mainLayout->addWidget(m_infoGroup);
    
    // Status bar
    statusBar()->showMessage("Ready");
}

void MainWindow::setupConnections()
{
    // Recording controls
    connect(m_startRecordBtn, &QPushButton::clicked, this, &MainWindow::onStartRecording);
    connect(m_stopRecordBtn, &QPushButton::clicked, this, &MainWindow::onStopRecording);
    connect(m_clearBtn, &QPushButton::clicked, this, &MainWindow::onClearMacro);
    
    // Playback controls
    connect(m_playBtn, &QPushButton::clicked, this, &MainWindow::onPlayMacro);
    connect(m_stopPlayBtn, &QPushButton::clicked, this, &MainWindow::onStopPlayback);
    
    // File operations
    connect(m_loadBtn, &QPushButton::clicked, this, &MainWindow::onLoadMacro);
    connect(m_saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveMacro);
    
    // Settings
    connect(m_recordMouseMovement, &QCheckBox::toggled, this, &MainWindow::onRecordMouseMovementChanged);
    connect(m_playbackSpeed, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onPlaybackSpeedChanged);
    
    // Macro recorder signals
    connect(m_macroRecorder, &MacroRecorder::recordingStarted, this, &MainWindow::onRecordingStarted);
    connect(m_macroRecorder, &MacroRecorder::recordingStopped, this, &MainWindow::onRecordingStopped);
    connect(m_macroRecorder, &MacroRecorder::playbackStarted, this, &MainWindow::onPlaybackStarted);
    connect(m_macroRecorder, &MacroRecorder::playbackStopped, this, &MainWindow::onPlaybackStopped);
    connect(m_macroRecorder, &MacroRecorder::playbackProgress, this, &MainWindow::onPlaybackProgress);
    connect(m_macroRecorder, &MacroRecorder::macroCleared, this, &MainWindow::onMacroCleared);
    connect(m_macroRecorder, QOverload<int>::of(&MacroRecorder::stateChanged), this, &MainWindow::onRecorderStateChanged);
    
    // Mouse controller signals
    connect(m_mouseController, &MouseController::mousePositionChanged, this, &MainWindow::onMousePositionChanged);
    
    // Status timer
    connect(m_statusTimer, &QTimer::timeout, this, &MainWindow::updateStatusBar);
}

void MainWindow::onStartRecording()
{
    m_macroRecorder->startRecording();
    m_logOutput->append(QString("[%1] Recording started")
                       .arg(QTime::currentTime().toString("hh:mm:ss")));
}

void MainWindow::onStopRecording()
{
    m_macroRecorder->stopRecording();
    m_logOutput->append(QString("[%1] Recording stopped - %2 events captured")
                       .arg(QTime::currentTime().toString("hh:mm:ss"))
                       .arg(m_macroRecorder->eventCount()));
}

void MainWindow::onClearMacro()
{
    m_macroRecorder->clearMacro();
    m_logOutput->append(QString("[%1] Macro cleared")
                       .arg(QTime::currentTime().toString("hh:mm:ss")));
}

void MainWindow::onPlayMacro()
{
    if (!m_macroRecorder->hasMacro()) {
        QMessageBox::information(this, "No Macro", "Please record or load a macro first.");
        return;
    }
    
    m_macroRecorder->playMacro();
    m_logOutput->append(QString("[%1] Playback started")
                       .arg(QTime::currentTime().toString("hh:mm:ss")));
}

void MainWindow::onStopPlayback()
{
    m_macroRecorder->stopPlayback();
    m_logOutput->append(QString("[%1] Playback stopped")
                       .arg(QTime::currentTime().toString("hh:mm:ss")));
}

void MainWindow::onLoadMacro()
{
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString fileName = QFileDialog::getOpenFileName(this,
        "Load Macro", documentsPath, "RCS Macro Files (*.rcs);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        if (m_macroRecorder->loadMacro(fileName)) {
            m_lastSaveFile = fileName;
            QFileInfo info(fileName);
            m_logOutput->append(QString("[%1] Loaded macro: %2 (%3 events)")
                               .arg(QTime::currentTime().toString("hh:mm:ss"))
                               .arg(info.baseName())
                               .arg(m_macroRecorder->eventCount()));
        } else {
            QMessageBox::warning(this, "Load Error", "Failed to load macro file.");
        }
    }
}

void MainWindow::onSaveMacro()
{
    if (!m_macroRecorder->hasMacro()) {
        QMessageBox::information(this, "No Macro", "Please record a macro first.");
        return;
    }
    
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Macro", documentsPath + "/macro.rcs", "RCS Macro Files (*.rcs);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        if (m_macroRecorder->saveMacro(fileName)) {
            m_lastSaveFile = fileName;
            QFileInfo info(fileName);
            m_logOutput->append(QString("[%1] Saved macro: %2")
                               .arg(QTime::currentTime().toString("hh:mm:ss"))
                               .arg(info.baseName()));
        } else {
            QMessageBox::warning(this, "Save Error", "Failed to save macro file.");
        }
    }
}

void MainWindow::onRecordMouseMovementChanged(bool enabled)
{
    m_macroRecorder->setRecordMouseMovement(enabled);
}

void MainWindow::onPlaybackSpeedChanged(double speed)
{
    m_macroRecorder->setPlaybackSpeed(speed);
}

void MainWindow::onRecordingStarted()
{
    updateControls();
}

void MainWindow::onRecordingStopped()
{
    updateControls();
}

void MainWindow::onPlaybackStarted()
{
    m_playbackProgress->setVisible(true);
    m_playbackProgress->setMaximum(m_macroRecorder->eventCount());
    updateControls();
}

void MainWindow::onPlaybackStopped()
{
    m_playbackProgress->setVisible(false);
    updateControls();
}

void MainWindow::onPlaybackProgress(int current, int total)
{
    m_playbackProgress->setValue(current);
    m_playbackProgress->setMaximum(total);
}

void MainWindow::onMacroCleared()
{
    updateControls();
}

void MainWindow::onRecorderStateChanged(int state)
{
    updateControls();
}

void MainWindow::onMousePositionChanged(const QPoint &position)
{
    m_mousePositionLabel->setText(QString("Mouse Position: (%1, %2)")
                                  .arg(position.x()).arg(position.y()));
}

void MainWindow::updateStatusBar()
{
    QString status;
    switch (m_macroRecorder->state()) {
    case MacroRecorder::Idle:
        status = "Ready";
        break;
    case MacroRecorder::Recording:
        status = "Recording...";
        break;
    case MacroRecorder::Playing:
        status = "Playing...";
        break;
    }
    
    statusBar()->showMessage(status);
}

void MainWindow::updateControls()
{
    bool isIdle = m_macroRecorder->state() == MacroRecorder::Idle;
    bool isRecording = m_macroRecorder->isRecording();
    bool isPlaying = m_macroRecorder->isPlaying();
    bool hasMacro = m_macroRecorder->hasMacro();
    
    // Recording controls
    m_startRecordBtn->setEnabled(isIdle);
    m_stopRecordBtn->setEnabled(isRecording);
    m_clearBtn->setEnabled(isIdle && hasMacro);
    m_recordMouseMovement->setEnabled(isIdle);
    
    // Playback controls
    m_playBtn->setEnabled(isIdle && hasMacro);
    m_stopPlayBtn->setEnabled(isPlaying);
    m_playbackSpeed->setEnabled(isIdle);
    
    // File operations
    m_loadBtn->setEnabled(isIdle);
    m_saveBtn->setEnabled(isIdle && hasMacro);
    
    // Update information
    m_eventCountLabel->setText(QString("Events: %1").arg(m_macroRecorder->eventCount()));
    
    QString statusText;
    if (isRecording) {
        statusText = "Status: Recording";
    } else if (isPlaying) {
        statusText = "Status: Playing";
    } else {
        statusText = "Status: Ready";
    }
    m_statusLabel->setText(statusText);
}

void MainWindow::loadStyleSheet()
{
    QFile file("style.qss");
    if (!file.open(QIODevice::ReadOnly)) {
        // If external file doesn't exist, use fallback
        file.setFileName(":/style.qss");
        if (!file.open(QIODevice::ReadOnly)) {
            return;
        }
    }
    
    QString styleSheet = file.readAll();
    setStyleSheet(styleSheet);
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    updateControls();
}

#include "mainwindow.moc"