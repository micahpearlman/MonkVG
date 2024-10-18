#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

/// system
#include <vector>
#include <array>
#include <cstring>
#include <fstream>
#include <iterator>
#include <thread>

/// MonkVG
#include <MonkVG/openvg.h>
#include <MonkVG/vgext.h>
#include <MonkVG/vgu.h>

/// vulkan
#include <vulkan/vulkan.h>

struct vulkan_test_ctx_t {
    GLFWwindow                 *glfw_window;
    VkInstance                  instance;
    VkPhysicalDevice            physical_device = VK_NULL_HANDLE;
    VkDevice                    logical_device  = VK_NULL_HANDLE;
    VkQueue                     graphics_queue  = VK_NULL_HANDLE;
    VkQueue                     present_queue   = VK_NULL_HANDLE;
    VkSurfaceKHR                surface         = VK_NULL_HANDLE;
    VkSwapchainKHR              swap_chain      = VK_NULL_HANDLE;
    std::vector<VkImage>        swap_chain_images;
    std::vector<VkImageView>    image_views;
    VkFormat                    swap_chain_format;
    VkExtent2D                  swap_chain_extent;
    VkShaderModule              vert_shader_module = VK_NULL_HANDLE;
    VkShaderModule              frag_shader_module = VK_NULL_HANDLE;
    VkRenderPass                render_pass        = VK_NULL_HANDLE;
    VkPipelineLayout            pipeline_layout    = VK_NULL_HANDLE;
    VkPipeline                  graphics_pipeline  = VK_NULL_HANDLE;
    std::vector<VkFramebuffer>  swap_chain_framebuffers;
    VkCommandPool               command_pool                = VK_NULL_HANDLE;
    VkCommandBuffer             command_buffer              = VK_NULL_HANDLE;
    VkSemaphore                 image_available_sema        = VK_NULL_HANDLE;
    VkSemaphore                 render_finished_sema        = VK_NULL_HANDLE;
    VkFence                     in_flight_fence             = VK_NULL_HANDLE;
    bool                        enable_vk_validation_layers = true;
    std::array<const char *, 1> vk_validation_layers        = {
        "VK_LAYER_KHRONOS_validation"};
    std::array<const char *, 1> vk_device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

void init_vulkan(vulkan_test_ctx_t &ctx) {

    // check vulkan validation layer support.  search for ours
    uint32_t layer_cnt = 0;
    vkEnumerateInstanceLayerProperties(&layer_cnt, nullptr);
    std::vector<VkLayerProperties> available_layers(layer_cnt);
    vkEnumerateInstanceLayerProperties(&layer_cnt, available_layers.data());
    uint32_t found_layer_cnt = 0;
    for (const char *layer_name : ctx.vk_validation_layers) {
        for (const auto &layer_props : available_layers) {
            std::cout << "available layer: " << layer_props.layerName
                      << std::endl;
            if (strcmp(layer_name, layer_props.layerName) == 0) {
                std::cout << "\tfound required layer: " << layer_name
                          << std::endl;
                found_layer_cnt++;
            }
        }
    }

    VkApplicationInfo app_info = {.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                  .pApplicationName = "Vulkan Test",
                                  .applicationVersion =
                                      VK_MAKE_VERSION(1, 0, 0),
                                  .pEngineName   = "No Engine",
                                  .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                                  .apiVersion    = VK_API_VERSION_1_2};

    uint32_t     glfw_extension_cnt = 0;
    const char **glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_cnt);

    // Get required GLFW extensions
    uint32_t     glfwExtensionCount = 0;
    const char **glfwExtensions =
        glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // Create a vector to hold all required extensions, including GLFW's and
    // your custom ones
    std::vector<const char *> extensions(glfw_extensions,
                                         glfw_extensions + glfw_extension_cnt);

