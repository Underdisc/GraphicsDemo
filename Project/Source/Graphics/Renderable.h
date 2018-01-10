
#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <GL\glew.h>

inline GLuint UploadArrayBuffer(void * data, unsigned int bytes)
{
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, bytes, data, GL_STATIC_DRAW);
  return vbo;
}

inline GLuint UploadIndexBuffer(void * data, unsigned int bytes)
{
  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytes, data, GL_STATIC_DRAW);
  return ebo;
}

struct Renderable
{
  GLuint _vao;
  GLuint _vbo;
  GLuint _ebo;
  unsigned int _numElements;

  void Purge()
  {
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
    glDeleteVertexArrays(1, &_vao);
  }
};

#endif // !RENDERABLE_H
