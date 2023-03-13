#include "session.h"

Session::Session(LengthGroup_t length_group, Wave_t wave_type, int new_default_intensity)
    : group(length_group), wave(wave_type), id(nextId++)
{
    // For this group, use the other constructor
    if (group == LENGTH_CUSTOM) {
        throw std::invalid_argument("length_group cannot be LENGTH_CUSTOM for this constructor");
    }

    // Check validity of input for default intensity
    if (new_default_intensity < MIN_INTENSITY) {
        defaultIntensity = MIN_INTENSITY;
        intensity = MIN_INTENSITY;
    } else if (new_default_intensity > MAX_INTENSITY) {
        defaultIntensity = MAX_INTENSITY;
        intensity = MAX_INTENSITY;
    } else {
        defaultIntensity = new_default_intensity;
        intensity = new_default_intensity;
    }

    // Set the length in seconds based on length group
    length = lengthGroupToSeconds(group);
}

Session::Session(Wave_t wave_type, int session_length, int new_default_intensity)
    : group(LENGTH_CUSTOM), wave(wave_type), id(nextId++)
{
    // Check validity of session length input
    if (session_length <= 0 || session_length > MAX_SESSION_LENGTH) {
        throw std::invalid_argument("length is out of bounds");
    }

    // Check validity of input for default intensity
    if (new_default_intensity < MIN_INTENSITY) {
        defaultIntensity = MIN_INTENSITY;
        intensity = MIN_INTENSITY;
    } else if (new_default_intensity > MAX_INTENSITY) {
        defaultIntensity = MAX_INTENSITY;
        intensity = MAX_INTENSITY;
    } else {
        defaultIntensity = new_default_intensity;
        intensity = new_default_intensity;
    }

    length = session_length;
}

void Session::increaseIntensity()
{
    if (intensity == MAX_INTENSITY) {
        return;
    }
    intensity += 1;
}

void Session::decreaseIntensity()
{
    if (intensity == MIN_INTENSITY) {
        return;
    }
    intensity -= 1;
}

void Session::saveIntensity()
{
    defaultIntensity = intensity;
}

int Session::getLength()
{
    return length;
}

int Session::getIntensity()
{
    return intensity;
}

int Session::getDefaultIntensity()
{
    return defaultIntensity;
}

void Session::resetIntensity()
{
    intensity = defaultIntensity;
}

int Session::lengthGroupToSeconds(LengthGroup_t l) {
    switch (l) {
        case LENGTH_20MIN:
            return 20*60;
        case LENGTH_45MIN:
            return 45*60;
        case LENGTH_3HRS:
            return 3*60*60;
        default:
            return -1;
    }
}



