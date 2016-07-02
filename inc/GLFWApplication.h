#pragma once

#include <GL/glew.h>
#include <glfw/glfw3.h>

class GLFWApplication
{
public:
  GLFWApplication(int width, int height, int ctx_major, int ctx_minor);
  
  virtual void init() = 0;
  virtual void update() = 0;
  virtual void render() = 0; // render 3d content
  virtual void resize(int width, int height) = 0;

  virtual void initUI() = 0;
  virtual void renderUI() = 0; // render user interface

  void run();

protected:
  GLFWwindow* getWindow(){ return window; }
  int getWidth();
  int getHeight();
  float getFrameTime(){ return frameTime; }
  float getAbsTime(){ return absTime; }
  double getMouseX(){ return mousex; }
  double getMouseY(){ return mousey; }
  double getMouseDx(){ return dmousex; }
  double getMouseDy(){ return dmousey; }


private:
  float frameTime;
  float absTime;
  GLFWwindow* window;

  // glfw callbacks
  double mousex = 0, mousey = 0;
  double dmousex = 0, dmousey = 0;
};