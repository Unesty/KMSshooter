#include <fcntl.h>
#include <unistd.h>

VkPipeline graphics_pipeline;
VkRenderPass renderPass;

int readFile(const char *path, size_t *nbyte_, void **buf_) {
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

void vulkan_shader(const char *path, VkDevice device, VkShaderModule *shaderModule_) {
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
void vulkan_compute_pipeline(VkDevice device, VkPipeline *compute_pipeline_, VkRenderPass *renderPass_, VkPipelineLayout *layout, VkDescriptorSetLayout *compute_descriptorSetLayout) {
	VkShaderModule compModule;
	vulkan_shader("comp.spv", device, &compModule);

	VkPipelineLayoutCreateInfo layoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.setLayoutCount = 1,
		.pSetLayouts = compute_descriptorSetLayout,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = NULL
	};
	VK_CHECK_RESULT(vkCreatePipelineLayout(device, &layoutCreateInfo, NULL, layout));

	VkPipelineShaderStageCreateInfo compStageInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext = NULL,
		.stage = VK_SHADER_STAGE_COMPUTE_BIT,
		.module = compModule,
		.pName = "main"
	};

	VkPipelineShaderStageCreateInfo stages[] = {compStageInfo};


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
		.layout = *layout,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = 0
	};

	
	VkPipeline compute_pipeline;
	VK_CHECK_RESULT(vkCreateComputePipelines(device, 0, 1, &computePipelineCreateInfo, 0, &compute_pipeline));
	
	
	
	vkDestroyShaderModule(device, compModule, NULL);
	*compute_pipeline_ = compute_pipeline;
}

void vulkan_graphics_pipeline(VkDevice device, VkPipeline *graphics_pipeline_, VkRenderPass
*renderPass_, VkPipelineLayout *layout) {
	VkShaderModule vertModule, fragModule;
	vulkan_shader("vert.spv", device, &vertModule);
	vulkan_shader("frag.spv", device, &fragModule);

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

	VkPipelineShaderStageCreateInfo stages[] = {vertStageInfo, fragStageInfo};

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

	VkViewport viewport = {0.0f, 0.0f, 400.0f, 400.0f, 0.0f, 1.0f}; //TODO

	VkRect2D scissor = {0, 0, 400, 400};

	VkPipelineViewportStateCreateInfo viewportState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor
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
	VK_CHECK_RESULT(vkCreatePipelineLayout(device, &layoutCreateInfo, NULL, layout));

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

	VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassCreateInfo, NULL,
		&renderPass));

	VkGraphicsPipelineCreateInfo graphics_pipelineCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.stageCount = sizeof(stages)/sizeof(VkPipelineShaderStageCreateInfo),
		.pStages = stages,
		.pVertexInputState = &vertexInputState,
		.pInputAssemblyState = &inputAssemblyState,
		.pTessellationState = NULL,
		.pViewportState = &viewportState,
		.pRasterizationState = &rasterizationState,
		.pMultisampleState = &multisampleState,
		.pDepthStencilState = NULL,
		.pColorBlendState = &colorBlendState,
		.pDynamicState = NULL,
		.layout = *layout,
		.renderPass = renderPass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};

	VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1,
		&graphics_pipelineCreateInfo, NULL, &graphics_pipeline));

	vkDestroyShaderModule(device, vertModule, NULL);
	vkDestroyShaderModule(device, fragModule, NULL);
	*graphics_pipeline_ = graphics_pipeline;
	*renderPass_ = renderPass;
}

void destroy_vulkan_graphics_pipeline(VkDevice device, VkPipeline *graphics_pipeline_, VkRenderPass *renderPass_, VkPipelineLayout *layout) {
    vkDestroyPipelineLayout(device, *layout, NULL);
    vkDestroyPipeline(device, graphics_pipeline, NULL);
}
void destroy_vulkan_compute_pipeline(VkDevice device, VkPipeline *compute_pipeline_, VkPipelineLayout *layout) {
    vkDestroyPipelineLayout(device, *layout, NULL);
    vkDestroyPipeline(device, *compute_pipeline_, NULL);
}
