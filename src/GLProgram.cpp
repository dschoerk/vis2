#include "GLProgram.h"

GL::GLProgram::GLProgram()
{
  
}

void GL::GLProgram::initialize()
{
  handle = glCreateProgram();
}

void GL::GLProgram::attachShader(GLShader const& shader)
{
  glAttachShader(handle, shader.handle);
}

void GL::GLProgram::link()
{
  
  glLinkProgram(handle);

  // Check the compilation status and report any errors
  GLint shaderStatus = GL_TRUE;
  glGetShaderiv(handle, GL_LINK_STATUS, &shaderStatus);

  // If the shader failed to compile, display the info log and quit out
  if (shaderStatus == GL_FALSE)
  {
	GLint infoLogLength = 0;
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::cout << "fail " << infoLogLength << std::endl;

	if (infoLogLength > 0)
	{
	  GLchar *strInfoLog = new GLchar[infoLogLength];
	  glGetShaderInfoLog(handle, infoLogLength, nullptr, strInfoLog);

	  std::cout << " shader linking failed: " << std::string(strInfoLog, infoLogLength) << std::endl;
	  delete[] strInfoLog;

	}
	else
	  std::cout << " shader linking failed: no message" << std::endl;

	std::cout << "Return to continue" << std::endl;
	std::cin.get();
  }
  else
  {
	std::cout << " shader linking OK" << std::endl;
  }


}

void GL::GLProgram::bind()
{
  glUseProgram(handle);
}

int GL::GLProgram::getNumberOfAttributes()
{
  GLint param;
  glGetProgramInterfaceiv(handle, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &param);
  return param;
}

int GL::GLProgram::getNumberOfUniforms()
{
  GLint param;
  glGetProgramInterfaceiv(handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &param);
  return param;
}

int GL::GLProgram::getAttributeByName(char const* name)
{
  return glGetProgramResourceIndex(handle, GL_PROGRAM_INPUT, name);
}

int GL::GLProgram::getUniformByName(char const* name)
{
  int loc;
  if (uniformCache.find(name) == uniformCache.end())
  {
	loc = glGetUniformLocation(handle, name);
	std::cout << "get Uniform " << name << std::endl;
	uniformCache[name] = loc;
  }
  else
  {
	loc = uniformCache[name];
  }

  if (loc == -1)
	std::cout << "Uniform " << name << " not found" << std::endl;

  return loc;
}

void GL::GLProgram::bindFragDataLocation(int loc, const char* variableName) {
	glBindFragDataLocation(handle, loc, variableName);
}

void GL::GLProgram::setUniform(char const* name, float u)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  glUniform1f(loc, u);
}

void GL::GLProgram::setUniform(char const* name, int u)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  glUniform1i(loc, u);
}

void GL::GLProgram::setUniform(const char* name, const glm::vec2& u)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  glUniform2f(loc, u[0], u[1]);
}

void GL::GLProgram::setUniform(const char* name, const glm::vec3& u)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  glUniform3f(loc, u[0], u[1], u[2]);
}

void GL::GLProgram::setUniform(const char* name, const glm::vec4& u)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  glUniform4f(loc, u.x, u.y, u.z, u.w);
}

void GL::GLProgram::setUniform(const char* name, const glm::mat3& u)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  glUniformMatrix3fv(loc, 1, GL_FALSE, &u[0][0]);
}

void GL::GLProgram::setUniform(const char* name, const glm::mat4& u)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  glUniformMatrix4fv(loc, 1, GL_FALSE, &u[0][0]);
}

void GL::GLProgram::setUniform(const char* name, const std::vector<glm::mat4>& u)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  glUniformMatrix4fv(loc, u.size(), GL_FALSE, &u[0][0][0]);
}

void GL::GLProgram::setUniform(const char* name, const GLTexture& tex, int iTextureSlot)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  tex.bind(iTextureSlot);
  glUniform1i(loc, iTextureSlot);
}

/*void GL::GLProgram::queryAttributes()
{
  const GLenum properties[3] = { GL_TYPE, GL_NAME_LENGTH, GL_LOCATION };
  const int n = getNumberOfAttributes();
  for (int attr = 0; attr < n; ++attr)
  {
	GLint values[3];
	glGetProgramResourceiv(handle, GL_PROGRAM_INPUT, attr, 3, properties, 3, nullptr, values);
	std::string nameData;
	nameData.resize(values[1]);
	glGetProgramResourceName(handle, GL_PROGRAM_INPUT, attr, nameData.size(), nullptr, &nameData[0]);
	std::string name(nameData.begin(), nameData.end() - 1);

	Attribute attribute;
	attribute.location = values[2];
	attribute.name = name;
	attribute.type = static_cast<FieldType>(values[0]);
	attributes.push_back(attribute);

	std::cout << toString(attribute.type) << std::endl;
  }
}*/