

#include "../../Utility/Error.h"
#include "TexturePool.h"

// byte depths
#define RGB  3
#define RGBA 4

// static initializations
TextureObject * TexturePool::_boundTextures[MAXBOUNDTEXTURES] = { nullptr };

TextureObject * TexturePool::Upload(const std::string & file)
{
  try{
    Texture texture(file);
    return Upload(texture);
  }
  catch (const Error & error) {
    ErrorLog::Write(error);
  }
}

TextureObject * TexturePool::Upload(const Texture & texture)
{
  TextureObject * new_texture_object = new TextureObject();
  // generating opengl texture object
  glGenTextures(1, &new_texture_object->_glID);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, new_texture_object->_glID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // for rgb or rgba
  switch (texture._channels)
  {
  case RGB:
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture._width, texture._height, 0,
      GL_RGB, GL_UNSIGNED_BYTE, texture._imageData);
    break;
  case RGBA:
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture._width, texture._height, 
      0, GL_RGBA, GL_UNSIGNED_BYTE, texture._imageData);
    break;
  default:
    Error error("TexturePool.cpp", "Upload");
    error.Add("Image file format not supported");
    error.Add("> Image file");
    error.Add(texture._imageFile.c_str());
    ErrorLog::Write(error);
    return nullptr;
  }
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0); // need to make sure you rebind the texture that was bound here
  return new_texture_object;
}

TextureObject * TexturePool::Upload(GLuint glID)
{
  TextureObject * new_texture_object = new TextureObject();
  new_texture_object->_glID = glID;
  return new_texture_object;
}

void TexturePool::Unload(TextureObject * texture_object)
{
  Unbind(texture_object);
  delete texture_object;
}

bool TexturePool::Bind(TextureObject * texture_object, int location)
{
  if (location >= MAXBOUNDTEXTURES) {
    Error error("TexturePool.cpp", "Bind");
    error.Add("Given location out of boundable range.");
    error.Add("> Given location");
    error.Add(std::to_string(location));
    throw(error);
  }
  if (texture_object->_boundLocation != -1)
    return false;
  glActiveTexture(GL_TEXTURE0 + location);
  glBindTexture(GL_TEXTURE_2D, texture_object->_glID);
  _boundTextures[location] = texture_object;
  texture_object->_boundLocation = location;
  return true;
}

bool TexturePool::Unbind(TextureObject * texture_object)
{
  if (texture_object->_boundLocation == -1)
    return false;
  glActiveTexture(GL_TEXTURE0 + texture_object->_boundLocation);
  glBindTexture(GL_TEXTURE_2D, 0);
  _boundTextures[texture_object->_boundLocation];
  texture_object->_boundLocation = -1;
  return true;
}