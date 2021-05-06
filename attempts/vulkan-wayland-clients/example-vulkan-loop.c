#include <swa/swa.h>
#include <swa/key.h>
#include <dlg/dlg.h>
#include <string.h>
#include <vulkan/vulkan.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// Macro to check and display Vulkan return results
#define VK_CHECK_RESULT(f)																				\
{																										\
	VkResult res = (f);																					\
	if (res != VK_SUCCESS)																				\
	{																									\
		printf("Fatal : VkResult is \" %d \" in %s at line %d", res, __FILE__, __LINE__); \
		if(res == VK_SUCCESS) write(1, "\nLOLWAT\n", 8);	\
		else exit(0); \
	}																									\
}

// TODO: handle surface lost error

struct render_buffer {
	VkCommandBuffer cb;
	VkImageView iv;
	VkFramebuffer fb;
};

struct state {
	VkInstance instance;
	VkDebugUtilsMessengerEXT messenger;
	VkPhysicalDevice phdev;
	VkDevice device;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapchain;
	VkSwapchainCreateInfoKHR swapchain_info;

	VkCommandPool cmd_pool;
	VkRenderPass rp;
	VkSemaphore acquire_sem;
	VkSemaphore render_sem;

	unsigned n_bufs;
	struct render_buffer* bufs;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet descriptorSet;
	VkDescriptorPool descriptorPool;
	VkPipelineLayout pipelineLayout;
	VkPipeline computePipeline;
	VkPipeline graphicsPipeline;
	VkBuffer ubo;
	VkDeviceMemory ubo_mem;

	struct {
		VkQueue present;
		VkQueue gfx;
		uint32_t present_fam;
		uint32_t gfx_fam;
	} qs;

	struct {
		PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsMessengerEXT;
		PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsMessengerEXT;
	} api;

	bool run;
	bool resized;
	unsigned w;
	unsigned h;
	struct swa_display* dpy;
};

// fwd decls of vulkan bits
static bool init_instance(struct state* state, unsigned n_dpy_exts,
						  const char** dpy_exts);
static bool init_renderer(struct state* state);
static bool init_render_buffers(struct state* state);
static void destroy_render_buffers(struct state* state);
static bool init_swapchain(struct state* state, unsigned width, unsigned height);
static const char *vulkan_strerror(VkResult err);
static void cleanup_renderer(struct state* state);
static void cleanup(struct state* state);

#define vk_error(res, fmt) dlg_error(fmt ": %s (%d)", vulkan_strerror(res), res)

void resize(struct state* state)
{
	// make sure all previous rendering has finished since we will
	// destroy rendering resources

	vkDeviceWaitIdle(state->device);
	destroy_render_buffers(state);
	VkResult res;

	// recreate swapchain
	VkSurfaceCapabilitiesKHR caps;
	res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(state->phdev,
			state->surface, &caps);

	if(res != VK_SUCCESS) {
		vk_error(res, "failed retrieve surface caps");
		state->run = false;
		return;
	}

	if(caps.currentExtent.width == 0xFFFFFFFFu) {
		state->swapchain_info.imageExtent.width = state->w;
		state->swapchain_info.imageExtent.height = state->h;

	} else {
		dlg_info("  fixed swapchain size: %d %d",
				 caps.currentExtent.width,
				 caps.currentExtent.height);
		state->swapchain_info.imageExtent.width = caps.currentExtent.width;
		state->swapchain_info.imageExtent.height = caps.currentExtent.height;
	}

	state->swapchain_info.oldSwapchain = state->swapchain;
	res = vkCreateSwapchainKHR(state->device, &state->swapchain_info,
							   NULL, &state->swapchain);

	vkDestroySwapchainKHR(state->device,
						  state->swapchain_info.oldSwapchain, NULL);
	state->swapchain_info.oldSwapchain = VK_NULL_HANDLE;

	if (res != VK_SUCCESS) {
		vk_error(res, "Failed to create vk swapchain");
		state->run = false;
		return;
	}

	// recreate render buffers
	if(!init_render_buffers(state)) {
		state->run = false;
		return;
	}

	state->resized = false;
}

static void window_resize(struct swa_window* win, unsigned w, unsigned h)
{
	struct state* state = swa_window_get_userdata(win);
	dlg_info("resized to %d %d", w, h);

	if(!state->swapchain) {
		if(!init_swapchain(state, w, h)) {
			dlg_error("Failed to init swapchain");
			return;
		}
	}

	state->resized = true;
	state->w = w;
	state->h = h;
}

