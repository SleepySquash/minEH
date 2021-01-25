//
//  Renderer.hpp
//  OpenGL App
//
//  Created by Никита Исаенко on 15.01.2021.
//

#pragma once

#include "../Support/Config.hpp"

#ifdef MINEH_OPENGL
    #include <OpenGL/OpenGL.h>
    #include <OpenGL/gl3.h>
#endif

#ifdef MINEH_VULKAN
    #include <vulkan/vulkan.h>
    #include <optional>
#endif

#include "../Graphics/Camera.hpp"
#include "../Window/Window.hpp"
#include "Vertex.hpp"

namespace mh
{
    struct Renderer
    {
        enum class Type { undef, GL, Vk, MTL, WGL, DX, DX12 } type;
    };
    
    struct CameraBufferObject { glm::mat4 view, proj; };
    
#ifdef MINEH_OPENGL
    namespace GL
    {
        struct Buffer { GLuint VAO = 0, VBO = 0, EBO = 0; };
        struct Image { unsigned int width, height; };
        struct Texture
        {
            Image image;
            GLuint id;
        };
        
        struct Context : Renderer
        {
            GLuint shaderID = 0, VAOID = 0, VBOID = 0, EBOID = 0, textureID = 0;
            Window* window = nullptr;
            Camera* camera = nullptr;
            
            Context();
            Context(Window* window);
            ~Context();
            
            void init();
            void create(Window* window);
            void setContext();
            void resize();
            void destroy();
            void wait();
            
            uint32_t beginDraw(float r = 0.f, float g = 0.f, float b = 0.f, float a = 1.f);
            void endDraw(const uint32_t& i = 0);
            
            void beginRecord(const uint32_t&);
            void endRecord(const uint32_t&);
            
            Texture loadTexture(const std::string& path, const GLenum& filter = GL_LINEAR);
            
    #if defined(MINEH_WIN32)
            HWND hwnd;
            HDC hdc;
            HGLRC hglrc;
    #elif defined(MINEH_MACOS) || defined(MINEH_IOS)
            void* view;
    #endif
        };
        
        void unsetContext(const Context& context);
        void destroyContext(const Context& context);
        bool getCurrentContext(const Context& context);
    }
#endif

    namespace exceptions
    {
        struct VK_ERROR_OUT_OF_DATE_KHR : std::exception { virtual const char* what() const throw(); };
    }
    
#ifdef MINEH_VULKAN
    namespace Vk
    {
        struct Allocation
        {
            VkDeviceMemory memory = VK_NULL_HANDLE;
            void* mapped = nullptr;
            uint32_t offset = 0;
        };

        struct Buffer
        {
            VkBuffer buffer = VK_NULL_HANDLE;
            uint32_t offset = 0;
            Allocation memory;
        };

        struct Image
        {
            uint32_t width, height;
            VkImage image = VK_NULL_HANDLE;
            VkImageView view = VK_NULL_HANDLE;
            Allocation memory;
        };

        struct Texture
        {
            Image image;
            VkSampler sampler = VK_NULL_HANDLE;
            uint32_t mip = 1;
        };

        struct Descriptor
        {
            VkDescriptorSetLayout layout = VK_NULL_HANDLE;
            VkDescriptorPool pool = VK_NULL_HANDLE;
            std::vector<VkDescriptorSet> sets = { VK_NULL_HANDLE };
        };

        template<typename T> struct Model
        {
            std::vector<Vertex<T>> vertices;
            std::vector<uint32_t> indices;
            T position{ 0.f }, rotation{ 0.f }, scale{ 1.f };
        };

        template<typename T> struct UBO
        {
            uint32_t dirty = 0;
            std::vector<Buffer> buffers;
            T model;
        };

        struct SwapChainProperties
        {
            VkExtent2D extent;
            float aspect;
            VkSurfaceFormatKHR format;
            uint32_t images;
        };
        
        struct Context : Renderer
        {
            int MAX_FRAMES_IN_FLIGHT = 3;
            std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
            bool enableValidationLayers = false;
            
            uint32_t vertexID, indexID, pipelineID, descriptorID;
            
            VkInstance instance = VK_NULL_HANDLE;
            VkSurfaceKHR surface = VK_NULL_HANDLE;
            VkPhysicalDevice GPU = VK_NULL_HANDLE;
            std::optional<uint32_t> graphicsQueueIndex, presentQueueIndex;
            VkQueue graphicsQueue = VK_NULL_HANDLE, presentQueue = VK_NULL_HANDLE;
            VkDevice device = VK_NULL_HANDLE;
            
            Window* window = nullptr;
            Camera* camera = nullptr;
            WindowSize lastSize;
            
            bool anisotropyEnable = false;
            VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
            
            VkCommandPool commandPool = VK_NULL_HANDLE, shortPool = VK_NULL_HANDLE;
            std::vector<VkCommandBuffer> commandBuffers = { VK_NULL_HANDLE };
            
            VkFormat depthFormat = VK_FORMAT_UNDEFINED;
            Image depthImage, colorImage;
            
