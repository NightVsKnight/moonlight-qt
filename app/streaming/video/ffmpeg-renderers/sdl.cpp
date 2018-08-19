#include "renderer.h"

#include <Limelight.h>

SdlRenderer::SdlRenderer()
    : m_Renderer(nullptr),
      m_Texture(nullptr)
{

}

SdlRenderer::~SdlRenderer()
{
    if (m_Texture != nullptr) {
        SDL_DestroyTexture(m_Texture);
    }

    if (m_Renderer != nullptr) {
        SDL_DestroyRenderer(m_Renderer);
    }
}

bool SdlRenderer::prepareDecoderContext(AVCodecContext*)
{
    /* Nothing to do */

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
                "Using SDL software renderer");

    return true;
}

bool SdlRenderer::needsTestFrame()
{
    // This renderer should always work
    return false;
}

bool SdlRenderer::initialize(SDL_Window* window,
                             int,
                             int width,
                             int height,
                             int)
{
    m_Renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_Renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "SDL_CreateRenderer() failed: %s",
                     SDL_GetError());
        return false;
    }

    // The window may be smaller than the stream size, so ensure our
    // logical rendering surface size is equal to the stream size
    SDL_RenderSetLogicalSize(m_Renderer, width, height);

    // Draw a black frame until the video stream starts rendering
    SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(m_Renderer);
    SDL_RenderPresent(m_Renderer);

    m_Texture = SDL_CreateTexture(m_Renderer,
                                  SDL_PIXELFORMAT_YV12,
                                  SDL_TEXTUREACCESS_STREAMING,
                                  width,
                                  height);
    if (!m_Texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "SDL_CreateRenderer() failed: %s",
                     SDL_GetError());
        return false;
    }

    return true;
}

void SdlRenderer::renderFrameAtVsync(AVFrame* frame)
{
    SDL_UpdateYUVTexture(m_Texture, nullptr,
                         frame->data[0],
                         frame->linesize[0],
                         frame->data[1],
                         frame->linesize[1],
                         frame->data[2],
                         frame->linesize[2]);

    SDL_RenderClear(m_Renderer);
    SDL_RenderCopy(m_Renderer, m_Texture, nullptr, nullptr);
    SDL_RenderPresent(m_Renderer);
}
