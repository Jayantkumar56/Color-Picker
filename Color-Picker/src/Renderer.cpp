
#include "glad/glad.h"
#include "stb_image.h"

#include "Renderer.h"

#include <iostream>
#include <fstream>
#include <filesystem>

namespace Renderer {

	// framebuffer properties
	static uint32_t targetWidth  = 0;
	static uint32_t targetHeight = 0;
	static uint32_t frameBuffer  = 0;
	static uint32_t frameColorBuffer  = 0;

	// image and projection of the image
	static Image	   image;
	static std::string imagePath;
	static glm::mat4   projectionMat;

	// renderer primitives for quad
	static uint32_t vertexBuffer = 0;
	static uint32_t indexBuffer  = 0;
	static uint32_t vertexArray  = 0;
	static uint32_t quadShader	 = 0;
	static QuadVertex vertexData[4];

	void CreateFrameBuffer(int width, int height, uint32_t& rendererId, uint32_t& frameColorBuffer) {
		glGenFramebuffers(1, &rendererId);
		glBindFramebuffer(GL_FRAMEBUFFER, rendererId);

		glCreateTextures(GL_TEXTURE_2D, 1, &frameColorBuffer);
		glBindTexture(GL_TEXTURE_2D, frameColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameColorBuffer, 0);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
			std::cout << "Uncomplete FrameBuffer!";

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void InvalidateFrameBuffers(uint32_t frameBuffer, uint32_t frameColorBuffer) {
		glDeleteTextures(1, &frameColorBuffer);
		glDeleteFramebuffers(1, &frameBuffer);
	}

	Image LoadImage(const std::string& filePath) {
		Image newImage = {};

		int width, height, channels;

		stbi_set_flip_vertically_on_load(1);
		unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
		newImage.Width = width;
		newImage.Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4) {
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3) {
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &newImage.ImageId);
		glTextureStorage2D(newImage.ImageId, 1, internalFormat, width, height);

		glTexParameteri(newImage.ImageId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(newImage.ImageId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTextureSubImage2D(newImage.ImageId, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);

		return newImage;
	}

	void FreeImage(Image& image) {
		glDeleteTextures(1, &image.ImageId);
	}

	int ShaderTypeFromString(const std::string& type) {
		if (type == "vertex" || type == "Vertex") {
			return 0;
		}

		if (type == "fragment" || type == "Fragment" || type == "pixel" || type == "Pixel") {
			return 1;
		}

		std::cout << "Invalid shader type specified";
		return -1;
	}

	unsigned int CompileShader(unsigned int glType, const char* source) {
		unsigned int shader = glCreateShader(glType);

		glShaderSource(shader, 1, &source, 0);
		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(shader);

			std::cout << infoLog.data();
			std::cout << "Vertex shader compilation failure!";
			return 0;
		}

		return shader;
	}

	uint32_t CreateShader(const std::string shaderSourcesArray[], uint8_t size) {
		GLuint shader[2] = { 0 };
		shader[0] = CompileShader(GL_VERTEX_SHADER, shaderSourcesArray[0].c_str());
		shader[1] = CompileShader(GL_FRAGMENT_SHADER, shaderSourcesArray[1].c_str());

		GLuint program = glCreateProgram();

		// Attach our shaders to our program
		for (uint8_t i = 0; i < size; ++i) {
			if (shader[i] != 0)
				glAttachShader(program, shader[i]);
		}
		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			for (uint8_t i = 0; i < size; ++i) {
				if (shader[i] != 0)
					glDeleteShader(shader[i]);
			}

			std::cout << infoLog.data();
			std::cout << "Shader link failure!";
			return -1;
		}

		// Always detach shaders after a successful link.
		for (uint8_t i = 0; i < size; ++i) {
			if (shader[i] != 0)
				glDetachShader(program, shader[i]);
		}

		return program;
	}

	uint32_t LoadShader(const std::string& filePath) {
		std::string source;
		std::ifstream in(filePath, std::ios::in | std::ios::binary);

		if (in) {
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();

			if (size != -1) {
				source.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&source[0], size);
			}
			else {
				std::cout << "Could not read from file " << filePath;
			}
		}
		else {
			std::cout << "Could not read from file " << filePath;
		}

