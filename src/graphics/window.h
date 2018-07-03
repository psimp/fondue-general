#pragma once

#ifdef FONDUE-WEB
    #define GLFW_INCLUDE_ES3
#else
    #include<glad/glad.h>
#endif

#include <glfw/glfw3.h>
#include <iostream>
#include <cstring>

namespace Fondue { namespace graphics {

#define NUM_KEYS 1024
#define NUM_BUTTONS 1024

	class Window
	{
	private:

		const char *mName;
		int mWidth, mHeight;
		bool mClosed;

        double mx = 0, my = 0;
        double lastMouseX = 0, lastMouseY = 0;
        bool firstMouseMovement = true;

		bool isCursorHidden = true;
		bool mFirstMouse = true;
		bool mKeys[NUM_KEYS];
		bool mPrevKeyState[NUM_KEYS];
		bool mKeyTyped[NUM_KEYS];
		bool mMouseButtons[NUM_BUTTONS];
		bool mPrevButtonState[NUM_BUTTONS];
		bool mButtonClicked[NUM_BUTTONS];

	public:

        GLFWwindow* window;

		Window(const char *name, int width, int height);
		~Window();
		void update();
		void clear() const;
		bool closed() const;

		int getWidth() const { return mWidth; }
		int getHeight() const { return mHeight; }

		bool isKeyPressed(unsigned int key) const;
		bool isKeyTyped(unsigned int key) const;
		bool isMouseButtonPressed(unsigned int button) const;
		bool isMouseButtonClicked(unsigned int button) const;
        std::pair<float, float> getMousePosition() const;
        std::pair<float, float> getMouseMovement();

		void captureCursor();
		void freeCursor();
		void toggleCursor();

        static void setGlState();

	private:

		bool init();
        friend void window_resize(GLFWwindow* window, int width, int height);
		friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		friend void mouse_callback(GLFWwindow* window, int button, int action, int mods);
		friend void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos);
        friend void error_callback(int error, const char* description);

	};

}}
