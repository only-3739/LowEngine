#include <Core/LifeCycle.h>

#include <SDL/Window/WindowCore.h>
#include <Graphic/Vulkan/VkEngine.h>
#include <Debug/Debug.h>

SDLWindowManager windowManager;
VkEngine engine;

void Begin() {

    windowManager.Init();
    engine.Init(&windowManager);
}

void Start() {
    DebugMain();

    windowManager.ReadyRun();
    engine.ReadyRun();
}

bool Loop() {
    
    windowManager.Run();
    if (windowManager.IsQuit)
        return false;

    engine.Run();

    return true;

}

void Quit() {
    
    engine.SleepRun();
    windowManager.SleepRun();

    engine.CleanUp();
    windowManager.CleanUp();
}

void End() {
    
}
