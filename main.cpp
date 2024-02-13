#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string.h>

const uint16_t window_width = 512;
const uint16_t window_height = 512;
const std::string app_title = "Triangle App";

class TriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        loop();
        clean();
    }
private:
    GLFWwindow* window;
    VkInstance vk_instance;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(window_width, window_height, app_title.c_str(), nullptr, nullptr);
    }
    void createVkInstance() {
        VkApplicationInfo app_info {};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = app_title.c_str();
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "NaN";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_0;
        

        VkInstanceCreateInfo instance_create_info {};
        instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pApplicationInfo = &app_info;

        // keeps track of the supported and required extension counts
        uint32_t glfw_supported_extension_count = 0;
        uint32_t glfw_required_extension_count = 0;

        // add all supported extensions into a vector
        vkEnumerateInstanceExtensionProperties(nullptr, &glfw_supported_extension_count, nullptr);
        std::vector<VkExtensionProperties> glfw_extensions(glfw_supported_extension_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &glfw_supported_extension_count, glfw_extensions.data());

        // get required extensions
        const char** glfw_required_extensions = glfwGetRequiredInstanceExtensions(&glfw_required_extension_count);


        // bad code... but it works
        int validated_count = 0;
        for (int i = 0; i < glfw_extensions.size(); i++) {
            for (int j = 0; j < glfw_required_extension_count; j++) {
                if (strcmp(glfw_extensions.at(i).extensionName, glfw_required_extensions[j]) == 0) validated_count++;
            }
        }


        // validate required extensions
        if (validated_count < glfw_required_extension_count) {
            throw std::runtime_error("gpu or gpu driver doesn't support required vulkan extensions");
        }

        instance_create_info.enabledExtensionCount = glfw_required_extension_count;
        instance_create_info.ppEnabledExtensionNames = glfw_required_extensions;

        VkResult result = vkCreateInstance(&instance_create_info, nullptr, &vk_instance);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan instance");
        }
    }
    void initVulkan() {
        createVkInstance();
    }
    void loop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }
    void clean() {
        vkDestroyInstance(vk_instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }
};

int main(int, char**){
    TriangleApplication app;
    
    try {
        app.run();
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
