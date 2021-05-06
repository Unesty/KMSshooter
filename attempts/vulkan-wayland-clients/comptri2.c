#define _GNU_SOURCE
#ifdef USE_VOLK
#include <volk.h>
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

static const char *vulkan_strerror(VkResult err)
{
#define ERR_STR(r) case VK_ ##r: return #r

	switch (err) {
		ERR_STR(SUCCESS);
		ERR_STR(NOT_READY);
		ERR_STR(TIMEOUT);
		ERR_STR(EVENT_SET);
		ERR_STR(EVENT_RESET);
		ERR_STR(INCOMPLETE);
		ERR_STR(SUBOPTIMAL_KHR);
		ERR_STR(ERROR_OUT_OF_HOST_MEMORY);
		ERR_STR(ERROR_OUT_OF_DEVICE_MEMORY);
		ERR_STR(ERROR_INITIALIZATION_FAILED);
		ERR_STR(ERROR_DEVICE_LOST);
		ERR_STR(ERROR_MEMORY_MAP_FAILED);
		ERR_STR(ERROR_LAYER_NOT_PRESENT);
		ERR_STR(ERROR_EXTENSION_NOT_PRESENT);
		ERR_STR(ERROR_FEATURE_NOT_PRESENT);
		ERR_STR(ERROR_INCOMPATIBLE_DRIVER);
		ERR_STR(ERROR_TOO_MANY_OBJECTS);
		ERR_STR(ERROR_FORMAT_NOT_SUPPORTED);
		ERR_STR(ERROR_SURFACE_LOST_KHR);
		ERR_STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
		ERR_STR(ERROR_OUT_OF_DATE_KHR);
		ERR_STR(ERROR_FRAGMENTED_POOL);
		ERR_STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
		ERR_STR(ERROR_VALIDATION_FAILED_EXT);
		ERR_STR(ERROR_INVALID_EXTERNAL_HANDLE);
		ERR_STR(ERROR_OUT_OF_POOL_MEMORY);
		ERR_STR(ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);

	default:
		return "<unknown>";
	}

#undef ERR_STR
}
#define vk_error(res, fmt) dlg_error(fmt ": %s (%d)", vulkan_strerror(res), res)

// Macro to check and display Vulkan return results
#define VK_CHECK_RESULT(f)																				\
{																										\
	VkResult res = (f);																					\
	if (res != VK_SUCCESS)																				\
	{																									\
		printf("Fatal : VkResult is \" %s \" in %s at line %d\n", vulkan_strerror(res), __FILE__, __LINE__); \
		exit(0); \
	}																									\
}

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
						VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}
	return 0;
}

//vulkan util functions
void createImageView(VkDevice device, VkImage image, VkImageView *imageView) {
	int r;
	VkImageSubresourceRange imageSubresourceRange = {
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};
	VkImageViewCreateInfo imageViewCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.image = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = VK_FORMAT_B8G8R8A8_UNORM,
		.components = {VK_COMPONENT_SWIZZLE_IDENTITY},
		.subresourceRange = imageSubresourceRange
	};
	r = vkCreateImageView(device, &imageViewCreateInfo, NULL, imageView);
	if (r) {
		fprintf(stderr, "[ERROR] vkCreateImageView: %d", r);
		exit(EXIT_FAILURE);
	}
}

void createFramebuffer(VkDevice device, VkRenderPass renderPass, VkImageView
imageView, VkFramebuffer *framebuffer, unsigned w, unsigned h) {
	int r;
	VkFramebufferCreateInfo framebufferCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.renderPass = renderPass,
		.attachmentCount = 1,
		.pAttachments = &imageView,
		.width = w,
		.height = h,
		.layers = 1
	};
	r = vkCreateFramebuffer(device, &framebufferCreateInfo, NULL, framebuffer);
	if (r) {
		fprintf(stderr, "[ERROR] framebufferCreateInfo: %d", r);
		exit(EXIT_FAILURE);
	}
}

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

