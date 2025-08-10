#include "macrorecorder.h"
#include "mousecontroller.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MacroRecorder::MacroRecorder(MouseController *mouseController, QObject *parent)
    : QObject(parent)
    , m_mouseController(mouseController)
    , m_state(Idle)
    , m_recordMouseMovement(true)
    , m_recordingStartTime(0)
    , m_lastRecordedPosition(0, 0)
    , m_playbackSpeed(1.0)
    , m_playbackTimer(new QTimer(this))
    , m_currentEventIndex(0)
    , m_playbackStartTime(0)
    , m_playbackPauseOffset(0)
{
    // Connect mouse controller signals
    connect(m_mouseController, &MouseController::mousePositionChanged,
            this, &MacroRecorder::onMousePositionChanged);
    connect(m_mouseController, &MouseController::mouseClicked,
            this, &MacroRecorder::onMouseClicked);
    
    // Set up playback timer
    m_playbackTimer->setSingleShot(true);
    connect(m_playbackTimer, &QTimer::timeout, this, &MacroRecorder::executeNextEvent);
}

MacroRecorder::~MacroRecorder()
{
    if (m_state != Idle) {
        stopRecording();
        stopPlayback();
    }
}

void MacroRecorder::startRecording()
{
    if (m_state != Idle) {
        return;
    }
    
    clearMacro();
    m_recordingStartTime = getCurrentTimestamp();
    m_lastRecordedPosition = m_mouseController->getCurrentMousePosition();
    setState(Recording);
    
    emit recordingStarted();
}

void MacroRecorder::stopRecording()
{
    if (m_state != Recording) {
        return;
    }
    
    setState(Idle);
    emit recordingStopped();
}

void MacroRecorder::pauseRecording()
{
    // For simplicity, we'll just stop recording when paused
    // A more sophisticated implementation could support true pause/resume
    if (m_state == Recording) {
        stopRecording();
    }
}

void MacroRecorder::resumeRecording()
{
    // Would continue recording from where it left off
    startRecording();
}

void MacroRecorder::playMacro()
{
    if (m_state != Idle || m_events.isEmpty()) {
        return;
    }
    
    m_currentEventIndex = 0;
    m_playbackStartTime = getCurrentTimestamp();
    m_playbackPauseOffset = 0;
    setState(Playing);
    
    emit playbackStarted();
    scheduleNextEvent();
}

void MacroRecorder::stopPlayback()
{
    if (m_state != Playing) {
        return;
    }
    
    m_playbackTimer->stop();
    setState(Idle);
    emit playbackStopped();
}

void MacroRecorder::pausePlayback()
{
    if (m_state == Playing) {
        m_playbackTimer->stop();
        m_playbackPauseOffset = getCurrentTimestamp() - m_playbackStartTime;
    }
}

void MacroRecorder::resumePlayback()
{
    if (m_state == Playing && !m_playbackTimer->isActive()) {
        m_playbackStartTime = getCurrentTimestamp() - m_playbackPauseOffset;
        scheduleNextEvent();
    }
}

void MacroRecorder::clearMacro()
{
    m_events.clear();
    emit macroCleared();
}

bool MacroRecorder::loadMacro(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file for reading:" << filename;
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) {
        qWarning() << "Failed to parse JSON from file:" << filename;
        return false;
    }
    
    QJsonObject root = doc.object();
    QJsonArray eventsArray = root["events"].toArray();
    
    clearMacro();
    
    for (const QJsonValue &value : eventsArray) {
        QJsonObject eventObj = value.toObject();
        MacroEvent event;
        event.type = static_cast<MacroEvent::Type>(eventObj["type"].toInt());
        event.position = QPoint(eventObj["x"].toInt(), eventObj["y"].toInt());
        event.timestamp = eventObj["timestamp"].toVariant().toLongLong();
        event.delayMs = eventObj["delay"].toInt();
        m_events.append(event);
    }
    
    return true;
}