    // Add the VK_KHR_portability_enumeration extension
    extensions.push_back("VK_KHR_portability_enumeration");
    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#if defined(__APPLE__)
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
#endif // __APPLE__
        .pApplicationInfo  = &app_info,
        .enabledLayerCount = found_layer_cnt,
        .ppEnabledLayerNames =
            found_layer_cnt == ctx.vk_validation_layers.size()
                ? ctx.vk_validation_layers.data()
                : 0,
        .enabledExtensionCount   = (uint32_t)extensions.size(),
        .ppEnabledExtensionNames = extensions.data()};

    VkResult result = vkCreateInstance(&create_info, nullptr, &ctx.instance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance");
    }

    // print out the supported extensions
    uint32_t vk_extensions_cnt = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &vk_extensions_cnt,
                                           nullptr);
    for (const auto &ext : extensions) {
        std::cout << '\t' << ext << std::endl;
    }

    /// setup vulkan device
    // get physical device
    uint32_t device_cnt = 0;
    vkEnumeratePhysicalDevices(ctx.instance, &device_cnt, nullptr);
    if (device_cnt == 0) {
        throw std::runtime_error("No Vulkan Devices");
    }

    std::vector<VkPhysicalDevice> devices(device_cnt);
    vkEnumeratePhysicalDevices(ctx.instance, &device_cnt, devices.data());
    for (const VkPhysicalDevice &device : devices) {
        VkPhysicalDeviceProperties device_props;
        vkGetPhysicalDeviceProperties(device, &device_props);
        std::cout << "Device: " << device_props.deviceName << std::endl;

        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceFeatures(device, &device_features);

        if (device_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            ctx.physical_device = device;
        } else if (device_props.deviceType ==
                   VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
            ctx.physical_device = device;
        }

        std::cout << "\tUsing device: " << device_props.deviceName << std::endl;
    }

    /// create a windows surface
    if (glfwCreateWindowSurface(ctx.instance, ctx.glfw_window, nullptr,
                                &ctx.surface) != VK_SUCCESS) {
        throw std::runtime_error("Cannot create windows surface");
    }

    /// get the graphics queue family
    uint32_t queue_family_cnt = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(ctx.physical_device,
                                             &queue_family_cnt, nullptr);
    std::vector<VkQueueFamilyProperties> queue_family_props(queue_family_cnt);
    vkGetPhysicalDeviceQueueFamilyProperties(
        ctx.physical_device, &queue_family_cnt, queue_family_props.data());
    uint32_t queue_family_graphics_idx = -1;
    int32_t  i                         = 0;
    for (const VkQueueFamilyProperties &queue_family : queue_family_props) {
        // make sure this queue family can also present
        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(ctx.physical_device, i,
                                             ctx.surface, &present_support);
        ;
        if ((queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
            present_support == true) {
            queue_family_graphics_idx = i;
        }
        i++;
    }

    /// create logical device
    float                   queue_priority                    = 1.0f;
    VkDeviceQueueCreateInfo device_queue_create_info_graphics = {
        .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queue_family_graphics_idx,
        .queueCount       = 1,
        .pQueuePriorities = &queue_priority};

    VkPhysicalDeviceFeatures physical_device_features = {VK_FALSE};

    // get the device extensions
    uint32_t device_ext_cnt = 0;
    vkEnumerateDeviceExtensionProperties(ctx.physical_device, nullptr,
                                         &device_ext_cnt, nullptr);
    std::vector<VkExtensionProperties> device_extensions(device_ext_cnt);
    vkEnumerateDeviceExtensionProperties(ctx.physical_device, nullptr,
                                         &device_ext_cnt,
                                         device_extensions.data());
    for (const VkExtensionProperties &prop : device_extensions) {
        std::cout << "\t" << prop.extensionName << std::endl;
    }

    VkDeviceCreateInfo device_create_info = {
        .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount    = 1,
        .pQueueCreateInfos       = &device_queue_create_info_graphics,
        .enabledLayerCount       = (uint32_t)ctx.vk_validation_layers.size(),
        .ppEnabledLayerNames     = ctx.vk_validation_layers.data(),
        .enabledExtensionCount   = (uint32_t)ctx.vk_device_extensions.size(),
        .ppEnabledExtensionNames = ctx.vk_device_extensions.data(),
        .pEnabledFeatures        = &physical_device_features};

    if (vkCreateDevice(ctx.physical_device, &device_create_info, nullptr,
                       &ctx.logical_device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device");
    }

    // get the device graphics queue
    vkGetDeviceQueue(ctx.logical_device, queue_family_graphics_idx, 0,
                     &ctx.graphics_queue);

    // get the device present queue
    vkGetDeviceQueue(ctx.logical_device, queue_family_graphics_idx, 0,
                     &ctx.present_queue);

    /// build the swap chain
    VkSurfaceCapabilitiesKHR surface_capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ctx.physical_device, ctx.surface,
                                              &surface_capabilities);
    uint32_t swapchain_image_cnt = surface_capabilities.minImageCount + 1;

    VkPresentModeKHR present_mode     = VK_PRESENT_MODE_FIFO_KHR;
    uint32_t         present_mode_cnt = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(ctx.physical_device, ctx.surface,
                                              &present_mode_cnt, nullptr);
    std::vector<VkPresentModeKHR> present_modes(present_mode_cnt);
    vkGetPhysicalDeviceSurfacePresentModesKHR(ctx.physical_device, ctx.surface,
                                              &present_mode_cnt,
                                              present_modes.data());
    // prefer mailbox over fifo
    for (const VkPresentModeKHR &pm : present_modes) {
        if (pm == VK_PRESENT_MODE_MAILBOX_KHR) {
            present_mode = pm;
            break;
        }
    }
    uint32_t surface_format_cnt = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(ctx.physical_device, ctx.surface,
                                         &surface_format_cnt, nullptr);
    std::vector<VkSurfaceFormatKHR> surface_formats(surface_format_cnt);
    vkGetPhysicalDeviceSurfaceFormatsKHR(ctx.physical_device, ctx.surface,
                                         &surface_format_cnt,
                                         surface_formats.data());

    VkSwapchainCreateInfoKHR swapchain_create_info = {
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface               = ctx.surface,
        .minImageCount         = swapchain_image_cnt,
        .imageFormat           = surface_formats[0].format,
        .imageColorSpace       = surface_formats[0].colorSpace,
        .imageExtent           = surface_capabilities.currentExtent,
        .imageArrayLayers      = 1,
        .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr,
        .preTransform          = surface_capabilities.currentTransform,
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode           = present_mode,
        .clipped               = VK_TRUE,
        .oldSwapchain          = VK_NULL_HANDLE

    };

    if (vkCreateSwapchainKHR(ctx.logical_device, &swapchain_create_info,
                             nullptr, &ctx.swap_chain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swap chain :-(");
    }

    ctx.swap_chain_format = swapchain_create_info.imageFormat;
    ctx.swap_chain_extent = swapchain_create_info.imageExtent;

    /// create image views
    uint32_t image_cnt = 0;
    vkGetSwapchainImagesKHR(ctx.logical_device, ctx.swap_chain, &image_cnt,
                            nullptr);
    ctx.swap_chain_images.resize(image_cnt);
    ctx.image_views.resize(image_cnt);
    vkGetSwapchainImagesKHR(ctx.logical_device, ctx.swap_chain, &image_cnt,
                            ctx.swap_chain_images.data());

    for (size_t i = 0; i < ctx.swap_chain_images.size(); i++) {
        VkImageViewCreateInfo image_create_info = {
            .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image            = ctx.swap_chain_images[i],
            .viewType         = VK_IMAGE_VIEW_TYPE_2D,
            .format           = ctx.swap_chain_format,
            .components       = {VK_COMPONENT_SWIZZLE_IDENTITY,
                                 VK_COMPONENT_SWIZZLE_IDENTITY,
                                 VK_COMPONENT_SWIZZLE_IDENTITY,
                                 VK_COMPONENT_SWIZZLE_IDENTITY},
            .subresourceRange = {.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                                 .baseMipLevel   = 0,
                                 .levelCount     = 1,
                                 .baseArrayLayer = 0,
                                 .layerCount     = 1}};

        if (vkCreateImageView(ctx.logical_device, &image_create_info, nullptr,
                              &ctx.image_views[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed creating image view");
        }
    }

    /// Create The Shaders
    std::ifstream     vert_is("assets/shaders/test.vert.spv",
                              std::ios::ate | std::ios::binary);
    std::vector<char> vert_shader_bin(vert_is.tellg());
    vert_is.seekg(0);
    vert_is.read(vert_shader_bin.data(), vert_shader_bin.size());

    VkShaderModuleCreateInfo vert_shader_create_info{
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = vert_shader_bin.size(),
        .pCode    = reinterpret_cast<const uint32_t *>(vert_shader_bin.data())};
    if (vkCreateShaderModule(ctx.logical_device, &vert_shader_create_info,
                             nullptr, &ctx.vert_shader_module) != VK_SUCCESS) {
        throw std::runtime_error("Could not create vertex shader module");
    }

    VkPipelineShaderStageCreateInfo vert_shader_stage_info = {
        .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage  = VK_SHADER_STAGE_VERTEX_BIT,
        .module = ctx.vert_shader_module,
        .pName  = "main"};

    std::ifstream     frag_is("assets/shaders/test.frag.spv",
                              std::ios::ate | std::ios::binary);
    std::vector<char> frag_shader_bin(frag_is.tellg());
    frag_is.seekg(0);
    frag_is.read(frag_shader_bin.data(), frag_shader_bin.size());
    VkShaderModuleCreateInfo frag_shader_create_info{
        .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = frag_shader_bin.size(),
        .pCode    = reinterpret_cast<const uint32_t *>(frag_shader_bin.data())};
    if (vkCreateShaderModule(ctx.logical_device, &frag_shader_create_info,
                             nullptr, &ctx.frag_shader_module) != VK_SUCCESS) {
        throw std::runtime_error("Could not create fragment shader module");
    }

    VkPipelineShaderStageCreateInfo frag_shader_stage_info = {
        .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage  = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = ctx.frag_shader_module,
        .pName  = "main"};

    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info,
                                                       frag_shader_stage_info};

    /// Create Render Pass
    VkAttachmentDescription color_attachment = {
        .format         = swapchain_create_info.imageFormat,
        .samples        = VK_SAMPLE_COUNT_1_BIT,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR};

    VkAttachmentReference color_attachment_ref = {
        .attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

    VkSubpassDescription subpass = {.pipelineBindPoint =
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    .colorAttachmentCount = 1,
                                    .pColorAttachments = &color_attachment_ref};

    VkRenderPassCreateInfo render_pass_create_info = {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments    = &color_attachment,
        .subpassCount    = 1,
        .pSubpasses      = &subpass};

    if (vkCreateRenderPass(ctx.logical_device, &render_pass_create_info,
                           nullptr, &ctx.render_pass) != VK_SUCCESS) {
        throw std::runtime_error("Could not create render pass");
    }

    /// Create pipeline object
    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount   = 0,
        .pVertexBindingDescriptions      = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions    = nullptr};

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE};

    VkViewport viewport = {
        .x        = 0,
        .y        = 0,
        .width    = (float)swapchain_create_info.imageExtent.width,
        .height   = (float)swapchain_create_info.imageExtent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f};

    VkRect2D scissor = {.offset = {0, 0},
                        .extent = swapchain_create_info.imageExtent};

    VkPipelineViewportStateCreateInfo viewport_state = {
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports    = &viewport,
        .scissorCount  = 1,
        .pScissors     = &scissor};

    VkPipelineRasterizationStateCreateInfo rasterizer = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable        = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode             = VK_POLYGON_MODE_FILL,
        .cullMode                = VK_CULL_MODE_BACK_BIT,
        .frontFace               = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable         = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = 0.0f,
        .lineWidth               = 1.0f};

    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable   = VK_FALSE,
        .minSampleShading      = 1.0f,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE};

    VkPipelineColorBlendAttachmentState color_blend_attachment = {
        .blendEnable         = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp        = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp        = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT

    };

    VkPipelineColorBlendStateCreateInfo color_blending_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable   = VK_FALSE,
        .logicOp         = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments    = &color_blend_attachment,
        .blendConstants  = {0, 0, 0, 0}};

    std::array<VkDynamicState, 2> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH};

    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount         = 0,
        .pSetLayouts            = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges    = nullptr};

    if (vkCreatePipelineLayout(ctx.logical_device, &pipeline_layout_create_info,
                               nullptr, &ctx.pipeline_layout) != VK_SUCCESS) {
        throw std::runtime_error("Could not create pipeline layout");
    }

    VkGraphicsPipelineCreateInfo pipeline_create_info = {
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount          = 2,
        .pStages             = shader_stages,
        .pVertexInputState   = &vertex_input_info,
        .pInputAssemblyState = &input_assembly,
        .pViewportState      = &viewport_state,
        .pRasterizationState = &rasterizer,
        .pMultisampleState   = &multisampling,
        .pDepthStencilState  = nullptr,
        .pColorBlendState    = &color_blending_create_info,
        .pDynamicState       = nullptr,
        .layout              = ctx.pipeline_layout,
        .renderPass          = ctx.render_pass,
        .subpass             = 0,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = -1};

    if (vkCreateGraphicsPipelines(ctx.logical_device, VK_NULL_HANDLE, 1,
                                  &pipeline_create_info, nullptr,
                                  &ctx.graphics_pipeline) != VK_SUCCESS) {
        throw std::runtime_error("Could not create pipeline");
    }

    /// Create the framebuffers
    ctx.swap_chain_framebuffers.resize(ctx.image_views.size());
    for (size_t i = 0; i < ctx.image_views.size(); i++) {
        VkImageView attachments[] = {ctx.image_views[i]};

        VkFramebufferCreateInfo framebuffer_create_info = {
            .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass      = ctx.render_pass,
            .attachmentCount = 1,
            .pAttachments    = attachments,
            .width           = ctx.swap_chain_extent.width,
            .height          = ctx.swap_chain_extent.height,
            .layers          = 1};
        if (vkCreateFramebuffer(ctx.logical_device, &framebuffer_create_info,
                                nullptr, &ctx.swap_chain_framebuffers[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("Could not create frame buffer");
        }
    }

    /// Command Pool
    VkCommandPoolCreateInfo command_pool_create_info = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queue_family_graphics_idx};

    if (vkCreateCommandPool(ctx.logical_device, &command_pool_create_info,
                            nullptr, &ctx.command_pool) != VK_SUCCESS) {
        throw std::runtime_error("Could not create command pool");
    }

    /// Command Buffer
    VkCommandBufferAllocateInfo command_buffer_alloc_info = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool        = ctx.command_pool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1};

    if (vkAllocateCommandBuffers(ctx.logical_device, &command_buffer_alloc_info,
                                 &ctx.command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Could not create command buffer");
    }

    /// Sync Objects
    VkSemaphoreCreateInfo sema_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT};

    vkCreateSemaphore(ctx.logical_device, &sema_create_info, nullptr,
                      &ctx.image_available_sema);
    vkCreateSemaphore(ctx.logical_device, &sema_create_info, nullptr,
                      &ctx.render_finished_sema);
    vkCreateFence(ctx.logical_device, &fence_create_info, nullptr,
                  &ctx.in_flight_fence);
}

