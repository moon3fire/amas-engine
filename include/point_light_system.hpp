#pragma once

#include "amas_camera.hpp"
#include "amas_device.hpp"
#include "amas_game_object.hpp"
#include "amas_pipeline.hpp"
#include "amas_frame_info.hpp"

// std
#include <memory>
#include <vector>
#include <chrono>

namespace amas {
	class PointLightSystem {
	public:
		PointLightSystem(AmasDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo& ubo);
		void render(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		AmasDevice& amasDevice;

		std::unique_ptr<AmasPipeline> amasPipeline;
		VkPipelineLayout pipelineLayout;

		//my variables
		std::chrono::steady_clock::time_point startTime;
		bool firstTime{ true };
	};
}  // namespace amas
