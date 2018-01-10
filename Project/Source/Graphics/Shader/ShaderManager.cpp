#include "ShaderManager.h"

SkyboxShader * ShaderManager::_skybox = nullptr;

void ShaderManager::Initialize()
{
  _skybox = new SkyboxShader();
}
void ShaderManager::Purge()
{
  _skybox->Purge();
  delete _skybox;
}