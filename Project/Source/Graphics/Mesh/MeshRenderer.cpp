/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#include "../../Math/Matrix4.h"
#include "../../Utility/Error.h"

#include "MeshRenderer.h"

// static initializations
Color MeshRenderer::_emissiveColor(0.0f, 0.0f, 0.0f);
Color MeshRenderer::_globalAmbientColor(0.2f, 0.2f, 0.2f);
Color MeshRenderer::_fogColor(0.3f, 0.3f, 0.3f);
float MeshRenderer::_fogNear = 10.0f;
float MeshRenderer::_fogFar = 20.0f;
float MeshRenderer::_nearPlane = 0.1f;
float MeshRenderer::_farPlane = 20.0f;
unsigned int MeshRenderer::_meshObjectsAdded = 0;
std::unordered_set<MeshRenderer::MeshObject *> MeshRenderer::_meshObjects;
LineShader * MeshRenderer::_lineShader = nullptr;
SolidShader * MeshRenderer::_solidShader = nullptr;
PhongShader * MeshRenderer::_phongShader = nullptr;
GouraudShader * MeshRenderer::_gouraudShader = nullptr;
BlinnShader * MeshRenderer::_blinnShader = nullptr;


void MeshRenderer::Initialize()
{ 
  _lineShader = new LineShader();
  _solidShader = new SolidShader();
  _phongShader = new PhongShader();
  _gouraudShader = new GouraudShader();
  _blinnShader = new BlinnShader();
}

/*****************************************************************************/
/*!
\brief
  Destroys all of the MeshObjects and destroys the shader for the mesh render.
*/
/*****************************************************************************/
void MeshRenderer::Purge()
{
  // deleting all mesh objects
  for (MeshObject * mesh_object : _meshObjects) {
    glDeleteBuffers(1, &mesh_object->_vbo);
    glDeleteBuffers(1, &mesh_object->_ebo);
    glDeleteVertexArrays(1, &mesh_object->_vao);
    // freeing normal line buffers
    glDeleteBuffers(1, &mesh_object->_vboVertexNormal);
    glDeleteVertexArrays(1, &mesh_object->_vaoVertexNormal);
    glDeleteBuffers(1, &mesh_object->_vboFaceNormal);
    glDeleteVertexArrays(1, &mesh_object->_vaoFaceNormal);
    delete mesh_object;
  }
  _meshObjects.clear();
  // deallocating all shaders
  _lineShader->Purge();
  _solidShader->Purge();
  _phongShader->Purge();
  _gouraudShader->Purge();
  _blinnShader->Purge();
  delete _lineShader;
  delete _solidShader;
  delete _phongShader;
  delete _gouraudShader;
  delete _blinnShader;
}

