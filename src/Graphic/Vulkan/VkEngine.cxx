#include <Graphic/Vulkan/VkEngine.h>

#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>

#include <Common/Exception.h>
#include <Graphic/Vulkan/VkTypes.h>
#include <Graphic/Vulkan/VkInitializer.h>
#include <Graphic/Vulkan/VkImages.h>
#include <SDL/Window/WindowCore.h>

#include <chrono>
#include <thread>
#include <vulkan/vulkan_core.h>


#ifdef LE_DEBUG
constexpr bool isUseValidationLayers{ VK_TRUE };
#else
constexpr bool isUseValidationLayers{ VK_FALSE };
#endif

VkEngine* loadedEngine = nullptr;

VkEngine& VkEngine::Get() noexcept { return *loadedEngine; }

void VkEngine::Init(SDLWindowManager* windowManager) {
    ASSERT(loadedEngine == nullptr);
    loadedEngine = this;

    WindowManager = windowManager;
    Window = WindowManager->GetWindow();

    InitVulkan();

	InitSwapchain();

	InitCommands();

	InitSyncStructures();

    IsInitialized = true;

    return;
}

void VkEngine::CleanUp() {
    
    if (IsInitialized) {
        
        vkDeviceWaitIdle(Device);

		for (int i = 0; i < FRAME_OVERLAP; i++) {
			vkDestroyCommandPool(Device, Frames[i].CommandPool, nullptr);

		    vkDestroyFence(Device, Frames[i].RenderFence, nullptr);
		    vkDestroySemaphore(Device, Frames[i].RenderSemaphore, nullptr);
		    vkDestroySemaphore(Device ,Frames[i].SwapchainSemaphore, nullptr);
		}

        MainDelQueue.Flush();

		DestroySwapchain();

		vkDestroySurfaceKHR(Instance, Surface, nullptr);
		vkDestroyDevice(Device, nullptr);
		
		vkb::destroy_debug_utils_messenger(Instance, DebugMessenger);
		vkDestroyInstance(Instance, nullptr);
    }
    else {
        WARRING("WARRING", "No init VkEngine");
    }

    loadedEngine = nullptr;


}

void VkEngine::ReadyRun() {

    if (!IsSleep)
        return;

    SDL_Event event;
    
    auto id = SDLWindowManager::AddEvent([&stopRendering = StopRendering](SDL_Event event){
        switch (event.type) {
            case SDL_EVENT_WINDOW_MINIMIZED:
                stopRendering = true;
                break;

            case SDL_EVENT_WINDOW_RESTORED:
                stopRendering = false;
                break;
        }
    });
    _ids.push_back(id);

    IsSleep = false;

    return;
}

void VkEngine::Run() {
    
    if (IsSleep)
        return;
    
    if (StopRendering) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    Draw();

}

void VkEngine::SleepRun() {
    
    if (IsSleep)
        return;

    IsSleep = true;

    for (auto id : _ids) {
        SDLWindowManager::RemoveEvent(id);
    }
    _ids.clear();
}

void VkEngine::Draw() {

    VK_CHECK(vkWaitForFences(Device, 1, &GetCurrentFrame().RenderFence, true, 1000000000));

    GetCurrentFrame().DelQueue.Flush();
    
	VK_CHECK(vkResetFences(Device, 1, &GetCurrentFrame().RenderFence));

    uint32_t swapchainImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(Device, Swapchain, 1000000000, GetCurrentFrame().SwapchainSemaphore, nullptr, &swapchainImageIndex));

    VkCommandBuffer cmd = GetCurrentFrame().MainCommandBuffer;

    VK_CHECK(vkResetCommandBuffer(cmd, 0));

	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    vkutil::TransitionImage(cmd, SwapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	//make a clear-color from frame number. This will flash with a 120 frame period.
	VkClearColorValue clearValue;
	float flash = std::abs(std::sin(FrameNumber / 120.f));
	clearValue = { { 0.0f, 0.0f, flash, 1.0f } };

	VkImageSubresourceRange clearRange = vkinit::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

	//clear image
	vkCmdClearColorImage(cmd, SwapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

	//make the swapchain image into presentable mode
	vkutil::TransitionImage(cmd, SwapchainImages[swapchainImageIndex],VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	//finalize the command buffer (we can no longer add commands, but it can now be executed)
	VK_CHECK(vkEndCommandBuffer(cmd));

    VkCommandBufferSubmitInfo cmdinfo = vkinit::CommandBufferSubmitInfo(cmd);	
	
	VkSemaphoreSubmitInfo waitInfo = vkinit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,GetCurrentFrame().SwapchainSemaphore);
	VkSemaphoreSubmitInfo signalInfo = vkinit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, GetCurrentFrame().RenderSemaphore);	
	
	VkSubmitInfo2 submit = vkinit::SubmitInfo(&cmdinfo,&signalInfo,&waitInfo);	

	//submit command buffer to the queue and execute it.
	// _renderFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit2(GraphicsQueue, 1, &submit, GetCurrentFrame().RenderFence));

    VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.pSwapchains = &Swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &GetCurrentFrame().RenderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	VK_CHECK(vkQueuePresentKHR(GraphicsQueue, &presentInfo));

	//increase the number of frames drawn
	FrameNumber++;
}

