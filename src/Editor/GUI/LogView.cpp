#include "LogView.hpp"
#include <imgui.h>
#include <array>
#include <Engine/MainWindow.hpp>
#include "../ImGui/Splitter.hpp"
#include <Utility/Log.hpp>
#include <iostream>

using namespace GUI;


LogView::LogView() {
    // Setup streams.
    Log().SetupStream(Log::DEFAULT, &defaultStringstream);
    Log().SetupStream(Log::INFO, &infoStringstream);
    Log().SetupStream(Log::WARNING, &warningStringstream);
    Log().SetupStream(Log::ERR, &errorStringstream);
}

LogView::~LogView() {
    // Reset streams to cout.
    Log().SetupStream(Log::DEFAULT, &std::cout);
    Log().SetupStream(Log::INFO, &std::cout);
    Log().SetupStream(Log::WARNING, &std::cout);
    Log().SetupStream(Log::ERR, &std::cout);
}

void LogView::Show() {
    // Window size.
    ImVec2 size(MainWindow::GetInstance()->GetSize().x, MainWindow::GetInstance()->GetSize().y);

    // Splitter.
    ImGui::VerticalSplitter(ImVec2(sceneWidth, 0), size.x - sceneWidth - editorWidth, splitterSize, logHeight, logResize, 20, size.y - 20);
    if (logResize)
        logHeight = size.y - logHeight;

    ImGui::SetNextWindowPos(ImVec2(sceneWidth, 20));
    ImGui::SetNextWindowSize(ImVec2(size.x - sceneWidth - editorWidth, logHeight));

    //Create log output string.
    std::string output;
    if (!defaultStringstream.str().empty())
        output += "[Default] " + defaultStringstream.str();

    if (!infoStringstream.str().empty())
        output += "[Info] " + infoStringstream.str();

    if (!warningStringstream.str().empty())
        output += "[Warning] " + warningStringstream.str();

    if (!errorStringstream.str().empty())
        output += "[Error] " + errorStringstream.str();
    
    // Add new lines to text buffer.
    int old_size = textBuffer.size();
    
    textBuffer.appendv(output.c_str(), nullptr);

    for (int newSize = textBuffer.size(); old_size < newSize; old_size++)
        if (textBuffer[old_size] == '\n')
            lineOffsets.push_back(old_size);

    // Start drawing window.
    ImGui::Begin("Log", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ShowBorders);
    // Options.

    // Clear log window from text.
    if (ImGui::Button("Clear"))
        textBuffer.clear();

    ImGui::SameLine();

    // Copy log contents to clipboard.
    if (ImGui::Button("Copy to clipboard")) {
        ImGui::LogToClipboard();
    }

    ImGui::SameLine();

    // Filter
    textFilter.Draw("Filter");

    ImGui::Separator();

    // Draw log. If filter is active, filter log contents.
    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    if(textFilter.IsActive()) {
        const char* bufferBegin = textBuffer.begin();
        const char* line = bufferBegin;
        for (int lineNumber = 0; line != NULL; lineNumber++) {
            const char* line_end = (lineNumber < lineOffsets.Size) ? bufferBegin + lineOffsets[lineNumber] : NULL;
            if (textFilter.PassFilter(line, line_end))
                ImGui::TextUnformatted(line, line_end);
            line = line_end && line_end[1] ? line_end + 1 : NULL;
        }
    } else
        ImGui::TextUnformatted(textBuffer.begin());

    ImGui::EndChild();
    ImGui::End();

    // Clear streams.
    defaultStringstream.str(std::string());
    infoStringstream.str(std::string());
    warningStringstream.str(std::string());
    errorStringstream.str(std::string());
}

bool LogView::IsVisible() {
    return visible;
}