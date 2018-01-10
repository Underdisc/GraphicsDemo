/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file main.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: CS 300
\date 02/10/2017
\brief
  Entry point for the program. Implementation handles all editor 
  functionality, other interface code, and GL code from drawing.
*/
/*****************************************************************************/

#include <iostream>

#include "Graphics\Light.h"
#include "Graphics\Material.h"
#include "Graphics\Renderer.h"
#include "Graphics\Skybox.h"
#include "Graphics\Texture\TexturePool.h"

#include <GL\glew.h>
#include "Utility\OpenGLError.h"

#include "Graphics\OpenGLContext.h"
#include "Core\Time.h"
#include "Math\Matrix4.h"
#include "Math\Vector3.h"
#include "Math\EulerAngles.h"
#include "Math\MathFunctions.h"

#include "Graphics\Mesh\Mesh.h"
#include "Graphics\Mesh\MeshRenderer.h"
#include "Graphics\Shader\ShaderLibrary.h"
#include "Graphics\Shader\ShaderManager.h"
#include "Graphics\Camera.h"

#include "Core\Input.h"
#include "Core\Framer.h"
#include "Editor\Editor.h"


// make sure to do this
//#include "vld.h"

#define PI 3.141592653589f
#define PI2 6.28318530718f
#define FPS 60
#define MODEL_PATH "Resource/Model/"
#define FILENAME_BUFFERSIZE 50

Mesh * mesh;

// GLOBAL
Camera camera(Math::Vector3(0.0f, 1.0f, 0.0f));
float movespeed = 1.0f;
float mouse_sensitivity = 0.3f;
float mouse_wheel_sensitivity = 2.0f;
// Global

void LoadMesh(const std::string & model);

void Update();

void Draw();

inline void ManageInput()
{
  if (Input::MouseButtonDown(LEFT)) {
    std::pair<int, int> mouse_motion = Input::MouseMotion();

    if (mouse_motion.first != 0)
      Editor::rotation.Angles.y += mouse_motion.first / 100.0f;
    if (mouse_motion.second != 0) {
      Editor::rotation.Angles.x -= mouse_motion.second / 100.0f;
    }
  }

  movespeed += movespeed * mouse_wheel_sensitivity * Time::DT() * Input::MouseWheelMotion();
  if (Input::KeyDown(W))
    camera.MoveBack(-Time::DT() * movespeed);
  if (Input::KeyDown(S))
    camera.MoveBack(Time::DT() * movespeed);
  if (Input::KeyDown(A))
    camera.MoveRight(-Time::DT() * movespeed);
  if (Input::KeyDown(D))
    camera.MoveRight(Time::DT() * movespeed);
  if (Input::KeyDown(Q))
    camera.MoveGlobalUp(-Time::DT() * movespeed);
  if (Input::KeyDown(E))
    camera.MoveGlobalUp(Time::DT() * movespeed);
  if (Input::MouseButtonDown(RIGHT) && !Editor::rotate_camera) {
    std::pair<int, int> mouse_motion = Input::MouseMotion();
    camera.MoveYaw((float)mouse_motion.first * Time::DT() * mouse_sensitivity);
    camera.MovePitch(-(float)mouse_motion.second * Time::DT() * mouse_sensitivity);
  }
}

inline void InitialUpdate()
{
  Time::Update();
  SDLContext::CheckEvents();
}

//--------------------// Main //--------------------//

int main(int argc, char * argv[])
{
  ErrorLog::Clean();
  SDLContext::Create("CS 300 - Assignment 4", true, OpenGLContext::AdjustViewport);
  OpenGLContext::Initialize();
  ShaderManager::Initialize();
  MeshRenderer::Initialize();
  Editor::Initialize();


  LoadMesh(Editor::current_mesh);
  Renderer::Initialize(*mesh);

  camera.MoveBack(2.0f);

  // loading normal map from specular bump map
  Texture bump_map("Resource/Texture/specular.tga");
  bump_map.CreateNormalMap("Resource/Texture/normal_map.png", 2.0f / 255.0f);

  // starting main program
  glEnable(GL_DEPTH_TEST);
  Framer::Lock(FPS);
  while (SDLContext::KeepOpen())
  {
    // frame start
    Framer::Start();
    InitialUpdate();
    Editor::Update(mesh, Renderer::_meshObject, LoadMesh);
    Update();
    Draw();
    Editor::Render();
    OpenGLContext::Swap();
    // frame end
    Framer::End();
  }

  Mesh::Purge(mesh);
  Renderer::Purge();
  MeshRenderer::Purge();
  ShaderManager::Purge();
  OpenGLContext::Purge();
  SDLContext::Purge();
}

//--------------------// Other //--------------------//

void LoadMesh(const std::string & model)
{
  // loading new mesh
  Mesh * new_mesh;
  std::string mesh_path = MODEL_PATH + model;
  try {
    new_mesh = Mesh::Load(mesh_path, Mesh::OBJ, MESH_MAPPING_SPHERICAL);
  }
  catch (const Error & error) {
    return;
  }
  // deleting old mesh
  if (mesh){
    MeshRenderer::Unload(Renderer::_meshObject);
    Mesh::Purge(mesh);
  }
  // uploading mesh data to gpu
  new_mesh->SetNormalLineLengthMeshRelative(0.1f);
  Renderer::_meshObject = MeshRenderer::Upload(new_mesh);
  // new mesh loaded
  mesh = new_mesh;
  Editor::current_mesh = model;
  Renderer::ReplaceMesh(*mesh);
  try
  {
    GLenum gl_error = glGetError();
    OPENGLERRORCHECK("main.cpp", "Initialize()", "During initialization", gl_error)
  }
  catch (const Error & error)
  {
    ErrorLog::Write(error);
  }
}

inline void RotateCamera()
{
  float camera_angle = Time::TotalTimeScaled() * Editor::camera_rotate_speed;
  Math::Vector3 new_position;
  new_position.x = Math::Cos(camera_angle) * Editor::camera_distance;
  new_position.y = Math::Sin(camera_angle);
  new_position.z = Math::Sin(camera_angle) * Editor::camera_distance;
  camera.SetPosition(new_position);
  camera.LookAt(Math::Vector3(Editor::trans.x, Editor::trans.y, Editor::trans.z));
}

inline void RotateLights()
{
  for (int i = 0; i < Light::_activeLights; ++i) {
    float percentage = (float)i / (float)Light::_activeLights;
    float light_angle = percentage * PI2 + Time::TotalTimeScaled() * Editor::rotate_light_speed;
    Light & light = Editor::lights[i];

    float cos_angle = Math::Cos(light_angle);
    float sin_angle = Math::Sin(light_angle);
    light._position.x = cos_angle * Editor::cur_scale + 0.5f * cos_angle + Editor::trans.x;
    light._position.y = Editor::trans.y;
    light._position.z = sin_angle * Editor::cur_scale + 0.5f * sin_angle + Editor::trans.z;

    light._direction.x = -cos_angle;
    light._direction.y = 0.0f;
    light._direction.z = -sin_angle;
  }


}

inline void Update()
{
  if(!Editor::MouseHovering())
    ManageInput();
  if(Editor::rotating_lights)
    RotateLights();
  if(Editor::rotate_camera)
    RotateCamera();
}


//--------------------// Draw //--------------------//

void Draw()
{
  Math::Matrix4 projection = Math::Matrix4::Perspective(PI / 2.0f,
    OpenGLContext::AspectRatio(), MeshRenderer::_nearPlane,
    MeshRenderer::_farPlane);
  Renderer::Render(projection, camera.ViewMatrix(), camera.GetPosition(), true);
}
