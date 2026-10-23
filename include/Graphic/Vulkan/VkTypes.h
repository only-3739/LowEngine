#pragma once

#include <Common/Types.h>
#include <Common/Exception.h>
#include <Common/Math.h>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#define VK_CHECK(x)                                                         \
    do {                                                                    \
        VkResult err = x;                                                   \
        if (err) {                                                          \
            EXCEPTION("runtime_error", string_VkResult(err));               \
        }                                                                   \
    } while (0)
