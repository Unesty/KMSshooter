#include "vulkan/vulkan.h"
void createImageView(VkDevice device, VkImage image, VkImageView *imageView);
void createFramebuffer(VkDevice device, VkRenderPass renderPass, VkImageView
imageView, VkFramebuffer *framebuffer, unsigned w, unsigned h);
void vulkan_framebuffers(VkDevice device, VkSwapchainKHR swapchain, VkRenderPass
renderPass, uint32_t *framebufferCount_, VkFramebuffer **framebuffers_, unsigned w, unsigned h);
int vulkan_swapchain(VkPhysicalDevice physicalDevice, VkDevice device,
VkSurfaceKHR surface, VkSwapchainKHR *swapchain, int w, int h);
void destroy_vulkan_framebuffers(VkDevice device, VkSwapchainKHR swapchain, VkRenderPass
renderPass, uint32_t *framebufferCount_, VkFramebuffer **framebuffers_);
void destroy_vulkan_image_views(VkDevice device, VkSwapchainKHR swapchain, VkRenderPass
renderPass, uint32_t *framebufferCount_, VkFramebuffer **framebuffers_);