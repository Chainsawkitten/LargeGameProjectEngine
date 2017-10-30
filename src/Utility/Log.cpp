#include "Log.hpp"

#include <ostream>
#include <iostream>

using namespace std;

ostream* Log::streams[NUMBER_OF_CHANNELS];

Log::Log(const Channel channel) {
    currentChannel = channel;
}

Log::~Log() {
    fflush(stderr);
}

Log& Log::operator<<(const string text) {
    *streams[currentChannel] << text;

    #ifdef LOGTESTING 
    if(currentChannel == ERR)
        throw(text);
    #endif

    return *this;
}

Log& Log::operator <<(const int value) {
    *streams[currentChannel] << value;

    #ifdef LOGTESTING 
        if (currentChannel == ERR)
            throw("Error: i " + std::to_string(value));
    #endif

    return *this;
}

Log& Log::operator <<(const unsigned int value) {
    *streams[currentChannel] << value;

    #ifdef LOGTESTING 
        if (currentChannel == ERR)
            throw("Error: u " + std::to_string(value));
    #endif

    return *this;
}

Log& Log::operator <<(const float value) {
    *streams[currentChannel] << value;

    #ifdef LOGTESTING 
        if (currentChannel == ERR)
            throw("Error: f " + std::to_string(value));
    #endif

    return *this;
}

Log& Log::operator <<(const double value) {
    *streams[currentChannel] << value;

    #ifdef LOGTESTING 
        if (currentChannel == ERR)
            throw("Error: d " + std::to_string(value));
    #endif

    return *this;
}

Log& Log::operator <<(const time_t value) {
    struct tm * timeinfo = localtime(&value);

    const unsigned int bufferLength = 24;

    char buffer[bufferLength] = {'\0'};
    strftime(buffer, bufferLength, "%Y-%m-%d %H:%M:%S", timeinfo);
    string const outString = string(buffer, bufferLength);

    #ifdef LOGTESTING 
        if (currentChannel == ERR)
            throw("Error: t " + outString);
    #endif

    *streams[currentChannel] << outString;

    return *this;
}

Log& Log::operator <<(const glm::vec2& value) {
    *streams[currentChannel] << "(" << value.x << "," << value.y << ")";

    #ifdef LOGTESTING 
        if (currentChannel == ERR)
            throw("Error: vec2 x: " + std::to_string(value.x) + " y: " + std::to_string(value.y));
    #endif

    return *this;
}

Log& Log::operator <<(const glm::vec3& value) {
    *streams[currentChannel] << "(" << value.x << "," << value.y << "," << value.z << ")";

    #ifdef LOGTESTING 
        if (currentChannel == ERR)
            throw("Error: vec3 x: " + std::to_string(value.x) + " y: " + std::to_string(value.y) + " z: " + std::to_string(value.z));
    #endif

    return *this;
}

Log & Log::operator<<(const glm::vec4 & value) {
    *streams[currentChannel] << "(" << value.x << "," << value.y << "," << value.z << "," << value.w << ")";

    #ifdef LOGTESTING 
        if (currentChannel == ERR)
            throw("Error: vec3 x: " + std::to_string(value.x) + " y: " + std::to_string(value.y) + " z: " + std::to_string(value.z) + " w: " + std::to_string(value.w));
    #endif

    return *this;
}

bool Log::SetupStream(const Channel channel, std::ostream* stream) {
    // Check if channel is outside the range of available channels
    if(channel < DEFAULT || channel >= NUMBER_OF_CHANNELS) {
        std::cout << "Error channel: out of range.";
        return false;
    }

    // Set the channel.
    streams[static_cast<int>(channel)] = stream;
    return true;
}

bool Log::SetupStreams(
        std::ostream* defaultStream, 
        std::ostream* info, 
        std::ostream* warning, 
        std::ostream* error, 
        std::ostream* console) {
    return  SetupStream(DEFAULT, defaultStream)    &&
            SetupStream(INFO, info)                &&
            SetupStream(WARNING, warning)          &&
            SetupStream(ERR, error)                &&
            SetupStream(DEBUG, console);
}