void VkEngine::InitVulkan() {

    vkb::InstanceBuilder builder;

	auto instRet = builder.set_app_name("Super Vulkan App")
		.request_validation_layers(isUseValidationLayers)
		.use_default_debug_messenger()
		.require_api_version(1, 4, 0)
		.build();

	vkb::Instance vkbInst = instRet.value();

	Instance = vkbInst.instance;
	DebugMessenger = vkbInst.debug_messenger;

    if (!SDL_Vulkan_CreateSurface(Window, Instance, nullptr, &Surface)) {
        EXCEPTION("runtime_error", SDL_GetError());
    }

    VkPhysicalDeviceVulkan14Features features14{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES };
    features14.pushDescriptor = VK_TRUE;

	VkPhysicalDeviceVulkan13Features features13{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
	features13.dynamicRendering = VK_TRUE;
	features13.synchronization2 = VK_TRUE;

	VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
	features12.bufferDeviceAddress = VK_TRUE;
	features12.descriptorIndexing = VK_TRUE;


	vkb::PhysicalDeviceSelector selector{ vkbInst };
	vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version(1, 4)
        .set_required_features_14(features14)
		.set_required_features_13(features13)
		.set_required_features_12(features12)
		.set_surface(Surface)
		.select()
		.value();


	vkb::DeviceBuilder deviceBuilder{ physicalDevice };

	vkb::Device vkbDevice = deviceBuilder.build().value();

	Device = vkbDevice.device;
	ChosenGPU = physicalDevice.physical_device;

    GraphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	GraphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
}

void VkEngine::CreateSwapchain(uint32_t width, uint32_t height) {

	vkb::SwapchainBuilder swapchainBuilder{ ChosenGPU,Device,Surface };

	SwapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

	vkb::Swapchain vkbSwapchain = swapchainBuilder
		//.use_default_format_selection()
		.set_desired_format(VkSurfaceFormatKHR{ .format = SwapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		//use vsync present mode
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(width, height)
		.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.build()
		.value();

	SwapchainExtent = vkbSwapchain.extent;
	//store swapchain and its related images
	Swapchain = vkbSwapchain.swapchain;
	SwapchainImages = vkbSwapchain.get_images().value();
	SwapchainImageViews = vkbSwapchain.get_image_views().value();
}

void VkEngine::InitSwapchain() {

	CreateSwapchain(WindowExtent.width, WindowExtent.height);
}

void VkEngine::DestroySwapchain() {

	vkDestroySwapchainKHR(Device, Swapchain, nullptr);

	for (int i = 0; i < SwapchainImageViews.size(); i++) {

		vkDestroyImageView(Device, SwapchainImageViews[i], nullptr);
	}
}

void VkEngine::InitCommands()
{
    VkCommandPoolCreateInfo commandPoolInfo = vkinit::CommandPoolCreateInfo(GraphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	
	for (int i = 0; i < FRAME_OVERLAP; i++) {

		VK_CHECK(vkCreateCommandPool(Device, &commandPoolInfo, nullptr, &Frames[i].CommandPool));

		VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::CommandBufferAllocateInfo(Frames[i].CommandPool, 1);

		VK_CHECK(vkAllocateCommandBuffers(Device, &cmdAllocInfo, &Frames[i].MainCommandBuffer));
	}
}
void VkEngine::InitSyncStructures()
{
    
    VkFenceCreateInfo fenceCreateInfo = vkinit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::SemaphoreCreateInfo();

	for (int i = 0; i < FRAME_OVERLAP; i++) {
		VK_CHECK(vkCreateFence(Device, &fenceCreateInfo, nullptr, &Frames[i].RenderFence));

		VK_CHECK(vkCreateSemaphore(Device, &semaphoreCreateInfo, nullptr, &Frames[i].SwapchainSemaphore));
		VK_CHECK(vkCreateSemaphore(Device, &semaphoreCreateInfo, nullptr, &Frames[i].RenderSemaphore));
	}
}
