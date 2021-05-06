#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "xdg-shell-client.h"
//#include <wayland-client.h>
#include "wayland.c"

#define VOLK_IMPLEMENTATION
#include "subprojects/volk/volk.h"

//because of volk there is no platform specific defines
#define VK_KHR_wayland_surface 1
#define VK_KHR_WAYLAND_SURFACE_SPEC_VERSION 6
#define VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME "VK_KHR_wayland_surface"

static const VkFormat format = VK_FORMAT_B8G8R8A8_SRGB;

int r=0;

static int is_ext_name(const void *a_, const void *b_) {
	const char *a = a_;
	const VkExtensionProperties *b = b_;
	return strcmp(a, b->extensionName);
}

char check_ext(int n_ext, VkExtensionProperties *ext_p, const char *ext_req) {
	void *found = bsearch(ext_req, ext_p, n_ext,
			    sizeof(VkExtensionProperties), is_ext_name);
	char ret = found != 0;
	const char *words[] = {"is NOT", "is"};
	printf("Extension %s %s supported", ext_req, words[ret]);
	return ret;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
VkDebugUtilsMessageTypeFlagsEXT messageType, const
VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	fprintf(stderr, "validation layer: %s\n", pCallbackData->pMessage);
	return VK_FALSE;
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

struct window_vars{

}w;

int main(int argc, char** argv, char** envp)
{
	struct wayland_client wayland-client;
	
	r=volkInitialize();
	if(r){
		printf("volkInitialize() failed");
		return 0;
	}
	VkInstance instance;
	void volkLoadInstance(VkInstance instance);
	VkDebugUtilsMessengerEXT messenger;

	char explicit_fencing;

	struct {
		PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsMessengerEXT;
		PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsMessengerEXT;
		PFN_vkGetMemoryFdPropertiesKHR getMemoryFdPropertiesKHR;
		PFN_vkGetSemaphoreFdKHR getSemaphoreFdKHR;
		PFN_vkImportSemaphoreFdKHR importSemaphoreFdKHR;
	} api;

	VkDevice dev;


	uint32_t queue_family;
	VkQueue queue;

	// pipeline
	VkDescriptorSetLayout ds_layout;
	VkCommandPool command_pool;
	VkDescriptorPool ds_pool;
	
	char platformSurfaceExtFound = 0;
	char SurfaceExtFound = 0;
	uint32_t instance_extension_count = 0;
	uint32_t enabled_extension_count = 0;
	uint32_t enabled_layer_count = 0;
	char *extension_names[64];
	char *enabled_layers[64];
	char is_minimized = 0;
	VkCommandPool cmd_pool = VK_NULL_HANDLE;
	r = vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, NULL);
	if (r) return 1;
	if (instance_extension_count > 0) {
		VkExtensionProperties *instance_extensions = malloc(sizeof(VkExtensionProperties) * instance_extension_count);
		r = vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, instance_extensions);
		if (r) return -1;
		for (uint32_t i = 0; i < instance_extension_count; i++) {
			if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
				SurfaceExtFound = 1;
				extension_names[enabled_extension_count++] = VK_KHR_SURFACE_EXTENSION_NAME;
			}
			if (!strcmp(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
				platformSurfaceExtFound = 1;
				extension_names[enabled_extension_count++] = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
			}
			if (!strcmp(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, instance_extensions[i].extensionName)) {
				extension_names[enabled_extension_count++] = VK_KHR_SURFACE_EXTENSION_NAME;
				extension_names[enabled_extension_count++] = VK_KHR_DISPLAY_EXTENSION_NAME;
				extension_names[enabled_extension_count++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

			}
			//if(enabled_extension_count < 64) return 1;
		}

		free(instance_extensions);
	}
	if (!platformSurfaceExtFound) {
		printf("no sufrace ext\n");
	}

	const VkApplicationInfo app = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = NULL,
		.pApplicationName = "vulkan nothing",
		.applicationVersion = 0,
		.pEngineName = "my engine",
		.engineVersion = 0,
		.apiVersion = VK_API_VERSION_1_1,
	};

	VkDebugUtilsMessengerCreateInfoEXT createInfo2 = {
		.sType =
		VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = debugCallback
	};

	VkInstanceCreateInfo instanceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = &createInfo2,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL,
		.enabledExtensionCount = enabled_extension_count,
		.ppEnabledExtensionNames = (const char *const *)extension_names
	};
	if((r=vkCreateInstance(&instanceCreateInfo,NULL,&instance))){
		printf("vkCreateInstance failed\n");
	}
	volkLoadInstance(instance);

	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger =
	(PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
	"vkCreateDebugUtilsMessengerEXT");
	VkDebugUtilsMessengerEXT debugMessenger;
	vkCreateDebugUtilsMessenger(instance, &createInfo2, 0, &debugMessenger);

	uint32_t pdevn = 1;
	vkEnumeratePhysicalDevices(instance, &pdevn, NULL);
	VkPhysicalDevice pdev;
	vkEnumeratePhysicalDevices(instance, &pdevn, &pdev);
	if (pdevn == 0) {
		printf("No physical devices");
		return VK_NULL_HANDLE;
	}

