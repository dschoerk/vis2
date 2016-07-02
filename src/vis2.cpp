
#include "Vis2Application.h"
#include <iostream>

static void opengl_error_callback(int error, const char* description)
{
  fputs(description, stdout);
}

int main()
{
  if (!glfwInit())
  {
    std::cout << "GLFW Initialization failed" << std::endl;
    exit(EXIT_FAILURE);
  }

  glfwSetErrorCallback(opengl_error_callback);

  Vis2Application vis2app(800, 600, 3, 3);
  vis2app.run();

  return 0;
}
