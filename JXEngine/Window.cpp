//#include "Window.h"
//#include <iostream>
//
//Window::Window(int WIDTH, int HEIGHT)
//{
//	window = std::make_unique<GLFWwindow>(glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL));
//
//	TestAndSet();
//}
//
//Window::Window(int WIDTH, int HEIGHT, std::string Name)
//{
//	window = std::make_unique<GLFWwindow>(glfwCreateWindow(WIDTH, HEIGHT, Name.c_str(), NULL, NULL));
//
//	TestAndSet();
//}
//
//GLFWwindow* Window::Self() const
//{
//	return window.get();
//}
//
//void Window::TestAndSet() const
//{
//	if (window.get() == NULL)
//	{
//		std::cout << "Failed to create GLFW window" << std::endl;
//		glfwTerminate();
//		return;
//	}
//	glfwMakeContextCurrent(window.get());
//	glfwSetFramebufferSizeCallback(window.get(), framebuffer_size_callback);
//}
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
//	// make sure the viewport matches the new window dimensions; note that width and 
//	// height will be significantly larger than specified on retina displays.
//	glViewport(0, 0, width, height);
//}
