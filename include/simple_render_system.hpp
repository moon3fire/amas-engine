#pragma once

#include "amas_camera.hpp"
#include "amas_device.hpp"
#include "amas_game_object.hpp"
#include "amas_pipeline.hpp"
#include "amas_frame_info.hpp"

// std
#include <memory>
#include <vector>

namespace amas {
	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(AmasDevice& device, VkRenderPass renderPass, const std::vector<std::unique_ptr<AmasDescriptorSetLayout>>& layouts);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(FrameInfo& frameInfo, std::vector<VkDescriptorSet>& componentSets, std::vector<std::unique_ptr<AmasBuffer>>& componentUboBuffers);

	private:
		void createPipelineLayout(const std::vector<std::unique_ptr<AmasDescriptorSetLayout>>& layouts);
		void createPipeline(VkRenderPass& renderPass);

		AmasDevice& amasDevice;

		std::unique_ptr<AmasPipeline> amasPipeline;
		VkPipelineLayout pipelineLayout;
	};
}  // namespace amas
