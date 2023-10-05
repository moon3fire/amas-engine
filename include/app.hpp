#pragma once

#include "amas_game_object.hpp"
#include "amas_device.hpp"
#include "amas_descriptors.hpp"
#include "amas_renderer.hpp"
#include "amas_window.hpp"

// std
#include <memory>
#include <vector>

namespace amas {

	class App {
	public:
		static constexpr int WIDTH = 1200;
		static constexpr int HEIGHT = 800;

		App();
		~App();

		App(const App&) = delete;
		App& operator=(const App&) = delete;

		void run();
		void initDescriptorPool(int materialObjectsCount, int texturesCount);
		void initDescriptorLayouts();
		std::vector<std::unique_ptr<AmasBuffer>> initUboBuffers(
			int vecSize,
			int uboSize,
			int count,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryFlags,
			VkDeviceSize minOffsetAlignment = 1);
		void initDescriptorSets(std::vector<VkDescriptorSet>& globalSets, std::vector<std::unique_ptr<AmasBuffer>>& globalBuffers,
								std::vector<VkDescriptorSet>& componentSets, std::vector<std::unique_ptr<AmasBuffer>>& componentBuffers);


		int getMaterialHavingObjectsCount() const;

	private:
		void loadGameObjects();

		AmasWindow amasWindow{ WIDTH, HEIGHT, "Amas engine v1.0.0" };
		AmasDevice amasDevice{ amasWindow };
		AmasRenderer AmasRenderer{ amasWindow, amasDevice };

		std::vector<std::shared_ptr<AmasTexture>> textures;
		std::unique_ptr<AmasDescriptorPool> globalPool{};
		std::vector<std::unique_ptr<AmasDescriptorSetLayout>> descriptorSetLayouts;
		AmasGameObject::Map gameObjects;
	};
}  // namespace amas
