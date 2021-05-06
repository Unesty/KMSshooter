#pragma once
#ifdef USE_VOLK
#include <volk.h>
#include <volk.c>
#else
#include <vulkan/vulkan.h>
#endif
#include <stdbool.h>

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