static bool window_draw(struct swa_window* win)
{
	struct state* state = swa_window_get_userdata(win);
	VkResult res;

	if(!state->swapchain) {
		dlg_warn("No swapchain!");
		return false;
	}

	/*
	 struct timespec now;
	 struct timespec last_redraw;
	 timespec_get(&now, TIME_UTC);
	 float ms = (now.tv_nsec - last_redraw.tv_nsec) / (1000.f * 1000.f);
	 ms += 1000.f * (now.tv_sec - last_redraw.tv_sec);
	 dlg_info("Time between redraws: %f", ms);
	 last_redraw = now;
	*/

	vkDeviceWaitIdle(state->device);

	// acquire image
	// we treat suboptimal as success here
	uint32_t id;

	while(true) {
		res = vkAcquireNextImageKHR(state->device, state->swapchain,
									UINT64_MAX, state->acquire_sem, VK_NULL_HANDLE, &id);

		if(res == VK_SUCCESS || res == VK_SUBOPTIMAL_KHR) {
			break;

		} else if(res == VK_ERROR_OUT_OF_DATE_KHR) {
			dlg_warn("Got out of date swapchain (acquire)");
			return false;

		} else {
			vk_error(res, "vkAcquireNextImageKHR");
			state->run = false;
			return false;
		}
	}

	// submit render commands
	VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo si = {0};
	si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	si.pCommandBuffers = &state->bufs[id].cb;
	si.commandBufferCount = 1u;
	si.waitSemaphoreCount = 1u;
	si.pWaitSemaphores = &state->acquire_sem;
	si.pWaitDstStageMask = &stage;
	si.signalSemaphoreCount = 1u;
	si.pSignalSemaphores = &state->render_sem;

	res = vkQueueSubmit(state->qs.gfx, 1, &si, VK_NULL_HANDLE);

	if(res != VK_SUCCESS) {
		vk_error(res, "vkQueueSubmit");
		state->run = false;
		return false;
	}

	// present
	VkPresentInfoKHR present_info = {0};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &state->swapchain;
	present_info.pImageIndices = &id;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &state->render_sem;

	res = vkQueuePresentKHR(state->qs.present, &present_info);

	if(res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
		if(res == VK_ERROR_OUT_OF_DATE_KHR) {
			dlg_warn("Got out of date swapchain (present)");
			return true;
		}

		vk_error(res, "vkQueuePresentKHR");
		state->run = false;
		return false;
	}

	return true;
}

static void window_close(struct swa_window* win)
{
	struct state* state = swa_window_get_userdata(win);
	state->run = false;
}

static void window_key(struct swa_window* win, const struct swa_key_event* ev)
{
	struct state* state = swa_window_get_userdata(win);

	if(ev->pressed && ev->keycode == swa_key_escape) {
		dlg_info("Escape pressed, exiting");
		state->run = false;
	}
}

static const struct swa_window_listener window_listener = {
	.close = window_close,
	.resize = window_resize,
	.key = window_key,
};

// Initialization of window and vulkan in general needs the following
// steps, in that order:
// - Create swa display
// - Create vulkan instance (with extensions from instance)
// - Create swa surface with swa_surface_vulkan,
//   You have to set the vulkan instance in swa_window_settings
// - Create vulkan device with at least one queue that can present
//   to the created vulkan surface
// - Create swpachain and other rendering resources
// The last step presents us with a problem: on some platforms we have
// to choose the swapchain size. How to do that, we don't know the
// initial window size yet if we used SWA_DEFAULT_SIZE.
// So an even better approach (that needs a bit more application logic)
// would be to postpone swapchain creation until we get the first
// size event. Note that this size event is only guaranteed if we
// used SWA_DEFAULT_SIZE.
int main()
{
	int ret = EXIT_SUCCESS;
	struct swa_display* dpy = swa_display_autocreate("swa example-vulkan");

	if(!dpy) {
		dlg_fatal("No swa backend available");
		return EXIT_FAILURE;
	}

	// Make sure the display has support for vulkan surfaces
	if(!(swa_display_capabilities(dpy) & swa_display_cap_vk)) {
		dlg_fatal("Display doesn't support vulkan");
		ret = EXIT_FAILURE;
		goto cleanup_dpy;
	}

	// Get the vulkan instance extensions required by the display
	// to create vulkan surfaces.
	unsigned n_exts;
	const char** exts = swa_display_vk_extensions(dpy, &n_exts);

	// Create the vulkan instance. Make sure to enable the extensions
	// the display requires. We can't create the device here since
	// the queues we need depend on the created vulkan surface which
	// in turn depens on the vulkan instance.
	struct state state = {0};

	if(!init_instance(&state, n_exts, exts)) {
		ret = EXIT_FAILURE;
		goto cleanup_state;
	}

	// create window for vulkan surface
	struct swa_window_settings settings;
	swa_window_settings_default(&settings);
	settings.title = "swa-example-window";
	settings.surface = swa_surface_vk;
	settings.surface_settings.vk.instance = (uint64_t) state.instance;
	settings.listener = &window_listener;
	struct swa_window* win = swa_display_create_window(dpy, &settings);

	if(!win) {
		dlg_fatal("Failed to create window");
		ret = EXIT_FAILURE;
		goto cleanup_state;
	}

	state.surface = (VkSurfaceKHR) swa_window_get_vk_surface(win);

	if(!state.surface) {
		ret = EXIT_FAILURE;
		dlg_error("Couldn't get vk surface from swa window");
		goto cleanup_win;
	}

	if(!init_renderer(&state)) {
		ret = EXIT_FAILURE;
		goto cleanup_win;
	}

	swa_window_set_userdata(win, &state);
	// timespec_get(&last_redraw, TIME_UTC);

	// main loop
	state.run = true;
	state.dpy = dpy;

	while(state.run) {
		if(!swa_display_dispatch(dpy, false)) {
			break;
		}

		if(state.resized) {
			resize(&state);
		}

		window_draw(win);
	}

cleanup_win:
	cleanup_renderer(&state);
	swa_window_destroy(win);
cleanup_state:
	cleanup(&state);
cleanup_dpy:
	swa_display_destroy(dpy);
	dlg_trace("Exiting cleanly");
	return ret;
}

