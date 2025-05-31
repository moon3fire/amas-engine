#include "../include/app.hpp"
#include "../include/amas_buffer.hpp"
#include "../include/keyboard_movement_controller.hpp"
#include "../include/amas_camera.hpp"
#include "../include/simple_render_system.hpp"
#include "../include/point_light_system.hpp"
#include "../include/amas_texture.hpp"

// libs
#include "../externals/include/stb_image.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


// std
#include <array>
#include <cassert>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <numeric>
#include <iostream>

namespace amas {

	App::App() {
		loadGameObjects();
		initDescriptorPool(getMaterialHavingObjectsCount(), textures.size());
	}

	App::~App() {}

	int App::getMaterialHavingObjectsCount() const {
		int count = 0;

		for (int i = 0; i < gameObjects.size(); i++) {
			if (gameObjects.at(i).material == nullptr || gameObjects.at(i).pointLight != nullptr) continue;
			++count;
		}
		return count;
	}

	void App::initDescriptorPool(int materialObjectsCount, int texturesCount) {
		globalPool = AmasDescriptorPool::Builder(amasDevice)
			.setMaxSets(100)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, AmasSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AmasSwapChain::MAX_FRAMES_IN_FLIGHT * texturesCount)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, materialObjectsCount)
			.build();
	}

	void App::initDescriptorLayouts() {
		
		auto builder1 = AmasDescriptorSetLayout::Builder(amasDevice);
		builder1.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);

		//lets guess for now that we have at least 1 material but this works fine i think
		for (int i = 0; i < textures.size(); i++) {
			builder1.addBinding(i + 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		}

		auto globalSetLayout = builder1.build();
		descriptorSetLayouts.push_back(std::move(globalSetLayout));

		auto builder2 = AmasDescriptorSetLayout::Builder(amasDevice);

		builder2.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
		auto componentSetLayout = builder2.build();

		descriptorSetLayouts.push_back(std::move(componentSetLayout));
	}

	std::vector<std::unique_ptr<AmasBuffer>> App::initUboBuffers(
		int vecSize,
		int uboSize,
		int count,
		VkBufferUsageFlags usageFlags,
		VkMemoryPropertyFlags memoryFlags,
		VkDeviceSize minOffsetAlignment) {

		std::vector<std::unique_ptr<AmasBuffer>> buffers(vecSize);

		for (int i = 0; i < buffers.size(); i++) {
			buffers[i] = std::make_unique<AmasBuffer>(
				amasDevice,
				uboSize,
				count,
				usageFlags,
				memoryFlags);
			buffers[i]->map();
		}
		return buffers;
	}

	void App::initDescriptorSets(std::vector<VkDescriptorSet>& globalSets, std::vector<std::unique_ptr<AmasBuffer>>& globalBuffers,
								 std::vector<VkDescriptorSet>& componentSets, std::vector<std::unique_ptr<AmasBuffer>>& componentBuffers) {
		AmasDescriptorWriter writer1(*descriptorSetLayouts[0], *globalPool);
		for (int i = 0; i < globalSets.size(); i++) {
			auto bufferInfo = globalBuffers[i]->descriptorInfo();
			writer1.writeBuffer(0, &bufferInfo);
			for (int j = 0; j < getMaterialHavingObjectsCount(); j++) {
				//need to implement a predicate for map 
				writer1.writeImage(j + 1, &gameObjects.at(j).material->info);
			}
			writer1.build(globalSets[i]);
		}

		AmasDescriptorWriter writer2(*descriptorSetLayouts[1], *globalPool);

		for (int i = 0; i < componentSets.size(); i++) {
			auto bufferInfo = componentBuffers[i]->descriptorInfo();
			writer2.writeBuffer(0, &bufferInfo);
			writer2.build(componentSets[i]);
		}
	}


	void App::run() {
		initDescriptorLayouts();
		std::vector<std::unique_ptr<AmasBuffer>> uboBuffers = initUboBuffers(
			AmasSwapChain::MAX_FRAMES_IN_FLIGHT,
			sizeof(GlobalUbo),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		std::vector<std::unique_ptr<AmasBuffer>> componentUboBuffers = initUboBuffers(
			getMaterialHavingObjectsCount(),
			sizeof(ComponentUbo),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

		std::cout << getMaterialHavingObjectsCount() << std::endl;

		for (int i = 0; i < componentUboBuffers.size(); i++) {
			componentUboBuffers[i]->writeToBuffer(&gameObjects.at(i).gameObjectUBO);
			componentUboBuffers[i]->flush();
		}

		std::vector<VkDescriptorSet> globalDescriptorSets(AmasSwapChain::MAX_FRAMES_IN_FLIGHT);
		std::vector<VkDescriptorSet> componentSets(getMaterialHavingObjectsCount());

		initDescriptorSets(globalDescriptorSets, uboBuffers, componentSets, componentUboBuffers);



		SimpleRenderSystem simpleRenderSystem{ amasDevice, AmasRenderer.getSwapChainRenderPass(), descriptorSetLayouts };
		PointLightSystem pointLightSystem{ amasDevice, AmasRenderer.getSwapChainRenderPass(), descriptorSetLayouts[0]->getDescriptorSetLayout()};
		AmasCamera camera{};

		auto viewerObject = AmasGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!amasWindow.shouldClose()) {
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime =
				std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveInPlaneXZ(amasWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = AmasRenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10000.f);

			if (auto commandBuffer = AmasRenderer.beginFrame()) {
				int frameIndex = AmasRenderer.getFrameIndex();
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
				AmasRenderer.beginSwapChainRenderPass(commandBuffer);

				simpleRenderSystem.renderGameObjects(frameInfo, componentSets, componentUboBuffers);
				pointLightSystem.render(frameInfo);

				AmasRenderer.endSwapChainRenderPass(commandBuffer);
				AmasRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(amasDevice.device());
	}

	void App::loadGameObjects() {
		AmasGameObject::setDevice(amasDevice);

		std::shared_ptr<AmasTexture> texture1 = std::make_shared<AmasTexture>(amasDevice, "objs/lain.jpg");
		textures.push_back(texture1);

		std::shared_ptr<AmasModel> cubeModel =
			AmasModel::createModelFromFile(amasDevice, "objs/Cube.obj");

		auto cube1 = AmasGameObject::createGameObject();
		cube1.model = cubeModel;
		cube1.transform.translation = { -.5f, .5f, 0 };
		cube1.transform.scale = { 1.f, 1.f, 1.f };
		cube1.attachMaterial(texture1);
		gameObjects.emplace(cube1.getId(), std::move(cube1));

		//point lights creation
		{
			auto pointLight1 = AmasGameObject::makePointLight(2.f);
			pointLight1.color = glm::vec3(.4f, .7f, .3f);
			pointLight1.pointLight->lightIntensity = .3f;
			pointLight1.transform.translation += glm::vec3(4.f, -.7f, -.3f);
			gameObjects.emplace(pointLight1.getId(), std::move(pointLight1));
			auto pointLight2 = AmasGameObject::makePointLight(1.f);
			pointLight2.pointLight->lightIntensity = 1.5f;
			pointLight2.color = glm::vec3(.5f, .2f, .8f);
			pointLight2.transform.translation += glm::vec3(-1.2f, -1.3f, .5f);
			gameObjects.emplace(pointLight2.getId(), std::move(pointLight2));
			auto pointLight3 = AmasGameObject::makePointLight(.5f);
			pointLight3.pointLight->lightIntensity = .5f;
			pointLight3.color = glm::vec3(.2f, .5f, .5f);
			pointLight3.transform.translation += glm::vec3(1.8f, -1.7f, -.7f);
			gameObjects.emplace(pointLight3.getId(), std::move(pointLight3));
		}

		std::cout << "GameObjects overall count:  " << gameObjects.size() << "\n";
	}

}  // namespace amas
