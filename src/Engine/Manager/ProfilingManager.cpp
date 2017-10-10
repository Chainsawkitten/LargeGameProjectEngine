#include "ProfilingManager.hpp"

#include <imgui.h>
#include <GLFW/glfw3.h>
#ifdef MEASURE_RAM
#include <windows.h>
#include <psapi.h>
#endif

#include "../Utility/Log.hpp"

ProfilingManager::ProfilingManager() : active(false) {
    for (int i = 0; i < Type::COUNT; ++i) {
        root[i] = new Result("Root: " + TypeToString((Type)i), nullptr);
        root[i]->parent = nullptr;
    }

    frameQuery = new Video::Query(Video::Query::TIME_ELAPSED);

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
        delete root[i];
    }

    delete frameQuery;

#ifdef MEASURE_VRAM
    dxgiAdapter3->Release();
    dxgiFactory->Release();
#endif
}

void ProfilingManager::BeginFrame() {
    if (!active) {
        Log() << "ProfilingManager::BeginFrame warning: Not active.\n";
        return;
    }

    // Clear previous results.
    for (int i = 0; i < Type::COUNT; ++i) {
        root[i]->children.clear();
        current[i] = root[i];
    }
    frameStart = glfwGetTime();
    frameQuery->Begin();
}

void ProfilingManager::ShowResults() {
    if (!active) {
        Log() << "ProfilingManager::ShowResults warning: Not active.\n";
        return;
    }
    
    // Calculate the CPU time of this frame.
    frameTimes[0][frame] = static_cast<float>((glfwGetTime() - frameStart) * 1000.0);

    // Calculate the GPU time of this frame.
    frameQuery->End();
    frameTimes[1][frame] = static_cast<float>(frameQuery->Resolve() / 1000000.0);

    if (++frame >= frames)
        frame = 0;

    // Resolve and reset queries.
    for (auto& it : queryMap) {
        switch (it.second->GetType()) {
            case Video::Query::Type::TIME_ELAPSED:
                it.first->value = it.second->Resolve() / 1000000.0;
                break;
            case Video::Query::Type::SAMPLES_PASSED:
                it.first->value = it.second->Resolve();
                break;
            default:
                assert(false);
                break;
        }
        queryPool[it.second->GetType()].push_back(it.second);
    }
    queryMap.clear();
    
    // Show the results.
    ImGui::Begin("Profiling", nullptr, ImGuiWindowFlags_ShowBorders);
    
    // Frametimes.
    if (ImGui::CollapsingHeader("Frametimes")) {
        if (ImGui::TreeNode("CPU Frametimes")) {
            ImGui::PlotLines("Frametimes in ms", frameTimes[0], frames, 0, nullptr, 0.f, FLT_MAX, ImVec2(0.f, 300.f));
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("GPU Frametimes")) {
            ImGui::PlotLines("Frametimes in ms", frameTimes[1], frames, 0, nullptr, 0.f, FLT_MAX, ImVec2(0.f, 300.f));
            ImGui::TreePop();
        }
    }

    // Breakdown.
    if (ImGui::CollapsingHeader("Breakdown")) {
        for (int i = 0; i < COUNT; ++i) {
            int flags = root[i]->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0;
            if (ImGui::TreeNodeEx(TypeToString((Type)i).c_str(), flags)) {
                ImGui::Columns(2);
                for (Result& child : root[i]->children)
                    ShowResult(&child);
                ImGui::Columns(1);
                ImGui::TreePop();
            }
        }
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
    assert(active);
    assert(type != COUNT);

    current[type]->children.push_back(ProfilingManager::Result(name, current[type]));
    Result* result = &current[type]->children.back();
    current[type] = result;

    // Begin query if type is not CPU.
    if (type != Type::CPU_TIME) {
        Video::Query::Type queryType;
        switch (type) {
            case ProfilingManager::Type::GPU_TIME_ELAPSED:
                queryType = Video::Query::TIME_ELAPSED;
                break;
            case ProfilingManager::Type::GPU_SAMPLES_PASSED:
                queryType = Video::Query::SAMPLES_PASSED;
                break;
            default:
                assert(false);
                break;
        }
        // Find available query.
        Video::Query* query;
        std::list<Video::Query*>& queries = queryPool[queryType];
        if (queries.empty())
            query = new Video::Query(queryType);
        else {
            query = queries.back();
            queries.pop_back();
        }
        queryMap[result] = query;
        query->Begin();
    }
    
    return result;
}

void ProfilingManager::FinishResult(Result* result, Type type) {
    assert(active);
    assert(type != COUNT);
    assert(result == current[type]);

    // End query if type is GPU.
    if (type != Type::CPU_TIME)
        queryMap[result]->End();

    current[type] = result->parent;
}

void ProfilingManager::ShowResult(Result* result) {
    assert(active);

    ImGui::AlignFirstTextHeightToWidgets();
    int flags = result->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0;
    bool expanded = ImGui::TreeNodeEx(result->name.c_str(), flags);
        
    ImGui::NextColumn();
    if (result->parent->parent != nullptr) {
        ImGui::ProgressBar(result->value / result->parent->value, ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    }

    ImGui::Text(std::to_string(result->value).c_str());
    ImGui::NextColumn();

    if (expanded) {
        double otherTime = result->value;
        for (Result& child : result->children) {
            ShowResult(&child);
            otherTime -= child.value;
        }
        
        if (!result->children.empty()) {
            Result other("Other", result);
            other.value = otherTime;
            ShowResult(&other);
        }
        
        ImGui::TreePop();
    }
}

std::string ProfilingManager::TypeToString(Type type) const {
    switch (type) {
        case ProfilingManager::CPU_TIME:
            return "CPU time (ms)";
            break;
        case ProfilingManager::GPU_TIME_ELAPSED:
            return "GPU time (ms)";
            break;
        case ProfilingManager::GPU_SAMPLES_PASSED:
            return "GPU samples passed (number of fragments)";
            break;
        default:
            assert(false);
            return "ProfilingManager::TypeToString warning: No valid type to string";
            break;
    }
}

ProfilingManager::Result::Result(const std::string& name, Result* parent) : name (name) {
    this->parent = parent;
}