struct state {
	VkInstance instance;
	VkPhysicalDevice physicalDevice; //TODO for multiple GPUs
	VkDevice device;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapchain;
	VkSwapchainCreateInfoKHR swapchain_info; //why?
	VkDescriptorSetLayout compute_descriptorSetLayout;
	VkDescriptorSetLayout graphics_descriptorSetLayout;
	VkDescriptorPool compute_descriptorPool;
	VkDescriptorPool graphics_descriptorPool;
	VkDescriptorSet graphics_descriptorSet;
	VkDescriptorSet compute_descriptorSet;
	VkPipelineLayout compute_pipelineLayout;
	VkPipelineLayout graphics_pipelineLayout;
	VkPipeline graphics_pipeline;
	VkPipeline compute_pipeline;
	VkCommandPool commandPool;
	VkCommandBuffer *commandBuffers;
	VkImageView *imageViews;
	VkImage *images;
	VkFramebuffer *framebuffers;
	VkRenderPass renderPass;
	VkSemaphore acquire_sem;
	VkSemaphore render_sem;
	VkSemaphore compute_sem;
	unsigned n_bufs;
	//VkQueue gfx_queue;
	//uint32_t gfx_fam;
	VkQueue queue;
	uint32_t present_fam;
	//VkQueue compute_queue; //compute only every frame for now. Even if it will not be needed, command buffer can be rewritten instead of running separate queue.
	//uint32_t compute_fam;
	bool run;
	bool resized;
	unsigned w;
	unsigned h;
	VkViewport viewport;
	VkRect2D scissor;
	struct swa_display* dpy;
	uint32_t vcnt;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
} state;

uint32_t vcnt=12;
VkBuffer vertexBuffer;
VkDeviceMemory vertexBufferMemory;

void vertex_buffer()
{
	VkBufferCreateInfo bufferCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.size = vcnt,
		.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_STORAGE_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
	};

	VK_CHECK_RESULT(vkCreateBuffer(state.device, &bufferCreateInfo, NULL, &vertexBuffer));

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(state.device, vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = NULL,
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = findMemoryType(state.physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	};
	VK_CHECK_RESULT(vkAllocateMemory(state.device, &memoryAllocateInfo, NULL, &vertexBufferMemory));
	vkBindBufferMemory(state.device, vertexBuffer, vertexBufferMemory, 0);
}

void compute_descriptor_sets()
{
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

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(state.device, vertexBuffer, &memRequirements);

	VkDescriptorBufferInfo descriptorBufferInfo = {
		.buffer = vertexBuffer,
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
}

void graphics_descriptor_sets()
{
/*
	VkDescriptorSetLayoutBinding graphics_descriptorSetLayoutBindings[1] = {
		{
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_FRAGMENT_BIT,
			.pImmutableSamplers = NULL
		}
	};

	VkDescriptorSetLayoutCreateInfo graphics_descriptorSetLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.bindingCount = 1,
		.pBindings = graphics_descriptorSetLayoutBindings
	};

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(state.device, &graphics_descriptorSetLayoutCreateInfo, 0, &state.graphics_descriptorSetLayout));

	VkDescriptorPoolSize g_descriptorPoolSize = {
		.type = VK_DESCRIPTOR_TYPE_,
		.descriptorCount = 1
	};

	VkDescriptorPoolCreateInfo g_descriptorPoolCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.maxSets = 1,
		.poolSizeCount = 1,
		.pPoolSizes = &g_descriptorPoolSize
	};

	VK_CHECK_RESULT(vkCreateDescriptorPool(state.device, &g_descriptorPoolCreateInfo, 0, &state.graphics_descriptorPool));

	VkDescriptorSetAllocateInfo g_descriptorSetAllocateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = NULL,
		.descriptorPool = state.graphics_descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &state.graphics_descriptorSetLayout
	};

	VK_CHECK_RESULT(vkAllocateDescriptorSets(state.device, &g_descriptorSetAllocateInfo, &state.graphics_descriptorSet));

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(state.device, vertexBuffer, &memRequirements);

	VkDescriptorBufferInfo g_descriptorBufferInfo = {
		.buffer = vertexBuffer,
		.offset = 0,
		.range = memRequirements.size
	};

	VkWriteDescriptorSet g_writeDescriptorSet[1] = {
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = state.graphics_descriptorSet,
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_VERTEX_BUFFER,
			.pImageInfo = NULL,
			.pBufferInfo = &g_descriptorBufferInfo,
			.pTexelBufferView = NULL
		}
	};

	vkUpdateDescriptorSets(state.device, 1, g_writeDescriptorSet, 0, 0);
*/
}
void create_buffers()
{
	compute_descriptor_sets();
	vertex_buffer();
	graphics_descriptor_sets();
}

void compute_pipeline()
{
  VkShaderModule compModule;
  vulkan_shader("spv/main.comp.spv", state.device, &compModule);

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .setLayoutCount = 1,
      .pSetLayouts = &state.compute_descriptorSetLayout,
      .pushConstantRangeCount = 0,
      .pPushConstantRanges = NULL};
  VK_CHECK_RESULT(vkCreatePipelineLayout(state.device,
                                         &pipelineLayoutCreateInfo, NULL,
                                         &state.compute_pipelineLayout));

  VkComputePipelineCreateInfo computePipelineCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .stage = {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .stage = VK_SHADER_STAGE_COMPUTE_BIT,
                .module = compModule,
                .pName = "main", // XXX: Hardcoded name
                .pSpecializationInfo = NULL},
      .layout = state.compute_pipelineLayout,
      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = 0};

  VK_CHECK_RESULT(vkCreateComputePipelines(state.device, 0, 1,
                                           &computePipelineCreateInfo, 0,
                                           &state.compute_pipeline));

  vkDestroyShaderModule(state.device, compModule, NULL);
}

