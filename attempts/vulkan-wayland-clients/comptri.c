#include <volk.h>
#include <swa/swa.h>
#include <swa/key.h>
#include <dlg/dlg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

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

#include "command.c"
#include "device.c"
#include "instance.c"
#include "pipeline.c"
#include "swapchain.c"
/*
struct render_buffer {
	VkCommandBuffer cb;
	VkImageView im;
	VkFramebuffer fb;
}; // I hope it is not required by SWA
*/
struct state {
	VkInstance instance;
	VkPhysicalDevice physicalDevice; //TODO for multiple GPUs
	VkDevice device;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapchain;
	VkSwapchainCreateInfoKHR swapchain_info; //why?
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
       	VkDescriptorSet descriptorSet;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphics_pipeline;
	VkPipeline compute_pipeline;
	VkCommandPool commandPool;
	VkCommandBuffer *commandBuffers;
	VkImageView *imageviews;
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
	//VkQueue compute_queue; //compute only every frame for now
	//uint32_t compute_fam;
	bool run;
	bool resized;
	unsigned w;
	unsigned h;
	struct swa_display* dpy;

} state;

VkBuffer vertexBuffer;
VkDeviceMemory vertexBufferMemory;

void resize()
{
	// make sure all previous rendering has finished since we will
	// destroy rendering resources
	/*
		vkDeviceWaitIdle(state.device);
		//destroy_render_buffers(state);
		VkResult res;

		// recreate swapchain
		VkSurfaceCapabilitiesKHR caps;
		res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(state.physicalDevice,
			state.surface, &caps);
		if(res != VK_SUCCESS) {
			vk_error(res, "failed retrieve surface caps");
			state.run = false;
			return;
		}

		if(caps.currentExtent.width == 0xFFFFFFFFu) {
			state.swapchain_info.imageExtent.width = state.w;
			state.swapchain_info.imageExtent.height = state.h;
		} else {
			dlg_info("  fixed swapchain size: %d %d",
				caps.currentExtent.width,
				caps.currentExtent.height);
			state.swapchain_info.imageExtent.width = caps.currentExtent.width;
			state.swapchain_info.imageExtent.height = caps.currentExtent.height;
		}

		state.swapchain_info.oldSwapchain = state.swapchain;
		res = vkCreateSwapchainKHR(state.device, &state.swapchain_info,
			NULL, &state.swapchain);

		vkDestroySwapchainKHR(state.device,
			state.swapchain_info.oldSwapchain, NULL);
		state.swapchain_info.oldSwapchain = VK_NULL_HANDLE;

		if (res != VK_SUCCESS) {
			vk_error(res, "Failed to create vk swapchain");
			state.run = false;
			return;
		}
	*/
	//TODO
	/*
	// recreate render buffers
	if(!init_render_buffers(state)) {
		state.run = false;
		return;
	}
	*/

	/*
	vkDeviceWaitIdle(state.device);

	destroy_vulkan_framebuffers(state.device, state.swapchain, state.renderPass, &state.n_bufs, &state.framebuffers);

	if(state.swapchain) {
		vkDestroySwapchainKHR(state.device, state.swapchain, NULL);
	}
	vkFreeCommandBuffers(state.device, state.commandPool, 1, state.commandBuffers);
	*/
	/*

	VkSurfaceCapabilitiesKHR caps;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);

	if(caps.currentExtent.width == 0xFFFFFFFFu) {
		state.swapchain_info.imageExtent.width = state.w;
		state.swapchain_info.imageExtent.height = state.h;
	} else {
		dlg_info("  fixed swapchain size: %d %d",
			caps.currentExtent.width,
			caps.currentExtent.height);
		state.swapchain_info.imageExtent.width = caps.currentExtent.width;
		state.swapchain_info.imageExtent.height = caps.currentExtent.height;
	}

	state.swapchain_info.oldSwapchain = state.swapchain;
	res = vkCreateSwapchainKHR(state.device, &state.swapchain_info,
		NULL, &state.swapchain);

	vkDestroySwapchainKHR(state.device,
		state.swapchain_info.oldSwapchain, NULL);
	state.swapchain_info.oldSwapchain = VK_NULL_HANDLE;

	if (res != VK_SUCCESS) {
		vk_error(res, "Failed to create vk swapchain");
		state.run = false;
		return;
	}
	*/

	state.resized = false;
}

