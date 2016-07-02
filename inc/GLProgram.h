#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <map>

#include "GLTexture.h"
#include "GLShader.h"

namespace GL
{
  class GLProgram
  {
  public:
	explicit GLProgram();
	void initialize();
	void attachShader(const GLShader& shader);
	void link();
	void bind();
	int getNumberOfAttributes();
	int getNumberOfUniforms();
	int getAttributeByName(char const* name);
	int getUniformByName(char const* name);
	void bindFragDataLocation(int loc, const char* variableName);
	  
	// uniform setters
	void setUniform(const char* name, float u);
	void setUniform(const char* name, int u);
	void setUniform(const char* name, const glm::vec2& u);
	void setUniform(const char* name, const glm::vec3& u);
	void setUniform(const char* name, const glm::vec4& u);
	void setUniform(const char* name, const glm::mat3& u);
	void setUniform(const char* name, const glm::mat4& u);
	void setUniform(const char* name, const std::vector<glm::mat4>& u);
	void setUniform(const char* name, const GLTexture& tex, int iTextureSlot);

  private:
	GLuint handle;
	std::map<std::string, GLuint> uniformCache;
  };
}