/*****************************************************************************/
/*!
\brief Used to upload a mesh, its vertex normal lines, and its face normal
  lines to the gpu.

\param mesh
  The mesh to be uploaded.

\return The mesh's id within the mesh renderer.
*/
/*****************************************************************************/
MeshRenderer::MeshObject * MeshRenderer::Upload(Mesh * mesh)
{
  // primary mesh upload
  GLuint vbo, ebo, vao;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, mesh->VertexDataSizeBytes(), 
    mesh->VertexData(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->IndexDataSizeBytes(), 
    mesh->IndexData(), GL_STATIC_DRAW);
  // enable all vertex attributes
  //_solidShader->EnableAttributes();
  //_gouraudShader->EnableAttributes();
  //_blinnShader->EnableAttributes();
  // These can't all happen at the same time. The reason it was working
  // earlier is due to the fact that the last EnableAttributes set up the
  // attributes in such a way that they were correct for each shader.
  // AKA, they overwrote the EnableAttribute calls made prior.
  // If shaders are meant to use the same vertex buffer of data and process 
  // it to do whatever, those shaders NEED to use the same attribute layouts
  // within the buffer or they won't work. Might be wise to have a set of 
  // attributes that ALL shaders will have regardless of their type. All
  // shaders that share a common buffer type that is.
  _phongShader->EnableAttributes();
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  // vertex Normal upload
  GLuint vbo_vn, vao_vn;
  UploadLineBuffer(&vbo_vn, &vao_vn, mesh->VertexNormalLineData(),
    mesh->VertexNormalLineSizeBytes());
  // vertex tangent upload
  GLuint vbo_vt, vao_vt;
  UploadLineBuffer(&vbo_vt, &vao_vt, mesh->VertexTangentLineData(),
    mesh->VertexTangentLineSizeBytes());
  // vertex bitangent upload
  GLuint vbo_vb, vao_vb;
  UploadLineBuffer(&vbo_vb, &vao_vb, mesh->VertexBitangentLineData(),
    mesh->VertexBitangentLineSizeBytes());
  // face normal upload 
  GLuint vbo_fn, vao_fn;
  UploadLineBuffer(&vbo_fn, &vao_fn, mesh->FaceNormalLineData(),
    mesh->FaceNormalLineSizeBytes());
  GLuint vbo_ft, vao_ft;
  UploadLineBuffer(&vbo_ft, &vao_ft, mesh->FaceTangentLineData(),
    mesh->FaceTangentLineSizeBytes());
  GLuint vbo_fb, vao_fb;
  UploadLineBuffer(&vbo_fb, &vao_fb, mesh->FaceBitangentLineData(),
    mesh->FaceBitangentLineSizeBytes());


  // creating and adding new mesh object
  MeshObject * new_mesh_object = new MeshObject(vbo, ebo, vao, 
    mesh->IndexDataSize(),
    vbo_vn, vao_vn, mesh->VertexNormalLineSizeVertices(),
    vbo_vt, vao_vt, mesh->VertexTangentLineSizeVertices(),
    vbo_vb, vao_vb, mesh->VertexBitangentLineSizeVertices(),
    vbo_fn, vao_fn, mesh->FaceNormalLineSizeVertices(),
    vbo_ft, vao_ft, mesh->FaceTangentLineSizeVertices(),
    vbo_fb, vao_fb, mesh->FaceBitangentLineSizeVertices());

  _meshObjects.insert(new_mesh_object);
  _meshObjectsAdded++;
  return new_mesh_object;
}

void MeshRenderer::Unload(MeshObject * mesh_object)
{
  // freeing mesh buffers
  glDeleteBuffers(1, &mesh_object->_vbo);
  glDeleteBuffers(1, &mesh_object->_ebo);
  glDeleteVertexArrays(1, &mesh_object->_vao);
  // freeing normal line buffers
  glDeleteBuffers(1, &mesh_object->_vboVertexNormal);
  glDeleteVertexArrays(1, &mesh_object->_vaoVertexNormal);
  glDeleteBuffers(1, &mesh_object->_vboFaceNormal);
  glDeleteVertexArrays(1, &mesh_object->_vaoFaceNormal);
  // removing mesh from mesh object set and de-allocating
  _meshObjects.erase(mesh_object);
  delete mesh_object;
}

