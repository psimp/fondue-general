#include "window.h"

namespace Fondue {	namespace graphics{	

	Window::Window(const char *name, int width, int height)
	{
		mName = name;
		mWidth = width;
		mHeight = height;
		if (!init())
			glfwTerminate();

		for (int i = 0; i < NUM_KEYS; i++)
		{
			mKeys[i] = false;
			mPrevKeyState[i] = false;
			mKeyTyped[i] = false;
		}

		for (int i = 0; i < NUM_BUTTONS; i++)
		{
			mMouseButtons[i] = false;
			mPrevButtonState[i] = false;
			mButtonClicked[i] = false;
		}
	}

	Window::~Window()
	{
        glfwDestroyWindow(window);

        glfwTerminate();
	}

    void window_resize(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
        Window* win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        win->mWidth = width;
        win->mHeight = height;
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Window* win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        win->mKeys[key] = action != GLFW_RELEASE;
    }

    void mouse_callback(GLFWwindow* window, int button, int action, int mods)
    {
        Window* win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        win->mMouseButtons[button] = action != GLFW_RELEASE;
    }

    void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos)
    {
        Window* win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

        win->mx = xpos;
        win->my = ypos;
    }

    void error_callback(int error, const char* description)
    {
        fprintf(stderr, "Error: %s\n", description);
    }

    void Window::setGlState()
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glDepthFunc(GL_LEQUAL);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glEnable(GL_MULTISAMPLE);
    }

	bool Window::init()
	{
		// Initialize GLFW
		if (!glfwInit())
		{ 
			std::cout << "GLFW Failed to Initialize..." << std::endl;
			return false;
		}

        glfwSetErrorCallback(error_callback);
        glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Initialize window
        window = glfwCreateWindow(mWidth, mHeight, mName, NULL, NULL);
        if (!window)
		{
			glfwTerminate();
			std::cout << "Failed to initialize GLFW window..." << std::endl;
			return false;
		}

        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, window_resize);
        glfwSetKeyCallback(window, key_callback);
        glfwSetMouseButtonCallback(window, mouse_callback);
        glfwSetCursorPosCallback(window, mouse_pos_callback);
        //glfwSwapInterval(0.0); // VSYNC OFF

        // Initialize GLAD
        if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        setGlState();

		std::cout << "Loaded OpenGL " << glGetString(GL_VERSION) << std::endl;

		return true;
	}

	void Window::update()
	{
		for (int i = 0; i < NUM_KEYS; i++)
			mKeyTyped[i] = mKeys[i] && !mPrevKeyState[i];

        std::memcpy(mPrevKeyState, mKeys, NUM_KEYS * sizeof(bool));

		for (int i = 0; i < NUM_BUTTONS; i++)
			mButtonClicked[i] = mMouseButtons[i] && !mPrevButtonState[i];

        std::memcpy(mPrevButtonState, mMouseButtons, NUM_BUTTONS * sizeof(bool));

        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
            std::cout << "OpenGL error:" << error << std::endl;

		glfwPollEvents();
        glfwSwapBuffers(window);
	}

	void Window::clear() const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	bool Window::closed() const
	{
        return glfwWindowShouldClose(window) == 1;
	}

	bool Window::isKeyPressed(unsigned int key) const
	{
		if (key >= NUM_KEYS)
			return false;
		return mKeys[key];
	}

	bool Window::isKeyTyped(unsigned int key) const
	{
		if (key >= NUM_KEYS)
			return false;
		return mKeyTyped[key];
	}

	bool Window::isMouseButtonPressed(unsigned int button) const
	{
		if (button >= NUM_BUTTONS)
			return false;
		return mMouseButtons[button];
	}

	bool Window::isMouseButtonClicked(unsigned int button) const
	{
		if (button >= NUM_KEYS)
			return false;
		return mButtonClicked[button];
	}

    std::pair<float, float> Window::getMousePosition() const
	{
        return std::make_pair(mx, my);
	}

    std::pair<float, float> Window::getMouseMovement()
    {
        if (firstMouseMovement && (mx != 0 || my != 0))
        {
            firstMouseMovement = false;
            lastMouseX = mx;
            lastMouseY = my;
        }

        auto out = std::make_pair(mx - lastMouseX, my - lastMouseY);

        lastMouseX = mx;
        lastMouseY = my;

        return out;
    }

	void Window::captureCursor()
	{
        lastMouseX = mx;
        lastMouseY = my;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		isCursorHidden = true;
	}

	void Window::freeCursor()
	{
        lastMouseX = mx;
        lastMouseY = my;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		isCursorHidden = false;
	}

	void Window::toggleCursor()
	{
		if (isCursorHidden)
			freeCursor();
		else captureCursor();
	}

}}
