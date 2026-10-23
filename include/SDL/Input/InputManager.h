#pragma once

#include <Common/Exception.h>
#include <SDL3/SDL.h>
#include <SDL/Window/WindowCore.h>

void ALog(SDL_Event event) {
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_A)
        fmt::println("키눌림 ㄷㄷ");
}

void AddEvent() {
    SDLWindowManager::AddEvent(ALog);
}



