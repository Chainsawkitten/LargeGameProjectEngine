#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Engine/MainWindow.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ScriptManager.hpp>
#include <Engine/Manager/ProfilingManager.hpp>
#include <Engine/Manager/RenderManager.hpp>
#include <Engine/Manager/SoundManager.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Input/Input.hpp>
#include <Utility/Log.hpp>
#include <thread>
#include <iostream>
#include <fstream>
#include <ctime>
#include "Util/GameSettings.hpp"

int main(int argc, char* argv[]) {
    if (!glfwInit())
        return 1;

    const int maxRamLimit = 256;
    const int maxVramLimit = 512;
    
    Log().SetupStreams(&std::cout, &std::cout, &std::cout, &std::cerr);

    Log() << "Game started - " << time(nullptr) << "\n";

    bool testing = false;

    // Quick fix in order to implement a testing parameter, 
    for(int i = 1; i < argc; i++) {
        if(std::string(argv[i]) == "t") {
            testing = true;
            Log() << "Frame and memory testing enabled\n";
        }
    }
    
    int numberOfBadFrames = 0;
    double maxFrameTime = 0.0;
    double totalFrameTime = 0.0;
    double averageFrameTime = 0.0;
    int ramUsed = 0;
    int vramUsed = 0;
    unsigned int maxRamUsed = 0;
    unsigned int maxVramUsed = 0;

    Log() << "Initializing main window\n";
    MainWindow* window = new MainWindow(1920, 1080, true, false, "Hymn to Beauty", false);
    glewInit();
    window->Init(false);
    unsigned int numberOfFrames = 1;
    Input::GetInstance().SetWindow(window->GetGLFWWindow());
    
    Managers().StartUp();

    Log() << "Loading hymn\n";
    // Load hymn from json file.
    Hymn().Load(".");
    
    // Load game settings from ini file.
    GameSettings::GetInstance().Load();
    Managers().renderManager->SetTextureReduction(GameSettings::GetInstance().GetLong("Texture Reduction"));
    
    Log() << "Loading world\n";
    // Load world.
    Hymn().world.Load(Hymn().GetPath() + "/" + Hymn().startupScene + ".json");

    Log() << "Compiling scripts\n";
    // Compile scripts.
    Managers().scriptManager->RegisterInput();
    Managers().scriptManager->BuildAllScripts();

    // Create audio environment.
    Managers().soundManager->CreateAudioEnvironment();
    
    // Main loop.
    double targetFPS = 60.0;
    double lastTime = glfwGetTime();
    double lastTimeRender = glfwGetTime();
    Log() << "Main loop starting\n";
    while (!window->ShouldClose() && numberOfFrames < 2400) {
        double deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();

        window->Update();
        Hymn().Update(static_cast<float>(deltaTime));
        Hymn().Render();

    if(testing)
        glFinish();
        
        // Swap buffers and wait until next frame.
        window->SwapBuffers();
        
        if(testing) {
            // Frame measurements.
            double frameTime = (glfwGetTime() - lastTimeRender);
            totalFrameTime += frameTime;
            averageFrameTime = (totalFrameTime / numberOfFrames) * 1000.0;

            if (frameTime > maxFrameTime)
                maxFrameTime = frameTime;

            if (frameTime * 1000.0 > 32.0)
                numberOfBadFrames++;
        }

        long wait = static_cast<long>((1.0 / targetFPS + lastTimeRender - glfwGetTime()) * 1000000.0);
        if (wait > 0)
            std::this_thread::sleep_for(std::chrono::microseconds(wait));
        lastTimeRender = glfwGetTime();
        
        // Get input.
        glfwPollEvents();

        if(testing) {
            ramUsed = Managers().profilingManager->MeasureRAM();
            vramUsed = Managers().profilingManager->MeasureVRAM();

        if (ramUsed > maxRamLimit && ramUsed > maxRamUsed)
                maxRamUsed = ramUsed;
            
        if (vramUsed > maxVramLimit && vramUsed > maxVramUsed)
                maxVramUsed = vramUsed;

        numberOfFrames++;
        }
    }
    
    if( testing ) {
        std::fstream myfile("Log1080_FullGame.txt", std::ios::out);
        if (myfile) {
            myfile << "Frame rundown:\n";
            myfile << "Frames: " << numberOfFrames << "\n";
            myfile << "Bad frames: " << numberOfBadFrames << "\n";
            myfile << "Percentage of bad frames: " << (numberOfBadFrames / static_cast<double>(numberOfFrames))*100.0 << "%\n";
            myfile << "Average frame time: " << averageFrameTime << " ms\n";
            myfile << "Max frame time: " << maxFrameTime * 1000.0 << " ms\n";
            myfile.close();
        }
        Log() << "Frame rundown:\n";
        Log() << "Frames: " << numberOfFrames << "\n";
        Log() << "Bad frames: " << numberOfBadFrames << "\n";
        Log() << "Percentage of bad frames: " << (numberOfBadFrames / static_cast<double>(numberOfFrames))*100.0 << "%\n";
        Log() << "Average frame time: " << averageFrameTime << " ms\n";
        Log() << "Max frame time: " << maxFrameTime * 1000.0 << " ms\n";
    }

    // Save game settings.
    GameSettings::GetInstance().Save();

    Hymn().world.Clear();
    Managers().ShutDown();
    
    delete window;
    
    glfwTerminate();

    int returnValue = 0;

    if ( maxRamUsed > maxRamLimit ) {
        Log() << "Ram limit exceeded.\n";
        returnValue = returnValue | 1;
    }
    
    if ( maxVramUsed > maxVramLimit ) {
        Log() << "Vram limit exceeded.\n";
        returnValue = returnValue | 2;
    }

    if ( (numberOfBadFrames / static_cast<double>(numberOfFrames))*100.0 > 5 ) {
        Log() << "Frame limit exceeded.\n";
        returnValue = returnValue | 4;
    }

    Log() << "Game ended - " << time(nullptr) << "\n";
    
    return returnValue;
}