void graphics_pipeline()
{

  VkShaderModule vertModule, fragModule;
  vulkan_shader("spv/main.vert.spv", state.device, &vertModule);
  vulkan_shader("spv/main.frag.spv", state.device, &fragModule);

  VkPipelineShaderStageCreateInfo vertStageInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = NULL,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = vertModule,
      .pName = "main"};

  VkPipelineShaderStageCreateInfo fragStageInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = NULL,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = fragModule,
      .pName = "main"};

  VkPipelineShaderStageCreateInfo graphics_stages[] = {vertStageInfo,
                                                       fragStageInfo};

  VkVertexInputBindingDescription inputBindingDesc = {
      .binding = 0,
      .stride = sizeof(double), // TODO: check this
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};

  VkVertexInputAttributeDescription inputAttributeDesc = {
      .location = 0,
      .binding = 0,
      .format = VK_FORMAT_R32G32_SFLOAT,
      .offset = 0};

  VkPipelineVertexInputStateCreateInfo vertexInputState = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .vertexBindingDescriptionCount = 1,
      .pVertexBindingDescriptions = &inputBindingDesc,
      .vertexAttributeDescriptionCount = 1,
      .pVertexAttributeDescriptions = &inputAttributeDesc};

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
      .primitiveRestartEnable = VK_FALSE};

  state.viewport.x = 0;
  state.viewport.y = 0;
  state.viewport.width = 1366;//TODO: get screen dimensions
  state.viewport.height = 768;
  state.viewport.minDepth = 0;
  state.viewport.maxDepth = 1;

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
      .pScissors = &state.scissor};

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
      .lineWidth = 1.0f};

  VkPipelineMultisampleStateCreateInfo multisampleState = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable = VK_FALSE,
      .minSampleShading = 1.0f,
      .pSampleMask = NULL,
      .alphaToCoverageEnable = VK_FALSE,
      .alphaToOneEnable = VK_FALSE};

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {
      .blendEnable = VK_FALSE,
      .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
      .colorBlendOp = VK_BLEND_OP_ADD,
      .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
      .alphaBlendOp = VK_BLEND_OP_ADD,
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};
  VkPipelineColorBlendStateCreateInfo colorBlendState = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .logicOpEnable = VK_FALSE,
      .logicOp = VK_LOGIC_OP_COPY,
      .attachmentCount = 1,
      .pAttachments = &colorBlendAttachment,
      .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}};

  VkPipelineLayoutCreateInfo layoutCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .setLayoutCount = 0,
      .pSetLayouts = NULL,
      .pushConstantRangeCount = 0,
      .pPushConstantRanges = NULL};
  VK_CHECK_RESULT(vkCreatePipelineLayout(state.device, &layoutCreateInfo, NULL,
                                         &state.graphics_pipelineLayout));

  VkAttachmentDescription colorAttachment = {
      .flags = 0,
      .format = VK_FORMAT_B8G8R8A8_UNORM, // TODO
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR};

  VkAttachmentReference colorAttachmentRef = {
      .attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

  VkSubpassDescription subpass = {.flags = 0,
                                  .pipelineBindPoint =
                                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                                  .inputAttachmentCount = 0,
                                  .pInputAttachments = NULL,
                                  .colorAttachmentCount = 1,
                                  .pColorAttachments = &colorAttachmentRef,
                                  .pResolveAttachments = 0,
                                  .pDepthStencilAttachment = NULL,
                                  .preserveAttachmentCount = 0,
                                  .pPreserveAttachments = NULL};

  VkSubpassDependency subpassDependency = {
      .srcSubpass = VK_SUBPASS_EXTERNAL,
      .dstSubpass = 0,
      .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      .srcAccessMask = 0,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                       VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .dependencyFlags = 0};

  VkRenderPassCreateInfo renderPassCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .attachmentCount = 1,
      .pAttachments = &colorAttachment,
      .subpassCount = 1,
      .pSubpasses = &subpass,
      .dependencyCount = 1,
      .pDependencies = &subpassDependency};

  VK_CHECK_RESULT(vkCreateRenderPass(state.device, &renderPassCreateInfo, NULL,
                                     &state.renderPass));

  VkGraphicsPipelineCreateInfo graphics_pipelineCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .stageCount =
          sizeof(graphics_stages) / sizeof(VkPipelineShaderStageCreateInfo),
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
      .basePipelineIndex = -1};

  VK_CHECK_RESULT(vkCreateGraphicsPipelines(state.device, VK_NULL_HANDLE, 1,
                                            &graphics_pipelineCreateInfo, NULL,
                                            &state.graphics_pipeline));

  vkDestroyShaderModule(state.device, vertModule, NULL);
  vkDestroyShaderModule(state.device, fragModule, NULL);
}

