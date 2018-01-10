/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
#include "../External/Imgui/imgui.h"
#include "../External/Imgui/imgui_impl_sdl_gl3.h"
#include "../Graphics/SDLContext.h"
#include "../Core/Framer.h"
#include "../Core/Time.h"
#include "../Core/Input.h"
#include "../Presets.h"
#include "../Utility/Error.h"
#include "Editor.h"

bool Editor::show_light_editor = false;
bool Editor::show_material_editor = false;
bool Editor::show_error_log = false;

std::string Editor::_currentTextureDiffuse(DIFFUSEPRESET);
std::string Editor::_currentTextureSpecular(SPECULARPRESET);
std::string Editor::_currentTextureNormal(NORMALPRESET);

std::string Editor::current_mesh(MESHPRESET);
char Editor::next_mesh[FILENAME_BUFFERSIZE] = MESHPRESET;
Light Editor::lights[MAXLIGHTS];
unsigned int Editor::active_lights = 2;
MeshRenderer::ShaderType Editor::shader_in_use = MeshRenderer::PHONG;
std::string Editor::error_log;

// these things probably shouldn't be here
bool Editor::rotating_lights = true;
float Editor::rotate_light_speed = 1.5f;
bool Editor::rotate_camera = false;
float Editor::camera_rotate_speed = 1.0f;
float Editor::camera_distance = 2.0f;
Math::Vector3 Editor::trans(0.0f, 0.0f, 0.0f);
float Editor::cur_scale = 1.0f;
//initializing to pie so rotations make sense at the start
Math::EulerAngles Editor::rotation(0.0f, PI, 0.0f, Math::EulerOrders::XYZs);


void Editor::Initialize()
{
  ImGui_ImplSdlGL3_Init(SDLContext::SDLWindow());
  SDLContext::AddEventProcessor(ImGui_ImplSdlGL3_ProcessEvent);
  ErrorLog::AddErrorString(&error_log);
}

void Editor::Render()
{
  ImGui::Render();
}

bool Editor::MouseHovering()
{
  const ImGuiIO & imgui_io = ImGui::GetIO();
  return imgui_io.WantCaptureMouse;
}