// All the vulkan bits
static bool has_extension(const VkExtensionProperties *avail,
						  uint32_t availc, const char *req)
{
	// check if all required extensions are supported
	for (size_t j = 0; j < availc; ++j) {
		if (!strcmp(avail[j].extensionName, req)) {
			return true;
		}
	}

	return false;
}

static VkBool32 debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
							   VkDebugUtilsMessageTypeFlagsEXT type,
							   const VkDebugUtilsMessengerCallbackDataEXT *debug_data,
							   void *data)
{

	((void) data);
	((void) type);

	// we ignore some of the non-helpful warnings
	static const char *const ignored[] = {
		// Error when we create a swapchain with "invalid" imageExtent.
		// On X11 there is a data race and this therefore triggers
		// randomly. Not the fault of swa or this application but
		// rather a bug in the validation layers/x11 or badly
		// specified in vulkan spec.
		// See:
		// - https://github.com/KhronosGroup/Vulkan-ValidationLayers/issues/624
		// - https://github.com/KhronosGroup/Vulkan-ValidationLayers/pull/1015
		//   that pr introduced the behavior in the validation layers
		// "VUID-VkSwapchainCreateInfoKHR-imageExtent-01274"
		NULL,
	};

	if(debug_data->pMessageIdName) {
		for(unsigned i = 0; i < sizeof(ignored) / sizeof(ignored[0]); ++i) {
			if(ignored[i] && !strcmp(debug_data->pMessageIdName, ignored[i])) {
				return false;
			}
		}
	}

	switch(severity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		dlg_error("%s (%s, %d)", debug_data->pMessage,
				  debug_data->pMessageIdName, debug_data->messageIdNumber);
		break;

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		dlg_warn("%s (%s, %d)", debug_data->pMessage,
				 debug_data->pMessageIdName, debug_data->messageIdNumber);
		break;

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		dlg_info("%s (%s, %d)", debug_data->pMessage,
				 debug_data->pMessageIdName, debug_data->messageIdNumber);
		break;

	default:
		break;
	}

	if (debug_data->queueLabelCount > 0) {
		const char *name = debug_data->pQueueLabels[0].pLabelName;

		if (name) {
			dlg_debug("    last queue label '%s'", name);
		}
	}

	if (debug_data->cmdBufLabelCount > 0) {
		const char *name = debug_data->pCmdBufLabels[0].pLabelName;

		if (name) {
			dlg_debug("    last cmdbuf label '%s'", name);
		}
	}

	for (unsigned i = 0; i < debug_data->objectCount; ++i) {
		if (debug_data->pObjects[i].pObjectName) {
			dlg_debug("    involving '%s'", debug_data->pMessage);
		}
	}

	// Returning true not allowed by spec but helpful for debugging
	// makes function that caused the error return validation_failed
	// error which we can detect
	// return true;

	return false;
}

static void destroy_render_buffers(struct state* state)
{
	if(!state->n_bufs) {
		return;
	}

	VkCommandBuffer* cbs = calloc(state->n_bufs, sizeof(*cbs));

	for(uint32_t i = 0; i < state->n_bufs; i++) {
		struct render_buffer* buf = &state->bufs[i];
		cbs[i] = buf->cb;

		if(buf->fb) {
			vkDestroyFramebuffer(state->device, buf->fb, NULL);
		}

		if(buf->iv) {
			vkDestroyImageView(state->device, buf->iv, NULL);
		}
	}
	
	if (state->ubo) {
		vkDestroyBuffer(state->device, state->ubo, NULL);
	}
	if (state->ubo_mem) {
		vkFreeMemory(state->device, state->ubo_mem, NULL);
	}

	vkFreeCommandBuffers(state->device, state->cmd_pool, state->n_bufs, cbs);
	free(cbs);

	state->n_bufs = 0;
	free(state->bufs);
}

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
	VkResult r = vkCreateShaderModule(device, &shaderModuleCreateInfo, NULL,
		&shaderModule);
	if (r) {
		fprintf(stderr, "[ERROR] vkCreateShaderModule: %d", r);
		exit(EXIT_FAILURE);
	}

	*shaderModule_ = shaderModule;
}

int find_mem_type(VkPhysicalDevice phdev,
	VkMemoryPropertyFlags flags, uint32_t req_bits)
{

	VkPhysicalDeviceMemoryProperties props;
	vkGetPhysicalDeviceMemoryProperties(phdev, &props);

	for (unsigned i = 0u; i < props.memoryTypeCount; ++i) {
		if (req_bits & (1 << i)) {
			if ((props.memoryTypes[i].propertyFlags & flags) == flags) {
				return i;
			}
		}
	}

	return -1;
}


