#define _GNU_SOURCE
#ifdef USE_VOLK
#include <volk.h>
#include <volk.c>
#else
#include <vulkan/vulkan.h>
#endif
#include <swa/swa.h>
#include <swa/key.h>
#include <dlg/dlg.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <time.h>

#include "globals.h"

#include "utils/vkutil.h"

#include "vulkan/instance.c"
#include "vulkan/device.c"
#include "vulkan/swapchain.c"
#include "vulkan/pipeline.c"
#include "vulkan/command.c"


#include "swa_listeners.c"

static const struct swa_window_listener window_listener = {
	.close = window_close,
	.resize = window_resize,
	.key = window_key,
	.mouse_move = mouse_move,
	.mouse_cross = mouse_cross,
	.mouse_button = mouse_button,
	.mouse_wheel = mouse_wheel,
	.touch_begin = touch_begin,
	.touch_update = touch_update,
	.touch_end = touch_end,
	.touch_cancel = touch_cancel,
	.focus = window_focus,
};

int readFile(const char *path, size_t *nbyte_, void **buf_)
{
	int fd = open(path, O_RDONLY);

	if (fd < 0)
		return -1;

	size_t nbyte = lseek(fd, 0, SEEK_END);

	if (nbyte < 0)
		goto clean0;

	void *buf = malloc(nbyte);

	if (!buf)
		goto clean0;

	if (lseek(fd, 0, SEEK_SET) < 0)
		goto clean1;

	if (read(fd, buf, nbyte) < 0)
		goto clean1;

	if (close(fd) < 0)
		goto clean1;

	*nbyte_ = nbyte;
	*buf_ = buf;
	return 0;
clean1:
	free(buf);
clean0:
	close(fd);
	return -1;
}

void vulkan_shader(const char *path, VkDevice device, VkShaderModule *shaderModule_)
{
	size_t codeSize;
	uint32_t *code;

	if (readFile(path, &codeSize, (void**)&code)) {
		perror("[ERROR] readFile");
		exit(EXIT_FAILURE);
	}

	VkShaderModuleCreateInfo shaderModuleCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.codeSize = codeSize,
		.pCode = code
	};
	VkShaderModule shaderModule;
	VK_CHECK_RESULT(vkCreateShaderModule(device, &shaderModuleCreateInfo, NULL, &shaderModule));

	*shaderModule_ = shaderModule;
}

int main(int argc, char *argv[], char *envp[])
{
#ifdef USE_VOLK
	volkInitialize();
#endif
	struct swa_display* dpy = swa_display_autocreate("compute triangle");

	if(!dpy) {
		dlg_fatal("No swa backend available");
		return EXIT_FAILURE;
	}

	// Make sure the display has support for vulkan surfaces
	if(!(swa_display_capabilities(dpy) & swa_display_cap_vk)) {
		dlg_fatal("Display doesn't support vulkan");
		goto cleanup_dpy;
	}
	// Get the vulkan instance extensions required by the display
	// to create vulkan surfaces.

	uint32_t vkExtensionCount = 1;
	const char** instance_extensions = swa_display_vk_extensions(dpy, &vkExtensionCount);

	for (int i = 0; i < vkExtensionCount; ++i) {
		printf("%s\n", instance_extensions[i]);
	}

	VkApplicationInfo applicationInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "hello vulkan compute triangle",
		.apiVersion = VK_MAKE_VERSION(1,2,0)
	};

	VkInstanceCreateInfo instanceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = NULL,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL,
		.enabledExtensionCount = vkExtensionCount+1,
		.ppEnabledExtensionNames = instance_extensions,
		.pApplicationInfo = &applicationInfo
	};

	VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, NULL, &state.instance));

#ifdef USE_VOLK
	volkLoadInstance(state.instance);