/*	struct VkPhysicalDevicePCIBusInfoPropertiesEXT busProps;
	struct VkPhysicalDeviceProperties2 aa = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
		.pNext = &busProps,
	}; XXX broken
	vkGetPhysicalDeviceProperties22(pdev, &aa);*/
	VkPhysicalDeviceProperties props;
	vkGetPhysicalDeviceProperties(pdev, &props);

	VkPhysicalDeviceFeatures pdevf;
	vkGetPhysicalDeviceFeatures(pdev, &pdevf);
	printf("sparseBinding %d\n",pdevf.sparseBinding);
	
	float priority = 1.0f;
	VkDeviceQueueCreateInfo infoQueue = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueCount = 1,
		.pQueuePriorities = &priority
	};

	const char *extensions[] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	VkDeviceCreateInfo cdinfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &infoQueue,
		.enabledExtensionCount = sizeof(extensions)/sizeof(char*),
		.ppEnabledExtensionNames = extensions
	};
	if ((r = vkCreateDevice(pdev, &cdinfo, NULL, &dev))) {
		fprintf(stderr, "ERROR: create_device() failed.\n");
		return r;
	}
	volkLoadDevice(dev);

	static PFN_vkGetDeviceProcAddr g_gdpa = NULL;

	PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
	PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
	PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
	PFN_vkQueuePresentKHR fpQueuePresentKHR;
	PFN_vkGetRefreshCycleDurationGOOGLE fpGetRefreshCycleDurationGOOGLE;
	PFN_vkGetPastPresentationTimingGOOGLE fpGetPastPresentationTimingGOOGLE;