//window resize function is required or it will crash
static void window_resize(struct swa_window* win, unsigned w, unsigned h)
{
	if(!state.swapchain) {
		//TODO
		/*
		if(!init_swapchain(state, w, h)) {
			dlg_error("Failed to init swapchain");
			return;
		}
		*/
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
	state.viewport.width = 640;
	state.viewport.height = 480;
	state.viewport.minDepth = 0;
	state.viewport.maxDepth = 1;
	

	state.scissor.offset.x = 0;
	state.scissor.offset.y = 0;
	state.scissor.extent.width = 640;
	state.scissor.extent.height = 480;

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

	vkGetSwapchainImagesKHR(state.device, state.swapchain, &state.n_bufs, NULL);
	state.images = malloc(state.n_bufs*sizeof(VkImage)); //TODO: free
	vkGetSwapchainImagesKHR(state.device, state.swapchain, &state.n_bufs, state.images);

	state.imageViews = malloc(state.n_bufs*sizeof(VkImageView)); //TODO: free
	state.framebuffers = malloc(state.n_bufs*sizeof(VkFramebuffer)); //TODO: free

	for (int i=0; i<state.n_bufs; i++) {
		createImageView(state.device, state.images[i], state.imageViews+i);
		createFramebuffer(state.device, state.renderPass, state.imageViews[i], state.framebuffers+i, state.w, state.h);
	}
		//state.n_bufs=framebufferCount;

		//create_buffer(device, physicalDevice, 2*M*sizeof(float), &vertexBuffer, &vertexBufferMemory);

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

			write(1,"resizing buf\n",13);
			vkCmdBindPipeline(state.commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, state.compute_pipeline);
			vkCmdBindDescriptorSets(state.commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, state.compute_pipelineLayout, 0, 1, &state.compute_descriptorSet, 0, 0);
			vkCmdDispatch(state.commandBuffers[i], vcnt, 1, 1);

			vkCmdBindDescriptorSets(state.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, state.graphics_pipelineLayout, 0, 1, &state.graphics_descriptorSet, 0, 0);
			vkCmdBeginRenderPass(state.commandBuffers[i], &renderPassBeginInfo,
								VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(state.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
							state.graphics_pipeline);
			VkDeviceSize offset = 0;
			vkCmdBindVertexBuffers(state.commandBuffers[i], 0, 1, &vertexBuffer, &offset);
			vkCmdDraw(state.commandBuffers[i], vcnt, 1, 0, 0);

			vkCmdEndRenderPass(state.commandBuffers[i]);
			VK_CHECK_RESULT(vkEndCommandBuffer(state.commandBuffers[i]));
		}

	}

	state.resized = true;
	state.w = w;
	state.h = h;
}

