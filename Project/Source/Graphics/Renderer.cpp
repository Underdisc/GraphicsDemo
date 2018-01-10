/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
#include "Renderer.h"

#include "../Editor/Editor.h"
#include "../Math/MathFunctions.h"
#include "../Utility/OpenGLError.h"

#define PI 3.141592653589f
#define PI2 6.28318530718f

// framebuffer locations in environment render vector
#define FB_UP 0 // up
#define FB_DN 1 // down
#define FB_LT 2 // left
#define FB_RT 3 // right
#define FB_FT 4 // front
#define FB_BK 5 // back

// static initializations
Mesh * Renderer::_mesh = nullptr;
MeshRenderer::MeshObject * Renderer::_meshObject = nullptr;
MeshRenderer::MeshObject * Renderer::_sphereMeshObject = nullptr;

TextureObject * Renderer::_diffuseTextureObject = nullptr;
TextureObject * Renderer::_specularTextureObject = nullptr;
TextureObject * Renderer::_normalTextureObject = nullptr;

bool Renderer::_renderSkybox = true;
Skybox * Renderer::_skybox = nullptr;

std::vector<Renderer::EnvironmentRender> Renderer::_environmentRenders;

#include <iostream>

void Renderer::Initialize(Mesh & mesh) 
{
  // mesh stuff
  _meshObject = MeshRenderer::Upload(&mesh);
  Mesh sphere_mesh(MODEL_PATH + std::string("sphere.obj"), Mesh::OBJ);
  _sphereMeshObject = MeshRenderer::Upload(&sphere_mesh);

  // texture stuff
  _diffuseTextureObject = TexturePool::Upload("Resource/Texture/diffuse.tga");
  _specularTextureObject = TexturePool::Upload("Resource/Texture/specular.tga");
  _normalTextureObject = TexturePool::Upload("Resource/Texture/normal.png");

  // skybox
  _skybox = new Skybox("Resource/Texture/Skybox/Crater/",
    "up.tga", "dn.tga", "lf.tga", "rt.tga", "ft.tga", "bk.tga");
  _skybox->Upload();

  // initializing environment framebuffers
  // Creating the linear parts of the view matrices used for
  // environment rendering
  Math::Vector4 pos_x(1.0f, 0.0f, 0.0f, 0.0f);
  Math::Vector4 pos_y(0.0f, 1.0f, 0.0f, 0.0f);
  Math::Vector4 pos_z(0.0f, 0.0f, 1.0f, 0.0f);
  Math::Vector4 neg_x(-1.0f, 0.0f, 0.0f, 0.0f);
  Math::Vector4 neg_y(0.0f, -1.0f, 0.0f, 0.0f);
  Math::Vector4 neg_z(0.0f, 0.0f, -1.0f, 0.0f);
  Math::Vector4 basis_w(0.0f, 0.0f, 0.0f, 1.0f);
  Math::Matrix4 up(pos_x, pos_z, neg_y, basis_w);
  up.Transpose();
  Math::Matrix4 down(pos_x, neg_z, pos_y, basis_w);
  down.Transpose();
  Math::Matrix4 left(neg_z, pos_y, pos_x, basis_w);
  left.Transpose();
  Math::Matrix4 right(pos_z, pos_y, neg_x, basis_w);
  right.Transpose();
  Math::Matrix4 front(pos_x, pos_y, pos_z, basis_w);
  front.Transpose();
  Math::Matrix4 back(neg_x, pos_y, neg_z, basis_w);
  back.Transpose();
  // creating environment render instances
  _environmentRenders.reserve(6);
  _environmentRenders.push_back(EnvironmentRender(512, 512, up));
  _environmentRenders.push_back(EnvironmentRender(512, 512, down));
  _environmentRenders.push_back(EnvironmentRender(512, 512, left));
  _environmentRenders.push_back(EnvironmentRender(512, 512, right));
  _environmentRenders.push_back(EnvironmentRender(512, 512, front));
  _environmentRenders.push_back(EnvironmentRender(512, 512, back));
}

void Renderer::Purge()
{
  MeshRenderer::Unload(_sphereMeshObject);
  TexturePool::Unload(_diffuseTextureObject);
  TexturePool::Unload(_specularTextureObject);
  TexturePool::Unload(_normalTextureObject);
  _skybox->Unload();
  delete _skybox;
}

