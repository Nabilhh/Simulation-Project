#ifndef SESSION_H
#define SESSION_H

#include <stdexcept>
#include <QString>

// Arbitrarily sets a maximum session length of
#define MAX_SESSION_LENGTH 36000
#define MAX_INTENSITY 8
#define MIN_INTENSITY 1

static int nextId = 0;

enum LengthGroup_t {
    LENGTH_20MIN,
    LENGTH_45MIN,
    LENGTH_3HRS,
    LENGTH_CUSTOM
};

enum Wave_t {
    WAVE_MET,
    WAVE_SUBDELTA,
    WAVE_DELTA,
    WAVE_THETA,
    WAVE_ALPHA,
    WAVE_SMR,
    WAVE_BETA,
    WAVE_100HZ
};

/*
 * This stores the information needed to execute a session. This includes
 * the length catergory, wave type, length, current intensity, and the starting
 * intensity for new sessions.
 */
class Session
{
public:
    // Length category which the session should be displayed in
    const LengthGroup_t group;

    // Wave type for this session
    const Wave_t wave;

    // It could be useful to uniquely identify sessions.
    const int id;

    //these both throw invalid_argument exceptions
    // Constructor for sessions with lengths LENGTH_20MIN LENGTH_45MIN LENGTH_3HRS
    Session(LengthGroup_t, Wave_t, int = MIN_INTENSITY);
    // Constructor for sessions with custom length (user-defined sessions) LENGTH_CUSTOM
    Session(Wave_t, int, int = MIN_INTENSITY);

    // increase or decrease intensity by 1 (of whatever unit)
    void increaseIntensity();
    void decreaseIntensity();

    // updates the record file with a new default intensity for this session #
    void saveIntensity();
    int getIntensity();
    int getDefaultIntensity();
    //
    void resetIntensity();

    // get length in seconds
    int getLength();

    static int lengthGroupToSeconds(LengthGroup_t l);

private:
    // Intensity while the session is running, this can change without affecting the
    // starting intensity next time the session is run.
    int intensity;

    // Starting intensity for when this session begins.
    int defaultIntensity;

    // Length of the session, measured in seconds
    int length;
};

#endif // SESSION_H
