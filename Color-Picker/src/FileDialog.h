
#pragma once

#ifdef PLATFORM_WINDOWS

#include <windows.h>
#include <commdlg.h>
#include "GLFW/glfw3.h"
#define	GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include <string>

std::string OpenFileDialog(const char* filters, GLFWwindow* window) {
	OPENFILENAMEA ofn;
	CHAR szFile[260] = { 0 };
	CHAR currentDir[256] = { 0 };
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = glfwGetWin32Window(window);
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	if (GetCurrentDirectoryA(256, currentDir))
		ofn.lpstrInitialDir = currentDir;
	ofn.lpstrFilter = filters;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileNameA(&ofn) == TRUE)
		return ofn.lpstrFile;

	return std::string();

}

#endif // PLATFORM_WINDOWS
