/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#pragma once
//FLEEB
//mesh objects should have the material
#include <unordered_set>
#include <GL\glew.h>
 
#include "../../Math/Matrix4.h"
#include "../Shader/ShaderLibrary.h"
#include "../Color.h"
#include "../Material.h"
#include "Mesh.h"

/*****************************************************************************/
/*!
\class MeshRenderer
\brief
  Used for rendering a mesh, the vertex normals of the mesh, and the face
  normals of the mesh.
*/
/*****************************************************************************/
class MeshRenderer
{
public:
  /***************************************************************************/
  /*!
  \class MeshObject
  \brief
    Contains all of the identifiers for the OpenGL buffers associated with
    a mesh. This also contains bools that represent how the mesh will
    be displayed.
  */
  /***************************************************************************/
  struct MeshObject
  {
    MeshObject(GLuint vbo, GLuint ebo, GLuint vao, unsigned int elements,
      GLuint vbo_vn, GLuint vao_vn, unsigned int vertices_vn,
      GLuint vbo_vt, GLuint vao_vt, unsigned int vertices_vt,
      GLuint vbo_vb, GLuint vao_vb, unsigned int vertices_vb,
      GLuint vbo_fn, GLuint vao_fn, unsigned int vertices_fn,
      GLuint vbo_ft, GLuint vao_ft, unsigned int vertices_ft,
      GLuint vbo_fb, GLuint vao_fb, unsigned int vertices_fb):
      _vbo(vbo), _ebo(ebo), _vao(vao), _elements(elements),
      _vboVertexNormal(vbo_vn), _vaoVertexNormal(vao_vn),
      _vertexNormalVertexCount(vertices_vn),
      _vboVertexTangent(vbo_vt), _vaoVertexTangent(vao_vt),
      _vertexTangentVertexCount(vertices_vt),
      _vboVertexBitangent(vbo_vb), _vaoVertexBitangent(vao_vb),
      _vertexBitangentVertexCount(vertices_vb),
      _vboFaceNormal(vbo_fn), _vaoFaceNormal(vao_fn),
      _faceNormalVertexCount(vertices_fn),
      _vboFaceTangent(vbo_ft), _vaoFaceTangent(vao_ft),
      _faceTangentVertexCount(vertices_ft),
      _vboFaceBitangent(vbo_fb), _vaoFaceBitangent(vao_fb),
      _faceBitangentVertexCount(vertices_fb),
      _showVertexNormals(false), _showVertexTangents(false), 
      _showVertexBitangents(false),
      _showFaceNormals(false), _showFaceTangents(false),
      _showFaceBitangents(false),
      _showWireframe(false),
      _vertexNormalColor(0.0f, 0.0f, 1.0f),
      _vertexTangentColor(1.0f, 0.0f, 0.0f), 
      _vertexBitangentColor(0.0f, 1.0f, 0.0f),
      _faceNormalColor(0.0f, 0.0f, 1.0f),
      _faceTangentColor(1.0f, 0.0f, 0.0f),
      _faceBitangentColor(0.0f, 1.0f, 0.0f)
      {}
    //! VBO for the mesh
    GLuint _vbo;
    //! EBO for the mesh
    GLuint _ebo;
    //! VAO forn the mesh
    GLuint _vao;
    //! The number of elements in the EBO
    unsigned int _elements;
    //! Vertex Normal line buffer info
    GLuint _vboVertexNormal;
    GLuint _vaoVertexNormal;
    unsigned int _vertexNormalVertexCount;
    // Vertex Tangent line buffer info
    GLuint _vboVertexTangent;
    GLuint _vaoVertexTangent;
    unsigned int _vertexTangentVertexCount;
    // Vertex Bitangent line buffer info
    GLuint _vboVertexBitangent;
    GLuint _vaoVertexBitangent;
    unsigned int _vertexBitangentVertexCount;

    // Face Normal line buffer info
    GLuint _vboFaceNormal;
    GLuint _vaoFaceNormal;
    unsigned int _faceNormalVertexCount;
    // Face Tangent line buffer info
    GLuint _vboFaceTangent;
    GLuint _vaoFaceTangent;
    unsigned int _faceTangentVertexCount;
    // Face Bitangent line buffer info
    GLuint _vboFaceBitangent;
    GLuint _vaoFaceBitangent;
    unsigned int _faceBitangentVertexCount;

    //! Determines whether the vertex normals should be displayed
    bool _showVertexNormals;
    bool _showVertexTangents;
    bool _showVertexBitangents;
    //! Determines whether the face normals should be displayed
    bool _showFaceNormals;
    bool _showFaceTangents;
    bool _showFaceBitangents;

    //! Determines whether the model is displayed in wireframe mode or not
    bool _showWireframe;

    Material _material;

    //! The color of the vertex normals
    Color _vertexNormalColor;
    Color _vertexTangentColor;
    Color _vertexBitangentColor;
    //! The color of the face normals
    Color _faceNormalColor;
    Color _faceTangentColor;
    Color _faceBitangentColor;

  };
public:
  enum ShaderType
  {
    PHONG,
    GOURAUD,
    BLINN,
    SOLID,
    NUMSHADERTYPES
  };
public:
  static Color _emissiveColor;
  static Color _globalAmbientColor;
  static Color _fogColor;
  static float _fogNear;
  static float _fogFar;
  static float _nearPlane;
  static float _farPlane;
public:
  static void Initialize();
  static void Purge();
  static MeshObject * Upload(Mesh * mesh);
  static void Unload(MeshObject * mesh_object);
  static void Render(MeshObject * mesh_object, ShaderType shader_type,
    const Math::Matrix4 & projection, const Math::Matrix4 & view, 
    const Math::Matrix4 & model);
  static void ReloadShader(ShaderType shader_type);
  static SolidShader * GetSolidShader();
  static PhongShader * GetPhongShader();
  static GouraudShader * GetGouraudShader();
  static BlinnShader * GetBlinnShader();
  static LineShader * GetLineShader();
  static int ShaderTypeToInt(ShaderType shader_type);
  static ShaderType IntToShaderType(int shader_int);
private:
  static void UploadLineBuffer(GLuint * vbo, GLuint * vao, void * data, 
    unsigned int data_size);
  static void DisplayLineBuffer(const Color & color, GLuint vao, 
    unsigned int num_vertices);
  //! The vector of currently loaded Mesh objects
  static std::unordered_set<MeshObject *> _meshObjects;
  //! The number of mesh objects that have been added to the MeshRenderer
  static unsigned int _meshObjectsAdded;
  //! The shader used for drawing vertex and face normals
  static LineShader * _lineShader;
  //! The shader used for drawing single color meshes
  static SolidShader * _solidShader;
  //! The shader used for Phong
  static PhongShader * _phongShader;
  //! The shader used for Gouraud
  static GouraudShader * _gouraudShader;
  //! The shader used for Blinn
  static BlinnShader * _blinnShader;
};