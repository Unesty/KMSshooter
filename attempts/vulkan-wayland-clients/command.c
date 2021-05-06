void vulkan_record_graphics_command(VkPipeline graphics_pipeline, VkPipeline compute_pipeline, VkRenderPass renderPass,
VkFramebuffer framebuffer, VkBuffer vertexBuffer, uint32_t vertexCount,
VkCommandBuffer commandBuffer, VkPipelineLayout *compute_pipelineLayout, VkDescriptorSet *compute_descriptorSet) {
	VkCommandBufferBeginInfo commandBufferBeginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = 0,
		.pInheritanceInfo = NULL
	};

	VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

	VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
	VkRenderPassBeginInfo renderPassBeginInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = NULL,
		.renderPass = renderPass,
		.framebuffer = framebuffer,
		.renderArea = {0, 0, 400, 400},
		.clearValueCount = 1,
		.pClearValues = &clearColor
	};

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *compute_pipelineLayout, 0, 1, compute_descriptorSet, 0, 0);
	vkCmdDispatch(commandBuffer, vertexCount, 1, 1);

	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo,
	VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
	graphics_pipeline);
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);
	vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);
	VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));
}

void vulkan_compute_commands(VkDevice device, VkPipeline compute_pipeline,
VkBuffer vertexBuffer, uint32_t vertexCount, VkCommandBuffer **commandBuffers_,
VkCommandPool *commandPool, VkPipelineLayout *layout) {
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding =
	{
	.binding = 0,
	.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
	.descriptorCount = 1,
	.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
	.pImmutableSamplers = NULL
	};
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
	.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
	.pNext = NULL,
	.flags = 0,
	.bindingCount = 1,
	.pBindings = &descriptorSetLayoutBinding
	};

	VkDescriptorSetLayout descriptorSetLayout;

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, 0, &descriptorSetLayout));

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

    VkDescriptorPool descriptorPool;
	VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolCreateInfo,
        0, &descriptorPool));
	
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayout
    };

    VkDescriptorSet descriptorSet;
    VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet));
	
	VkDescriptorBufferInfo descriptorBufferInfo = {
		.buffer = vertexBuffer,
		.offset = 0,
		.range = VK_WHOLE_SIZE
	};

	VkWriteDescriptorSet writeDescriptorSet = {
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.pNext = NULL,
		.dstSet = descriptorSet,
		.dstBinding = 0,
		.dstArrayElement = 0,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		.pImageInfo = NULL,
		.pBufferInfo = &descriptorBufferInfo,
		.pTexelBufferView = NULL
	};

	vkUpdateDescriptorSets(device, 2, &writeDescriptorSet, 0, 0);
	
	VkCommandBuffer commandBuffer;
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = *commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};
	VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer));
	
	VkCommandBufferBeginInfo commandBufferBeginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = 0,
		.pInheritanceInfo = NULL
	};

	VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));
	
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, *layout, 0, 1, &descriptorSet, 0, 0);
	//vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);
	vkCmdDispatch(commandBuffer, vertexCount, 1, 1);
	
	VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));
	
	*commandBuffers_ = &commandBuffer;
}

void vulkan_graphics_commands(VkDevice device, VkPipeline graphics_pipeline,  VkPipeline compute_pipeline, VkRenderPass
renderPass, uint32_t framebufferCount, VkFramebuffer *framebuffers, VkBuffer
vertexBuffer, uint32_t vertexCount, VkCommandBuffer **commandBuffers_,
VkCommandPool *commandPool, VkPipelineLayout *compute_pipelineLayout, VkDescriptorSet *compute_descriptorSet) {
	int r;

	VkCommandPoolCreateInfo commandPoolCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.queueFamilyIndex = 0
	};
	r = vkCreateCommandPool(device, &commandPoolCreateInfo, NULL,
	commandPool);
	if (r) {
		fprintf(stderr, "[ERROR] vkCreateCommandPool: %d", r);
		exit(EXIT_FAILURE);
	}

	VkCommandBuffer *commandBuffers = malloc(framebufferCount*sizeof(VkCommandBuffer)); //TODO: free
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = *commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = framebufferCount
	};
	r = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo,
	commandBuffers);
	if (r) {
		fprintf(stderr, "[ERROR] vkAllocateCommandBuffers: %d", r);
		exit(EXIT_FAILURE);
	}

	for (int i=0; i<framebufferCount; i++)
		vulkan_record_graphics_command(graphics_pipeline, compute_pipeline, renderPass, framebuffers[i],
		vertexBuffer, vertexCount, commandBuffers[i], compute_pipelineLayout, compute_descriptorSet);

	*commandBuffers_ = commandBuffers;
}
