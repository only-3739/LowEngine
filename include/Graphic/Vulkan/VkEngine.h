#pragma once

#include <Graphic/Vulkan/VkTypes.h>

#include <vulkan/vulkan_core.h>

constexpr unsigned int FRAME_OVERLAP{2};

struct DeletionQueue
    {
        std::deque<std::function<void()>> deletors;

        void PushFunction(std::function<void()>&& function) {
            deletors.push_back(function);
        }

        void Flush() {
            for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
                (*it)();
            }

            deletors.clear();
        }
    };

struct FrameData {

    VkCommandPool CommandPool;
    VkCommandBuffer MainCommandBuffer;

    VkSemaphore SwapchainSemaphore;
    VkSemaphore RenderSemaphore;
    VkFence RenderFence;

    DeletionQueue DelQueue;

};

class VkEngine {

private:

    std::vector<size_t> _ids;

    void InitVulkan();

    void CreateSwapchain(uint32_t width, uint32_t height);
    void InitSwapchain();
    void DestroySwapchain();

    void InitCommands();

    void InitSyncStructures();


public:

    bool IsInitialized{ false };
    bool IsSleep{ true };
    bool StopRendering{ false };
    int FrameNumber{0};
    VkExtent2D WindowExtent{ 1920 , 1080 };

    struct SDL_Window* Window{ nullptr };
    struct SDLWindowManager* WindowManager{ nullptr };

    VkInstance Instance;
    VkDebugUtilsMessengerEXT DebugMessenger;
    VkPhysicalDevice ChosenGPU;
    VkDevice Device;
    VkSurfaceKHR Surface;

    VkSwapchainKHR Swapchain;
    VkFormat SwapchainImageFormat;

    std::vector<VkImage> SwapchainImages;
    std::vector<VkImageView> SwapchainImageViews;
    VkExtent2D SwapchainExtent;

    FrameData Frames[FRAME_OVERLAP];

    FrameData& GetCurrentFrame() noexcept { return Frames[FrameNumber % FRAME_OVERLAP]; };

    VkQueue GraphicsQueue;
    uint32_t GraphicsQueueFamily;

    DeletionQueue MainDelQueue;

    [[nodiscard]]
    static VkEngine& Get() noexcept;

    void Init(SDLWindowManager* windowManager);
    void CleanUp();
    void ReadyRun();
    void Run();
    void SleepRun();
    void Draw();
};
