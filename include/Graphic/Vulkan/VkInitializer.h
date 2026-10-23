#pragma once

#include <Graphic/Vulkan/VkTypes.h>

namespace vkinit {

    [[nodiscard]]
    VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0) noexcept;

    [[nodiscard]]
    VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool pool, uint32_t count = 1) noexcept;

    [[nodiscard]]
    VkFenceCreateInfo FenceCreateInfo (VkFenceCreateFlags flags = 0) noexcept;

    [[nodiscard]]
    VkSemaphoreCreateInfo SemaphoreCreateInfo (VkSemaphoreCreateFlags flags = 0) noexcept;

    [[nodiscard]]
    VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0) noexcept;
    
    [[nodiscard]]
    VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspectMask) noexcept;

    [[nodiscard]]
    VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore) noexcept;

    [[nodiscard]]
    VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer cmd) noexcept;

    [[nodiscard]]
    VkSubmitInfo2 SubmitInfo(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo, VkSemaphoreSubmitInfo* waitSemaphoreInfo) noexcept;



}
