#include "Managers.hpp"

#include "ResourceManager.hpp"
#include "RenderManager.hpp"
#include "ParticleManager.hpp"
#include "PhysicsManager.hpp"
#include "SoundManager.hpp"
#include "ScriptManager.hpp"
#include "DebugDrawingManager.hpp"
#include "ProfilingManager.hpp"
#include "VRManager.hpp"
#include "TriggerManager.hpp"

#include "Utility/Log.hpp"

#include "../Component/Animation.hpp"

#ifdef USINGMEMTRACK
#include <MemTrackInclude.hpp>
#endif

Hub::Hub() {
    
}

Hub& Managers() {
    static Hub instance;
    
    return instance;
}

void Hub::StartUp() {
    Log() << "Resource manager\n";
    resourceManager = new ResourceManager();
    Log() << "VR manager\n";
    vrManager = new VRManager();
    Log() << "Render manager\n";
    renderManager = new RenderManager();
    Log() << "Particle manager\n";
    particleManager = new ParticleManager();
    Log() << "Physics manager\n";
    physicsManager = new PhysicsManager();
    Log() << "Sound manager\n";
    soundManager = new SoundManager();
    Log() << "Script manager\n";
    scriptManager = new ScriptManager();
    Log() << "Debug drawing manager\n";
    debugDrawingManager = new DebugDrawingManager();
    Log() << "Profiling manager\n";
    profilingManager = new ProfilingManager();
    Log() << "Trigger manager\n";
    triggerManager = new TriggerManager();
}

void Hub::ShutDown() {
    delete triggerManager;
    delete profilingManager;
    delete debugDrawingManager;
    delete scriptManager;
    delete soundManager;
    delete renderManager;
    delete vrManager;
    delete particleManager;
    delete physicsManager;
    delete resourceManager;
    
    shutdown = true;
}

void Hub::ClearKilledComponents() {
    if (!shutdown) {
        triggerManager->ClearKilledComponents();
        renderManager->ClearKilledComponents();
        particleManager->ClearKilledComponents();
        physicsManager->ClearKilledComponents();
        soundManager->ClearKilledComponents();
        scriptManager->ClearKilledComponents();
        vrManager->ClearKilledComponents();
    }
}
