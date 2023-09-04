#include "../include/first_app.hpp"

#include "../include/keyboard_movement_controller.hpp"
#include "../include/lve_camera.hpp"
#include "../include/simple_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>

namespace lve {

	FirstApp::FirstApp() { loadGameObjects(); }

	FirstApp::~FirstApp() {}

	void FirstApp::run() {
		SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.getSwapChainRenderPass() };
		LveCamera camera{};

		auto viewerObject = LveGameObject::createGameObject();
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!lveWindow.shouldClose()) {
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime =
				std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = lveRenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

			if (auto commandBuffer = lveRenderer.beginFrame()) {
				lveRenderer.beginSwapChainRenderPass(commandBuffer);

				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);

				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(lveDevice.device());
	}

	void FirstApp::loadGameObjects() {
		std::shared_ptr<LveModel> lveModel =
			LveModel::createModelFromFile(lveDevice, "C:/Users/arman/source/repos/VulkanFirstProj/VulkanFirstProj/3d/flat_vase.obj");
		auto flatVase = LveGameObject::createGameObject();
		flatVase.model = lveModel;
		flatVase.transform.translation = { -.5f, .5f, 2.5f };
		flatVase.transform.scale = { 3.f, 1.5f, 3.f };
		gameObjects.push_back(std::move(flatVase));
		
		lveModel = LveModel::createModelFromFile(lveDevice, "C:/Users/arman/source/repos/VulkanFirstProj/VulkanFirstProj/3d/smooth_vase.obj");
		auto smoothVase = LveGameObject::createGameObject();
		smoothVase.model = lveModel;
		smoothVase.transform.translation = { .5f, .5f, 2.5f };
		smoothVase.transform.scale = { 3.f, 1.5f, 3.f };
		gameObjects.push_back(std::move(smoothVase));
	}

}  // namespace lve
