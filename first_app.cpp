#include "first_app.hpp"

#include <iostream>

namespace lve {
	void FirstApp::run()
	{
		while (!lveWindow.shouldClose())
		{
			try {
				glfwPollEvents();
			}
			catch (const std::exception &e)
			{
				std::cout << e.what();
			}
		}
	}
}