void Editor::Update(Mesh * mesh, MeshRenderer::MeshObject * mesh_object,
  void(*load_mesh)(const std::string &))
{
  ImGui_ImplSdlGL3_NewFrame(SDLContext::SDLWindow());
  ImGui::Begin("Editor");
  if (ImGui::CollapsingHeader("Help"))
  {
    ImGui::TextWrapped("Hello! There are a few controls I would like to "
      "mention so you know how to use this application.");
    ImGui::Separator();
    if (ImGui::TreeNode("QOL Controls"))
    {
      ImGui::Separator();
      ImGui::TextWrapped("Left click and drag anywhere outside the ImGui "
        "window to rotate the object.");
      ImGui::Separator();
      ImGui::TextWrapped("Use WASD to move the camera around world. E will "
        "raise the camera and Q will lower the camera.");
      ImGui::Separator();
      ImGui::TextWrapped("Right click and drag anywhere outside the ImGui "
        "window to rotate the camera.");
      ImGui::Separator();
      ImGui::TextWrapped("Use the mouse scroll wheel to increase and "
        "decrease the movement speed of the camera.");
      ImGui::Separator();
      ImGui::TextWrapped("These will not work when the mouse is hovering over "
        "an ImGui window.");
      ImGui::Separator();
      ImGui::TreePop();
    }
    ImGui::Separator();
    if (ImGui::TreeNode("Editor Tabs")) {
      ImGui::Separator();
      ImGui::TextWrapped("The Debug tab only contains the average FPS and "
        "average frame usage over a single second.");
      ImGui::Separator();
      ImGui::TextWrapped("The Global tab contains parameters for "
        "adjusting global colors (Emissive/Global Ambient/Fog), fog near/far "
        "planes and time scale");
      ImGui::Separator();
      ImGui::TextWrapped("The Camera tab contains parameters for the "
        "near/far planes of the view frustum. It also contains the rotating "
        "camera option. When rotating the camera after disabling this "
        "option, the camera might get flipped around");
      ImGui::Separator();
      ImGui::TextWrapped("The Mesh tab allows you to edit mesh properites, "
        "display normal lines, and load new meshes.");
      ImGui::Separator();
      ImGui::TextWrapped("The Shader tab contains an option for swapping out "
        "the shader that is used for rendering.");
      ImGui::Separator();
      ImGui::TextWrapped("The Material and Light Editor checkboxes will open "
        "new ImGui windows for editing material and light properties when "
        "selected.");
      ImGui::TreePop();
    }
    ImGui::Separator();
    if (ImGui::TreeNode("Light Editor")) {
      ImGui::Separator();
      ImGui::TextWrapped("You can add, remove and adjust light properties in "
        "the light editor. The Scene tab at the top contains the two presets "
        "for lighting the scene and the option to rotate the lights around "
        "the model.");
      ImGui::TreePop();
    }

    ImGui::Separator();
  }
  if (ImGui::CollapsingHeader("Debug")) {
    ImGui::Separator();
    ImGui::Text("Average FPS: %f", Framer::AverageFPS());
    ImGui::Text("Average Frame Usage: %f", Framer::AverageFrameUsage() * 100.0f);
    ImGui::Separator();
    ImGui::Checkbox("Show Error Log", &show_error_log);
    ImGui::Separator();
  }
  if (ImGui::CollapsingHeader("Global")) {
    ImGui::Text("Global Colors");
    ImGui::ColorEdit3("Emissive", MeshRenderer::_emissiveColor._values);
    ImGui::ColorEdit3("Global Amibent",
      MeshRenderer::_globalAmbientColor._values);
    ImGui::ColorEdit3("Fog Color", MeshRenderer::_fogColor._values);
    ImGui::Separator();
    ImGui::Text("Fog Properties");
    ImGui::SliderFloat("Fog Near", &MeshRenderer::_fogNear, 0.0f, MeshRenderer::_fogFar);
    ImGui::SliderFloat("Fog Far", &MeshRenderer::_fogFar, MeshRenderer::_fogNear, 100.0f);
    ImGui::Separator();
    ImGui::DragFloat("Time Scale", &Time::m_TimeScale, 0.01f);
    ImGui::Separator();
  }
  if (ImGui::CollapsingHeader("Camera")) {
    ImGui::SliderFloat("Near Plane", &MeshRenderer::_nearPlane, 0.01f, 5.0f);
    ImGui::SliderFloat("Far Plane", &MeshRenderer::_farPlane, MeshRenderer::_nearPlane, 100.0f);
    ImGui::Separator();
    ImGui::Checkbox("Rotating Camera", &rotate_camera);
    ImGui::DragFloat("Rotation Distance", &camera_distance, 0.01f);
    ImGui::DragFloat("Rotation Speed", &camera_rotate_speed, 0.01f);
    ImGui::Separator();
  }
  if (ImGui::CollapsingHeader("Mesh")) {
    ImGui::Text("Load Different Mesh");
    ImGui::InputText("", next_mesh, FILENAME_BUFFERSIZE);
    if (ImGui::Button("Load Mesh"))
      load_mesh(next_mesh);
    ImGui::Separator();
    ImGui::Text("Mesh Stats");
    ImGui::Text("Current Mesh: %s", current_mesh.c_str());
    ImGui::Text("Vertex Count: %d", mesh->VertexCount());
    ImGui::Text("Face Count: %d", mesh->FaceCount());
    ImGui::Separator();
    ImGui::Text("Translation");
    ImGui::DragFloat("TX", &trans.x, 0.01f);
    ImGui::DragFloat("TY", &trans.y, 0.01f);
    ImGui::DragFloat("TZ", &trans.z, 0.01f);
    ImGui::Separator();
    ImGui::Text("Rotation");
    ImGui::DragFloat("RX", &rotation.Angles.x, 0.01f);
    ImGui::DragFloat("RY", &rotation.Angles.y, 0.01f);
    ImGui::DragFloat("RZ", &rotation.Angles.z, 0.01f);
    ImGui::Separator();
    ImGui::Text("Scale");
    ImGui::DragFloat("s", &cur_scale, 0.01f);
    ImGui::Separator();
    ImGui::Text("TNB Lines");
    ImGui::ColorEdit3("Vertex Normal Color", 
      mesh_object->_vertexNormalColor._values);
    ImGui::ColorEdit3("Vertex Tangent Color",
      mesh_object->_vertexTangentColor._values);
    ImGui::ColorEdit3("Vertex Bitangent Color",
      mesh_object->_vertexBitangentColor._values);
    ImGui::ColorEdit3("Face Normal Color",
      mesh_object->_faceNormalColor._values);
    ImGui::ColorEdit3("Face Tangent Color",
      mesh_object->_faceTangentColor._values);
    ImGui::ColorEdit3("Face Bitangent Color",
      mesh_object->_faceBitangentColor._values);
    ImGui::Checkbox("Show Vertex Normals", &mesh_object->_showVertexNormals);
    ImGui::Checkbox("Show Vertex Tangents", &mesh_object->_showVertexTangents);
    ImGui::Checkbox("Show Vertex Bitangents", &mesh_object->_showVertexBitangents);
    ImGui::Checkbox("Show Face Normals", &mesh_object->_showFaceNormals);
    ImGui::Checkbox("Show Face Tangents", &mesh_object->_showFaceTangents);
    ImGui::Checkbox("Show Face Bitangents", &mesh_object->_showFaceBitangents);
    ImGui::Separator();
    ImGui::Text("Other");
    ImGui::Checkbox("Wireframe", &mesh_object->_showWireframe);
    ImGui::Separator();
  }
  if (ImGui::CollapsingHeader("Shader")) {
    int shader_int = MeshRenderer::ShaderTypeToInt(shader_in_use);
    ImGui::Combo("Shader Type", &shader_int, "Phong\0\0");
    shader_in_use = MeshRenderer::IntToShaderType(shader_int);
    if (ImGui::Button("Reload Selected Shader"))
      MeshRenderer::ReloadShader(shader_in_use);
    ImGui::Separator();
  }
  ImGui::Checkbox("Show Material Editor", &show_material_editor);
  ImGui::Checkbox("Show Light Editor", &show_light_editor);

  ImGui::End();
  if (show_material_editor)
    MaterialEditorUpdate(mesh_object->_material);
  if (show_light_editor)
    LightEditorUpdate();
  if(show_error_log)
    ErrorLogUpdate();
    
}

