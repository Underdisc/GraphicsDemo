/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
#pragma once

#include <string>
#include "../Graphics/Mesh/Mesh.h"
#include "../Graphics/Mesh/MeshRenderer.h"
#include "../Graphics/Light.h"
#include "../Graphics/Material.h"
#include "../Graphics/Camera.h"
#include "../Math/Vector3.h"
#include "../Math/EulerAngles.h"


#define PI 3.141592653589f
#define PI2 6.28318530718f
#define FPS 60
#define MODEL_PATH "Resource/Model/"
#define FILENAME_BUFFERSIZE 50

class Editor
{
// editor windows
public:
  static void Initialize();
  static void Render();
  static bool MouseHovering();
  static void Update(Mesh * mesh, MeshRenderer::MeshObject * mesh_object,
    void (*load_mesh)(const std::string &));
  static void MaterialEditorUpdate(Material & material);
  static void LightEditorUpdate();
  static void ErrorLogUpdate();
public:
  static void SceneMix();
  static void SceneSame();
public:
  static bool show_light_editor;
  static bool show_material_editor;
  static bool show_error_log;

  static std::string _currentTextureDiffuse;
  static std::string _currentTextureSpecular;
  static std::string _currentTextureNormal;

  static std::string current_mesh;
  static char next_mesh[FILENAME_BUFFERSIZE];
  static unsigned int active_lights;
  static MeshRenderer::ShaderType shader_in_use;

  static std::string error_log;

  // these things should probably not be here
  static bool rotating_lights;
  static float rotate_light_speed;
  static Light lights[MAXLIGHTS];
  static bool rotate_camera;
  static float camera_rotate_speed;
  static float camera_distance;
  static Math::Vector3 trans;
  static float cur_scale;
   // initializing to pie so rotations make sense at the start
  static Math::EulerAngles rotation;


  // really really shouldn't be here but w/e for now
  static bool _environmentMapping;
  static int _environmentMappingType;
private:
  Editor();
};