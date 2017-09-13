#pragma once

#include <string>
#include <ctime>
#include <glm/glm.hpp>

/// Logging class.
/**
 * Usage:
 * @code{.cpp}
 * Log() << "Testing: " << 5 << "\n";
 * @endcode
 */
class Log {
    public:
        /// Destructor.
        ~Log();
        
        /// Output some text to stderr.
        /**
         * @param text Text to output.
         * @return The %Log instance
         */
        Log& operator<<(std::string text);
        
        /// Output an integer to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance
         */
        Log& operator<<(int value);
        
        /// Output an unsigned integer to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance.
         */
        Log& operator<<(unsigned int value);
        
        /// Output a float to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance
         */
        Log& operator<<(float value);
        
        /// Output a double to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance.
         */
        Log& operator<<(double value);
        
        /// Output a time to stderr.
        /**
         * Formatted Y-m-d H:M:S.
         * @param value Value to output.
         * @return The %Log instance
         */
        Log& operator<<(time_t value);
        
        /// Output a vec2 to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance.
         */
        Log& operator<<(const glm::vec2& value);
        
        /// Output a vec3 to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance.
         */
        Log& operator<<(const glm::vec3& value);
        
        /// Output a vec4 to stderr.
        /**
        * @param value Value to output.
        * @return The %Log instance.
        */
        Log& operator<<(const glm::vec4& value);
};