//window resize function is required or it will crash
static void window_resize(struct swa_window* win, unsigned w, unsigned h)
{
	//struct state* state = swa_window_get_userdata(win);
	dlg_info("resized to %d %d", w, h);

	if(!state.swapchain) {
		//TODO
		/*
		if(!init_swapchain(state, w, h)) {
			dlg_error("Failed to init swapchain");
			return;
		}
		*/
		if (vulkan_swapchain(state.physicalDevice, state.device, state.surface, &state.swapchain, w, h)) {
			printf("not resizing");
			return;
		}


		vulkan_graphics_pipeline(state.device, &state.graphics_pipeline, &state.renderPass, &state.pipelineLayout);

		vulkan_framebuffers(state.device, state.swapchain, state.renderPass, &state.n_bufs,
							&state.framebuffers, state.w, state.h);

		//create_buffer(state.device, physicalDevice, 2*M*sizeof(float), &vertexBuffer, &vertexBufferMemory);

		vulkan_graphics_commands(state.device, graphics_pipeline, state.renderPass, state.n_bufs,
						framebuffers, &vertexBuffer, M, &state.commandBuffers, &state.commandPool);
	}

	state.resized = true;
	state.w = w;
	state.h = h;
}

static bool window_draw(struct swa_window* win)
{
	//struct state* state = swa_window_get_userdata(win);
	VkResult res;
	write(1,"redrawing\n",10);

	if(!state.swapchain) {
		dlg_warn("No swapchain!");
		return false;
	}

	// struct timespec now;
	// timespec_get(&now, TIME_UTC);
	// float ms = (now.tv_nsec - last_redraw.tv_nsec) / (1000.f * 1000.f);
	// ms += 1000.f * (now.tv_sec - last_redraw.tv_sec);
	// dlg_info("Time between redraws: %f", ms);
	// last_redraw = now;

	vkDeviceWaitIdle(state.device);

	// acquire image
	// we treat suboptimal as success here
	uint32_t id;

	while(true) {
		res = vkAcquireNextImageKHR(state.device, state.swapchain,
									UINT64_MAX, NULL, VK_NULL_HANDLE, &id);

		if(res == VK_SUCCESS || res == VK_SUBOPTIMAL_KHR) {
			break;

		} else if(res == VK_ERROR_OUT_OF_DATE_KHR) {
			dlg_warn("Got out of date swapchain (acquire)");
			return false;

		} else {
			vk_error(res, "vkAcquireNextImageKHR");
			state.run = false;
			return false;
		}
	}

	// submit render commands
	VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo si = {0};
	si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	si.pCommandBuffers = &state.bufs[id].cb;
	si.commandBufferCount = 1u;
	si.waitSemaphoreCount = 1u;
	si.pWaitSemaphores = NULL;
	si.pWaitDstStageMask = &stage;
	si.signalSemaphoreCount = 1u;
	si.pSignalSemaphores = &state.render_sem;

	res = vkQueueSubmit(state.qs.gfx, 1, &si, VK_NULL_HANDLE);

	if(res != VK_SUCCESS) {
		vk_error(res, "vkQueueSubmit");
		state.run = false;
		return false;
	}

	// present
	VkPresentInfoKHR present_info = {0};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &state.swapchain;
	present_info.pImageIndices = &id;
	present_info.waitSemaphoreCount = 0;
	present_info.pWaitSemaphores = NULL;//&state.render_sem;

	res = vkQueuePresentKHR(state.qs.present, &present_info);

	if(res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
		if(res == VK_ERROR_OUT_OF_DATE_KHR) {
			dlg_warn("Got out of date swapchain (present)");
			return true;
		}

		vk_error(res, "vkQueuePresentKHR");
		state.run = false;
		return false;
	}

	return true;
}

