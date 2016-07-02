#pragma once

#include "GLFWApplication.h"
#include "GLProgram.h"
#include "Quad.h"

#include <glm/glm.hpp>

// Ant forward declaration
struct CTwBar;
typedef CTwBar TwBar;

class Vis2Application : public GLFWApplication
{
public:
  Vis2Application(int width, int height, int ctx_major, int ctx_minor);
  
  void init() override;
  void update() override;
  void render() override;
  void resize(int width, int height) override;

  void initUI() override;
  void renderUI() override;

private:
  TwBar* bar;

  GL::GLProgram splatDepthProg;
  GL::GLProgram splatAttribProg;
  GL::GLProgram splatShadingProg;

  size_t numPoints;

  GLuint vbPointBuffer;
  GL::Quad quad;

  struct {
	float splatSizeFactor = 0.0005f;
	float filterRadius = 2.0f;
	bool useLenght = true;
  } settings;

  struct {
	GLuint depth;
	GLuint accumWeight; // accumulated weights for normalization
	GLuint accumNormal;
	GLuint accumColor;
  } tex;

  GLuint depthFBO;
  GLuint fb;
  GLuint va;


  glm::mat4 projection;
  glm::mat4 view;
};