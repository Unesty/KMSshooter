#include "globals.h"

int vulkan()
{
	volkInitialize();
	struct swa_display* dpy = swa_display_autocreate("compute triangle");
	if(!dpy) {
		dlg_fatal("No swa backend available");
		return EXIT_FAILURE;
	}
		// Make sure the display has support for vulkan surfaces
	if(!(swa_display_capabilities(dpy) & swa_display_cap_vk)) {
		dlg_fatal("Display doesn't support vulkan");
//		ret = EXIT_FAILURE;
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

	volkLoadInstance(state.instance);

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

	volkLoadDevice(state.device);

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

	state.run = true;
	state.dpy = dpy;

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
		.minImageCount = surfaceCapabilities.minImageCount,
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

	VkBufferCreateInfo bufferCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.size = gfx.vcnt,
		.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_STORAGE_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
	};

	VK_CHECK_RESULT(vkCreateBuffer(state.device, &bufferCreateInfo, NULL, &gfx.vertexBuffer));

	VkMemoryRequirements memRequirements; //TODO probably move somewhere else
	vkGetBufferMemoryRequirements(state.device, gfx.vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = NULL,
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = findMemoryType(state.physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	};
	VkDeviceMemory vertexBufferMemory;
	VK_CHECK_RESULT(vkAllocateMemory(state.device, &memoryAllocateInfo, NULL, &vertexBufferMemory));
	vkBindBufferMemory(state.device, gfx.vertexBuffer, vertexBufferMemory, 0);

	//input buffer

	VkMemoryRequirements input_buffer_memoryRequirements;
	vkGetBufferMemoryRequirements(state.device, state.input_buffer, &input_buffer_memoryRequirements);	

	VkBufferCreateInfo input_bufferCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.size = sizeof(shader_input),
		.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
	};
	
	VK_CHECK_RESULT(vkCreateBuffer(state.device, &input_bufferCreateInfo, NULL, &state.input_buffer));

	VkMemoryAllocateInfo input_buffer_memoryAllocateInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = input_buffer_memoryRequirements.size,
		.memoryTypeIndex = findMemoryType(state.physicalDevice,
		 input_buffer_memoryRequirements.memoryTypeBits,
		  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	};
	VK_CHECK_RESULT(vkAllocateMemory(state.device,&input_buffer_memoryAllocateInfo,NULL,&state.input_buffer_memory));
	VK_CHECK_RESULT(vkBindBufferMemory(state.device,state.input_buffer,state.input_buffer_memory,0));
	VK_CHECK_RESULT(vkMapMemory(state.device,state.input_buffer_memory,0,sizeof(shader_input),0,state.input_buffer_map));

	//descriptor sets

	VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[2] = {
		{
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT|VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_FRAGMENT_BIT,
			.pImmutableSamplers = NULL
		},
		{
			.binding = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT|VK_SHADER_STAGE_VERTEX_BIT,
			.pImmutableSamplers = NULL
		}
	};

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.bindingCount = 2,
		.pBindings = descriptorSetLayoutBindings
	};

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(state.device, &descriptorSetLayoutCreateInfo, 0, &state.compute_descriptorSetLayout));

	VkDescriptorPoolSize descriptorPoolSizes[2] = {
	{
		.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1
	},
	{
		.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		.descriptorCount = 1
	}
	};

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.maxSets = 2,
		.poolSizeCount = 2,
		.pPoolSizes = descriptorPoolSizes
	};

	VK_CHECK_RESULT(vkCreateDescriptorPool(state.device, &descriptorPoolCreateInfo, 0, &state.compute_descriptorPool));

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = NULL,
		.descriptorPool = state.compute_descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &state.compute_descriptorSetLayout
	};

	VK_CHECK_RESULT(vkAllocateDescriptorSets(state.device, &descriptorSetAllocateInfo, &state.descriptorSet));

	vkGetBufferMemoryRequirements(state.device, gfx.vertexBuffer, &memRequirements);


	VkDescriptorBufferInfo input_descriptorBufferInfo = {
		.buffer = state.input_buffer,
		.offset = 0,
		.range = sizeof(shader_input)
	};
	
	VkDescriptorBufferInfo vertex_descriptorBufferInfo = {
		.buffer = gfx.vertexBuffer,
		.offset = sizeof(shader_input),
		.range = memRequirements.size
	};

	VkWriteDescriptorSet writeDescriptorSet[2] = {
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = state.descriptorSet,
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.pImageInfo = NULL,
			.pBufferInfo = &vertex_descriptorBufferInfo,
			.pTexelBufferView = NULL
		},
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = state.descriptorSet,
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pImageInfo = NULL,
			.pBufferInfo = &input_descriptorBufferInfo,
			.pTexelBufferView = NULL
		}
	};

	vkUpdateDescriptorSets(state.device, 2, writeDescriptorSet, 0, 0);

	//compute pipeline

	VkShaderModule compModule;
	vulkan_shader("comp.spv", state.device, &compModule);

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
	vulkan_shader("vert.spv", state.device, &vertModule);
	vulkan_shader("frag.spv", state.device, &fragModule);

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
	state.viewport = (VkViewport){0.0f, 0.0f, 640.0f, 480.0f, 0.0f, 1.0f};

	state.scissor = (VkRect2D){0, 0, 640, 480};

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

	for(uint32_t i = 0; i < state.n_bufs; i++) {

		// image view
		VkImageViewCreateInfo view_info = {0};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.format = state.swapchain_info.imageFormat;
		view_info.components.r = VK_COMPONENT_SWIZZLE_R;
		view_info.components.g = VK_COMPONENT_SWIZZLE_G;
		view_info.components.b = VK_COMPONENT_SWIZZLE_B;
		view_info.components.a = VK_COMPONENT_SWIZZLE_A;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.flags = 0;
		view_info.image = state.images[i];

		VK_CHECK_RESULT(vkCreateImageView(state.device, &view_info, NULL, &state.imageViews[i]));


		// framebuffer
		VkFramebufferCreateInfo fb_info = {0};
		fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fb_info.attachmentCount = 1;
		fb_info.pAttachments = &state.imageViews[i];
		fb_info.renderPass = state.renderPass;
		fb_info.width = state.swapchain_info.imageExtent.width;
		fb_info.height = state.swapchain_info.imageExtent.height;
		fb_info.layers = 1;

		VK_CHECK_RESULT(vkCreateFramebuffer(state.device, &fb_info, NULL, &state.framebuffers[i]));
	}

	free(state.imageViews);
	free(state.images);

	//command pool

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
			.framebuffer = state.framebuffers[i],
			.renderArea = {0, 0, 640, 480},
			.clearValueCount = 1,
			.pClearValues = &clearColor
		};

		vkCmdBindPipeline(state.commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, state.compute_pipeline);
		vkCmdBindDescriptorSets(state.commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, state.compute_pipelineLayout, 0, 1, &state.descriptorSet, 0, 0);
		vkCmdDispatch(state.commandBuffers[i], gfx.vcnt, 1, 1);

		vkCmdBeginRenderPass(state.commandBuffers[i], &renderPassBeginInfo,
							 VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(state.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
						  state.graphics_pipeline);
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(state.commandBuffers[i], 0, 1, &gfx.vertexBuffer, &offset);
		vkCmdDraw(state.commandBuffers[i], gfx.vcnt, 1, 0, 0);

		vkCmdEndRenderPass(state.commandBuffers[i]);
		VK_CHECK_RESULT(vkEndCommandBuffer(state.commandBuffers[i]));
	}
	