static bool init_render_buffers(struct state* state)
{
	VkResult res;
	VkDevice dev = state->device;

	res = vkGetSwapchainImagesKHR(dev, state->swapchain, &state->n_bufs, NULL);

	if (res != VK_SUCCESS) {
		vk_error(res, "Failed to get swapchain images (1)");
		return false;
	}

	VkImage* images = calloc(state->n_bufs, sizeof(*images));
	res = vkGetSwapchainImagesKHR(dev, state->swapchain,
								  &state->n_bufs, images);

	if (res != VK_SUCCESS) {
		vk_error(res, "Failed to get swapchain images (2)");
		return false;
	}



	state->bufs = calloc(state->n_bufs, sizeof(*state->bufs));

	for(uint32_t i = 0; i < state->n_bufs; i++) {
		struct render_buffer* buf = &state->bufs[i];

		// image view
		VkImageViewCreateInfo view_info = {0};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.format = state->swapchain_info.imageFormat;
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
		view_info.image = images[i];

		res = vkCreateImageView(dev, &view_info, NULL, &state->bufs[/*i*/ 0].iv);

		if (res != VK_SUCCESS) {
			vk_error(res, "vkCreateImageView");
			goto end_images;
		}

		// framebuffer
		VkFramebufferCreateInfo fb_info = {0};
		fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fb_info.attachmentCount = 1;
		fb_info.pAttachments = &buf->iv;
		fb_info.renderPass = state->rp;
		fb_info.width = state->swapchain_info.imageExtent.width;
		fb_info.height = state->swapchain_info.imageExtent.height;
		fb_info.layers = 1;

		res = vkCreateFramebuffer(dev, &fb_info, NULL, &buf->fb);

		if (res != VK_SUCCESS) {
			vk_error(res, "vkCreateFramebuffer");
			goto end_images;
		}
	}

	free(images);
	
	const float ubo_size = 4;
	VkBufferCreateInfo bufferCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.size = ubo_size,
		.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
	};
	
	VK_CHECK_RESULT(vkCreateBuffer(dev, &bufferCreateInfo, NULL, &state->ubo));
	
	VkMemoryRequirements buffer_memoryRequirements;
	vkGetBufferMemoryRequirements(dev, state->ubo, &buffer_memoryRequirements);	

	// the vulkan spec guarantees that non-sparse buffers can
	// always be allocated on host visible, coherent memory, i.e.
	// we must find a valid memory type.
	int mem_type = find_mem_type(state->phdev,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer_memoryRequirements.memoryTypeBits);
	if(mem_type >= 0) goto end_images;
	VkMemoryAllocateInfo memoryAllocateInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = buffer_memoryRequirements.size,
		.memoryTypeIndex = mem_type
	};
	
	VK_CHECK_RESULT(vkAllocateMemory(dev, &memoryAllocateInfo, NULL, &state->ubo_mem));
	VK_CHECK_RESULT(vkBindBufferMemory(dev, state->ubo, state->ubo_mem, 0));
	void* ubo_map;
	VK_CHECK_RESULT(vkMapMemory(dev, state->ubo_mem, 0, ubo_size, 0, &ubo_map));
	
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = state->descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &state->descriptorSetLayout
	};
	
	VK_CHECK_RESULT(vkAllocateDescriptorSets(dev, &descriptorSetAllocateInfo, &state->descriptorSet));
	
	VkDescriptorBufferInfo descriptorBufferInfo = {
		.buffer = state->ubo,
		.offset = 0,
		.range = ubo_size
	};
	
	VkWriteDescriptorSet writeDescriptorSet = {
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.pBufferInfo = &descriptorBufferInfo,
		.descriptorCount = 1,
		.dstSet = state->descriptorSet
	};
	vkUpdateDescriptorSets(dev, 1, &writeDescriptorSet, 0, NULL);
	
	// allocate command buffers
	VkCommandBuffer* cbs = calloc(state->n_bufs, sizeof(*cbs));

	VkCommandBufferAllocateInfo cmd_buf_info = {0};
	cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmd_buf_info.commandPool = state->cmd_pool;
	cmd_buf_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmd_buf_info.commandBufferCount = state->n_bufs;
	res = vkAllocateCommandBuffers(dev, &cmd_buf_info, cbs);

	if (res != VK_SUCCESS) {
		vk_error(res, "vkAllocateCommandBuffers");
		goto end_cbs;
	}

	// record command buffers
	for(uint32_t i = 0; i < state->n_bufs; i++) {
	uint32_t i = 0;
		struct render_buffer* buf = &state->bufs[i];
		buf->cb = cbs[i];

		VkCommandBufferBeginInfo begin_info = {0};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkBeginCommandBuffer(buf->cb, &begin_info);

		uint32_t width = state->swapchain_info.imageExtent.width;
		uint32_t height = state->swapchain_info.imageExtent.height;
		VkRect2D rect = {{0, 0}, {width, height}};

		VkClearValue clearValue = {
			.color.float32 = {0.8f, 0.6f, 0.3f, 1.f},
		};

		VkRenderPassBeginInfo rp_info = {0};
		rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		rp_info.renderArea = rect;
		rp_info.renderPass = state->rp;
		rp_info.framebuffer = buf->fb;
		rp_info.clearValueCount = 1;
		rp_info.pClearValues = &clearValue;
		vkCmdBeginRenderPass(buf->cb, &rp_info, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport vp = {0.f, 0.f, (float) width, (float) height, 0.f, 1.f};
		vkCmdSetViewport(buf->cb, 0, 1, &vp);
		vkCmdSetScissor(buf->cb, 0, 1, &rect);
		
		// here could be your render commands!
		//vkCmdBindPipeline(buf->cb, VK_PIPELINE_BIND_POINT_COMPUTE, state->computePipeline);
		//vkCmdBindDescriptorSets(buf->cb, VK_PIPELINE_BIND_POINT_COMPUTE, state->pipelineLayout, 0, 1, &state->descriptorSet, 0, NULL);
		//vkCmdDispatch(buf->cb, width*height*4, 1, 1);
		vkCmdBindPipeline(buf->cb, VK_PIPELINE_BIND_POINT_GRAPHICS, state->graphicsPipeline);
		vkCmdBindDescriptorSets(buf->cb, VK_PIPELINE_BIND_POINT_GRAPHICS, state->pipelineLayout, 0, 1, &state->descriptorSet, 0, NULL);
		vkCmdDraw(buf->cb, 4, 1, 0, 0);
		

		vkCmdEndRenderPass(buf->cb);
		vkEndCommandBuffer(buf->cb);
	}

	free(cbs);
	return true;

end_cbs:
	free(cbs);
	return false;

end_images:
	free(images);
	return false;
}

