#pragma once

#include "amas_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace amas {

	class AmasDescriptorSetLayout {
	public:
		class Builder {
		public:
			Builder(AmasDevice& amasDevice) : amasDevice{ amasDevice } {}

			Builder& addBinding(
				uint32_t binding,
				VkDescriptorType descriptorType,
				VkShaderStageFlags stageFlags,
				uint32_t count = 1);
			std::unique_ptr<AmasDescriptorSetLayout> build() const;

		private:
			AmasDevice& amasDevice;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
		};

		AmasDescriptorSetLayout(
			AmasDevice& amasDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
		~AmasDescriptorSetLayout();
		AmasDescriptorSetLayout(const AmasDescriptorSetLayout&) = delete;
		AmasDescriptorSetLayout& operator=(const AmasDescriptorSetLayout&) = delete;

		VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

	private:
		AmasDevice& amasDevice;
		VkDescriptorSetLayout descriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

		friend class AmasDescriptorWriter;
	};

	class AmasDescriptorPool {
	public:
		class Builder {
		public:
			Builder(AmasDevice& amasDevice) : amasDevice{ amasDevice } {}

			Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
			Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& setMaxSets(uint32_t count);
			std::unique_ptr<AmasDescriptorPool> build() const;

		private:
			AmasDevice& amasDevice;
			std::vector<VkDescriptorPoolSize> poolSizes{};
			uint32_t maxSets = 1000;
			VkDescriptorPoolCreateFlags poolFlags = 0;
		};

		AmasDescriptorPool(
			AmasDevice& amasDevice,
			uint32_t maxSets,
			VkDescriptorPoolCreateFlags poolFlags,
			const std::vector<VkDescriptorPoolSize>& poolSizes);
		~AmasDescriptorPool();
		AmasDescriptorPool(const AmasDescriptorPool&) = delete;
		AmasDescriptorPool& operator=(const AmasDescriptorPool&) = delete;

		bool allocateDescriptor(
			const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

		void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

		void resetPool();

	private:
		AmasDevice& amasDevice;
		VkDescriptorPool descriptorPool;

		friend class AmasDescriptorWriter;
	};

	class AmasDescriptorWriter {
	public:
		AmasDescriptorWriter(AmasDescriptorSetLayout& setLayout, AmasDescriptorPool& pool);

		AmasDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		AmasDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

		bool build(VkDescriptorSet& set);
		void overwrite(VkDescriptorSet& set);

	private:
		AmasDescriptorSetLayout& setLayout;
		AmasDescriptorPool& pool;
		std::vector<VkWriteDescriptorSet> writes;
	};
}