void Renderer::Clear()
{
  Color & fog_color = MeshRenderer::_fogColor;
  glClearColor(fog_color._r, fog_color._g, fog_color._b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::RenderEnvironment()
{
  // custom projection for square texture (90 fov)
  Math::Matrix4 environment_projection = Math::Matrix4::Perspective(PI / 2.0f,
    1.0f, MeshRenderer::_nearPlane, MeshRenderer::_farPlane);


  Math::Matrix4 translation(
    1.0f, 0.0f, 0.0f, -Editor::trans.x,
    0.0f, 1.0f, 0.0f, -Editor::trans.y,
    0.0f, 0.0f, 1.0f, -Editor::trans.z,
    0.0f, 0.0f, 0.0f, 1.0f);

  for (EnvironmentRender & er : _environmentRenders)
  {
    er._fb.Bind();
    Clear();
    Math::Matrix4 view(er._linear * translation);
    RenderFrame(environment_projection, view, Editor::trans, false);
    Framebuffer::BindDefault();
  }
}

void Renderer::Render(const Math::Matrix4 & projection, 
  const Math::Matrix4 & view, const Math::Vector3 & view_position, bool mesh)
{
  RenderEnvironment();
  Clear();
  RenderFrame(projection, view, view_position, mesh);
}

void Renderer::RenderFrame(const Math::Matrix4 & projection,
  const Math::Matrix4 & view, const Math::Vector3 & view_position, bool mesh)
{
  if (_renderSkybox)
    _skybox->Render(projection, view);

  // bind textures for rendering
  // model textures
  TexturePool::Bind(_diffuseTextureObject, 0);
  TexturePool::Bind(_specularTextureObject, 1);
  TexturePool::Bind(_normalTextureObject, 2);
  // environment map textures
  TexturePool::Bind(_environmentRenders[FB_UP]._fb._texture, 3);
  TexturePool::Bind(_environmentRenders[FB_DN]._fb._texture, 4);
  TexturePool::Bind(_environmentRenders[FB_LT]._fb._texture, 5);
  TexturePool::Bind(_environmentRenders[FB_RT]._fb._texture, 6);
  TexturePool::Bind(_environmentRenders[FB_FT]._fb._texture, 7);
  TexturePool::Bind(_environmentRenders[FB_BK]._fb._texture, 8);


  SolidShader * solid_shader = MeshRenderer::GetSolidShader();
  PhongShader * phong_shader = MeshRenderer::GetPhongShader();
  GouraudShader * gouraud_shader = MeshRenderer::GetGouraudShader();
  BlinnShader * blinn_shader = MeshRenderer::GetBlinnShader();
  Math::Matrix4 model;
  Math::Matrix4 translate;
  Math::Matrix4 rotate;
  Math::Matrix4 scale;

  // render lights
  solid_shader->Use();

  for (int i = 0; i < Light::_activeLights; ++i) {
    translate.Translate(Editor::lights[i]._position.x, Editor::lights[i]._position.y, Editor::lights[i]._position.z);
    scale.Scale(0.25f, 0.25f, 0.25f);
    model = translate * scale;
    Color & color = Editor::lights[i]._diffuseColor;
    glUniform3f(solid_shader->UColor, color._r, color._g, color._b);
    MeshRenderer::Render(_sphereMeshObject, MeshRenderer::SOLID, projection, view, model);
  }

  translate.Translate(Editor::trans.x, Editor::trans.y, Editor::trans.z);
  scale.Scale(Editor::cur_scale, Editor::cur_scale, Editor::cur_scale);
  Math::ToMatrix4(Editor::rotation, &rotate);
  model = translate * rotate * scale;

  switch (Editor::shader_in_use)
  {
    //PHONG SHADER
  case MeshRenderer::ShaderType::PHONG:
    phong_shader->Use();
    glUniform3f(phong_shader->UCameraPosition, view_position.x, view_position.y, view_position.z);
    glUniform1i(phong_shader->UActiveLights, Light::_activeLights);
    for (int i = 0; i < Light::_activeLights; ++i)
      Editor::lights[i].SetUniforms(i, phong_shader);
    break;
    // GOURAUD SHADER
  case MeshRenderer::ShaderType::GOURAUD:
    gouraud_shader->Use();
    glUniform3f(gouraud_shader->UCameraPosition, view_position.x, view_position.y, view_position.z);
    glUniform1i(gouraud_shader->UActiveLights, Light::_activeLights);
    glUniform1i(gouraud_shader->UActiveLights, Light::_activeLights);
    for (int i = 0; i < Light::_activeLights; ++i)
      Editor::lights[i].SetUniforms(i, gouraud_shader);
    break;
  case MeshRenderer::ShaderType::BLINN:
    blinn_shader->Use();
    glUniform3f(blinn_shader->UCameraPosition, view_position.x, view_position.y, view_position.z);
    glUniform1i(blinn_shader->UActiveLights, Light::_activeLights);
    for (int i = 0; i < Light::_activeLights; ++i)
      Editor::lights[i].SetUniforms(i, blinn_shader);
    break;
  default:
    break;
  }
  // rendering mesh
  if (mesh)
    MeshRenderer::Render(_meshObject, Editor::shader_in_use, projection, view, model);

  // unbind textures
  TexturePool::Unbind(_diffuseTextureObject);
  TexturePool::Unbind(_specularTextureObject);
  TexturePool::Unbind(_normalTextureObject);
  TexturePool::Unbind(_environmentRenders[FB_UP]._fb._texture);
  TexturePool::Unbind(_environmentRenders[FB_DN]._fb._texture);
  TexturePool::Unbind(_environmentRenders[FB_LT]._fb._texture);
  TexturePool::Unbind(_environmentRenders[FB_RT]._fb._texture);
  TexturePool::Unbind(_environmentRenders[FB_FT]._fb._texture);
  TexturePool::Unbind(_environmentRenders[FB_BK]._fb._texture);
  // disable writing to error strings
  try
  {
    GLenum gl_error = glGetError();
    OPENGLERRORCHECK("Renderer.cpp", "Render()", "During Render", gl_error)
  }
  catch (const Error & error)
  {
    ErrorLog::Write(error);
  }
}

void Renderer::ReplaceMesh(Mesh & mesh)
{
  MeshRenderer::Unload(_meshObject);
  _meshObject = MeshRenderer::Upload(&mesh);
}