#ifdef USE_SWA
int vulkan_instance(VkInstance *instance, struct swa_display *dpy) {
#else
int vulkan_instance(VkInstance *instance) {
#endif
	int r;

#ifdef USE_GLFW
	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	const char **extensions = malloc((glfwExtensionCount+1)*sizeof(char*));
	for (int i=0; i<glfwExtensionCount; i++) {
		extensions[i] = glfwExtensions[i];
	}
	extensions[glfwExtensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
#elif defined USE_SWA
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
#else

	uint32_t glfwExtensionCount = 2;
	const char **extensions = malloc((glfwExtensionCount+1)*sizeof(char*));
	extensions[0] = VK_KHR_SURFACE_EXTENSION_NAME;
	extensions[1] =	"VK_KHR_wayland_surface";
	extensions[2] =	VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
#endif

	VkApplicationInfo applicationInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "physics",
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

	//if (r = vkCreateInstance(&instanceCreateInfo, NULL, instance))
	//	return r;
	VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, NULL, instance));

#ifndef USE_SWA
	free(extensions);
#endif
	return 0;
}