bool init_instance(struct state* state, unsigned n_dpy_exts,
				   const char** dpy_exts)
{
	// setup vulkan instance
	// query extension support
	uint32_t avail_extc = 0;
	VkResult res;
	res = vkEnumerateInstanceExtensionProperties(NULL, &avail_extc, NULL);

	if((res != VK_SUCCESS) || (avail_extc == 0)) {
		vk_error(res, "Could not enumerate instance extensions (1)");
		return false;
	}

	VkExtensionProperties *avail_exts = calloc(avail_extc, sizeof(*avail_exts));
	res = vkEnumerateInstanceExtensionProperties(NULL, &avail_extc, avail_exts);

	if(res != VK_SUCCESS) {
		free(avail_exts);
		vk_error(res, "Could not enumerate instance extensions (2)");
		return false;
	}

	for(size_t j = 0; j < avail_extc; ++j) {
		dlg_info("Vulkan Instance extensions %s", avail_exts[j].extensionName);
	}

	// create instance
	for(unsigned i = 0u; i < n_dpy_exts; ++i) {
		if(!has_extension(avail_exts, avail_extc, dpy_exts[i])) {
			free(avail_exts);
			dlg_fatal("Required extension %s not supported", dpy_exts[i]);
			return false;
		}
	}

	const char** enable_exts = malloc((n_dpy_exts + 1) * sizeof(*enable_exts));
	memcpy(enable_exts, dpy_exts, sizeof(*dpy_exts) * n_dpy_exts);
	uint32_t enable_extc = n_dpy_exts;

	// TODO: layers seem to crash when using VkDisplayKHR api (used by
	// swa kms backend).
	bool use_layers = true;
	const char* req = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
	bool has_debug = has_extension(avail_exts, avail_extc, req);
	bool use_debug = has_debug && use_layers;

	if(use_debug) {
		enable_exts[enable_extc++] = req;
	}

	free(avail_exts);

	VkApplicationInfo application_info = {0};
	application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	application_info.pApplicationName = "swa-example-vulkan";
	application_info.applicationVersion = 1;
	application_info.pEngineName = "swa-example-vulkan";
	application_info.engineVersion = 1;
	// will only run on the latest drivers anyways so we can require
	// vulkan 1.1 without problems
	application_info.apiVersion = VK_MAKE_VERSION(1,1,0);

	// layer reports error in api usage to debug callback
	const char *layers[] = {
		"VK_LAYER_KHRONOS_validation",
	};

	VkInstanceCreateInfo instance_info = {0};
	instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_info.pApplicationInfo = &application_info;
	instance_info.enabledExtensionCount = enable_extc;
	instance_info.ppEnabledExtensionNames = enable_exts;

	if(use_layers) {
		instance_info.enabledLayerCount = sizeof(layers) / sizeof(layers[0]);
		instance_info.ppEnabledLayerNames = layers;
	}

	res = vkCreateInstance(&instance_info, NULL, &state->instance);
	free(enable_exts);

	if(res != VK_SUCCESS) {
		vk_error(res, "Could not create instance");
		return false;
	}

		// debug callback
		VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
		if(use_debug) {
			state->api.createDebugUtilsMessengerEXT =
				(PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
						state->instance, "vkCreateDebugUtilsMessengerEXT");
			state->api.destroyDebugUtilsMessengerEXT =
				(PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
						state->instance, "vkDestroyDebugUtilsMessengerEXT");

			dlg_assert(state->api.createDebugUtilsMessengerEXT);
			dlg_assert(state->api.destroyDebugUtilsMessengerEXT);

			VkDebugUtilsMessageSeverityFlagsEXT severity =
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			VkDebugUtilsMessageTypeFlagsEXT types =
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

			VkDebugUtilsMessengerCreateInfoEXT debug_info = {0};
			debug_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debug_info.messageSeverity = severity;
			debug_info.messageType = types;
			debug_info.pfnUserCallback = &debug_callback;

			state->api.createDebugUtilsMessengerEXT(state->instance, &debug_info,
				NULL, &messenger);
		}

		state->messenger = messenger;
	return true;
}

