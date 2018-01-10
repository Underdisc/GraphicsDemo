#include "ShaderLibrary.h"

#pragma once


// for future reference
// only the renderer should have this
// not the mesh renderer
class ShaderManager
{
public:
  static void Initialize();
  static void Purge();
public:
  static SkyboxShader * _skybox;
private:
  ShaderManager();
};
