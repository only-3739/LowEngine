#include <Graphic/Vulkan/VkInitializer.h>


VkCommandPoolCreateInfo vkinit::CommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags/* = 0 */) noexcept {
    return {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .flags = flags,
    .queueFamilyIndex = queueFamilyIndex};
}

VkCommandBufferAllocateInfo vkinit::CommandBufferAllocateInfo(VkCommandPool pool, uint32_t count/* = 1 */) noexcept {
    return {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .commandPool = pool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = count};
}

VkFenceCreateInfo vkinit::FenceCreateInfo (VkFenceCreateFlags flags/* = 0 */) noexcept {
    return {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .flags = flags };
}

VkSemaphoreCreateInfo vkinit::SemaphoreCreateInfo (VkSemaphoreCreateFlags flags/* = 0 */) noexcept {
    return {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .flags = flags };
}

VkCommandBufferBeginInfo vkinit::CommandBufferBeginInfo(VkCommandBufferUsageFlags flags/* = 0 */) noexcept {
    return {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .flags = flags };
}

VkImageSubresourceRange vkinit::ImageSubresourceRange(VkImageAspectFlags aspectMask) noexcept {
    return {
    .aspectMask = aspectMask,
    .levelCount = VK_REMAINING_MIP_LEVELS,
    .layerCount = VK_REMAINING_ARRAY_LAYERS };
}

// TODO - init 함수 구조 개선
VkSemaphoreSubmitInfo vkinit::SemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore) noexcept {
	VkSemaphoreSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.semaphore = semaphore;
	submitInfo.stageMask = stageMask;
	submitInfo.deviceIndex = 0;
	submitInfo.value = 1;

	return submitInfo;
}

VkCommandBufferSubmitInfo vkinit::CommandBufferSubmitInfo(VkCommandBuffer cmd) noexcept {
	VkCommandBufferSubmitInfo info{};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	info.pNext = nullptr;
	info.commandBuffer = cmd;
	info.deviceMask = 0;

	return info;
}

VkSubmitInfo2 vkinit::SubmitInfo(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo, VkSemaphoreSubmitInfo* waitSemaphoreInfo) noexcept {
    VkSubmitInfo2 info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    info.pNext = nullptr;

    info.waitSemaphoreInfoCount = waitSemaphoreInfo == nullptr ? 0 : 1;
    info.pWaitSemaphoreInfos = waitSemaphoreInfo;

    info.signalSemaphoreInfoCount = signalSemaphoreInfo == nullptr ? 0 : 1;
    info.pSignalSemaphoreInfos = signalSemaphoreInfo;

    info.commandBufferInfoCount = 1;
    info.pCommandBufferInfos = cmd;

    return info;
}
