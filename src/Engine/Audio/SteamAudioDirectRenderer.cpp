#include "SteamAudioDirectRenderer.hpp"

SteamAudioDirectRenderer::SteamAudioDirectRenderer() {
    context = nullptr;
    environment = nullptr;
}

SteamAudioDirectRenderer::SteamAudioDirectRenderer(IPLContext* context, IPLhandle* environment) {
    this->context = context;
    this->environment = environment;
}

SteamAudioDirectRenderer::~SteamAudioDirectRenderer() {
}
