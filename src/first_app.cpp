#include "../include/first_app.hpp"
#include "../include/lve_buffer.hpp"
#include "../include/keyboard_movement_controller.hpp"
#include "../include/lve_camera.hpp"
#include "../include/simple_render_system.hpp"
#include "../include/point_light_system.hpp"

#include "../include/lve_texture.hpp"

#include "../externals/include/stb_image.h"

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
#include <numeric>
#include <iostream>

namespace lve {

	FirstApp::FirstApp() { 
		globalPool = LveDescriptorPool::Builder(lveDevice)
			.setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::run() {
		std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); ++i) {
			uboBuffers[i] = std::make_unique<LveBuffer>(
				lveDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = LveDescriptorSetLayout::Builder(lveDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		Texture texture = Texture(lveDevice, "C:/Users/arman/source/repos/VulkanFirstProj/VulkanFirstProj/3d/metal.png");
		VkDescriptorImageInfo imageInfo{};
		imageInfo.sampler = texture.getSampler();
		imageInfo.imageView = texture.getImageView();
		imageInfo.imageLayout = texture.getImageLayout();

		std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			LveDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.writeImage(1, &imageInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		PointLightSystem pointLightSystem{ lveDevice, lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		LveCamera camera{};

		auto viewerObject = LveGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
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
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10000.f);

			if (auto commandBuffer = lveRenderer.beginFrame()) {
				int frameIndex = lveRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects
				};

				//update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				pointLightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				//render
				lveRenderer.beginSwapChainRenderPass(commandBuffer);

				simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);

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
		flatVase.transform.translation = { -.5f, .5f, 0 };
		flatVase.transform.scale = { 3.f, 1.5f, 3.f };
		gameObjects.emplace(flatVase.getId(), std::move(flatVase));

		lveModel = LveModel::createModelFromFile(lveDevice, "C:/Users/arman/source/repos/VulkanFirstProj/VulkanFirstProj/3d/smooth_vase.obj");
		auto smoothVase = LveGameObject::createGameObject();
		smoothVase.model = lveModel;
		smoothVase.transform.translation = { .5f, .5f, 0 };
		smoothVase.transform.scale = { 3.f, 1.5f, 3.f };
		gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

		std::shared_ptr<LveModel> lveModelQuad = LveModel::createModelFromFile(lveDevice, "C:/Users/arman/source/repos/VulkanFirstProj/VulkanFirstProj/3d/quad.obj");
		auto quad = LveGameObject::createGameObject();
		quad.model = lveModelQuad;
		quad.transform.translation = { 0.f, .5f, 0 };
		quad.transform.scale = { 10.f, 1.f, 10.f };
		gameObjects.emplace(quad.getId(), std::move(quad));

		{
			auto pointLight1 = LveGameObject::makePointLight(.2f);
			pointLight1.color = glm::vec3(.4f, .7f, .3f);
			pointLight1.pointLight->lightIntensity = .3f;
			pointLight1.transform.translation += glm::vec3(4.f, -.7f, -.3f);
			gameObjects.emplace(pointLight1.getId(), std::move(pointLight1));
			auto pointLight2 = LveGameObject::makePointLight(1.f);
			pointLight2.pointLight->lightIntensity = 1.5f;
			pointLight2.color = glm::vec3(.5f, .2f, .8f);
			pointLight2.transform.translation += glm::vec3(-1.2f, -1.3f, .5f);
			gameObjects.emplace(pointLight2.getId(), std::move(pointLight2));
			auto pointLight3 = LveGameObject::makePointLight(.5f);
			pointLight3.pointLight->lightIntensity = .5f;
			pointLight3.color = glm::vec3(.2f, .5f, .5f);
			pointLight3.transform.translation += glm::vec3(1.8f, -1.7f, -.7f);
			gameObjects.emplace(pointLight3.getId(), std::move(pointLight3));
		}
		
		
		/*
		std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "C:/Users/arman/source/repos/VulkanFirstProj/VulkanFirstProj/3d/cube.obj");

		float step = 20.f;

		for (int x = 0; x < 14; ++x) {
			for (int y = 0; y < 14; ++y) {
				for (int z = 0; z < 14; ++z) {
					auto cube = LveGameObject::createGameObject();
					cube.model = lveModel;
					cube.transform.translation = { x * step, y * step, z * step };
					cube.transform.scale = { 1.f, 1.f, 1.f };
					gameObjects.push_back(std::move(cube));
				}
			}
		}
		*/

		std::cout << gameObjects.size() << "\n";
	}

}  // namespace lve
