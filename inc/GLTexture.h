#pragma once
#include <GL/glew.h>
//#include <FreeImage.h>

namespace GL
{
  class GLTexture
  {
  public:
    GLTexture();
    ~GLTexture();
    void bind(int texture_slot) const;
    bool loadFromFile(const char* path, bool swapRB);
    void fromHandle(GLuint handle);
  private:
    GLuint handle;
  };
}