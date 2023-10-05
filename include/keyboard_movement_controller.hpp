#pragma once

#include "amas_game_object.hpp"
#include "amas_window.hpp"
#include "amas_camera.hpp"

namespace amas {
	class KeyboardMovementController {
	public:
		struct KeyMappings {
			int moveLeft = GLFW_KEY_A;
			int moveRight = GLFW_KEY_D;
			int moveForward = GLFW_KEY_W;
			int moveBackward = GLFW_KEY_S;
			int moveUp = GLFW_KEY_E;
			int moveDown = GLFW_KEY_Q;
			int lookLeft = GLFW_KEY_LEFT;
			int lookRight = GLFW_KEY_RIGHT;
			int lookUp = GLFW_KEY_UP;
			int lookDown = GLFW_KEY_DOWN;
			int speedUp = GLFW_KEY_LEFT_SHIFT;
		};

		void moveInPlaneXZ(GLFWwindow* window, float dt, AmasGameObject& gameObject);
		
		KeyMappings keys{};
		float moveSpeed{ 5.f };
		float lookSpeed{ 1.5f };
	};
}  // namespace amas