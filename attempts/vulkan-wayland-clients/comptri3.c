#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

#include <swa/swa.h>
#include <swa/key.h>
#include <dlg/dlg.h>

//#include <vulkan/vulkan.h>
#define VOLK_IMPLEMENTATION
#include <volk.h>

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
	VkDescriptorSet descriptorSet;
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
	VkBuffer input_buffer;
	VkDeviceMemory input_buffer_memory;
	void* input_buffer_map;
} state;

struct gfx {
    uint32_t vcnt;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
} gfx;
struct shaders_uniforms{ //same to shadertoy
    float iMouse[4];//iMouse.xy updated always
    float iDate[4];
    int iMouse_lr[2]; //is mouse left[0], right[1] clicked
    float iResolution[2];
    int debugdraw;
    int pause;
    float iTime;
    float iTimeDelta;
    int iFrame;
} shader_input;

//window resize function is required or it will crash
static void window_resize(struct swa_window* win, unsigned w, unsigned h)
{
	//TODO
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

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
						VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}
	return -1;
}


int main(int argc, char** argv, char ** envp)
{
	int ret;
	ret = vulkan();
    if(ret != 0)
		return ret;
	return 0;
}
