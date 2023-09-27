#pragma once

#include "amas_device.hpp"
#include "amas_swap_chain.hpp"
#include "amas_window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace amas {
	class AmasRenderer {
	public:
		AmasRenderer(AmasWindow& window, AmasDevice& device);
		~AmasRenderer();

		AmasRenderer(const AmasRenderer&) = delete;
		AmasRenderer& operator=(const AmasRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return amasSwapChain->getRenderPass(); }
		float getAspectRatio() const { return amasSwapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		AmasWindow& amasWindow;
		AmasDevice& amasDevice;
		std::unique_ptr<AmasSwapChain> amasSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };
	};
}  // namespace amas
