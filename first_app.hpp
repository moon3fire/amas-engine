#pragma once

#include "lve_window.hpp"
#include "lve_pipeline.hpp"

namespace lve {
	class FirstApp {
	public:
		void run();
	private:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan from Arman :)"};
		LvePipeline lvePipeline{ "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv" };
	
	};
}