bool MacroRecorder::saveMacro(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << filename;
        return false;
    }
    
    QJsonArray eventsArray;
    for (const MacroEvent &event : m_events) {
        QJsonObject eventObj;
        eventObj["type"] = static_cast<int>(event.type);
        eventObj["x"] = event.position.x();
        eventObj["y"] = event.position.y();
        eventObj["timestamp"] = QJsonValue::fromVariant(event.timestamp);
        eventObj["delay"] = event.delayMs;
        eventsArray.append(eventObj);
    }
    
    QJsonObject root;
    root["events"] = eventsArray;
    root["version"] = "1.0";
    
    QJsonDocument doc(root);
    file.write(doc.toJson());
    
    return true;
}

qint64 MacroRecorder::totalDuration() const
{
    if (m_events.isEmpty()) {
        return 0;
    }
    
    return m_events.last().timestamp - m_events.first().timestamp;
}

void MacroRecorder::onMousePositionChanged(const QPoint &position)
{
    if (m_state != Recording || !m_recordMouseMovement) {
        return;
    }
    
    // Only record significant movements to avoid cluttering
    QPoint diff = position - m_lastRecordedPosition;
    if (abs(diff.x()) > 5 || abs(diff.y()) > 5) {
        MacroEvent event(MacroEvent::MouseMove, position, getCurrentTimestamp());
        recordEvent(event);
        m_lastRecordedPosition = position;
    }
}

void MacroRecorder::onMouseClicked(Qt::MouseButton button, const QPoint &position)
{
    if (m_state != Recording) {
        return;
    }
    
    MacroEvent::Type eventType;
    switch (button) {
    case Qt::LeftButton:
        eventType = MacroEvent::LeftClick;
        break;
    case Qt::RightButton:
        eventType = MacroEvent::RightClick;
        break;
    default:
        return; // Ignore other buttons
    }
    
    MacroEvent event(eventType, position, getCurrentTimestamp());
    recordEvent(event);
}

void MacroRecorder::executeNextEvent()
{
    if (m_state != Playing || m_currentEventIndex >= m_events.size()) {
        stopPlayback();
        return;
    }
    
    const MacroEvent &event = m_events[m_currentEventIndex];
    
    switch (event.type) {
    case MacroEvent::MouseMove:
        m_mouseController->setMousePosition(event.position);
        break;
    case MacroEvent::LeftClick:
        m_mouseController->leftClick(event.position);
        break;
    case MacroEvent::RightClick:
        m_mouseController->rightClick(event.position);
        break;
    case MacroEvent::DoubleClick:
        m_mouseController->doubleClick(event.position);
        break;
    case MacroEvent::Delay:
        // Just wait - the timer will handle this
        break;
    }
    
    emit playbackProgress(m_currentEventIndex, m_events.size());
    
    m_currentEventIndex++;
    scheduleNextEvent();
}

void MacroRecorder::setState(State newState)
{
    if (m_state != newState) {
        m_state = newState;
        emit stateChanged(newState);
    }
}

void MacroRecorder::recordEvent(const MacroEvent &event)
{
    m_events.append(event);
}

void MacroRecorder::scheduleNextEvent()
{
    if (m_currentEventIndex >= m_events.size()) {
        stopPlayback();
        return;
    }
    
    int delay = 50; // Default delay between events
    
    if (m_currentEventIndex > 0) {
        qint64 timeDiff = m_events[m_currentEventIndex].timestamp - 
                         m_events[m_currentEventIndex - 1].timestamp;
        delay = static_cast<int>(timeDiff / m_playbackSpeed);
        delay = qMax(10, qMin(5000, delay)); // Clamp between 10ms and 5s
    }
    
    m_playbackTimer->start(delay);
}

qint64 MacroRecorder::getCurrentTimestamp() const
{
    return QDateTime::currentMSecsSinceEpoch();
}

#include "macrorecorder.moc"