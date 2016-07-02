#include "Vis2Application.h"

#include <AntTweakBar.h>


inline void TwEventMouseButtonGLFW3(GLFWwindow* window, int button, int action, int mods){ TwEventMouseButtonGLFW(button, action); }
inline void TwEventMousePosGLFW3(GLFWwindow* window, double xpos, double ypos){ TwMouseMotion(int(xpos), int(ypos)); }
inline void TwEventMouseWheelGLFW3(GLFWwindow* window, double xoffset, double yoffset){ TwEventMouseWheelGLFW((int)yoffset); }
inline void TwEventKeyGLFW3(GLFWwindow* window, int key, int scancode, int action, int mods){ TwEventKeyGLFW(key, action); }
inline void TwEventCharGLFW3(GLFWwindow* window, int codepoint){ TwEventCharGLFW(codepoint, GLFW_PRESS); }
inline void TwEventWindowsizeGLFW3(GLFWwindow* window, int w, int h){ TwWindowSize(w, h); }

void Vis2Application::initUI()
{
  TwInit(TW_OPENGL_CORE, NULL);
  TwWindowSize(getWidth(), getHeight());
  bar = TwNewBar("Settings");

  TwDefine(" Settings size='240 420' ");
  TwDefine(" Settings refresh=.2 ");
  TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' ");

  TwAddVarRW(bar, "SplatSize", TW_TYPE_FLOAT, &settings.splatSizeFactor, " label='Splat Size Factor' min=0 max=5 step=0.05 keyIncr=s keyDecr=S help='Factor for Splatsize' ");
  TwAddVarRW(bar, "filterRadius", TW_TYPE_FLOAT, &settings.filterRadius, " label='Filter Radius' min=0 max=5 step=0.05 keyIncr=F keyDecr=f help='The size of the EWA-Filter' ");
  TwAddVarRW(bar, "useLenght", TW_TYPE_BOOL8, &settings.useLenght, " label='Use Lenght' ");

  glfwSetMouseButtonCallback(getWindow(), (GLFWmousebuttonfun)TwEventMouseButtonGLFW3);
  glfwSetCursorPosCallback(getWindow(), (GLFWcursorposfun)TwEventMousePosGLFW3);
  glfwSetScrollCallback(getWindow(), (GLFWscrollfun)TwEventMouseWheelGLFW3);
  glfwSetKeyCallback(getWindow(), (GLFWkeyfun)TwEventKeyGLFW3);
  glfwSetCharCallback(getWindow(), (GLFWcharfun)TwEventCharGLFW3);
  glfwSetWindowSizeCallback(getWindow(), TwEventWindowsizeGLFW3);
}

void Vis2Application::renderUI()
{
  TwDraw();
}