uint32_t start_render(vulkan_test_ctx_t &ctx) {
    vkWaitForFences(ctx.logical_device, 1, &ctx.in_flight_fence, VK_TRUE,
                    UINT64_MAX);
    vkResetFences(ctx.logical_device, 1, &ctx.in_flight_fence);

    uint32_t image_idx = 0;
    vkAcquireNextImageKHR(ctx.logical_device, ctx.swap_chain, UINT64_MAX,
                          ctx.image_available_sema, VK_NULL_HANDLE, &image_idx);

    vkResetCommandBuffer(ctx.command_buffer, 0);

    VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags            = 0,
        .pInheritanceInfo = nullptr};

    if (vkBeginCommandBuffer(ctx.command_buffer, &command_buffer_begin_info) !=
        VK_SUCCESS) {
        throw std::runtime_error("Could not begin command buffer.");
    }

    VkClearValue          clear_color            = {1, 0, 0, 1};
    VkRenderPassBeginInfo render_pass_begin_info = {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass      = ctx.render_pass,
        .framebuffer     = ctx.swap_chain_framebuffers[image_idx],
        .renderArea      = {.offset = {0, 0}, .extent = ctx.swap_chain_extent},
        .clearValueCount = 1,
        .pClearValues    = &clear_color};

    vkCmdBeginRenderPass(ctx.command_buffer, &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);

    return image_idx;

}
void end_render(vulkan_test_ctx_t &ctx, uint32_t image_idx) {

    // vkCmdBindPipeline(ctx.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
    //                   ctx.graphics_pipeline);

    // vkCmdDraw(ctx.command_buffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(ctx.command_buffer);

    if (vkEndCommandBuffer(ctx.command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer");
    }

    /// submit the command buffer
    VkSemaphore          wait_semaphores[] = {ctx.image_available_sema};
    VkPipelineStageFlags wait_stages[]     = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore  signal_semaphores[] = {ctx.render_finished_sema};
    VkSubmitInfo submission          = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                        .waitSemaphoreCount   = 1,
                                        .pWaitSemaphores      = wait_semaphores,
                                        .pWaitDstStageMask    = wait_stages,
                                        .commandBufferCount   = 1,
                                        .pCommandBuffers      = &ctx.command_buffer,
                                        .signalSemaphoreCount = 1,
                                        .pSignalSemaphores    = signal_semaphores};
    if (vkQueueSubmit(ctx.graphics_queue, 1, &submission,
                      ctx.in_flight_fence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer");
    }

    VkSwapchainKHR   swap_chains[] = {ctx.swap_chain};
    VkPresentInfoKHR present_info  = {.sType =
                                          VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                      .waitSemaphoreCount = 1,
                                      .pWaitSemaphores    = signal_semaphores,
                                      .swapchainCount     = 1,
                                      .pSwapchains        = swap_chains,
                                      .pImageIndices      = &image_idx};
    vkQueuePresentKHR(ctx.present_queue, &present_info);
}

void terminate_vulkan(vulkan_test_ctx_t &ctx) {
    vkDestroySemaphore(ctx.logical_device, ctx.image_available_sema, nullptr);
    vkDestroySemaphore(ctx.logical_device, ctx.render_finished_sema, nullptr);
    vkDestroyFence(ctx.logical_device, ctx.in_flight_fence, nullptr);
    vkDestroyCommandPool(ctx.logical_device, ctx.command_pool, nullptr);
    for (auto fb : ctx.swap_chain_framebuffers) {
        vkDestroyFramebuffer(ctx.logical_device, fb, nullptr);
    }
    vkDestroyPipeline(ctx.logical_device, ctx.graphics_pipeline, nullptr);
    vkDestroyPipelineLayout(ctx.logical_device, ctx.pipeline_layout, nullptr);
    vkDestroyRenderPass(ctx.logical_device, ctx.render_pass, nullptr);
    vkDestroyShaderModule(ctx.logical_device, ctx.vert_shader_module, nullptr);
    vkDestroyShaderModule(ctx.logical_device, ctx.frag_shader_module, nullptr);
    for (auto image_view : ctx.image_views) {
        vkDestroyImageView(ctx.logical_device, image_view, nullptr);
    }
    vkDestroySwapchainKHR(ctx.logical_device, ctx.swap_chain, nullptr);
    vkDestroySurfaceKHR(ctx.instance, ctx.surface, nullptr);
    vkDestroyDevice(ctx.logical_device, nullptr);
    vkDestroyInstance(ctx.instance, nullptr);
}

int main(int, char **) {
    vulkan_test_ctx_t vulkan_test_ctx;

    const uint32_t WIDTH  = 800;
    const uint32_t HEIGHT = 600;

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    vulkan_test_ctx.glfw_window =
        glfwCreateWindow(WIDTH, HEIGHT, "Vulkan window", nullptr, nullptr);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported\n";

    init_vulkan(vulkan_test_ctx);

    // Initilize MonkVG
    vgCreateContextMNK(WIDTH, HEIGHT, VG_RENDERING_BACKEND_TYPE_VULKAN);

    // Initialize Vulkan Specific MonkVG with the Vulkan Context
    vgSetVulkanContextMNK(
        vulkan_test_ctx.instance, vulkan_test_ctx.physical_device,
        vulkan_test_ctx.logical_device, vulkan_test_ctx.render_pass,
        vulkan_test_ctx.command_buffer, 0);

    // create a simple box path
    VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1,
                               0, 0, 0, VG_PATH_CAPABILITY_ALL);
    vguRect(path, 0.0f, 0.0f, 100.0f, 150.0f);

    // create fill and stroke paints
    VGPaint fill_paint = vgCreatePaint();
    vgSetPaint(fill_paint, VG_FILL_PATH);
    VGfloat fill_color[4] = {0.0f, 1.0f, 0.0f, 1.0f};
    vgSetParameterfv(fill_paint, VG_PAINT_COLOR, 4, &fill_color[0]);

    VGPaint stroke_paint = vgCreatePaint();
    vgSetPaint(stroke_paint, VG_STROKE_PATH);
    VGfloat stroke_color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
    vgSetParameterfv(stroke_paint, VG_PAINT_COLOR, 4, &stroke_color[0]);

    while (!glfwWindowShouldClose(vulkan_test_ctx.glfw_window)) {
        uint32_t image_idx = start_render(vulkan_test_ctx);

        /// do an ortho camera
        // NOTE:  this is not standard OpenVG
        // NOTE: Bottom left is 0,0
        vgPushOrthoCamera(0.0f, (float)WIDTH, 0.0f, (float)HEIGHT, -1.0f, 1.0f);

        /// draw the basic path
        // set up path trasnform
        vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
        vgLoadIdentity();
        vgTranslate(WIDTH / 2, HEIGHT / 2);

        // stroke wideth
        vgSetf(VG_STROKE_LINE_WIDTH, 5.0f);

        // fill and stroke paints
        vgSetPaint(fill_paint, VG_FILL_PATH);
        vgSetPaint(stroke_paint, VG_STROKE_PATH);

        // draw the path with fill and stroke
        vgDrawPath(path, VG_FILL_PATH | VG_STROKE_PATH);

        vgPopOrthoCamera();

        end_render(vulkan_test_ctx, image_idx);

        glfwPollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }

    vgDestroyContextMNK();

    terminate_vulkan(vulkan_test_ctx);

    glfwDestroyWindow(vulkan_test_ctx.glfw_window);

    glfwTerminate();
}
