

#pragma once

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <string>
#include <utility>

namespace Renderer {

	struct Image {
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t ImageId = 0;
	};

	struct QuadVertex {
		glm::vec3 Position;
		glm::vec2 TextureCoords;
	};

	Image LoadImage(const std::string& filePath);
	
	// explicitly use this to free the image data
	void FreeImage(Image& image);

	// must be called before using any other function from Renderer
	void InitRenderer();

	// must call this after the end of renderer use
	void TerminateRenderer();

	// returns the id (in the gpu) of the drawn image
	int RenderImage(uint32_t imageWidth, uint32_t imageHeight, const std::string& filePath);

	glm::vec4 ReadPixel(int x, int y);

}
