#pragma once

#include "amas_model.hpp"
#include "amas_descriptors.hpp"
#include "amas_texture.hpp"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>

namespace amas {
	struct ComponentUbo {
		int currentGameObjectID;
	};

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{};

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();

		glm::mat3 normalMatrix();
	};

	struct PointLightComponent {
		float lightIntensity = 1.0f;
	};

	struct MaterialComponent {
		std::shared_ptr<AmasTexture> AmasTexture;
		VkDescriptorImageInfo info{};
	};

	class AmasGameObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, AmasGameObject>;

		static AmasGameObject createGameObject() {
			static id_t currentId = 0;
			return AmasGameObject{ currentId++ };
		}

		void attachMaterial(std::shared_ptr<AmasTexture> AmasTexture);
		static void setDevice(AmasDevice& device_);
		static AmasGameObject makePointLight(float intensity = 10.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

		AmasGameObject(const AmasGameObject&) = delete;
		AmasGameObject& operator=(const AmasGameObject&) = delete;
		AmasGameObject(AmasGameObject&&) = default;
		AmasGameObject& operator=(AmasGameObject&&) = default;

		id_t getId() { return id; }

		glm::vec3 color{};
		TransformComponent transform{};

		static AmasDevice* device;

		// Optional pointer components
		std::shared_ptr<AmasModel> model{};
		std::unique_ptr<PointLightComponent> pointLight = nullptr;
		std::unique_ptr<MaterialComponent> material = nullptr;

		//descriptors stuff
		//static VkDescriptorSetLayout& currentSetLayout;

		ComponentUbo gameObjectUBO{};

	private:

		AmasGameObject(id_t objId) : id{ objId } {
			gameObjectUBO.currentGameObjectID = objId;
		}

		id_t id;
	};
}  // namespace amas