void create_swapchain() {
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(state.physicalDevice, state.surface,
                                            &surfaceCapabilities);

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
      .oldSwapchain = state.swapchain};
  VK_CHECK_RESULT(vkCreateSwapchainKHR(state.device, &swapchainCreateInfo, NULL,
                                       &state.swapchain))
  vkDestroySwapchainKHR(state.device, state.swapchain_info.oldSwapchain, NULL);
  state.swapchain_info.oldSwapchain = VK_NULL_HANDLE;
}

static void window_close(struct swa_window* win)
{
	//struct state* state = swa_window_get_userdata(win);
	//TODO
	state.run = false;
}

static void window_key(struct swa_window* win, const struct swa_key_event* ev)
{
	//struct state* state = swa_window_get_userdata(win);

	if(ev->pressed && ev->keycode == swa_key_escape) {
		dlg_info("Escape pressed, exiting");
		state.run = false;
	}
}

static void window_focus(struct swa_window* win, bool gained)
{
	dlg_info("focus %s", gained ? "gained" : "lost");
}

static void mouse_move(struct swa_window* win,
					   const struct swa_mouse_move_event* ev)
{
	dlg_info("mouse moved to (%d, %d)", ev->x, ev->y);
}

static void mouse_cross(struct swa_window* win,
						const struct swa_mouse_cross_event* ev)
{
	dlg_info("mouse %s at (%d, %d)", ev->entered ? "entered" : "left",
			 ev->x, ev->y);
}

static void mouse_button(struct swa_window* win,
						 const struct swa_mouse_button_event* ev)
{
	dlg_info("mouse button: button = %d %s, pos = (%d, %d)",
			 ev->button, ev->pressed ? "pressed" : "released",
			 ev->x, ev->y);
}

static void mouse_wheel(struct swa_window* win,
						float dx, float dy)
{
	dlg_info("mouse wheel: (%f, %f)", dx, dy);
}

static void touch_begin(struct swa_window* win,
						const struct swa_touch_event* ev)
{
	dlg_info("touch begin: id = %d, pos = (%d, %d)",
			 ev->id, ev->x, ev->y);

	//showing_keyboard = !showing_keyboard;
	//show_keyboard((struct swa_display*) swa_window_get_userdata(win),
	//	showing_keyboard);
}

static void touch_update(struct swa_window* win,
						 const struct swa_touch_event* ev)
{
	dlg_info("touch update: id = %d, pos = (%d, %d)",
			 ev->id, ev->x, ev->y);
}

static void touch_end(struct swa_window* win, unsigned id)
{
	dlg_info("touch end: id = %d", id);
}

static void touch_cancel(struct swa_window* win)
{
	dlg_info("touch cancel");
}

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

