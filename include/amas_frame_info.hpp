#pragma once

#include "amas_camera.hpp"
#include "amas_game_object.hpp"

// lib
#include <vulkan/vulkan.h>

namespace amas {

	constexpr int MAX_LIGHTS = 1000;

	struct PointLight {
		glm::vec4 position{}; // ignore w
		glm::vec4 color{}; // w is intensity
	};

	struct GlobalUbo {
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 inverseView{ 1.f };
		glm::vec4 LightColor{ 1.f, 1.f, 1.f, .02f };
		PointLight pointLights[MAX_LIGHTS];
		int activeLightsCount;
	};

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		AmasCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		AmasGameObject::Map& gameObjects;
	};

} // namespace amas