inline void Editor::MaterialEditorUpdate(Material & material)
{
  ImGui::Begin("Material", &show_material_editor);
  // window body start
  ImGui::Checkbox("Texture Mapping", &material._textureMapping);
  ImGui::Checkbox("Specular Mapping", &material._specularMapping);
  ImGui::Checkbox("Normal Mapping", &material._normalMapping);
  ImGui::Separator();
  ImGui::ColorEdit3("Color", material._color._values);
  ImGui::SliderFloat("Ambient Factor", &material._ambientFactor, 0.0f, 1.0f);
  if(!material._textureMapping)
    ImGui::SliderFloat("Diffuse Factor", &material._diffuseFactor, 0.0f, 1.0f);
  if(!material._specularMapping)
    ImGui::SliderFloat("Specular Factor", &material._specularFactor, 
      0.0f, 1.0f);
  ImGui::SliderFloat("Specular Exponent", &material._specularExponent, 
    0.0f, 30.0f);
  ImGui::Separator();
  // Texture mapping section
  if (material._textureMapping || material._specularMapping ||
    material._normalMapping) {
    // mapping type
    ImGui::Combo("Mapping Type", &material._mappingType,
      "Spherical\0Cylindrical\0Planar\0\0");
    ImGui::Separator();
    // texture maps
    if (material._textureMapping) {
      ImGui::Text("Current Diffusue Texture: %s",
        _currentTextureDiffuse.c_str());
    }
    if (material._specularMapping) {
      ImGui::Text("Current Specular Texture: %s",
        _currentTextureSpecular.c_str());
    }
    if (material._normalMapping) {
      ImGui::Text("Current Normal Texture: %s",
        _currentTextureNormal.c_str());
    }
    ImGui::Separator();
  }
  ImGui::Checkbox("Environment Mapping", &material._environmentMapping);
  if(material._environmentMapping){
    ImGui::SliderFloat("Environment Factor", &material._environmentFactor,
      0.0f, 1.0f);
    ImGui::SliderFloat("Refraction Index", &material._refractionIndex,
      1.0f, 2.0f);
    ImGui::Checkbox("Chromatic Abberation", &material._chromaticAbberation);
    ImGui::SliderFloat("Chromatic Offset", &material._chromaticOffset, 0.0f, 0.1f);
    ImGui::Checkbox("Fresnel Reflection", &material._fresnelReflection);
    ImGui::SliderFloat("Refraction <--> Reflection", &material._fresnelRatio, 0.0f, 1.0f);
  }
  ImGui::Separator();
  ImGui::End();
}

