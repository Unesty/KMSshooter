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

	/*
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
		*/
	return true;
}

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

static void window_close(struct swa_window* win)
{
	struct state* state = swa_window_get_userdata(win);
	state->run = false;
}

static void window_key(struct swa_window* win, const struct swa_key_event* ev)
{
	struct state* state = swa_window_get_userdata(win);

	if(ev->pressed){
		switch(ev->keycode)
		{
		case swa_key_escape:
			dlg_info("Escape pressed, exiting");
			state->run = false;
		case swa_key_enter:
			write(1,"Enter pressed\n", 13);
		}
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
