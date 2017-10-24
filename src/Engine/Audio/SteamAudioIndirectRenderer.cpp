#include "SteamAudioIndirectRenderer.hpp"

SteamAudioIndirectRenderer::SteamAudioIndirectRenderer() {
    environment = nullptr;
    context = nullptr;
}

SteamAudioIndirectRenderer::SteamAudioIndirectRenderer(IPLContext* context, IPLhandle* environment) {
    this->context = context;
    this->environment = environment;
}

SteamAudioIndirectRenderer::~SteamAudioIndirectRenderer() {
}