static void window_close(struct swa_window* win)
{
	//struct state* state = swa_window_get_userdata(win);
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

int main(int argc, char *argv[])
{
	volkInitialize();
	int ret = 0;
	struct swa_display* dpy = swa_display_autocreate("compute triangle");

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
	
	uint32_t glfwExtensionCount = 1;
	const char** extensions = swa_display_vk_extensions(dpy, &glfwExtensionCount);
	
	//uint32_t glfwExtensionCount = 0;
	//const char **extensions = malloc((glfwExtensionCount+1)*sizeof(char*));
	//extensions[0] = VK_KHR_SURFACE_EXTENSION_NAME;
	//extensions[1] =	VK_KHR_DISPLAY_EXTENSION_NAME;
	for (int i = 0; i < glfwExtensionCount; ++i) {
		printf("%s\n", extensions[i]);
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
		.enabledExtensionCount = glfwExtensionCount+1,
		.ppEnabledExtensionNames = extensions,
		.pApplicationInfo = &applicationInfo
	};

	VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, NULL, &state.instance));

	volkLoadInstance(state.instance);

	uint32_t n = 1;
	vkEnumeratePhysicalDevices(state.instance, &n, state.physicalDevice);
	float priority = 1.0f;
	VkDeviceQueueCreateInfo infoQueue = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueCount = 1,
		.pQueuePriorities = &priority
	};

	const char *extensions[] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	VkDeviceCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &infoQueue,
		.enabledExtensionCount = sizeof(extensions)/sizeof(char*),
		.ppEnabledExtensionNames = extensions
	};
	VK_CHECK_RESULT(vkCreateDevice(*state.physicalDevice, &info, NULL, state.device));
	
	volkLoadDevice(state.device);

	// Create the vulkan instance. Make sure to enable the extensions
	// the display requires. We can't create the device here since
	// the queues we need depend on the created vulkan surface which
	// in turn depens on the vulkan instance.
	//struct state state = {0};
	state.w=640;
	state.h=480;

	// create window for vulkan surface
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
		ret = EXIT_FAILURE;
		goto cleanup_state;
	}

	state.surface = (VkSurfaceKHR) swa_window_get_vk_surface(win);

	if(!surface) {
		ret = EXIT_FAILURE;
		dlg_error("Couldn't get vk surface from swa window");
		goto cleanup_win;
	}

	swa_window_set_userdata(win, &state);
	// timespec_get(&last_redraw, TIME_UTC);

	// main loop
	state.run = true;
	state.dpy = dpy;

	VkSemaphore semaphore1, semaphore2;
	VkSemaphoreCreateInfo semaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0
	};
	vkCreateSemaphore(state.device, &semaphoreCreateInfo, NULL, &semaphore1);
	vkCreateSemaphore(state.device, &semaphoreCreateInfo, NULL, &semaphore2);

	vkGetDeviceQueue(state.device, 0, 0, &state.queue);

	while (state.run) {
		if(!swa_display_dispatch(dpy, false)) {
			break;
		}

		if(state.resized) {
			window_resize(win,state.w,state.h);
		}
		uint32_t imageIndex;
		VK_CHECK_RESULT(vkAcquireNextImageKHR(state.device, state.swapchain, UINT64_MAX, semaphore1, VK_NULL_HANDLE, &imageIndex))

		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		VkSubmitInfo submitInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = NULL,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &semaphore1,
			.pWaitDstStageMask = waitStages,
			.commandBufferCount = 1,
			.pCommandBuffers = &state.commandBuffers[imageIndex],
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &semaphore2
		};
		VK_CHECK_RESULT(vkQueueSubmit(state.queue, 1, &submitInfo, VK_NULL_HANDLE))
		VkPresentInfoKHR queuePresentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.pNext = NULL,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &semaphore2,
			.swapchainCount = 1,
			.pSwapchains = &state.swapchain,
			.pImageIndices = &imageIndex,
			.pResults = NULL
		};
		vkQueuePresentKHR(state.queue, &queuePresentInfo);

		vkQueueWaitIdle(state.queue);
	}

	if(state.device) {
		vkDeviceWaitIdle(state.device);
	}

	//destroy_render_buffers(state);
	if(vertexBufferMemory) {
		vkFreeMemory(state.device, vertexBufferMemory, NULL);
	}

	if(vertexBuffer) {
		vkDestroyBuffer(state.device, vertexBuffer, NULL);
	}

	destroy_vulkan_framebuffers(state.device, state.swapchain, state.renderPass, &state.n_bufs, &state.framebuffers);

	if(state.swapchain) {
		vkDestroySwapchainKHR(state.device, state.swapchain, NULL);
	}

	if(state.renderPass) {
		vkDestroyRenderPass(state.device, state.renderPass, NULL);
	}

	destroy_vulkan_graphics_pipeline(state.device, &state.graphics_pipeline, &state.renderPass, &state.pipelineLayout);
	destroy_vulkan_image_views(state.device, state.swapchain, state.renderPass, &state.n_bufs, &state.framebuffers);

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
		vkDestroyCommandPool(state.device, commandPool, NULL);
	}

	//free(commandBuffers);
	swa_window_destroy(win);
cleanup_state:

	if(state.device) {
		vkDestroyDevice(state.device, NULL);
	}

	/*
	if(state.messenger) {
		dlg_assert(state.api.destroyDebugUtilsMessengerEXT);
		state.api.destroyDebugUtilsMessengerEXT(state.instance,
			state.messenger, NULL);
	}
	*/
	if(state.instance) {
		vkDestroyInstance(state.instance, NULL);
	}

cleanup_dpy:
	swa_display_destroy(dpy);
	dlg_trace("Exiting cleanly");
	return ret;
}
