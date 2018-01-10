
#include "Shader/ShaderManager.h"
#include "Mesh/Mesh.h"

#include "Skybox.h"

Skybox::Skybox(const std::string & directory,
  const std::string & up, const std::string & down,
  const std::string & left, const std::string & right,
  const std::string & front, const std::string & back) :
  _directory(directory), _fUp(up), _fDown(down), _fLeft(left), _fRight(right),
  _fFront(front), _fBack(back)
{
  // uploading skybox mesh (sm)
  Mesh sm("Resource/Model/skybox.obj", Mesh::OBJ);
  glGenVertexArrays(1, &_sky._vao);
  glBindVertexArray(_sky._vao);
  _sky._vbo = UploadArrayBuffer(sm.VertexData(), sm.VertexDataSizeBytes());
  _sky._ebo = UploadIndexBuffer(sm.IndexData(), sm.IndexDataSizeBytes());
  ShaderManager::_skybox->EnableAttributes();
  glBindVertexArray(0);
  _sky._numElements = sm.IndexDataSize();
}

Skybox::~Skybox()
{
  _sky.Purge();
}

bool Skybox::Upload()
{
  _tUp = TexturePool::Upload(_directory + _fUp);
  _tDown = TexturePool::Upload(_directory + _fDown);
  _tLeft = TexturePool::Upload(_directory + _fLeft);
  _tRight = TexturePool::Upload(_directory + _fRight);
  _tFront = TexturePool::Upload(_directory + _fFront);
  _tBack = TexturePool::Upload(_directory + _fBack);
  if (_tUp && _tDown && _tLeft && _tRight && _tFront && _tBack)
    return true;
  return false;
}

void Skybox::Unload()
{
  // TODO: make sure the skybox was successfully loaded first
  TexturePool::Unload(_tUp);
  TexturePool::Unload(_tDown);
  TexturePool::Unload(_tLeft);
  TexturePool::Unload(_tRight);
  TexturePool::Unload(_tFront);
  TexturePool::Unload(_tBack);
}

// use the linear part of your view matrix
void Skybox::Render(const Math::Matrix4 & projection,
  const Math::Matrix4 & view)
{
  SkyboxShader * shader = ShaderManager::_skybox;
  ShaderManager::_skybox->Use();
  // transformation uniforms
  glUniformMatrix4fv(shader->UProjection, 1, GL_TRUE, projection.array);
  glUniformMatrix4fv(shader->UView, 1, GL_TRUE, view.array);
  // sampler uniforms
  glUniform1i(shader->UUp, 0);
  glUniform1i(shader->UDown, 1);
  glUniform1i(shader->ULeft, 2);
  glUniform1i(shader->URight, 3);
  glUniform1i(shader->UFront, 4);
  glUniform1i(shader->UBack, 5);
  // binding textures
  TexturePool::Bind(_tUp, 0);
  TexturePool::Bind(_tDown, 1);
  TexturePool::Bind(_tLeft, 2);
  TexturePool::Bind(_tRight, 3);
  TexturePool::Bind(_tFront, 4);
  TexturePool::Bind(_tBack, 5);
  // drawing
  glDepthMask(GL_FALSE);
  glBindVertexArray(_sky._vao);
  glPolygonMode(GL_FRONT, GL_FILL);
  glDrawElements(GL_TRIANGLES, _sky._numElements, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
  glDepthMask(GL_TRUE);
  // unbind textures
  TexturePool::Unbind(_tUp);
  TexturePool::Unbind(_tDown);
  TexturePool::Unbind(_tLeft);
  TexturePool::Unbind(_tRight);
  TexturePool::Unbind(_tFront);
  TexturePool::Unbind(_tBack);
}