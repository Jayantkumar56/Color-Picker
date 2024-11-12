

#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace ImguiUi {

	// must be called before writing any imgui code
	void InitImgui(GLFWwindow* window);

	// must be called before context is destroyed
	void Terminate();

	// call before creating an imgui window
	void Begin();

	// must call after imgui window code
	void End();

}