            VkSwapchainKHR swapchain = VK_NULL_HANDLE;
            SwapChainProperties swapChainProps;
            std::vector<VkImage> swapchainImages = { VK_NULL_HANDLE };
            std::vector<VkImageView> swapchainImageViews = { VK_NULL_HANDLE };
            VkRenderPass renderPass = VK_NULL_HANDLE;
            std::vector<VkFramebuffer> framebuffer = { VK_NULL_HANDLE };
            
            std::vector<VkSemaphore> renderFinishedSemaphore = { VK_NULL_HANDLE }, imageAvailableSemaphore = { VK_NULL_HANDLE };
            std::vector<VkFence> inFlightFences = { VK_NULL_HANDLE }, imagesInFlight = { VK_NULL_HANDLE };
            size_t currentFrame = 0;
            
            uint32_t _imageIndex;
            
            
            Context();
            Context(Window* window);
            
            void setContext();
            void create(Window* window);
            bool resize();
            void destroy();
            void wait();
            
            void createInstance();
            void createSurface(Window* window);
            void pickPhysicalDevice();
            void createDevice();
            
#ifdef MINEH_DEBUG
            VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
            bool checkValidationLayerSupport();
            static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
            void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
            void setupDebugMessenger();
            VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
            void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
            
            VkSurfaceFormatKHR chooseSwapSurfaceFormat();
            VkExtent2D chooseSwapExtent2D(VkSurfaceCapabilitiesKHR& capabilities, uint32_t windowWidth, uint32_t windowHeight); // glfwGetFramebufferSize(window, &w, &h);
            VkPresentModeKHR chooseSwapPresentMode();
#endif
            
            void createSwapchain();
            void createColorImage();
            void createDepthImage();
            void createRenderPass();
            void createFramebuffers();
            
            void createSemaphores();
            
            VkShaderModule loadShader(const std::string& path);
            
            std::pair<VkPipeline, VkPipelineLayout> generateDefaultPipeline(VkShaderModule& vertexShader, VkShaderModule& fragmentShader, std::vector<VkVertexInputAttributeDescription>& vAttributeDescription, VkVertexInputBindingDescription& vBindingDescription, Descriptor* descriptor, bool depthEnabled, const VkPolygonMode& polygonMode, const VkCullModeFlags& cullMode, const VkFrontFace& frontFace);
            void generateDefaultVertexAndIndexBuffers(const VkDeviceSize &bufferSizeV, Buffer &vertexBuffer, const void* vertexData, const VkDeviceSize &bufferSizeI, Buffer &indexBuffer, const void* indexData);
            void generateSingleBuffer(const VkDeviceSize& bufferSize, Buffer& buffer, const void* vectorData, VkBufferUsageFlags flags);
            Texture generateTexture(const std::string& textureName, uint32_t maxMipLevels = 0, const VkFilter& samplerFilter = VK_FILTER_LINEAR);
            
            void createCommandPool();
            void createCommandBuffer();
            VkCommandBuffer beginSingleTimeCommands();
            void endSingleTimeCommands(VkCommandBuffer& commandBuffer);
            
            void beginAllRecords();
            void beginRecord(const uint32_t& i);
            void endRecord(const uint32_t& i);
            void endAllRecords();
            
            void freeTexture(Texture& texture);
            void freeImage(Image& image);
            void freeBuffer(Buffer& buffer);
            void freeDescriptor(Descriptor& descriptor);
            
            uint32_t findMemoryType(const uint32_t& typeFilter, const VkMemoryPropertyFlags& properties);
            void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
            void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
            
            void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits samples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
            VkImageView createImageView(VkImage& image, VkFormat format, VkImageAspectFlags aspect, uint32_t mipLevels);
            void transitionImageLayout(VkImage image, uint32_t mipLevels, VkImageLayout oldLayout, VkImageLayout newLayout);
            void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
            void generateMipmaps(VkImage image, VkFormat format, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
            
            VkSampleCountFlagBits getMaxUsableSampleCount();
            VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
            VkFormat findDepthFormat();
            
            uint32_t beginDraw();
            void endDraw(const uint32_t& imageIndex);
        };
        
        bool isDeviceSuitable(const VkPhysicalDevice& d, const VkSurfaceKHR& surface);
        int ratePhysicalDevice(const VkPhysicalDevice& d);
        
        std::pair<std::optional<uint32_t>, std::optional<uint32_t>> findQueueFamilies(const VkPhysicalDevice& d, const VkSurfaceKHR& surface);
        VkSampleCountFlagBits getMaxUsableSampleCount();
        std::vector<const char*> getRequiredExtensions(const bool& getRequiredExtensions);
        std::vector<char> loadFileInBuffer(const std::string& filename);
        void loadModel(const std::string& MODEL_NAME, std::vector<Vertex<glm::vec3>>& vertices, std::vector<uint32_t>& indices);
        
        bool hasStencilComponent(VkFormat format);
    }
#endif

    namespace MTL
    {
        
    }

    namespace WGL
    {
        
    }

    namespace DX
    {
        
    }

    namespace DX12
    {
        
    }

}