void MeshRenderer::Render(MeshObject * mesh_object, ShaderType shader_type,
  const Math::Matrix4 & projection,
  const Math::Matrix4 & view, const Math::Matrix4 & model)
{
  // updating shader
  switch (shader_type)
  {
  // PHONG SHADING 
  case ShaderType::PHONG:
    _phongShader->Use();
    mesh_object->_material.SetUniforms(_phongShader);
    glUniformMatrix4fv(_phongShader->UProjection, 1, GL_TRUE, projection.array);
    glUniformMatrix4fv(_phongShader->UView, 1, GL_TRUE, view.array);
    glUniformMatrix4fv(_phongShader->UModel, 1, GL_TRUE, model.array);
    glUniform3f(_phongShader->UEmissiveColor,
      _emissiveColor._r, _emissiveColor._g, _emissiveColor._b);
    glUniform3f(_phongShader->UGlobalAmbientColor,
      _globalAmbientColor._r, _globalAmbientColor._g, _globalAmbientColor._b);
    glUniform3f(_phongShader->UFogColor, 
      _fogColor._r, _fogColor._g, _fogColor._b);
    glUniform1f(_phongShader->UNearPlane, _fogNear);
    glUniform1f(_phongShader->UFarPlane, _fogFar);
    break;
  // GOURAUD SHADING
  case ShaderType::GOURAUD:
    _gouraudShader->Use();
    glUniformMatrix4fv(_gouraudShader->UProjection, 1, GL_TRUE, projection.array);
    glUniformMatrix4fv(_gouraudShader->UView, 1, GL_TRUE, view.array);
    glUniformMatrix4fv(_gouraudShader->UModel, 1, GL_TRUE, model.array);
    glUniform3f(_gouraudShader->UEmissiveColor,
      _emissiveColor._r, _emissiveColor._g, _emissiveColor._b);
    glUniform3f(_gouraudShader->UGlobalAmbientColor,
      _globalAmbientColor._r, _globalAmbientColor._g, _globalAmbientColor._b);
    glUniform3f(_gouraudShader->UFogColor,
      _fogColor._r, _fogColor._g, _fogColor._b);
    glUniform1f(_gouraudShader->UNearPlane, _fogNear);
    glUniform1f(_gouraudShader->UFarPlane, _fogFar);
    break;
  // BLINN SHADING
  case ShaderType::BLINN:
    _blinnShader->Use();
    glUniformMatrix4fv(_blinnShader->UProjection, 1, GL_TRUE, projection.array);
    glUniformMatrix4fv(_blinnShader->UView, 1, GL_TRUE, view.array);
    glUniformMatrix4fv(_blinnShader->UModel, 1, GL_TRUE, model.array);
    glUniform3f(_blinnShader->UEmissiveColor,
      _emissiveColor._r, _emissiveColor._g, _emissiveColor._b);
    glUniform3f(_blinnShader->UGlobalAmbientColor,
      _globalAmbientColor._r, _globalAmbientColor._g, _globalAmbientColor._b);
    glUniform3f(_blinnShader->UFogColor,
      _fogColor._r, _fogColor._g, _fogColor._b);
    glUniform1f(_blinnShader->UNearPlane, _fogNear);
    glUniform1f(_blinnShader->UFarPlane, _fogFar);
    break;
  // SOLID SHADING
  case ShaderType::SOLID:
    _solidShader->Use();
    glUniformMatrix4fv(_solidShader->UProjection, 1, GL_TRUE, projection.array);
    glUniformMatrix4fv(_solidShader->UView, 1, GL_TRUE, view.array);
    glUniformMatrix4fv(_solidShader->UModel, 1, GL_TRUE, model.array);
    break;
  default:
    break;
  }
  // drawing mesh
  glBindVertexArray(mesh_object->_vao);
  if (mesh_object->_showWireframe)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else
    glPolygonMode(GL_FRONT, GL_FILL);
  glDrawElements(GL_TRIANGLES, mesh_object->_elements, GL_UNSIGNED_INT, nullptr);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBindVertexArray(0);

  // drawing normal lines
  if(mesh_object->_showVertexNormals || mesh_object->_showVertexTangents || 
    mesh_object->_showVertexBitangents || mesh_object->_showFaceNormals ||
    mesh_object->_showFaceTangents || mesh_object->_showFaceBitangents){
    _lineShader->Use();
    glUniformMatrix4fv(_lineShader->UProjection, 1, GL_TRUE, projection.array);
    glUniformMatrix4fv(_lineShader->UView, 1, GL_TRUE, view.array);
    glUniformMatrix4fv(_lineShader->UModel, 1, GL_TRUE, model.array);
  }
  if (mesh_object->_showVertexNormals) {
    // vertex normals
    DisplayLineBuffer(mesh_object->_vertexNormalColor,
      mesh_object->_vaoVertexNormal, mesh_object->_vertexNormalVertexCount);
  }
  if (mesh_object->_showVertexTangents) {
    // tangents
    DisplayLineBuffer(mesh_object->_vertexTangentColor,
      mesh_object->_vaoVertexTangent, mesh_object->_vertexTangentVertexCount);
  }
  if (mesh_object->_showVertexBitangents) {
    // bitangents
    DisplayLineBuffer(mesh_object->_vertexBitangentColor,
      mesh_object->_vaoVertexBitangent, mesh_object->_vertexBitangentVertexCount);
  }
  if (mesh_object->_showFaceNormals) {
    // face normals
    DisplayLineBuffer(mesh_object->_faceNormalColor,
      mesh_object->_vaoFaceNormal, mesh_object->_faceNormalVertexCount);
  }
  if (mesh_object->_showFaceTangents) {
    // face normals
    DisplayLineBuffer(mesh_object->_faceTangentColor,
      mesh_object->_vaoFaceTangent, mesh_object->_faceTangentVertexCount);
  }
  if (mesh_object->_showFaceBitangents) {
    // face normals
    DisplayLineBuffer(mesh_object->_faceBitangentColor,
      mesh_object->_vaoFaceBitangent, mesh_object->_faceBitangentVertexCount);
  }

}

