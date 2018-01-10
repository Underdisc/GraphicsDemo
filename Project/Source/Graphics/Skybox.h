
#include <string>

#include "../Math/Matrix4.h"
#include "Texture/TexturePool.h"
#include "Renderable.h"

#ifndef SKYBOX_H
#define SKYBOX_H


class Skybox
{
public:
  Skybox(const std::string & directory,
    const std::string & up, const std::string & down,
    const std::string & left, const std::string & right,
    const std::string & front, const std::string & back);
  ~Skybox();

  bool Upload();
  void Unload();
  void Render(const Math::Matrix4 & projection, const Math::Matrix4 & view);
  // The texture objects used for rendering
  TextureObject * _tUp;
  TextureObject * _tDown;
  TextureObject * _tLeft;
  TextureObject * _tRight;
  TextureObject * _tFront;
  TextureObject * _tBack;
  // The filenames for the skybox textures
  std::string _directory;
  std::string _fUp;
  std::string _fDown;
  std::string _fLeft;
  std::string _fRight;
  std::string _fFront;
  std::string _fBack;
  // Buffer IDs
  Renderable _sky;
};


#endif // SKYBOX_H