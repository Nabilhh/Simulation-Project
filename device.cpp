#include "device.h"
#include <QTimer>
#include <stdexcept>
#include <iostream>

Device::Device(QObject *parent) : QObject(parent)
{
    // Create sessions
    initSessions();
}

Device::~Device()
{
    // Free heap memory from sessions
    while (sessions.size() > 0) {
        delete sessions.takeLast();
    }
}

void Device::powerOn()
{
    if (isPoweredOn) return;

    // Set device to on
    isPoweredOn = true;

    // Set inactivity timer (if no selection is selected, turns off device)
    currentTimer = new QTimer(this);
    connect(currentTimer, SIGNAL(timeout()), this, SLOT(inactivityCallback()));
    currentTimer->start(1000 * INACTIVITY_TIME_SCALING_FACTOR);
}

void Device::powerOff()
{
    // Turn off
    time = 0;

    // Reset inactivity timer gui display
    emit inactivityTimerUpdate(0);
    isPoweredOn = false;

    // We are not running a session
    currentSession = NULL;
    delete currentTimer;
}

void Device::decreaseIntensity()
{
    if (!isPoweredOn) return;
    currentSession->decreaseIntensity();
}

void Device::increaseIntensity()
{
    if (!isPoweredOn) return;
    currentSession->increaseIntensity();
}

void Device::saveIntensity()
{
    currentSession->saveIntensity();
}

void Device::selectSession(LengthGroup_t lg, Wave_t w)
{
    if (currentSession != NULL) return;

    bool sessionFound = false;

    // Search for the session with the provided length group and wave type
    // No session has the same length group AND wave type, so we can use these
    // properties to uniquely identify a session.
    for (int i = 0; i < sessions.size(); i++) {
        if (sessions.at(i)->group == lg && sessions.at(i)->wave == w) {
            startSession(sessions.at(i));
            sessionFound = true;
            break;
        }
    }

    if (!sessionFound)
        std::cout << "Warning: a session that was selected could not be found." << std::endl;
}

Session* Device::getCurrentSession()
{
    return currentSession;
}

void Device::startSession(Session *s)
{
    // Update currentSession to point to the session we will run
    currentSession = s;

    // Stop the inactivity timer & reset the gui display for it
    currentTimer->stop();
    emit inactivityTimerUpdate(0);
    delete currentTimer;

    currentTimer = new QTimer(this);
    currentTimer->setSingleShot(true); // Sessions should only run once, and then end
    connect(currentTimer, SIGNAL(timeout()), this, SLOT(endSession()));

    // Multiply the session length by 1000 since its stored in seconds
    currentTimer->start(s->getLength() * 1000 * SESSION_TIME_SCALING_FACTOR);
}

void Device::stopSession(){
    // We are not running a session, clear the pointer
    currentSession = NULL;
    currentTimer->stop();
    time = 0;
    delete currentTimer;

    // Restart inactivity timer
    currentTimer = new QTimer(this);
    connect(currentTimer, SIGNAL(timeout()), this, SLOT(inactivityCallback()));
    currentTimer->start(1000 * INACTIVITY_TIME_SCALING_FACTOR);
}

void Device::endSession() {
    currentSession = NULL;
    delete currentTimer;
    currentTimer = NULL;

    // Tell the GUI we are turning off
    emit powerOffSignal();
}

bool Device::isSessionInProgress()
{
    if (currentSession == NULL) {
        return false;
    } else {
        return true;
    }
}

void Device::pauseSession(){
    isPaused = true;
    pauseTime = currentTimer->remainingTime();
    currentTimer->stop();
    //set timer to turn off device because of inactivity
    currentTimer->start(60000);
}

void Device::resumeSession(){
    isPaused = false;
    //stop inactivity time and set to remianing time
    currentTimer->stop();
    currentTimer->start(pauseTime);
}

bool Device::isSessionPaused(){
    return isPaused;
}

bool Device::isDeviceOn()
{
    return isPoweredOn;
}

void Device::inactivityCallback()
{
    time += 1;

    // If time has run out, stop and power off
    // Otherwise, update the GUI timer display
    if (time >= INACTIVITY_TIME_LIMIT) {
        time = 0;
        currentTimer->stop();
        emit powerOffSignal();
    } else {
        emit inactivityTimerUpdate(time);
    }
}

void Device::initSessions()
{
    // Create default sessions
    sessions.append(new Session(LENGTH_20MIN, WAVE_ALPHA));
    sessions.append(new Session(LENGTH_20MIN, WAVE_BETA));
    sessions.append(new Session(LENGTH_20MIN, WAVE_THETA));
    sessions.append(new Session(LENGTH_20MIN, WAVE_100HZ));
    sessions.append(new Session(LENGTH_45MIN, WAVE_ALPHA));
    sessions.append(new Session(LENGTH_45MIN, WAVE_BETA));
    sessions.append(new Session(LENGTH_45MIN, WAVE_THETA));
    sessions.append(new Session(LENGTH_45MIN, WAVE_100HZ));

    // Create user-defined sessions
    sessions.append(new Session(WAVE_ALPHA, 5*60)); // 5min length
    sessions.append(new Session(WAVE_BETA, 10*60)); // 10min
    sessions.append(new Session(WAVE_THETA, 5*60)); // 5min
    sessions.append(new Session(WAVE_100HZ, 15*60)); // 15min
}

//Determines connection based upon wire connectivity and that of individual electrodes
int Device::getConnection(){
    if(isWired == false){
        return NO_CONNECTION;
    }
    int left = leftElectrode.getConnection();
    int right = rightElectrode.getConnection();

    if(left < right){
        return left;
    } else {
        return right;
    }
}

//sets both left and right earclip electrode
void Device::setConnection(int status){
    setConnectionLeft(status);
    setConnectionRight(status);
}

void Device::setConnectionLeft(int status){
    leftElectrode.setConnection(status);
}

void Device::setConnectionRight(int status){
    rightElectrode.setConnection(status);
}

bool Device::getWire(){
    return isWired;
}

void Device::setWire(bool wire){
    isWired = wire;
}

//Lowers battery by set low amount as in case when device is on but there is no active session
void Device::lowerBattery(){
    battery -= 0.01;
}

//Lowers battery as a function of intensity for when session is active
void Device::lowerBattery(double intensity){
    battery -= (0.05 * intensity);
}

void Device::setBattery(double value){
    battery = value;
}

double Device::getBattery(){
    return battery;
}

bool Device::isPower(){
    return isPoweredOn;
}

//returns length of a custom session in minutes
int Device::getCustomSessionLength(Wave_t w) {
    for (int i = 0; i < sessions.size(); i++) {
        if (sessions.at(i)->wave == w && sessions.at(i)->group == LENGTH_CUSTOM) {
            return sessions.at(i)->getLength() / 60;
        }
    }
    return -1;
}
