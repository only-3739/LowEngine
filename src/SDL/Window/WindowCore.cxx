#include <SDL/Window/WindowCore.h>

#include <Common/Types.h>
#include <Common/Math.h>
#include <Common/Exception.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>

size_t SDLEvents::nextId{0};
SDLEvents SDLWindowManager::_events{};

size_t SDLEvents::GetId() { return nextId++; }

void SDLEvents::Clear() {
    id.clear();
    func.clear();
}

size_t SDLWindowManager::AddEvent(SDLEventCallback func) {
    auto id = SDLEvents::GetId();
    _events.id.push_back(id);
    _events.func.push_back(func);

    return id;
}

void SDLWindowManager::RemoveEvent(size_t id) {
    auto iter = std::ranges::find(_events.id, id);

    if (iter == _events.id.end())
        EXCEPTION("runtime_error","Try Removing missing id");
    size_t index = (iter - _events.id.begin());

    _events.id.erase(_events.id.begin() + index);
    _events.func.erase(_events.func.begin() + index);

    return;
}

void SDLWindowManager::Init() {
    
    if (!SDL_Init(SDL_INIT_VIDEO))
        EXCEPTION("runtime_error", SDL_GetError());

    SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    _window = SDL_CreateWindow(
        "LowEngine",
        WindowSize.x,
        WindowSize.y,
        windowFlags);

    return;
}

void SDLWindowManager::DestoryWindow() {

    if (_window != nullptr) {
        SDL_DestroyWindow(_window);
    }

    return;
}

void SDLWindowManager::InvokeEvents(SDL_Event event) {

    for (auto func : _events.func) {
        func(event);
    }
}

void SDLWindowManager::ReadyRun() {
    IsQuit = false;
    IsSleep = false;

    _ids.push_back(SDLWindowManager::AddEvent([&isQuit = IsQuit](SDL_Event event){ if (event.type == SDL_EVENT_QUIT) isQuit = true; }));
}

void SDLWindowManager::Run() {

    if (IsSleep)
        return;

    if (_events.func.empty())
        return;

    static SDL_Event event;

    while (SDL_PollEvent(&event)) {
        InvokeEvents(event);
    }

    return;
}

void SDLWindowManager::SleepRun() {
    IsSleep = true;

    for (auto id : _ids) {
        SDLWindowManager::RemoveEvent(id);
    }
}

void SDLWindowManager::CleanUp() {
    DestoryWindow();
    _events.Clear();

    return;
}













