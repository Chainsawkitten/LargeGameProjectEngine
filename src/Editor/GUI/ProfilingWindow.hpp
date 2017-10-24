#pragma once

#include <Engine/Manager/ProfilingManager.hpp>

namespace GUI {
    class ProfilingWindow {
        public:
        /// Show profiling results.
        void Show();

        private:
        void ShowResult(ProfilingManager::Result* result);
    };
}