inline void Editor::LightEditorUpdate()
{
  ImGui::Begin("Lights", &show_light_editor);
  if (ImGui::CollapsingHeader("Scenes")) {
    if (ImGui::Button("Same"))
      SceneSame();
    ImGui::SameLine();
    if (ImGui::Button("Mix"))
      SceneMix();
    ImGui::Separator();
    ImGui::Checkbox("Rotate Lights", &rotating_lights);
    ImGui::DragFloat("Rotation Speed", &rotate_light_speed, 0.01f);
    ImGui::Separator();
  }
  ImGui::InputInt("Active Lights", &Light::_activeLights, 1, 1);
  if (Light::_activeLights > MAXLIGHTS)
    Light::_activeLights = MAXLIGHTS;
  else if (Light::_activeLights < 0)
    Light::_activeLights = 0;

  for (int i = 0; i < Light::_activeLights; ++i) {
    std::string light_name("Light" + std::to_string(i));
    if (ImGui::TreeNode(light_name.c_str())) {
      ImGui::Combo("Type", &lights[i]._type, "Point\0Directional\0Spot\0\0");
      int type = lights[i]._type;
      if (type == Light::_typePoint || type == Light::_typeSpot) {
        ImGui::Text("Position");
        ImGui::DragFloat("PX", &lights[i]._position.x, 0.01f);
        ImGui::DragFloat("PY", &lights[i]._position.y, 0.01f);
        ImGui::DragFloat("PZ", &lights[i]._position.z, 0.01f);
      }
      if (type == Light::_typeDirectional || type == Light::_typeSpot) {
        ImGui::Text("Direction");
        ImGui::DragFloat("DX", &lights[i]._direction.x, 0.01f);
        ImGui::DragFloat("DY", &lights[i]._direction.y, 0.01f);
        ImGui::DragFloat("DZ", &lights[i]._direction.z, 0.01f);
      }
      if (type == Light::_typeSpot) {
        ImGui::Text("Spotlight Properties");
        float * inner_angle = &lights[i]._innerAngle;
        float * outer_angle = &lights[i]._outerAngle;
        ImGui::SliderFloat("Inner", inner_angle, 0.0f, *outer_angle);
        ImGui::SliderFloat("Outer", outer_angle, *inner_angle, PI / 2.0f);
        ImGui::SliderFloat("Exponent", &lights[i]._spotExponent, 0.0f, 30.0f);
      }
      if (type != Light::_typeDirectional) {
        ImGui::Text("Attenuation Coefficients");
        ImGui::SliderFloat("C0", &lights[i]._attenuationC0, 0.0f, 5.0f);
        ImGui::SliderFloat("C1", &lights[i]._attenuationC1, 0.0f, 5.0f);
        ImGui::SliderFloat("C2", &lights[i]._attenuationC2, 0.0f, 5.0f);
      }
      ImGui::Text("Light Colors");
      ImGui::ColorEdit3("Ambient Color", lights[i]._ambientColor._values);
      ImGui::ColorEdit3("Diffuse Color", lights[i]._diffuseColor._values);
      ImGui::ColorEdit3("Specular Color", lights[i]._specularColor._values);
      ImGui::TreePop();
    }
    ImGui::Separator();
  }
  ImGui::End();
}