cleanup_win:
	if(state.device) {
		vkDeviceWaitIdle(state.device);
	}
	//destroy_render_buffers(state);
	if(gfx.vertexBufferMemory) {
		vkFreeMemory(state.device, gfx.vertexBufferMemory, NULL);
	}

	if(gfx.vertexBuffer) {
		vkDestroyBuffer(state.device, gfx.vertexBuffer, NULL);
	}
	for(int i = 0;i<state.n_bufs;++i) {
		vkDestroyFramebuffer(state.device,state.framebuffers[i],NULL);
	}

	if(state.swapchain) {
		vkDestroySwapchainKHR(state.device, state.swapchain, NULL);
	}
	if(state.renderPass) {
		vkDestroyRenderPass(state.device, state.renderPass, NULL);
	}
	vkDestroyPipelineLayout(state.device, state.compute_pipelineLayout, NULL);
    vkDestroyPipeline(state.device, state.compute_pipeline, NULL);
	vkDestroyPipelineLayout(state.device, state.graphics_pipelineLayout, NULL);
    vkDestroyPipeline(state.device, state.graphics_pipeline, NULL);
	for (int i = 0; i < state.n_bufs; ++i) {
			vkDestroyImageView(state.device,state.imageViews[i], NULL);
			//vkDestroyImage(device, images[i], NULL);
	}

	//TODO destroy fences
	
	if(state.commandBuffers) {
		vkFreeCommandBuffers(state.device, state.commandPool, 1, state.commandBuffers);
	}

	if(state.commandPool) {
		vkDestroyCommandPool(state.device, state.commandPool, NULL);
	}

		swa_window_destroy(win);

cleanup_state:
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