int main(int argc, char **argv, char **envp) {
#ifdef USE_VOLK
  volkInitialize();
#endif
  struct swa_display *dpy = swa_display_autocreate("compute triangle");

  if (!dpy) {
    dlg_fatal("No swa backend available");
    return EXIT_FAILURE;
  }

  // Make sure the display has support for vulkan surfaces
  if (!(swa_display_capabilities(dpy) & swa_display_cap_vk)) {
    dlg_fatal("Display doesn't support vulkan");
    //		ret = EXIT_FAILURE;
    goto cleanup_dpy;
  }

  // Get the vulkan instance extensions required by the display
  // to create vulkan surfaces.

  uint32_t vkExtensionCount = 1;
  const char **instance_extensions =
      swa_display_vk_extensions(dpy, &vkExtensionCount);

  for (int i = 0; i < vkExtensionCount; ++i) {
    printf("%s\n", instance_extensions[i]);
  }

  VkApplicationInfo applicationInfo = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = "hello vulkan compute triangle",
      .apiVersion = VK_MAKE_VERSION(1, 2, 0)};

  VkInstanceCreateInfo instanceCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = NULL,
      .enabledLayerCount = 0,
      .ppEnabledLayerNames = NULL,
      .enabledExtensionCount = vkExtensionCount + 1,
      .ppEnabledExtensionNames = instance_extensions,
      .pApplicationInfo = &applicationInfo};

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
      .pQueuePriorities = &priority};

  const char *device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  VkDeviceCreateInfo deviceCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &deviceQueueCreateInfo,
      .enabledExtensionCount = sizeof(device_extensions) / sizeof(char *),
      .ppEnabledExtensionNames = device_extensions};
  VK_CHECK_RESULT(vkCreateDevice(state.physicalDevice, &deviceCreateInfo, NULL,
                                 &state.device));

#ifdef USE_VOLK
  volkLoadDevice(state.device);