inline void Editor::ErrorLogUpdate()
{
  ImGui::Begin("Errors", &show_error_log);
  ImGui::Text(error_log.c_str());
  ImGui::Separator();
  if(ImGui::Button("Clear"))
    error_log.clear();
  ImGui::End();
}


inline void Editor::SceneSame()
{
  Light::_activeLights = 2;

  lights[0]._position = Math::Vector3(-2.5f, 0.0f, -1.0f);
  lights[1]._position = Math::Vector3(1.0f, 1.0f, 0.0f);

  for (int i = 0; i < Light::_activeLights; ++i) {
    lights[i]._type = Light::_typePoint;
    lights[i]._attenuationC0 = 1.0f;
    lights[i]._attenuationC1 = 0.1f;
    lights[i]._attenuationC2 = 0.0f;
    lights[i]._ambientColor = Color(0.0f, 0.4f, 0.0f);
    lights[i]._diffuseColor = Color(0.2f, 0.4f, 0.4f);
    lights[i]._specularColor = Color(1.0f, 1.0f, 1.0f);
  }
}

inline void Editor::SceneMix()
{
  Light::_activeLights = 3;
  lights[0]._type = Light::_typeDirectional;
  lights[0]._position = Math::Vector3(-5.0f, 0.0f, 0.0f);
  lights[0]._direction = Math::Vector3(1.0f, -1.0f, 0.0f);
  lights[0]._ambientColor = Color(0.0f, 0.0f, 0.4f);
  lights[0]._diffuseColor = Color(0.23f, 0.2f, 0.23f);
  lights[0]._specularColor = Color(1.0f, 0.8f, 0.53f);

  lights[1]._type = Light::_typePoint;
  lights[1]._position = Math::Vector3(0.0f, -2.0f, 0.0f);
  lights[1]._attenuationC0 = 0.0f;
  lights[1]._attenuationC1 = 0.4f;
  lights[1]._attenuationC2 = 0.4f;
  lights[1]._ambientColor = Color(0.0f, 0.0f, 0.0f);
  lights[1]._diffuseColor = Color(0.0f, 0.8f, 0.53f);
  lights[1]._specularColor = Color(1.0f, 1.0f, 1.0f);

  lights[2]._type = Light::_typeSpot;
  lights[2]._position = Math::Vector3(1.9f, 1.7f, -0.8f);
  lights[2]._direction = Math::Vector3(-1.0f, -1.0f, 0.5f);
  lights[2]._innerAngle = 0.027f;
  lights[2]._outerAngle = 0.126f;
  lights[2]._attenuationC0 = 0.0f;
  lights[2]._attenuationC1 = 0.0f;
  lights[2]._attenuationC2 = 0.5f;
  lights[2]._ambientColor = Color(0.0f, 0.0f, 0.0f);
  lights[2]._diffuseColor = Color(0.6f, 0.3f, 0.3f);
  lights[2]._specularColor = Color(0.6f, 0.4f, 0);
}