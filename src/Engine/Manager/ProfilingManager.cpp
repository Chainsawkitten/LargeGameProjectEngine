#include "ProfilingManager.hpp"

#include <imgui.h>
#include <GLFW/glfw3.h>
#ifdef MEASURE_RAM
#include <windows.h>
#include <psapi.h>
#endif

#include <Video/Profiling/Query.hpp>
#include "../Utility/Log.hpp"

ProfilingManager::ProfilingManager() : active(false) {
    for (int i = 0; i < Type::COUNT; ++i) {
        first[i] = new Result("", nullptr);
        current[i] = nullptr;
    }

#ifdef MEASURE_VRAM
    dxgiFactory = nullptr;
    HRESULT error = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory));

    if (SUCCEEDED(error)) {
        IDXGIAdapter* firstAdapter;
        dxgiFactory->EnumAdapters(0, &firstAdapter);
        firstAdapter->QueryInterface(__uuidof(IDXGIAdapter3), (void**)&dxgiAdapter3);
    }
#endif
}

ProfilingManager::~ProfilingManager() {
    for (int i = 0; i < Type::COUNT; ++i) {
        delete first[i];
    }

#ifdef MEASURE_VRAM
    dxgiAdapter3->Release();
    dxgiFactory->Release();
#endif
}

void ProfilingManager::BeginFrame() {
    //if (!active) {
    //    Log() << "ProfilingManager::BeginFrame warning: Not active.\n";
    //    return;
    //}

    // Clear previous results.
    for (int i = 0; i < Type::COUNT; ++i) {
        first[i]->children.clear();
        first[i]->name = "";
        first[i]->duration = 0.0;
        current[i] = nullptr;
    }
    frameStart = glfwGetTime();
}

void ProfilingManager::ShowResults() {
    //if (!active) {
    //    Log() << "ProfilingManager::ShowResults warning: Not active.\n";
    //    return;
    //}
    
    // Calculate the time of this frame.
    frameTimes[frame++] = static_cast<float>((glfwGetTime() - frameStart) * 1000.0);
    if (frame >= frames)
        frame = 0;

    ResolveQueries();
    
    // Show the results.
    ImGui::Begin("Profiling", nullptr, ImGuiWindowFlags_ShowBorders);
    
    ImGui::Checkbox("Sync GPU and CPU", &syncGPU);
    
    if (ImGui::CollapsingHeader("Frametimes"))
        ShowFrametimes();
    
    if (ImGui::CollapsingHeader("CPU Breakdown")) {
        ImGui::Columns(2);
        ShowResult(first[Type::CPU]);
        ImGui::Columns(1);
    }

    if (ImGui::CollapsingHeader("GPU Breakdown")) {
        ImGui::Columns(2);
        ShowResult(first[Type::GPU]);
        ImGui::Columns(1);
    }
    
    if (ImGui::CollapsingHeader("Memory")) {
#ifdef MEASURE_RAM
        PROCESS_MEMORY_COUNTERS_EX memoryCounters;
        GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&memoryCounters), sizeof(memoryCounters));
        ImGui::Text("RAM: %u MiB", static_cast<unsigned int>(memoryCounters.PrivateUsage / 1024 / 1024));
#endif
        
#ifdef MEASURE_VRAM
        DXGI_QUERY_VIDEO_MEMORY_INFO info;
        dxgiAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info);
        unsigned int memoryUsage = info.CurrentUsage;
        ImGui::Text("VRAM: %u MiB", memoryUsage / 1024 / 1024);
#endif
    }
    
    ImGui::End();
}

bool ProfilingManager::Active() const {
    return active;
}

void ProfilingManager::SetActive(bool active) {
    this->active = active;
}

ProfilingManager::Result* ProfilingManager::StartResult(const std::string& name, Type type) {
    //assert(active);

    // Sync GPU and CPU.
    if (syncGPU && type == Type::CPU) {
        glFinish();
    }

    if (current[type] == nullptr) {
        first[type]->name = name;
        first[type]->parent = nullptr;
        current[type] = first[type];
    } else {
        current[type]->children.push_back(ProfilingManager::Result(name, current[type]));
        Result* result = &current[type]->children.back();
        current[type] = result;
    }

    // Begin query if type is GPU.
    if (type == Type::GPU) {
        // Find available query.
        Video::Query* query;
        if (queryPool.empty())
            query = new Video::Query(Video::Query::TIME_ELAPSED);
        else {
            query = queryPool.back();
            queryPool.pop_back();
        }
        queryMap[current[type]] = query;
        query->Begin();
    }
    
    return current[type];
}

void ProfilingManager::FinishResult(Result* result, Type type) {
    //assert(active);

    // Sync GPU and CPU.
    if (syncGPU && type == Type::CPU) {
        ProfilingManager::Result gpuFinish("GPU Finish", current[type]);
        double gpuFinishStart = glfwGetTime();
        glFinish();
        gpuFinish.duration = glfwGetTime() - gpuFinishStart;
        result->children.push_back(gpuFinish);
    }

    // End query if type is GPU.
    if (type == Type::GPU) {
        queryMap[current[type]]->End();
    }

    current[type] = result->parent;
}

void ProfilingManager::ShowFrametimes() {
    //assert(active);

    ImGui::PlotLines("Frametimes", frameTimes, frames, 0, nullptr, 0.f, FLT_MAX, ImVec2(0.f, 300.f));
}

void ProfilingManager::ShowResult(Result* result) {
    //assert(active);

    ImGui::AlignFirstTextHeightToWidgets();
    int flags = result->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0;
    bool expanded = ImGui::TreeNodeEx(result->name.c_str(), flags);
    
    ImGui::NextColumn();
    if (result->parent != nullptr) {
        ImGui::ProgressBar(result->duration / result->parent->duration, ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    }
    ImGui::Text((std::to_string(result->duration * 1000.0) + " ms").c_str());
    ImGui::NextColumn();
    
    if (expanded) {
        double otherTime = result->duration;
        for (Result& child : result->children) {
            ShowResult(&child);
            otherTime -= child.duration;
        }
        
        if (!result->children.empty()) {
            Result other("Other", result);
            other.duration = otherTime;
            ShowResult(&other);
        }
        
        ImGui::TreePop();
    }
}

void ProfilingManager::ResolveQueries() {
    //assert(active);

    // Resolve & reset GPU queries.
    for (auto& it : queryMap) {
        it.first->duration = it.second->Resolve() / 1000000000.0;
        queryPool.push_back(it.second);
    }
    queryMap.clear();
}

ProfilingManager::Result::Result(const std::string& name, Result* parent) : name (name) {
    this->parent = parent;
}