#endif

  // Create the vulkan instance. Make sure to enable the extensions
  // the display requires. We can't create the device here since
  // the queues we need depend on the created vulkan surface which
  // in turn depens on the vulkan instance.
  state.w = 640;
  state.h = 480;

  struct swa_window_settings settings;
  swa_window_settings_default(&settings);
  settings.title = "pheh";
  settings.surface = swa_surface_vk;
  settings.surface_settings.vk.instance = (uint64_t)state.instance;
  settings.listener = &window_listener;
  settings.width = state.w;
  settings.height = state.h;
  struct swa_window *win = swa_display_create_window(dpy, &settings);

  if (!win) {
    dlg_fatal("Failed to create window");
    // ret = EXIT_FAILURE;
    goto cleanup_state;
  }

  state.surface = (VkSurfaceKHR)swa_window_get_vk_surface(win);

  if (!state.surface) {
    // ret = EXIT_FAILURE;
    dlg_error("Couldn't get vk surface from swa window");
    goto cleanup_win;
  }

  swa_window_set_userdata(win, &state);
  // timespec_get(&last_redraw, TIME_UTC);

  VkBool32 supported;
  vkGetPhysicalDeviceSurfaceSupportKHR(state.physicalDevice, 0, state.surface,
                                       &supported);

  if (supported != VK_TRUE)
    return 1;

  create_swapchain();

  create_buffers();

  compute_pipeline();

  graphics_pipeline();

  // VkImageView *imageViews; //move this 3 vars to struct state
  // VkImage *images;
  // VkFramebuffer *framebuffers;

  vkGetSwapchainImagesKHR(state.device, state.swapchain, &state.n_bufs, NULL);
  state.images = malloc(state.n_bufs * sizeof(VkImage)); // TODO: free
  vkGetSwapchainImagesKHR(state.device, state.swapchain, &state.n_bufs,
                          state.images);

  state.imageViews = malloc(state.n_bufs * sizeof(VkImageView)); // TODO: free
  state.framebuffers =
      malloc(state.n_bufs * sizeof(VkFramebuffer)); // TODO: free

  for (int i = 0; i < state.n_bufs; i++) {
    createImageView(state.device, state.images[i], state.imageViews + i);
    createFramebuffer(state.device, state.renderPass, state.imageViews[i],
                      state.framebuffers + i, state.w, state.h);
  }

  //*framebufferCount_ = state.n_bufs;
  //*framebuffers_ = framebuffers;

  VkCommandPoolCreateInfo commandPoolCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .queueFamilyIndex = 0};
  VK_CHECK_RESULT(vkCreateCommandPool(state.device, &commandPoolCreateInfo,
                                      NULL, &state.commandPool))

  state.commandBuffers =
      malloc(state.n_bufs * sizeof(VkCommandBuffer)); // TODO: free
  VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = NULL,
      .commandPool = state.commandPool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = state.n_bufs};
  VK_CHECK_RESULT(vkAllocateCommandBuffers(
      state.device, &commandBufferAllocateInfo, state.commandBuffers))

  for (int i = 0; i < state.n_bufs; i++) {
    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = 0,
        .pInheritanceInfo = NULL};

    VK_CHECK_RESULT(
        vkBeginCommandBuffer(state.commandBuffers[i], &commandBufferBeginInfo));

    VkClearValue clearColor = {0.3f, 0.0f, 0.0f, 1.0f};
    VkRenderPassBeginInfo renderPassBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = NULL,
        .renderPass = state.renderPass,
        .framebuffer = state.framebuffers[i],
        .renderArea = {0, 0, 640, 480},
        .clearValueCount = 1,
        .pClearValues = &clearColor};

    write(1, "creating buf\n", 13);
    vkCmdBindPipeline(state.commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE,
                      state.compute_pipeline);
    vkCmdBindDescriptorSets(
        state.commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE,
        state.compute_pipelineLayout, 0, 1, &state.compute_descriptorSet, 0, 0);
    vkCmdDispatch(state.commandBuffers[i], vcnt, 1, 1);

    vkCmdBindDescriptorSets(state.commandBuffers[i],
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            state.graphics_pipelineLayout, 0, 1,
                            &state.graphics_descriptorSet, 0, 0);
    vkCmdBeginRenderPass(state.commandBuffers[i], &renderPassBeginInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(state.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                      state.graphics_pipeline);
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(state.commandBuffers[i], 0, 1, &vertexBuffer,
                           &offset);
    vkCmdDraw(state.commandBuffers[i], vcnt, 1, 0, 0);

    vkCmdEndRenderPass(state.commandBuffers[i]);
    VK_CHECK_RESULT(vkEndCommandBuffer(state.commandBuffers[i]));
  }

  // main loop
  state.run = true;
  state.dpy = dpy;

  /*
          VkSemaphoreCreateInfo semaphoreCreateInfo = {
                  .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                  .pNext = NULL,
                  .flags = 0
          };
  */

  VkSemaphoreCreateInfo sem_info = {0};
  sem_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  VK_CHECK_RESULT(
      vkCreateSemaphore(state.device, &sem_info, NULL, &state.acquire_sem))
  VK_CHECK_RESULT(
      vkCreateSemaphore(state.device, &sem_info, NULL, &state.render_sem))

  vkGetDeviceQueue(state.device, 0, 0, &state.queue);

  while (state.run) {
    if (!swa_display_dispatch(dpy, false)) {
      break;
    }

    if (state.resized) {
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

    // vkDeviceWaitIdle(state.device); // why so slow?

    timespec_get(&now, TIME_UTC);

    ms = (now.tv_nsec - last_redraw.tv_nsec) / (1000.f * 1000.f);

    ms += 1000.f * (now.tv_sec - last_redraw.tv_sec);

    dlg_info("vkDeviceWaitIdle time: %f", ms);

    timespec_get(&now, TIME_UTC);

    last_redraw = now;

    // acquire image
    // we treat suboptimal as success here
    uint32_t id;

    VK_CHECK_RESULT(vkAcquireNextImageKHR(state.device, state.swapchain,
                                          UINT64_MAX, state.acquire_sem,
                                          VK_NULL_HANDLE, &id));

    timespec_get(&now, TIME_UTC);
    ms = (now.tv_nsec - last_redraw.tv_nsec) / (1000.f * 1000.f);
    ms += 1000.f * (now.tv_sec - last_redraw.tv_sec);
    dlg_info("acquire image time: %f", ms);
    timespec_get(&now, TIME_UTC);
    last_redraw = now;

    // submit render commands
    VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo si = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                       .pCommandBuffers = &state.commandBuffers[id],
                       .commandBufferCount = 1u,
                       .waitSemaphoreCount = 1u,
                       .pWaitSemaphores = &state.acquire_sem,
                       .pWaitDstStageMask = &stage,
                       .signalSemaphoreCount = 1u,
                       .pSignalSemaphores = &state.render_sem};

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
