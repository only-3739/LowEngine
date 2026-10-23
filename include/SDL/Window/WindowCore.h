#pragma once

#include <Common/Types.h>
#include <Common/Math.h>
#include <SDL3/SDL_events.h>
#include <functional>

constexpr uint32_t WINDOW_WIDTH{1920};
constexpr uint32_t WINDOW_HEGIHT{1080};

using SDLEventCallback = std::function<void(SDL_Event)>;

struct SDLEventIds {
    std::vector<size_t> ids;
};

struct SDLEvents {

private:
    
    static size_t nextId/*{0}*/;

public:

    std::vector<size_t> id;
    std::vector<SDLEventCallback> func;

    [[nodiscard]]
    static size_t GetId();
    void Clear();
};

class SDLWindowManager {

private:

    bool _isInitialized{ false };

    std::vector<size_t> _ids;

    static SDLEvents _events;
    
    struct SDL_Window* _window{ nullptr };

    void InvokeEvents(SDL_Event event);

    void DestoryWindow();

public:

    bool IsQuit{ false };
    bool IsSleep{ false };

    glm::vec2 WindowSize{1920, 1080};

    static size_t AddEvent(SDLEventCallback func);
    static void RemoveEvent(size_t id);

    [[nodiscard]]
    SDL_Window* GetWindow() const noexcept { return _window; };

    void Init();
    void ReadyRun();
    void Run();
    void SleepRun();
    void CleanUp();
};