#endif

	uint32_t n = 1;
	vkEnumeratePhysicalDevices(state.instance, &n, &state.physicalDevice);
	float priority = 1.0f;
	VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueCount = 1,
		.pQueuePriorities = &priority
	};

	const char *device_extensions[] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	VkDeviceCreateInfo deviceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &deviceQueueCreateInfo,
		.enabledExtensionCount = sizeof(device_extensions)/sizeof(char*),
		.ppEnabledExtensionNames = device_extensions
	};
	VK_CHECK_RESULT(vkCreateDevice(state.physicalDevice, &deviceCreateInfo, NULL, &state.device));

#ifdef USE_VOLK
	volkLoadDevice(state.device);
#endif

	// Create the vulkan instance. Make sure to enable the extensions
	// the display requires. We can't create the device here since
	// the queues we need depend on the created vulkan surface which
	// in turn depens on the vulkan instance.
	state.w=640;
	state.h=480;

	struct swa_window_settings settings;
	swa_window_settings_default(&settings);
	settings.title = "pheh";
	settings.surface = swa_surface_vk;
	settings.surface_settings.vk.instance = (uint64_t) state.instance;
	settings.listener = &window_listener;
	settings.width = state.w;
	settings.height = state.h;
	/*
	struct swa_window_settings settings = {
		.cursor.type = swa_cursor_default,
		.title = "pheh",
		.state = swa_window_state_normal,
		.surface = swa_surface_vk,
		.surface_settings.vk.instance = (uint64_t) state.instance,
		.listener = &window_listener,
		.width = state.w,
		.height = state.h
	};
	*/
	struct swa_window* win = swa_display_create_window(dpy, &settings);

	if(!win) {
		dlg_fatal("Failed to create window");
		//ret = EXIT_FAILURE;
		goto cleanup_state;
	}
	
	state.surface = (VkSurfaceKHR) swa_window_get_vk_surface(win);

	if(!state.surface) {
		//ret = EXIT_FAILURE;
		dlg_error("Couldn't get vk surface from swa window");
		goto cleanup_win;
	}

	swa_window_set_userdata(win, &state);
	// timespec_get(&last_redraw, TIME_UTC);

	VkBool32 supported;
	vkGetPhysicalDeviceSurfaceSupportKHR(state.physicalDevice, 0, state.surface, &supported);

	if (supported != VK_TRUE)
		return 1;

	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(state.physicalDevice, state.surface, &surfaceCapabilities);

	VkSwapchainCreateInfoKHR swapchainCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = NULL,
		.flags = 0,
		.surface = state.surface,
		.minImageCount = state.n_bufs,
		.imageFormat = VK_FORMAT_B8G8R8A8_UNORM,
		.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.imageExtent = {state.w, state.h},
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
		.preTransform = surfaceCapabilities.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_FIFO_KHR,
		.clipped = VK_TRUE,
		.oldSwapchain = state.swapchain
	};
	VK_CHECK_RESULT(vkCreateSwapchainKHR(state.device, &swapchainCreateInfo, NULL, &state.swapchain))
	vkDestroySwapchainKHR(state.device, state.swapchain_info.oldSwapchain, NULL);
	state.swapchain_info.oldSwapchain = VK_NULL_HANDLE;

	// vertex buffer

	state.vcnt = 5;
	VkBufferCreateInfo bufferCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.size = state.vcnt,
		.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_STORAGE_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
	};

	VK_CHECK_RESULT(vkCreateBuffer(state.device, &bufferCreateInfo, NULL, &state.vertexBuffer));

	VkMemoryRequirements memRequirements; //TODO probably move somewhere else
	vkGetBufferMemoryRequirements(state.device, state.vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = NULL,
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = findMemoryType(state.physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	};
	VkDeviceMemory vertexBufferMemory;
	VK_CHECK_RESULT(vkAllocateMemory(state.device, &memoryAllocateInfo, NULL, &vertexBufferMemory));
	vkBindBufferMemory(state.device, state.vertexBuffer, vertexBufferMemory, 0);

	//descriptor sets

	VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[1] = {
		{
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
			.pImmutableSamplers = NULL
		}
	};

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.bindingCount = 1,
		.pBindings = descriptorSetLayoutBindings
	};

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(state.device, &descriptorSetLayoutCreateInfo, 0, &state.compute_descriptorSetLayout));

	VkDescriptorPoolSize descriptorPoolSize = {
		.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		.descriptorCount = 1
	};

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.maxSets = 1,
		.poolSizeCount = 1,
		.pPoolSizes = &descriptorPoolSize
	};

	VK_CHECK_RESULT(vkCreateDescriptorPool(state.device, &descriptorPoolCreateInfo, 0, &state.compute_descriptorPool));

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = NULL,
		.descriptorPool = state.compute_descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &state.compute_descriptorSetLayout
	};

	VK_CHECK_RESULT(vkAllocateDescriptorSets(state.device, &descriptorSetAllocateInfo, &state.compute_descriptorSet));

	vkGetBufferMemoryRequirements(state.device, state.vertexBuffer, &memRequirements);

	VkDescriptorBufferInfo descriptorBufferInfo = {
		.buffer = state.vertexBuffer,
		.offset = 0,
		.range = memRequirements.size
	};

	VkWriteDescriptorSet writeDescriptorSet[1] = {
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = state.compute_descriptorSet,
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.pImageInfo = NULL,
			.pBufferInfo = &descriptorBufferInfo,
			.pTexelBufferView = NULL
		}
	};

	vkUpdateDescriptorSets(state.device, 1, writeDescriptorSet, 0, 0);

	// pipeline

	VkShaderModule compModule;
	vulkan_shader("spv/main.comp.spv", state.device, &compModule);

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.setLayoutCount = 1,
		.pSetLayouts = &state.compute_descriptorSetLayout,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = NULL
	};
	VK_CHECK_RESULT(vkCreatePipelineLayout(state.device, &pipelineLayoutCreateInfo, NULL, &state.compute_pipelineLayout));

	VkPipelineShaderStageCreateInfo compStageInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext = NULL,
		.stage = VK_SHADER_STAGE_COMPUTE_BIT,
		.module = compModule,
		.pName = "main"
	};


	VkComputePipelineCreateInfo computePipelineCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.stage = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.stage = VK_SHADER_STAGE_COMPUTE_BIT,
			.module = compModule,
			.pName = "main", // XXX: Hardcoded name
			.pSpecializationInfo = NULL
		},
		.layout = state.compute_pipelineLayout,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = 0
	};


	VkPipeline compute_pipeline;
	VK_CHECK_RESULT(vkCreateComputePipelines(state.device, 0, 1, &computePipelineCreateInfo, 0, &compute_pipeline));



	vkDestroyShaderModule(state.device, compModule, NULL);

	// graphics pipeline

	VkShaderModule vertModule, fragModule;
	vulkan_shader("spv/main.vert.spv", state.device, &vertModule);
	vulkan_shader("spv/main.frag.spv", state.device, &fragModule);

	VkPipelineShaderStageCreateInfo vertStageInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext = NULL,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = vertModule,
		.pName = "main"
	};

	VkPipelineShaderStageCreateInfo fragStageInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext = NULL,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = fragModule,
		.pName = "main"
	};

	VkPipelineShaderStageCreateInfo graphics_stages[] = {vertStageInfo, fragStageInfo};

	VkVertexInputBindingDescription inputBindingDesc = {
		.binding = 0,
		.stride = sizeof(double), //TODO: check this
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};

	VkVertexInputAttributeDescription inputAttributeDesc = {
		.location = 0,
		.binding = 0,
		.format = VK_FORMAT_R32G32_SFLOAT,
		.offset = 0
	};

	VkPipelineVertexInputStateCreateInfo vertexInputState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &inputBindingDesc,
		.vertexAttributeDescriptionCount = 1,
		.pVertexAttributeDescriptions = &inputAttributeDesc
	};

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
		.primitiveRestartEnable = VK_FALSE
	};

	state.viewport.x = 0;
	state.viewport.y = 0;
	state.viewport.width = 1366;
	state.viewport.height = 768;
	state.viewport.minDepth = 0;
	state.viewport.maxDepth = 10000000000;
	

	state.scissor.offset.x = 0;
	state.scissor.offset.y = 0;
	state.scissor.extent.width = 1366;
	state.scissor.extent.height = 768;

	VkPipelineViewportStateCreateInfo viewportState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.viewportCount = 1,
		.pViewports = &state.viewport,
		.scissorCount = 1,
		.pScissors = &state.scissor
	};

	VkPipelineRasterizationStateCreateInfo rasterizationState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = 0.0f,
		.depthBiasClamp = 0.0f,
		.depthBiasSlopeFactor = 0.0f,
		.lineWidth = 1.0f
	};

	VkPipelineMultisampleStateCreateInfo multisampleState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE,
		.minSampleShading = 1.0f,
		.pSampleMask = NULL,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE
	};

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {
		.blendEnable = VK_FALSE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};
	VkPipelineColorBlendStateCreateInfo colorBlendState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = &colorBlendAttachment,
		.blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
	};

	VkPipelineLayoutCreateInfo layoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.setLayoutCount = 0,
		.pSetLayouts = NULL,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = NULL
	};
	VK_CHECK_RESULT(vkCreatePipelineLayout(state.device, &layoutCreateInfo, NULL, &state.graphics_pipelineLayout));

	VkAttachmentDescription colorAttachment = {
		.flags = 0,
		.format = VK_FORMAT_B8G8R8A8_UNORM, //TODO
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentReference colorAttachmentRef = {
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpass = {
		.flags = 0,
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.inputAttachmentCount = 0,
		.pInputAttachments = NULL,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachmentRef,
		.pResolveAttachments = 0,
		.pDepthStencilAttachment = NULL,
		.preserveAttachmentCount = 0,
		.pPreserveAttachments = NULL
	};

	VkSubpassDependency subpassDependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dependencyFlags = 0
	};

	VkRenderPassCreateInfo renderPassCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.attachmentCount = 1,
		.pAttachments = &colorAttachment,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 1,
		.pDependencies = &subpassDependency
	};

	VK_CHECK_RESULT(vkCreateRenderPass(state.device, &renderPassCreateInfo, NULL, &state.renderPass));

	VkGraphicsPipelineCreateInfo graphics_pipelineCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.stageCount = sizeof(graphics_stages)/sizeof(VkPipelineShaderStageCreateInfo),
		.pStages = graphics_stages,
		.pVertexInputState = &vertexInputState,
		.pInputAssemblyState = &inputAssemblyState,
		.pTessellationState = NULL,
		.pViewportState = &viewportState,
		.pRasterizationState = &rasterizationState,
		.pMultisampleState = &multisampleState,
		.pDepthStencilState = NULL,
		.pColorBlendState = &colorBlendState,
		.pDynamicState = NULL,
		.layout = state.graphics_pipelineLayout,
		.renderPass = state.renderPass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};

	VK_CHECK_RESULT(vkCreateGraphicsPipelines(state.device, VK_NULL_HANDLE, 1,
					&graphics_pipelineCreateInfo, NULL, &state.graphics_pipeline));

	vkDestroyShaderModule(state.device, vertModule, NULL);
	vkDestroyShaderModule(state.device, fragModule, NULL);

	//VkImageView *imageViews; //move this 3 vars to struct state
	//VkImage *images;
	//VkFramebuffer *framebuffers;

	vkGetSwapchainImagesKHR(state.device, state.swapchain, &state.n_bufs, NULL);
	state.images = malloc(state.n_bufs*sizeof(VkImage)); //TODO: free
	vkGetSwapchainImagesKHR(state.device, state.swapchain, &state.n_bufs, state.images);

	state.imageViews = malloc(state.n_bufs*sizeof(VkImageView)); //TODO: free
	state.framebuffers = malloc(state.n_bufs*sizeof(VkFramebuffer)); //TODO: free

	for (int i=0; i<state.n_bufs; i++) {
		createImageView(state.device, state.images[i], state.imageViews+i);
		createFramebuffer(state.device, state.renderPass, state.imageViews[i], state.framebuffers+i, state.w, state.h);
	}

	//*framebufferCount_ = state.n_bufs;
	//*framebuffers_ = framebuffers;

	VkCommandPoolCreateInfo commandPoolCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.queueFamilyIndex = 0
	};
	VK_CHECK_RESULT(vkCreateCommandPool(state.device, &commandPoolCreateInfo, NULL, &state.commandPool))

	state.commandBuffers = malloc(state.n_bufs*sizeof(VkCommandBuffer)); //TODO: free
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = state.commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = state.n_bufs
	};
	VK_CHECK_RESULT(vkAllocateCommandBuffers(state.device, &commandBufferAllocateInfo, state.commandBuffers))

	for (int i=0; i<state.n_bufs; i++) {
		VkCommandBufferBeginInfo commandBufferBeginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = NULL,
			.flags = 0,
			.pInheritanceInfo = NULL
		};

		VK_CHECK_RESULT(vkBeginCommandBuffer(state.commandBuffers[i], &commandBufferBeginInfo));

		VkClearValue clearColor = {0.3f, 0.0f, 0.0f, 1.0f};
		VkRenderPassBeginInfo renderPassBeginInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.pNext = NULL,
			.renderPass = state.renderPass,
			.framebuffer = framebuffers[i],
			.renderArea = {0, 0, 640, 480},
			.clearValueCount = 1,
			.pClearValues = &clearColor
		};

		vkCmdBindPipeline(state.commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, state.compute_pipeline);
		vkCmdBindDescriptorSets(state.commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, state.compute_pipelineLayout, 0, 1, &state.compute_descriptorSet, 0, 0);
		vkCmdDispatch(state.commandBuffers[i], state.n_bufs, 1, 1);

		vkCmdBeginRenderPass(state.commandBuffers[i], &renderPassBeginInfo,
							 VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(state.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
						  state.graphics_pipeline);
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(state.commandBuffers[i], 0, 1, &state.vertexBuffer, &offset);
		vkCmdDraw(state.commandBuffers[i], state.vcnt, 1, 0, 0);

		vkCmdEndRenderPass(state.commandBuffers[i]);
		VK_CHECK_RESULT(vkEndCommandBuffer(state.commandBuffers[i]));
	}

	// main loop
	state.run = true;
	state.dpy = dpy;

	VkSemaphoreCreateInfo semaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0
	};

	VkSemaphoreCreateInfo sem_info = {0};
	sem_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VK_CHECK_RESULT(vkCreateSemaphore(state.device, &sem_info, NULL, &state.acquire_sem))
	VK_CHECK_RESULT(vkCreateSemaphore(state.device, &sem_info, NULL, &state.render_sem))

	vkGetDeviceQueue(state.device, 0, 0, &state.queue);

	while (state.run) {
		if(!swa_display_dispatch(dpy, false)) {
			break;
		}

		if(state.resized) {
			window_resize(win, 1366, 768);
		}

		struct timespec last_redraw;

		struct timespec now;

		float ms;

		timespec_get(&now, TIME_UTC);

		ms = (now.tv_nsec - last_redraw.tv_nsec) / (1000.f * 1000.f);

		ms += 1000.f * (now.tv_sec - last_redraw.tv_sec);

		dlg_info("Time between redraws: %f", ms);

		timespec_get(&now, TIME_UTC);

		last_redraw = now;

		vkDeviceWaitIdle(state.device); // why so slow?

		timespec_get(&now, TIME_UTC);

		ms = (now.tv_nsec - last_redraw.tv_nsec) / (1000.f * 1000.f);

		ms += 1000.f * (now.tv_sec - last_redraw.tv_sec);

		dlg_info("vkDeviceWaitIdle time: %f", ms);

		timespec_get(&now, TIME_UTC);

		last_redraw = now;

		// acquire image
		// we treat suboptimal as success here
		uint32_t id;

		VK_CHECK_RESULT(vkAcquireNextImageKHR(state.device, state.swapchain, UINT64_MAX, state.acquire_sem, VK_NULL_HANDLE, &id));

		timespec_get(&now, TIME_UTC);
		ms = (now.tv_nsec - last_redraw.tv_nsec) / (1000.f * 1000.f);
		ms += 1000.f * (now.tv_sec - last_redraw.tv_sec);
		dlg_info("acquire image time: %f", ms);
		timespec_get(&now, TIME_UTC);
		last_redraw = now;


		// submit render commands
		VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo si = {0};
		si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		si.pCommandBuffers = &state.commandBuffers[id];
		si.commandBufferCount = 1u;
		si.waitSemaphoreCount = 1u;
		si.pWaitSemaphores = &state.acquire_sem;
		si.pWaitDstStageMask = &stage;
		si.signalSemaphoreCount = 1u;
		si.pSignalSemaphores = &state.render_sem;

		VK_CHECK_RESULT(vkQueueSubmit(state.queue, 1, &si, VK_NULL_HANDLE));

		timespec_get(&now, TIME_UTC);
		ms = (now.tv_nsec - last_redraw.tv_nsec) / (1000.f * 1000.f);
		ms += 1000.f * (now.tv_sec - last_redraw.tv_sec);
		dlg_info("vkQueueSubmit time: %f", ms);
		timespec_get(&now, TIME_UTC);
		last_redraw = now;

		swa_window_surface_frame(win);

		timespec_get(&now, TIME_UTC);
		ms = (now.tv_nsec - last_redraw.tv_nsec) / (1000.f * 1000.f);
		ms += 1000.f * (now.tv_sec - last_redraw.tv_sec);
		dlg_info("swa_window_surface_frame time: %f", ms);
		timespec_get(&now, TIME_UTC);
		last_redraw = now;

		// present
		VkPresentInfoKHR present_info = {0};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &state.swapchain;
		present_info.pImageIndices = &id;
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = &state.render_sem;

		VK_CHECK_RESULT(vkQueuePresentKHR(state.queue, &present_info));

		timespec_get(&now, TIME_UTC);
		ms = (now.tv_nsec - last_redraw.tv_nsec) / (1000.f * 1000.f);
		ms += 1000.f * (now.tv_sec - last_redraw.tv_sec);
		dlg_info("vkQueuePresentKHR time: %f", ms);
		timespec_get(&now, TIME_UTC);
		last_redraw = now;


		swa_window_refresh(win);

		timespec_get(&now, TIME_UTC);
		ms = (now.tv_nsec - last_redraw.tv_nsec) / (1000.f * 1000.f);
		ms += 1000.f * (now.tv_sec - last_redraw.tv_sec);
		dlg_info("swa_window_refresh time: %f", ms);
		timespec_get(&now, TIME_UTC);
		last_redraw = now;
	}
	
cleanup_win:
	/*
	if(state.acquire_sem) {
		vkDestroySemaphore(state.device, state.acquire_sem, NULL);
	}
	if(state.render_sem) {
		vkDestroySemaphore(state.device, state.render_sem, NULL);
	}
	*/
	if(state.commandBuffers) {
		vkFreeCommandBuffers(state.device, state.commandPool, 1, state.commandBuffers);
	}

	if(state.commandPool) {
		vkDestroyCommandPool(state.device, state.commandPool, NULL);
	}


	//free(commandBuffers);
cleanup_state:
	swa_window_destroy(win);

	if(state.device) {
		vkDestroyDevice(state.device, NULL);
	}

	if(state.instance) {
		vkDestroyInstance(state.instance, NULL);
	}

cleanup_dpy:
	swa_display_destroy(dpy);

	return 0;
}
