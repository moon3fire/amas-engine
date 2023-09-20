#pragma once
#include <vulkan/vulkan_core.h>
#include "lve_device.hpp"
#include <string>

namespace lve {
	class Texture {
	public:
		Texture(LveDevice& device, const std::string &filepath);
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;
		Texture(Texture&&) = delete;
		Texture& operator=(Texture&&) = delete;

		~Texture();

		VkSampler getSampler() { return sampler; }
		VkImageView getImageView() { return imageView; }
		VkImageLayout getImageLayout() { return imageLayout; }

	private:
		void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
		void generateMinmaps();

		int width, height, mipLevels;
		LveDevice& lveDevice;
		VkImage image;
		VkDeviceMemory imageMemory;
		VkImageView imageView;
		VkSampler sampler;
		VkFormat imageFormat;
		VkImageLayout imageLayout;

	};

} // namespace lve