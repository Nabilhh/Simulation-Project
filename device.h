#ifndef DEVICE_H
#define DEVICE_H

#include "electrode.h"
#include <QObject>
#include <QTimer>
#include <QVector>
#include "session.h"

// measured in seconds
#define INACTIVITY_TIME_LIMIT 121
#define INACTIVITY_TIME_SCALING_FACTOR 0.2

// Session time is scaled to 1%
#define SESSION_TIME_SCALING_FACTOR 0.01

/*
 * Device exists to store device state information outside of the gui, and to
 * perform functionality that should not depend on input methods (eg. session selection, battery
 * depletion, connection detection, session storage).
 */
class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = nullptr);
    ~Device();
    
    // fills sessionLengthGroups with the treatments that are provided
    // with the device upon purchase
    void initSessions();

    // This returns a pointer to the currently running session. This
    // can be helpful in the gui, since it doesn't have to store duplicate information
    // if it can access the current session info directly.
    Session* getCurrentSession();

    //Getter for where device is powered on
    bool isPower();

    // Methods to change the intensity while a session is running
    void increaseIntensity();
    void decreaseIntensity();

    // Start a session with properties specified in the Session object
    // uniquely identified by the provided length group and wave type
    void selectSession(LengthGroup_t, Wave_t);
    bool isSessionInProgress();
    bool isDeviceOn();

    // Turns device on, loads sessions, starts inactivity timer
    void powerOn();
    void powerOff();

    // Functions related to pausing a session upon dis/reconnection
    void pauseSession();
    void resumeSession();
    bool isSessionPaused();
    //differs from endSession as it does not emit powerOff
    void stopSession();

    // Search through sessions and return the length in seconds of
    // the session with LENGTH_CUSTOM and the given wave type
    int getCustomSessionLength(Wave_t);

    //Functions related to the connection of the earclips as well as the wire itself
    int getConnection();
    void setConnection(int);
    void setConnectionLeft(int);
    void setConnectionRight(int);
    void setWire(bool);
    bool getWire();

    // Functions to change/get battery level
    void setBattery(double);
    void lowerBattery();
    void lowerBattery(double);
    double getBattery();
signals:


public slots:
    void inactivityCallback();  // Called each second while a session has not been selected
    void endSession();

    // This makes the current intensity the new default for this session type
    void saveIntensity();

signals:
    void inactivityTimerUpdate(int t); // Update the inactivity timer GUI representation
    void powerOffSignal();   // Tell GUI we are powering off

private:
    Electrode leftElectrode;
    Electrode rightElectrode;
    bool isPoweredOn = false;
    bool isWired = true;
    bool isPaused = false;
    int time = 0;  // keeps track of elapsed inactivity time (seconds)
    int pauseTime = 0;
    double battery = 100.00;

    // Sessions that are provided with the device
    QVector<Session*> sessions;

    // A pointer to the current session, if we are not running a session, then NULL
    Session* currentSession = NULL;

    // This stores:
    //     - the inactivity timer, when no selection has been made yet
    //     - the session timer, once a session has begun
    QTimer* currentTimer = NULL;

    // This is a helper function for the session selection methods.
    // It places a new timer in currentTimer and deletes the previous one.
    // The new timer will call endSession() on timeout.
    void startSession(Session*);
};

#endif // DEVICE_H