#define GET_DEVICE_PROC_ADDR(dev, entrypoint)\
{\
	if (!g_gdpa)\
		g_gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(instance, "vkGetDeviceProcAddr");\
	fp##entrypoint = (PFN_vk##entrypoint)g_gdpa(dev, "vk" #entrypoint);\
	if (fp##entrypoint == NULL)\
		printf("vkGetDeviceProcAddr failed to find vk" #entrypoint, "vkGetDeviceProcAddr Failure");\
}
	GET_DEVICE_PROC_ADDR(dev, CreateSwapchainKHR);
	GET_DEVICE_PROC_ADDR(dev, DestroySwapchainKHR);
	GET_DEVICE_PROC_ADDR(dev, GetSwapchainImagesKHR);
	GET_DEVICE_PROC_ADDR(dev, AcquireNextImageKHR);
	GET_DEVICE_PROC_ADDR(dev, QueuePresentKHR);
	vkGetDeviceQueue(dev, 0, 0, &queue);

	/*
	char *dmabuf;
	char *dmabuf_mod;
	uint32_t n_ext;
	vkEnumerateDeviceExtensionProperties(pdev, NULL, &n_ext, NULL);
	VkExtensionProperties *ext_p = malloc(n_ext*sizeof(*ext_p));
	vkEnumerateDeviceExtensionProperties(pdev, NULL, &n_ext, ext_p);
	qsort(ext_p, n_ext, sizeof(VkExtensionProperties), compare_ext_names);
	
	*dmabuf = check_ext(n_ext, ext_p, VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME);
	*dmabuf &= check_ext(n_ext, ext_p, VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME);
	*dmabuf &= check_ext(n_ext, ext_p, VK_EXT_EXTERNAL_MEMORY_DMA_BUF_EXTENSION_NAME);
	*dmabuf &= check_ext(n_ext, ext_p, VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME);
	*dmabuf &= check_ext(n_ext, ext_p, VK_KHR_EXTERNAL_FENCE_FD_EXTENSION_NAME);
	*dmabuf_mod = *dmabuf;
	*dmabuf_mod &= check_ext(n_ext, ext_p, VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);
	*dmabuf_mod &= check_ext(n_ext, ext_p, VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME);
	*dmabuf_mod &= check_ext(n_ext, ext_p, VK_KHR_MAINTENANCE1_EXTENSION_NAME);
	*dmabuf_mod &= check_ext(n_ext, ext_p, VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
	*dmabuf_mod &= check_ext(n_ext, ext_p, VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME);
	*dmabuf_mod &= check_ext(n_ext, ext_p, VK_EXT_IMAGE_DRM_FORMAT_MODIFIER_EXTENSION_NAME);
	*/
	/*
	PFN_vkGetMemoryHostPointerPropertiesEXT vkGetMemoryHostPointerProperties = 0;
	PFN_vkGetFenceFdKHR vkGetFenceFd = 0;
	vkGetFenceFd = (PFN_vkGetFenceFdKHR) vkGetDeviceProcAddr(dev,
	"vkGetFenceFdKHR");
	vkGetDeviceQueue(dev, 0, 0, &queue);
*/
	short swapchainImageCount;
	VkSwapchain swapchain;
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = NULL,
		.flags = 0,
		.surface = surface,
		.minImageCount = surfaceCapabilities.minImageCount,
		.imageFormat = VK_FORMAT_B8G8R8A8_UNORM,
		.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.imageExtent = {400, 400},
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
		.preTransform = surfaceCapabilities.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE
	};

	if(vkCreateSwapchain(device, &swapchainCreateInfo, NULL, &swapchain) !=0) {
		
	}

	/*
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

	VkRenderPass renderPass;
	r = vkCreateRenderPass(dev, &renderPassCreateInfo, NULL, &renderPass);
	if (r) {
		fprintf(stderr, "[ERROR] vkCreateRenderPass: %d", r);
		exit(EXIT_FAILURE);
	}

	VkShaderModule vertModule, fragModule;
	vulkan_shader("vert.spv", dev, &vertModule);
	vulkan_shader("frag.spv", dev, &fragModule);

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
	VkPipelineLayout pipe_layout;
	VkResult r = vkCreatePipelineLayout(dev, &layoutCreateInfo, NULL,
		&pipe_layout);
	if (r) {
		fprintf(stderr, "[ERROR] vkCreatePipelineLayout: %d", r);
		exit(EXIT_FAILURE);
	}

	VkImageSubresourceRange imageSubresourceRange = {
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1
	};
	VkImage image;
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
	VkImageView imageView;
	r = vkCreateImageView(dev, &imageViewCreateInfo, NULL, imageView);
	if (r) {
		fprintf(stderr, "[ERROR] vkCreateImageView: %d", r);
		exit(EXIT_FAILURE);
	}

	VkFramebufferCreateInfo framebufferCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.renderPass = renderPass,
		.attachmentCount = 1,
		.pAttachments = &imageView,
		.width = 400,
		.height = 400,
		.layers = 1
	};
	r = vkCreateFramebuffer(dev, &framebufferCreateInfo, NULL, framebuffer);
	if (r) {
		fprintf(stderr, "[ERROR] framebufferCreateInfo: %d", r);
		exit(EXIT_FAILURE);
	}

	uint32_t imageCount = 0;
	vkGetSwapchainImagesKHR(dev, swapchain, &imageCount, NULL);
	VkImage *images = malloc(imageCount*sizeof(VkImage)); //TODO: free
	vkGetSwapchainImagesKHR(dev, swapchain, &imageCount, images);

	VkImageView *imageViews = malloc(imageCount*sizeof(VkImageView)); //TODO: free
	VkFramebuffer *framebuffers = malloc(imageCount*sizeof(VkFramebuffer)); //TODO: free
	for (int i=0; i<imageCount; i++) {
		createImageView(dev, images[i], imageViews+i);
		createFramebuffer(dev, renderPass, imageViews[i], framebuffers+i);
	}

	VkSurfaceKHR surface;

	VkBool32 supported;
	vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, 0, surface, &supported);
	if (supported != VK_TRUE)
		return 1;


	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

	VkSwapchainCreateInfoKHR swapchainCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = NULL,
		.flags = 0,
		.surface = surface,
		.minImageCount = surfaceCapabilities.minImageCount,
		.imageFormat = VK_FORMAT_B8G8R8A8_UNORM,
		.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
		.imageExtent = {400, 400},
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
		.preTransform = surfaceCapabilities.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_FIFO_KHR,
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE
	};
	if (r = vkCreateSwapchainKHR(dev, &swapchainCreateInfo, NULL, swapchain)) {
		fprintf(stderr, "ERROR: create_swapchain() failed.\n");
		return r;
	}

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
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
		.layout = pipe_layout,
		.renderPass = renderPass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};

	VkPipeline pipeline;
	r = vkCreateGraphicsPipelines(dev, VK_NULL_HANDLE, 1,
		&pipelineCreateInfo, NULL, &pipeline);
	if (r) {
		fprintf(stderr, "[ERROR] vkCreateGraphicsPipelines: %d", r);
		exit(EXIT_FAILURE);
	}

	vkDestroyShaderModule(dev, vertModule, NULL);
	vkDestroyShaderModule(dev, fragModule, NULL);
	


	VkCommandPoolCreateInfo cpinfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
	};

	VkCommandPool pool;
	if (vkCreateCommandPool(dev, &cpinfo, NULL, &pool)) {
		fprintf(stderr, "ERROR: create_command_pool() failed.\n");
		return VK_NULL_HANDLE;
	}

	const VkCommandBufferAllocateInfo cmdainfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};

	VkCommandBuffer cmd;  // Buffer for initialization commands

	if((r = vkAllocateCommandBuffers(dev, &cmdainfo, &cmd))) return r;
	
	VkCommandBufferBeginInfo cmd_buf_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = 0,
		.pInheritanceInfo = NULL,
	};

	if((r = vkBeginCommandBuffer(cmd, &cmd_buf_info))){
		printf("vkBeginCommandBuffer() failed\n");
	}

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
	
	vkCmdBeginRenderPass(cmd, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, &offset);
		uint32_t vertexCount=4;
		vkCmdDraw(cmd, vertexCount, 1, 0, 0);
	vkCmdEndRenderPass(cmd);
	r = vkEndCommandBuffer(cmd);
	if (r) {
		fprintf(stderr, "[ERROR] vkEndCommandBuffer: %d", r);
		exit(EXIT_FAILURE);
	}

	
	vkDestroyCommandPool(dev,pool,NULL);
	*/
	vkDeviceWaitIdle(dev);
	vkDestroyDevice(dev,NULL);
	vkDestroyInstance(instance,NULL);
	return 0;
}