void MeshRenderer::ReloadShader(ShaderType shader_type)
{
  // getting base shader type
  Shader * shader_to_reload;
  switch (shader_type)
  {
  case PHONG:
    shader_to_reload = _phongShader;
    break;
  case GOURAUD:
    shader_to_reload = _gouraudShader;
    break;
  case BLINN:
    shader_to_reload = _blinnShader;
    break;
  default:
    Error error("MeshRenderer.cpp", "ReloadShader");
    error.Add("ShaderType cannot be reloaded.");
    throw(error);
  }
  // disabling vertex attributes
  for (MeshObject * mesh_object : _meshObjects) {
    glBindVertexArray(mesh_object->_vao);
    shader_to_reload->DisableAttributes();
    glBindVertexArray(0);
  }
  // creating new shader
  switch (shader_type)
  {
  case PHONG:
    delete _phongShader;
    _phongShader = new PhongShader();
    shader_to_reload = _phongShader;
    break;
  case GOURAUD:
    delete _gouraudShader;
    _gouraudShader = new GouraudShader();
    shader_to_reload = _gouraudShader;
    break;
  case BLINN:
    delete _blinnShader;
    _blinnShader = new BlinnShader();
    shader_to_reload = _blinnShader;
    break;
  }
  // re-enabling vertex attributes
  for (MeshObject * mesh_object : _meshObjects) {
    glBindBuffer(GL_ARRAY_BUFFER, mesh_object->_vbo);
    glBindVertexArray(mesh_object->_vao);
    _phongShader->EnableAttributes();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

SolidShader * MeshRenderer::GetSolidShader()
{
  return _solidShader;
}

PhongShader * MeshRenderer::GetPhongShader()
{
  return _phongShader;
}

GouraudShader * MeshRenderer::GetGouraudShader()
{
  return _gouraudShader;
}

BlinnShader * MeshRenderer::GetBlinnShader()
{
  return _blinnShader;
}

LineShader * MeshRenderer::GetLineShader()
{
  return _lineShader;
}

int MeshRenderer::ShaderTypeToInt(ShaderType shader_type)
{
  switch (shader_type)
  {
  case ShaderType::PHONG:
    return 0;
  case ShaderType::GOURAUD:
    return 1;
  case ShaderType::BLINN:
    return 2;
  case ShaderType::SOLID:
    return 3;
  default:
    return 4;
  }
}

MeshRenderer::ShaderType MeshRenderer::IntToShaderType(int shader_int)
{
  switch (shader_int)
  {
  case 0:
    return ShaderType::PHONG;
  case 1:
    return ShaderType::GOURAUD;
  case 2:
    return ShaderType::BLINN;
  case 3:
    return ShaderType::SOLID;
  default:
    return ShaderType::NUMSHADERTYPES;
  }
}

inline void MeshRenderer::UploadLineBuffer(GLuint * vbo, GLuint * vao, 
  void * data, unsigned int data_size)
{
  glGenVertexArrays(1, vao);
  glGenBuffers(1, vbo);
  glBindVertexArray(*vao);
  glBindBuffer(GL_ARRAY_BUFFER, *vbo);
  glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);
  _lineShader->EnableAttributes();
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

inline void MeshRenderer::DisplayLineBuffer(const Color & color, GLuint vao,
  unsigned int num_vertices)
{
  glUniform3f(_lineShader->ULineColor, color._r, color._g, color._b);
  glBindVertexArray(vao);
  glDrawArrays(GL_LINES, 0, num_vertices);
  glBindVertexArray(0);
}
