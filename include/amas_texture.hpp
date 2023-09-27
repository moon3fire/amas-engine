#pragma once
#include <vulkan/vulkan_core.h>
#include "amas_device.hpp"
#include <string>

namespace amas {
	class AmasTexture {
	public:
		AmasTexture(AmasDevice& device, const std::string& filepath);
		AmasTexture(const AmasTexture&) = delete;
		AmasTexture& operator=(const AmasTexture&) = delete;
		AmasTexture(AmasTexture&&) = delete;
		AmasTexture& operator=(AmasTexture&&) = delete;

		~AmasTexture();

		VkSampler getSampler() { return sampler; }
		VkImageView getImageView() { return imageView; }
		VkImageLayout getImageLayout() { return imageLayout; }

	private:
		void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
		void generateMinmaps();

		int width, height, mipLevels;
		AmasDevice& amasDevice;
		VkImage image;
		VkDeviceMemory imageMemory;
		VkImageView imageView;
		VkSampler sampler;
		VkFormat imageFormat;
		VkImageLayout imageLayout;

	};

} // namespace amas