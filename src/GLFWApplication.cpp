#include "GLFWApplication.h"

#include <iostream>

GLFWApplication* globalAppRegister;
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  globalAppRegister->resize(width, height);
}

GLFWApplication::GLFWApplication(int width, int height, int ctx_major, int ctx_minor)
{
  globalAppRegister = this;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ctx_major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ctx_minor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
  //glfwWindowHint(GLFW_VISIBLE, true);

  window = glfwCreateWindow(width, height, "vis2", NULL, NULL);
  if (!window)
  {
	std::cout << "failed to create GLFW Window" << std::endl;
	glfwTerminate();
	exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);
  //glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);

//  glewExperimental = GL_TRUE; // why?
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
	fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	exit(EXIT_FAILURE);
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  //Bugfixes
  __glewGenSamplers = (PFNGLGENSAMPLERSPROC)glfwGetProcAddress("glGenSamplers");
  __glewSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)glfwGetProcAddress("glSamplerParameteri");
  __glewGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
  __glewBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
  __glewDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glfwGetProcAddress("glDeleteVertexArrays");
  __glewGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)glfwGetProcAddress("glGenerateMipmap");
  __glewCreateShader = (PFNGLCREATESHADERPROC)glfwGetProcAddress("glCreateShader");
  __glewCreateProgram = (PFNGLCREATEPROGRAMPROC)glfwGetProcAddress("glCreateProgram");
  __glewBindSampler = (PFNGLBINDSAMPLERPROC)glfwGetProcAddress("glBindSampler");
  __glewGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffers");
  __glewBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");
  __glewFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glfwGetProcAddress("glFramebufferTexture2D");
  __glewCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glfwGetProcAddress("glCheckFramebufferStatus");
  __glewDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffers");
  __glewGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)glfwGetProcAddress("glGenRenderbuffers");
  __glewBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)glfwGetProcAddress("glBindRenderbuffer");
  __glewDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)glfwGetProcAddress("glDeleteRenderbuffers");
}

void GLFWApplication::run()
{
  init();
  initUI();

  while (!glfwWindowShouldClose(window))
  {
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  float curTime = (float)glfwGetTime();
	  frameTime = curTime - absTime;
	  absTime = curTime;

	  double tmpMouseX = mousex;
	  double tmpMouseY = mousey;
	  glfwGetCursorPos(getWindow(), &mousex, &mousey);
	  dmousex = mousex - tmpMouseX;
	  dmousey = mousey - tmpMouseY;

	  update();
	  render();
	  renderUI();

	  glfwSwapBuffers(window);
	  glfwPollEvents();
  }

}

int GLFWApplication::getWidth() 
{
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  return width;
}

int GLFWApplication::getHeight()
{
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  return height;
}