static bool init_swapchain(struct state* state, unsigned width, unsigned height)
{
	VkResult res;
	VkDevice dev = state->device;

	VkSwapchainCreateInfoKHR* info = &state->swapchain_info;
	info->sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	info->surface = state->surface;

	// Get available present modes
	uint32_t present_mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(state->phdev, state->surface,
			&present_mode_count, NULL);
	VkPresentModeKHR *present_modes =
		calloc(present_mode_count, sizeof(VkPresentModeKHR));

	res = vkGetPhysicalDeviceSurfacePresentModesKHR(state->phdev,
			state->surface, &present_mode_count, present_modes);

	if(res != VK_SUCCESS || present_mode_count == 0) {
		vk_error(res, "Failed to retrieve surface present modes");
		return false;
	}

	// this mode is required to be supported
	info->presentMode = VK_PRESENT_MODE_FIFO_KHR;

	bool vsync = false;

	if(!vsync) {
		for (size_t i = 0; i < present_mode_count; i++) {
			if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
				info->presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;

			} else if (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
				info->presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}
	}

	free(present_modes);

	VkSurfaceCapabilitiesKHR caps;
	res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(state->phdev,
			state->surface, &caps);

	if(res != VK_SUCCESS) {
		vk_error(res, "failed retrieve surface caps");
		return false;
	}

	uint32_t pref_image_count = caps.minImageCount + 1;

	if((caps.maxImageCount > 0) && (pref_image_count > caps.maxImageCount)) {
		pref_image_count = caps.maxImageCount;
	}

	// transformation
	VkSurfaceTransformFlagBitsKHR transform =
		VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

	if(!(caps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)) {
		transform = caps.currentTransform;
	}

	// use alpha if possible
	VkCompositeAlphaFlagBitsKHR alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	VkCompositeAlphaFlagBitsKHR alpha_flags[] = {
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
	};

	for(int i = 0; i < 4; ++i) {
		if (caps.supportedCompositeAlpha & alpha_flags[i]) {
			alpha = alpha_flags[i];
			break;
		}
	}

	// In this case, we can freely choose the size.
	if(caps.currentExtent.width == 0xFFFFFFFFu) {
		info->imageExtent.width = width;
		info->imageExtent.height = height;

	} else {
		info->imageExtent.width = caps.currentExtent.width;
		info->imageExtent.height = caps.currentExtent.height;
	}

	// usage
	dlg_assert(caps.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
	info->imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// create swapchain
	info->minImageCount = pref_image_count;
	info->preTransform = transform;
	info->imageArrayLayers = 1;
	info->imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info->clipped = VK_TRUE;
	info->compositeAlpha = alpha;

	res = vkCreateSwapchainKHR(dev, info, NULL, &state->swapchain);

	if(res != VK_SUCCESS) {
		vk_error(res, "Failed to create vk swapchain");
		return false;
	}

	if(!init_render_buffers(state)) {
		return false;
	}

	return true;
}

bool init_renderer(struct state* state)
{
	VkResult res;

	// TODO: better device choosing
	// we just choose the first physical device for now
	VkPhysicalDevice phdevs[10];
	uint32_t count = 10;
	res = vkEnumeratePhysicalDevices(state->instance, &count, phdevs);

	if((res != VK_SUCCESS  && res != VK_INCOMPLETE) || count == 0) {
		vk_error(res, "Could not retrieve physical device");
		return false;
	}

	state->phdev = phdevs[0];

	// TODO: temporary workaround for my amdgpu kernel driver crashing
	// all the time... advanced penguin devices
	if(count > 1) {
		state->phdev = phdevs[1];
	}

	// output dev extensions
	VkExtensionProperties *phdev_exts = NULL;
	uint32_t phdev_extc = 0;

	res = vkEnumerateDeviceExtensionProperties(state->phdev, NULL,
			&phdev_extc, NULL);

	if((res != VK_SUCCESS) || (phdev_extc == 0)) {
		vk_error(res, "Could not enumerate device extensions (1)");
		return false;
	}

	phdev_exts = malloc(sizeof(*phdev_exts) * phdev_extc);
	res = vkEnumerateDeviceExtensionProperties(state->phdev, NULL,
			&phdev_extc, phdev_exts);

	if(res != VK_SUCCESS) {
		free(phdev_exts);
		vk_error(res, "Could not enumerate device extensions (2)");
		return false;
	}

	for(size_t j = 0; j < phdev_extc; ++j) {
		dlg_debug("Vulkan Device extensions %s", phdev_exts[j].extensionName);
	}

	const char* dev_ext = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

	if(!has_extension(phdev_exts, phdev_extc, dev_ext)) {
		dlg_error("Device has no support for swapchain extension");
		free(phdev_exts);
		return false;
	}

	const char* exts[2];

	unsigned n_exts = 1u;
	exts[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

	
	if(has_extension(phdev_exts, phdev_extc, VK_EXT_DISPLAY_CONTROL_EXTENSION_NAME)
	&& has_extension(phdev_exts, phdev_extc, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME)) {
		exts[n_exts++] = VK_EXT_DISPLAY_CONTROL_EXTENSION_NAME;
		exts[n_exts++] = VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME;
	}

	free(phdev_exts);

	// queue infos
	uint32_t qfam_count;
	vkGetPhysicalDeviceQueueFamilyProperties(state->phdev, &qfam_count, NULL);
	VkQueueFamilyProperties *qprops = calloc(sizeof(*qprops), qfam_count);
	vkGetPhysicalDeviceQueueFamilyProperties(state->phdev, &qfam_count, qprops);

	uint32_t gfx_qfam = 0xFFFFFFFFu; // graphics queue family

	for(unsigned i = 0u; i < qfam_count; ++i) {
		if(qprops[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			gfx_qfam = i;
			break;
		}
	}

	// TODO: use platform-specific queries. Integrate into swa?
	// TODO: when no queue supports presenting here, try another phdev
	uint32_t present_qfam = 0xFFFFFFFFu;

	for(unsigned i = 0u; i < qfam_count; ++i) {
		VkBool32 sup;
		res = vkGetPhysicalDeviceSurfaceSupportKHR(state->phdev, i,
				state->surface, &sup);

		if(res != VK_SUCCESS) {
			vk_error(res, "vkGetPhysicalDeviceSurfaceSupportKHR");
			continue;
		}

		if(sup) {
			present_qfam = i;
			break;
		}
	}

	free(qprops);
	dlg_assert(present_qfam != 0xFFFFFFFFu);
	dlg_assert(gfx_qfam != 0xFFFFFFFFu);

	// create device
	float prio = 1.f;
	unsigned n_qis = 1;
	VkDeviceQueueCreateInfo qis[2] = {0};
	qis[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	qis[0].queueFamilyIndex = gfx_qfam;
	qis[0].queueCount = 1;
	qis[0].pQueuePriorities = &prio;

	if(present_qfam != gfx_qfam) {
		++n_qis;
		qis[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		qis[1].queueFamilyIndex = present_qfam;
		qis[1].queueCount = 1;
		qis[1].pQueuePriorities = &prio;
	}

	VkDeviceCreateInfo dev_info = {0};
	dev_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	dev_info.queueCreateInfoCount = n_qis;
	dev_info.pQueueCreateInfos = qis;
	dev_info.enabledExtensionCount = n_exts;
	dev_info.ppEnabledExtensionNames = exts;

	VkDevice dev;
	res = vkCreateDevice(state->phdev, &dev_info, NULL, &dev);

	if(res != VK_SUCCESS) {
		vk_error(res, "Failed to create vulkan device");
		return false;
	}

	state->device = dev;
	state->qs.gfx_fam = gfx_qfam;
	state->qs.present_fam = present_qfam;

	vkGetDeviceQueue(dev, gfx_qfam, 0, &state->qs.gfx);

	if(gfx_qfam != present_qfam) {
		vkGetDeviceQueue(dev, gfx_qfam, 0, &state->qs.present);

	} else {
		state->qs.present = state->qs.gfx;
	}
	
	//descriptor pool
	VkDescriptorPoolSize descriptorPoolSize = {
		.descriptorCount = 3,
		.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
	};
	
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.maxSets = 3,
		.poolSizeCount = 1u,
		.pPoolSizes = &descriptorPoolSize
	};
	VK_CHECK_RESULT(vkCreateDescriptorPool(dev, &descriptorPoolCreateInfo, NULL, &state->descriptorPool));
	
	// query render format
	uint32_t formats_count;
	res = vkGetPhysicalDeviceSurfaceFormatsKHR(state->phdev,
			state->surface, &formats_count, NULL);

	if(res != VK_SUCCESS || formats_count == 0) {
		vk_error(res, "failed retrieve surface formats");
		return false;
	}

	VkSurfaceFormatKHR *formats = calloc(formats_count, sizeof(*formats));
	res = vkGetPhysicalDeviceSurfaceFormatsKHR(state->phdev, state->surface,
			&formats_count, formats);

	if(res != VK_SUCCESS) {
		vk_error(res, "failed retrieve surface formats");
		return false;
	}

	// try to find a format matching our needs if we don't have
	// free choice
	state->swapchain_info.imageFormat = formats[0].format;
	state->swapchain_info.imageColorSpace = formats[0].colorSpace;

	if(formats_count == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
		state->swapchain_info.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
	}

	free(formats);

	// render pass
	VkAttachmentDescription attachment = {0};
	attachment.format = state->swapchain_info.imageFormat;
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference color_ref = {0};
	color_ref.attachment = 0u;
	color_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {0};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_ref;

	VkRenderPassCreateInfo rp_info = {0};
	rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	rp_info.attachmentCount = 1;
	rp_info.pAttachments = &attachment;
	rp_info.subpassCount = 1;
	rp_info.pSubpasses = &subpass;

	res = vkCreateRenderPass(dev, &rp_info, NULL, &state->rp);

	if(res != VK_SUCCESS) {
		vk_error(res, "Failed to create render pass");
		return false;
	}
	
	// descriptor set layout
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {
		.binding = 0,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
	};

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = 1u,
		.pBindings = &descriptorSetLayoutBinding
	};

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(dev, &descriptorSetLayoutCreateInfo, NULL, &state->descriptorSetLayout));
	
	// pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 1,
		.pSetLayouts = &state->descriptorSetLayout
	};
	
	vkCreatePipelineLayout(dev, &pipelineLayoutCreateInfo, NULL, &state->pipelineLayout);
	
	// pipeline
	VkShaderModule compModule, vertModule, fragModule;
	//vulkan_shader("main.comp.spv", dev, &compModule); 
	vulkan_shader("main.vert.spv", dev, &vertModule); 
	vulkan_shader("main.frag.spv", dev, &fragModule); 

	/*
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
		.layout = state->pipelineLayout,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = 0
	};

	VK_CHECK_RESULT(vkCreateComputePipelines(dev, 0, 1, &computePipelineCreateInfo, 0, &state->computePipeline));
	*/

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

	/*
	VkVertexInputBindingDescription vertexInputBindingDescription = {
		.binding = 0,
		.stride = sizeof(double), //TODO: check this
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};

	VkVertexInputAttributeDescription vertexInputAttributeDescription = {
		.location = 0,
		.binding = 0,
		.format = VK_FORMAT_R32G32_SFLOAT,
		.offset = 0
	};
	*/

	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.vertexBindingDescriptionCount = 0,
		.pVertexBindingDescriptions = NULL,
		.vertexAttributeDescriptionCount = 0,
		.pVertexAttributeDescriptions = NULL
	};

	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
		.primitiveRestartEnable = VK_FALSE
	};

	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.scissorCount = 1
	};
	
	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_NONE,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.lineWidth = 1.f
	};
	
	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT
	};
	
	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {
		.blendEnable = false,
		.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT
	};
	
	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &pipelineColorBlendAttachmentState
	};
	
	VkDynamicState dynamicStates[2] = {
		VK_DYNAMIC_STATE_VIEWPORT,
    		VK_DYNAMIC_STATE_SCISSOR,
	};
	
	VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.pDynamicStates = dynamicStates,
		.dynamicStateCount = 2
	};
	
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.stageCount = sizeof(stages)/sizeof(VkPipelineShaderStageCreateInfo),
		.pStages = stages,
		.pVertexInputState = &pipelineVertexInputStateCreateInfo,
		.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo,
		.pTessellationState = NULL,
		.pViewportState = &pipelineViewportStateCreateInfo,
		.pRasterizationState = &pipelineRasterizationStateCreateInfo,
		.pMultisampleState = &pipelineMultisampleStateCreateInfo,
		.pDepthStencilState = NULL,
		.pColorBlendState = &pipelineColorBlendStateCreateInfo,
		.pDynamicState = &pipelineDynamicStateCreateInfo,
		.layout = state->pipelineLayout,
		.renderPass = state->rp,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};

	VK_CHECK_RESULT(vkCreateGraphicsPipelines(dev, 0, 1, &graphicsPipelineCreateInfo, 0, &state->graphicsPipeline));

	//vkDestroyShaderModule(dev, compModule, NULL);
	vkDestroyShaderModule(dev, vertModule, NULL);
	vkDestroyShaderModule(dev, fragModule, NULL);


	// command pool
	VkCommandPoolCreateInfo cpool_info = {0};
	cpool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cpool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	cpool_info.queueFamilyIndex = gfx_qfam;
	res = vkCreateCommandPool(dev, &cpool_info, NULL, &state->cmd_pool);

	// semaphore
	VkSemaphoreCreateInfo sem_info = {0};
	sem_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	res = vkCreateSemaphore(dev, &sem_info, NULL, &state->acquire_sem);

	if(res != VK_SUCCESS) {
		vk_error(res, "vkCreateSemaphore");
		return false;
	}

	res = vkCreateSemaphore(dev, &sem_info, NULL, &state->render_sem);

	if(res != VK_SUCCESS) {
		vk_error(res, "vkCreateSemaphore");
		return false;
	}

	return true;
}

