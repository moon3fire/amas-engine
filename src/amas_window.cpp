#include "../include/amas_window.hpp"

// std
#include <stdexcept>

namespace amas {

	AmasWindow::AmasWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
		initWindow();
	}

	AmasWindow::~AmasWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void AmasWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void AmasWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to craete window surface");
		}
	}

	void AmasWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto amasWindow = reinterpret_cast<AmasWindow*>(glfwGetWindowUserPointer(window));
		amasWindow->framebufferResized = true;
		amasWindow->width = width;
		amasWindow->height = height;
	}

}  // namespace amas
