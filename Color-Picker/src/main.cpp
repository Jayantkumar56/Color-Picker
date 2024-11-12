
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "ImguiUi.h"
#include "FontManager.h"
#include "Renderer.h"
#include "FileDialog.h"

#include <iostream>
#include <filesystem>

// give mouse pos relative to the current imgui window from which called
static ImVec2 GetRelativeMousePos() {
	ImVec2 globalPos = ImGui::GetMousePos();
	ImVec2 windowPos = ImGui::GetWindowPos();
	return {globalPos.x - windowPos.x, globalPos.y - windowPos.y};
}

static void RunApp() {
	if (glfwInit() == GLFW_FALSE) {
		std::cout << "Could not Initialized GLFW!";
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Set major version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5); // Set minor version
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1200, 800, "Color Picker", nullptr, nullptr);
	bool running = true;

	glfwMakeContextCurrent(window);
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cout << "Failed to load glad!";
	}

	// turning vsync on
	glfwSwapInterval(1);

	ImguiUi::InitImgui(window);
	FontManager::LoadFonts();

	Renderer::InitRenderer();

	// file path of the image to load
	std::string imagePath = "image path.......";
	imagePath.resize(512);

	// picked Color
	glm::vec4 pickedColor(0.0f);

	// width and height of the image to be shown
	int imageWidth = 0, imageHeight = 0;

	while (running) {
		glfwPollEvents();
		if (glfwWindowShouldClose(window)) {
			running = false;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImguiUi::Begin();
		ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoResize | ImGuiDockNodeFlags_HiddenTabBar);

		ImGui::AlignTextToFramePadding();
		ImGui::PushFont(FontManager::GetFont(FontManager::FontWeight::SemiBold, 22));
		ImGui::Text("Open an Image: ");
		ImGui::PopFont();

		ImGui::SameLine(0.0f, 15.0f);
		ImGui::PushFont(FontManager::GetFont(FontManager::FontWeight::Regular, 21));
		ImGui::InputText("##ImagePath", (char*)imagePath.c_str(), imagePath.length());
		ImGui::PopFont();

		ImGui::SameLine(0.0f, 15.0f);
		if (ImGui::Button("Open")) {
			std::string filePath = OpenFileDialog(
				"jpg\0*.jpg\0png\0*.png\0",
				window
			);

			if (filePath != "" && filePath.size() <= imagePath.size())
				strcpy((char*)imagePath.c_str(), (char*)filePath.c_str());
		}

		ImGui::AlignTextToFramePadding();
		ImGui::PushFont(FontManager::GetFont(FontManager::FontWeight::SemiBold, 22));
		ImGui::Text("Color: ");
		ImGui::PopFont();

		ImGui::SameLine(0.0f, 15.0f);
		ImGui::PushFont(FontManager::GetFont(FontManager::FontWeight::Regular, 21));
		ImGui::ColorEdit4("##color", glm::value_ptr(pickedColor));
		ImGui::PopFont();

		ImVec2 imageSize = ImGui::GetContentRegionAvail();
		if (imageSize.x > 0 || imageSize.y < 0) {
			imageWidth = (int)imageSize.x;
			imageHeight = (int)imageSize.y;
		}

		// rendering the image for picking color
		int imageId = Renderer::RenderImage(imageWidth, imageHeight, imagePath);
		if (imageId != -1) {
			ImVec2 imagePos = ImGui::GetCursorPos();

			bool clickedOnImage = ImGui::ImageButton(
				(ImTextureID)(intptr_t)imageId,
				ImVec2((float)imageWidth, (float)imageHeight),
				{ 0, 1 },
				{ 1, 0 },
				0
			);

			if (clickedOnImage) {
				ImVec2 mousePos = GetRelativeMousePos();

				// mouse position on the image button
				mousePos = { mousePos.x - imagePos.x, mousePos.y - imagePos.y };
				// inverting the y axis for mouse coords
				mousePos.y = imageHeight - mousePos.y;

				pickedColor = Renderer::ReadPixel((int)mousePos.x, (int)mousePos.y);
			}
		}

		ImGui::End();
		ImguiUi::End();

		glfwSwapBuffers(window);
	}

	Renderer::TerminateRenderer();
	ImguiUi::Terminate();
	glfwDestroyWindow(window);
}


#ifdef PLATFORM_WINDOWS

// to run app without console window in windows ( works in dist build only )
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	RunApp();
	return 0;
}

#endif // PLATFORM_WINDOWS

int main() {
	RunApp();
	return 0;
}
