
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <Windows.h>
#include <vulkan/vulkan_core.h>

#define APP_ASSERT(cond, ...) if (!(cond)) { fprintf(stderr, __VA_ARGS__); exit(1); }
#define APP_VK_ASSERT(expr) APP_ASSERT((vk_result = (expr)) >= VK_SUCCESS, "vulkan error '%s' returned from: %s", app_vk_result_string(vk_result), #expr)
#define APP_ARRAY_COUNT(array) (sizeof(array) / sizeof(*(array)))
#define for_range(idx, start, end) for (uintptr_t idx = start; idx < end; idx += 1)

const char* app_vk_result_string(VkResult result) {
	switch (result) {
	case VK_SUCCESS: return "VK_SUCCESS";
	case VK_NOT_READY: return "VK_NOT_READY";
	case VK_TIMEOUT: return "VK_TIMEOUT";
	case VK_EVENT_SET: return "VK_EVENT_SET";
	case VK_EVENT_RESET: return "VK_EVENT_RESET";
	case VK_INCOMPLETE: return "VK_INCOMPLETE";
	case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
	case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
	case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
	case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
	case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
	case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
	case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
	case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
	case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
	case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
	case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
	case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
	case VK_ERROR_UNKNOWN: return "VK_ERROR_UNKNOWN";
	case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
	case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
	case VK_ERROR_FRAGMENTATION: return "VK_ERROR_FRAGMENTATION";
	case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
	case VK_PIPELINE_COMPILE_REQUIRED: return "VK_PIPELINE_COMPILE_REQUIRED";
	case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
	case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
	case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
	case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
	case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
	case VK_ERROR_NOT_PERMITTED_KHR: return "VK_ERROR_NOT_PERMITTED_KHR";
	case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
	case VK_THREAD_IDLE_KHR: return "VK_THREAD_IDLE_KHR";
	case VK_THREAD_DONE_KHR: return "VK_THREAD_DONE_KHR";
	case VK_OPERATION_DEFERRED_KHR: return "VK_OPERATION_DEFERRED_KHR";
	case VK_OPERATION_NOT_DEFERRED_KHR: return "VK_OPERATION_NOT_DEFERRED_KHR";
	case VK_ERROR_COMPRESSION_EXHAUSTED_EXT: return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
	case VK_RESULT_MAX_ENUM: return "VK_RESULT_MAX_ENUM";
	default: return "??????";
	}
}

bool platform_file_read_all(const char* path, void** data_out, uintptr_t* size_out) {
	FILE* f;
	if (fopen_s(&f, path, "rb")) {
		return false;
	}

	fseek(f, 0, SEEK_END);
	long file_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	void* data = malloc(file_size);
	long read_size = fread(data, 1, file_size, f);
	if (read_size != file_size) {
		return false;
	}

	*data_out = data;
	*size_out = file_size;
	return true;
}