static void cleanup_renderer(struct state* state)
{
	if(state->device) {
		vkDeviceWaitIdle(state->device);
	}

	destroy_render_buffers(state);

	if(state->swapchain) {
		vkDestroySwapchainKHR(state->device, state->swapchain, NULL);
	}

	if(state->rp) {
		vkDestroyRenderPass(state->device, state->rp, NULL);
	}

	if(state->acquire_sem) {
		vkDestroySemaphore(state->device, state->acquire_sem, NULL);
	}

	if(state->render_sem) {
		vkDestroySemaphore(state->device, state->render_sem, NULL);
	}

	if(state->cmd_pool) {
		vkDestroyCommandPool(state->device, state->cmd_pool, NULL);
	}

	if(state->graphicsPipeline) {
		vkDestroyPipeline(state->device, state->graphicsPipeline, NULL);
	}

	if(state->pipelineLayout) {
		vkDestroyPipelineLayout(state->device, state->pipelineLayout, NULL);
	}

	/*
	if(state->descriptorSet) {
		vkFreeDescriptorSets(state->device, state->descriptorPool, 1, &state->descriptorSet);
	}

	if(state->descriptorSetLayout) {
		vkDestroyDescriptorSetLayout(state->device, state->descriptorSetLayout, NULL);
	}

	if(state->descriptorPool) {
		vkDestroyDescriptorPool(state->device, state->descriptorPool, NULL);
	}
	*/

	
}

static void cleanup(struct state* state)
{
	if(state->device) {
		vkDestroyDevice(state->device, NULL);
	}

	if(state->messenger) {
		dlg_assert(state->api.destroyDebugUtilsMessengerEXT);
		state->api.destroyDebugUtilsMessengerEXT(state->instance,
				state->messenger, NULL);
	}

	if(state->instance) {
		vkDestroyInstance(state->instance, NULL);
	}
}

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