#pragma once

#include <string>
#include "../Manager/Managers.hpp"
#include "../Manager/ProfilingManager.hpp"
#include "../linking.hpp"

/// Run profiling.
class Profiling {
    public:
    /// Start profiling.
    /**
         * @param name Name of the segment.
         */
    ENGINE_API Profiling(const std::string& name);

    /// End profiling.
    ENGINE_API ~Profiling();

    private:
    ProfilingManager::Result* result;
    double start;
};

#define PROFILE(name) Profiling __profileInstance(name)