int main(int argc, char** argv) {
	VkResult vk_result;
	VkInstance vk_instance;
	VkDevice vk_device;
	VkPhysicalDevice vk_physical_device;
	VkShaderModule vk_shader_module;
	VkQueue vk_queue;
	uint32_t vk_queue_family_idx;

	//
	// create instance
	//
	{
		static const char* layers[] = {
			"VK_LAYER_LUNARG_api_dump",
			"VK_LAYER_KHRONOS_validation",
		};

		uint32_t layers_count = APP_ARRAY_COUNT(layers);

		static const char* extensions[] = {
			"VK_KHR_surface",
			"VK_KHR_win32_surface",
		};

		VkApplicationInfo app = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = NULL,
			.pApplicationName = "APP_NAME",
			.applicationVersion = 0,
			.pEngineName = "none",
			.engineVersion = 0,
			.apiVersion = VK_API_VERSION_1_3,
		};
		VkInstanceCreateInfo create_info = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = NULL,
			.pApplicationInfo = &app,
			.enabledLayerCount = layers_count,
			.ppEnabledLayerNames = layers,
			.enabledExtensionCount = APP_ARRAY_COUNT(extensions),
			.ppEnabledExtensionNames = extensions,
		};

		APP_VK_ASSERT(vkCreateInstance(&create_info, NULL, &vk_instance));
	}

	{
#define PHYSICAL_DEVICES_CAP 128
		uint32_t physical_devices_count = PHYSICAL_DEVICES_CAP;
		VkPhysicalDevice physical_devices[PHYSICAL_DEVICES_CAP];
		APP_VK_ASSERT(vkEnumeratePhysicalDevices(vk_instance, &physical_devices_count, physical_devices));

		vk_physical_device = physical_devices[0];
	}

	{
#define QUEUE_FAMILIES_CAP 128
		uint32_t queue_families_count = QUEUE_FAMILIES_CAP;
		VkQueueFamilyProperties queue_families[QUEUE_FAMILIES_CAP];
		vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &queue_families_count, queue_families);

		vk_queue_family_idx = UINT32_MAX;
		for_range(queue_family_idx, 0, queue_families_count) {
			if (queue_families[queue_family_idx].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) {
				vk_queue_family_idx = queue_family_idx;
				break;
			}
		}

		APP_ASSERT(vk_queue_family_idx != UINT32_MAX, "could not find graphics and compute queue");

		float queue_priorities[1] = { 0.0 };
		VkDeviceQueueCreateInfo queue = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext = NULL,
			.queueFamilyIndex = vk_queue_family_idx,
			.queueCount = 1,
			.pQueuePriorities = queue_priorities
		};

		VkPhysicalDeviceVulkan12Features features_1_1 = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
			.pNext = NULL,
		};
		VkPhysicalDeviceVulkan12Features features_1_2 = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
			.pNext = &features_1_1,
			.vulkanMemoryModel = VK_TRUE,
			.vulkanMemoryModelDeviceScope = VK_TRUE,
			.shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
			.shaderStorageBufferArrayNonUniformIndexing = VK_TRUE,
			.shaderStorageImageArrayNonUniformIndexing = VK_TRUE,
			.shaderStorageTexelBufferArrayNonUniformIndexing = VK_TRUE,
			.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE,
			.descriptorBindingStorageImageUpdateAfterBind = VK_TRUE,
			.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE,
			.descriptorBindingStorageTexelBufferUpdateAfterBind = VK_TRUE,
			.descriptorBindingPartiallyBound = VK_TRUE,
			.scalarBlockLayout = VK_TRUE,
		};
		VkPhysicalDeviceVulkan13Features features_1_3 = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
			.pNext = &features_1_2,
			.dynamicRendering = VK_TRUE,
			.synchronization2 = VK_TRUE,
			.shaderDemoteToHelperInvocation = VK_TRUE,
			.maintenance4 = VK_TRUE,
		};
		VkPhysicalDeviceFeatures2 features = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
			.pNext = &features_1_3,
		};

		static const char* extensions[] = {
			"VK_KHR_swapchain",
		};

		VkDeviceCreateInfo create_info = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = &features,
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &queue,
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = NULL,
			.enabledExtensionCount = APP_ARRAY_COUNT(extensions),
			.ppEnabledExtensionNames = extensions,
			.pEnabledFeatures = NULL,
		};

		APP_VK_ASSERT(vkCreateDevice(vk_physical_device, &create_info, NULL, &vk_device));

		vkGetDeviceQueue(vk_device, vk_queue_family_idx, 0, &vk_queue);
	}

	{
		void* code;
		uintptr_t code_size;
		APP_ASSERT(platform_file_read_all("shader-works.spirv", &code, &code_size), "failed to read shader file from disk: shader-works.spirv");

		VkShaderModuleCreateInfo create_info = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.pNext = NULL,
			.codeSize = code_size,
			.pCode = code,
			.flags = 0,
		};

		APP_VK_ASSERT(vkCreateShaderModule(vk_device, &create_info, NULL, &vk_shader_module));
	}

	{
		void* code;
		uintptr_t code_size;
		APP_ASSERT(platform_file_read_all("shader-broken.spirv", &code, &code_size), "failed to read shader file from disk: shader-broken.spirv");

		VkShaderModuleCreateInfo create_info = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.pNext = NULL,
			.codeSize = code_size,
			.pCode = code,
			.flags = 0,
		};

		APP_VK_ASSERT(vkCreateShaderModule(vk_device, &create_info, NULL, &vk_shader_module));
	}

	printf("Successfully ran without any crashes\n");
}