		// array of all types of sahders present in the source file
		std::string shaderSources[2];

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken);

		while (pos != std::string::npos) {
			size_t begin = source.find_first_not_of(" \t\n\r", pos + typeTokenLength + 1);
			if(begin == std::string::npos) std::cout << "Syntax error in the shader!";

			size_t end = source.find_first_of(" \t\n\r", begin);
			if(end == std::string::npos) std::cout << "Empty shader source provided!";

			std::string type = source.substr(begin, end - begin);
			uint8_t shaderType = ShaderTypeFromString(type);

			size_t shaderStart = source.find_first_of("#version", end);
			if(shaderStart == std::string::npos) std::cout << "Syntax error";

			pos = source.find(typeToken, shaderStart);
			shaderSources[shaderType] = (pos == std::string::npos) ? source.substr(shaderStart) : source.substr(shaderStart, pos - shaderStart);
		}

		return CreateShader(shaderSources, 2);
	}

	void InitRenderer() {
		image	  = {};
		imagePath = "Image Path";
		imagePath.resize(512);

		glCreateVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		int size   = 4 * sizeof(QuadVertex);
		int offset = 0;

		glCreateBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

		// position (glm::vec3)
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (const void*)(uint64_t)offset);
		offset += sizeof(glm::vec3);

		// TextureCoords (glm::vec2)
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (const void*)(uint64_t)offset);

		uint32_t quadIndices[] = {
			0,1,2,
			2,3,0
		};

		glCreateBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), quadIndices, GL_STATIC_DRAW);

		quadShader = LoadShader("assets/Shaders/Quad.glsl");

		// required to be done only once
		vertexData[0].TextureCoords = { 0.0f, 0.0f };
		vertexData[1].TextureCoords = { 1.0f, 0.0f };
		vertexData[2].TextureCoords = { 1.0f, 1.0f };
		vertexData[3].TextureCoords = { 0.0f, 1.0f };

		// created temporary frameBuffer
		CreateFrameBuffer(1200, 800, frameBuffer, frameColorBuffer);
	}

	void TerminateRenderer() {	
		FreeImage(image);
		InvalidateFrameBuffers(frameBuffer, frameColorBuffer);

		glDeleteProgram(quadShader);
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &indexBuffer);
		glDeleteVertexArrays(1, &vertexArray);
	}

	int RenderImage(uint32_t width, uint32_t height, const std::string& filePath) {
		if (!std::filesystem::exists(filePath)) {
			return -1;
		}

		// if the file path is same and there is no change in width and height of the target
		// no need to render the image again
		if (filePath == imagePath && (targetWidth == width && targetHeight == height)) {
			return frameColorBuffer;
		}

		imagePath = filePath;

		if (targetWidth != width || targetHeight != height) {
			targetWidth  = width;
			targetHeight = height;

			// deleting previous buffer and creating a new one
			InvalidateFrameBuffers(frameBuffer, frameColorBuffer);
			CreateFrameBuffer(targetWidth, targetHeight, frameBuffer, frameColorBuffer);
			glViewport(0, 0, targetWidth, targetHeight);
		}

		float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		projectionMat	  = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);

		// free the last image
		FreeImage(image);
		image = LoadImage(filePath);

		float widthBegin, widthEnd;
		float heightBegin, heightEnd;

		// decide whether to draw image with max width or max height
		if (image.Width > image.Height) {
			widthBegin  = -aspectRatio;
			widthEnd    = aspectRatio;
			heightBegin = -1.0f;			// TO DO: correct the calculation according to the width		
			heightEnd   = 1.0f;				// TO DO: correct the calculation according to the width
		}
		else {
			widthBegin  = -aspectRatio;	// TO DO: correct the calculation according to the height		
			widthEnd    = aspectRatio;	// TO DO: correct the calculation according to the height		
			heightBegin = -1.0f;
			heightEnd   = 1.0f;
		}

		vertexData[0].Position = { widthBegin, heightBegin, 0.0f };
		vertexData[1].Position = { widthEnd,   heightBegin, 0.0f };
		vertexData[2].Position = { widthEnd,   heightEnd,   0.0f };
		vertexData[3].Position = { widthBegin, heightEnd,   0.0f };

		glBindVertexArray(vertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(QuadVertex), vertexData);

		// slot for image texture
		int texSlot = 0;

		glUseProgram(quadShader);
		glUniformMatrix4fv(glGetUniformLocation(quadShader, "u_ViewProjection"), 1, GL_FALSE, glm::value_ptr(projectionMat));
		glUniform1iv(glGetUniformLocation(quadShader, "u_ImageTexSlot"), 1, &texSlot);	

		glBindTextureUnit(texSlot, image.ImageId);

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// return the color buffer id of the framebuffer
		return frameColorBuffer;
	}

	glm::vec4 ReadPixel(int x, int y) {
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		GLubyte pixels[4];
		glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return { (float)pixels[0] / 255, (float)pixels[1] / 255, (float)pixels[2] / 255, (float)pixels[3] / 255 };
	}

}
