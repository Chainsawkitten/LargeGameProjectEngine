#pragma once

#include <string>
#include <list>
#include <map>

#include <Video/Profiling/Query.hpp>

#ifdef MEASURE_VRAM
#include <d3d11_3.h>
#include <dxgi1_4.h>
#undef CreateDirectory
#endif

/// Handles profiling.
class ProfilingManager {
    friend class Hub;
    friend class Profiling;
    friend class GPUProfiling;
    
    public:
        /// Begin profiling a frame.
        void BeginFrame();
        
        /// Show the results of the profiling.
        void ShowResults();

        /// Check whether %ProfilingManager is active.
        /**
         * @return Active state.
         */
        bool Active() const;

        /// Set whether %ProfilingManager is active.
        /**
         * @param active Active state.
         */
        void SetActive(bool active);
        
    private:
        ProfilingManager();
        ~ProfilingManager();
        ProfilingManager(ProfilingManager const&) = delete;
        void operator=(ProfilingManager const&) = delete;
        
        enum Type {
            CPU_TIME = 0,
            GPU_TIME_ELAPSED,
            GPU_SAMPLES_PASSED,
            COUNT
        };

        struct Result {
            std::string name;
            double value = 0.0;
            std::list<Result> children;
            Result* parent;
            
            Result(const std::string& name, Result* parent);
        };
        
        Result* StartResult(const std::string& name, Type type);
        void FinishResult(Result* result, Type type);
        
        void ShowResult(Result* result);

        std::string TypeToString(Type type) const;

        bool active;
        
        Result* root[Type::COUNT];
        Result* current[Type::COUNT];

        std::map<Video::Query::Type, std::list<Video::Query*>> queryPool;
        std::map<Result*, Video::Query*> queryMap;
        
        Video::Query* frameQuery;
        double frameStart;
        static const unsigned int frames = 100;
        unsigned int frame = 0;
        float frameTimes[2][frames];

#ifdef MEASURE_VRAM
        IDXGIFactory* dxgiFactory = nullptr;
        IDXGIAdapter3* dxgiAdapter3 = nullptr;
#endif
};
