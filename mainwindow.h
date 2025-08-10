#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QTextEdit>
#include <QFileDialog>
#include <QTimer>
#include <QStatusBar>

class MouseController;
class MacroRecorder;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Recording controls
    void onStartRecording();
    void onStopRecording();
    void onClearMacro();
    
    // Playback controls
    void onPlayMacro();
    void onStopPlayback();
    
    // File operations
    void onLoadMacro();
    void onSaveMacro();
    
    // Settings
    void onRecordMouseMovementChanged(bool enabled);
    void onPlaybackSpeedChanged(double speed);
    
    // Updates from recorder
    void onRecordingStarted();
    void onRecordingStopped();
    void onPlaybackStarted();
    void onPlaybackStopped();
    void onPlaybackProgress(int current, int total);
    void onMacroCleared();
    void onRecorderStateChanged(int state);
    
    // Mouse position updates
    void onMousePositionChanged(const QPoint &position);
    void updateStatusBar();

private:
    void setupUI();
    void setupConnections();
    void updateControls();
    void loadStyleSheet();
    void showEvent(QShowEvent *event) override;
    
    // Core components
    MouseController *m_mouseController;
    MacroRecorder *m_macroRecorder;
    
    // UI Components
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    
    // Recording controls
    QGroupBox *m_recordingGroup;
    QPushButton *m_startRecordBtn;
    QPushButton *m_stopRecordBtn;
    QPushButton *m_clearBtn;
    QCheckBox *m_recordMouseMovement;
    
    // Playback controls
    QGroupBox *m_playbackGroup;
    QPushButton *m_playBtn;
    QPushButton *m_stopPlayBtn;
    QDoubleSpinBox *m_playbackSpeed;
    QProgressBar *m_playbackProgress;
    
    // File operations
    QGroupBox *m_fileGroup;
    QPushButton *m_loadBtn;
    QPushButton *m_saveBtn;
    
    // Information display
    QGroupBox *m_infoGroup;
    QLabel *m_mousePositionLabel;
    QLabel *m_eventCountLabel;
    QLabel *m_statusLabel;
    QTextEdit *m_logOutput;
    
    // Status bar
    QTimer *m_statusTimer;
    
    QString m_lastSaveFile;
};