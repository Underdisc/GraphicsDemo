/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */

#include <vector>

#include "Mesh/Mesh.h"
#include "Mesh/MeshRenderer.h"
#include "Texture/TexturePool.h"
#include "Framebuffer.h"
#include "Skybox.h"
#include "Camera.h"

#ifndef RENDERER_H
#define RENDERER_H


class Renderer
{
public:
  static void Initialize(Mesh & mesh);
  static void Purge();
  static void Clear();
  static void RenderEnvironment();
  static void Render(const Math::Matrix4 & projection,
    const Math::Matrix4 & view, const Math::Vector3 & view_position, 
    bool mesh);
  static void RenderFrame(const Math::Matrix4 & projection,
    const Math::Matrix4 & view, const Math::Vector3 & view_position,
    bool mesh);
  static void ReplaceMesh(Mesh & mesh);
public:
  static Mesh * _mesh;
  static MeshRenderer::MeshObject * _meshObject;
  static MeshRenderer::MeshObject * _sphereMeshObject;
  static MeshRenderer::MeshObject * _planeMeshObject;

  static TextureObject * _diffuseTextureObject;
  static TextureObject * _specularTextureObject;
  static TextureObject * _normalTextureObject;

  static bool _renderSkybox;
  static Skybox * _skybox;

  struct EnvironmentRender
  {
    // Initializes the linear part of the view matrix and the width and height
    // of the framebuffer
    EnvironmentRender(unsigned int width, unsigned int height,
      const Math::Matrix4 & linear) : _linear(linear)
    {
      _fb.Initialize(width, height);
    }
    // The framebuffer used for the environment render
    Framebuffer _fb;
    // The linear part of the view matrix used for the environment render
    Math::Matrix4 _linear;
  };

  // Stores all environment renders used for dynmaic reflections and 
  // refractions. The environment renders are stored in the following
  // order.
  // up, down, left, right, front, back
  static std::vector<EnvironmentRender> _environmentRenders;

};

#endif