
#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <GL/glew.h>
#include "Texture.h"

#define MAXBOUNDTEXTURES 16

class TexturePool;

class TextureObject {
private:
  TextureObject() : _boundLocation(-1) {}
  GLuint _glID;
  int _boundLocation;
  friend TexturePool;
};

class TexturePool
{
public:
  static TextureObject * TexturePool::Upload(const std::string & file);
  static TextureObject * Upload(const Texture & texture);
  static TextureObject * Upload(GLuint glID);
  static void Unload(TextureObject * texture_object);
  static bool Bind(TextureObject * texture_object, int location);
  static bool Unbind(TextureObject * texture_object);
  static TextureObject * _boundTextures[MAXBOUNDTEXTURES];
};

#endif // !TEXTUREMANAGER_H