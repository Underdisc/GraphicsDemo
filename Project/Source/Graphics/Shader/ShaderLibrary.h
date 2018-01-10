/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file ShaderLibrary.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: CS 300
\date 02/10/2017
\brief
  A library of shaders that can be used for different drawing techniques.
*/
/*****************************************************************************/
#pragma once

#include "Shader.h"

#define MAXLIGHTS 10

//----------// Uniform Blocks //----------//


struct UMaterial
{
  // Material Factors
  GLuint UColor;
  GLuint UAmbientFactor;
  GLuint UDiffuseFactor;
  GLuint USpecularFactor;
  GLuint USpecularExponent;
  GLuint UEnvironmentFactor;
  GLuint URefractionIndex;
  GLuint UChromaticAbberation;
  GLuint UChromaticOffset;
  GLuint UFresnelReflection;
  GLuint UFresnelRatio;
  // Texture Mapping Properties
  GLuint UTextureMapping;
  GLuint USpecularMapping;
  GLuint UNormalMapping;
  GLuint UEnvironmentMapping;
  GLuint UMappingType;
  // Samplers
  GLuint UDiffuseMap;
  GLuint USpecularMap;
  GLuint UNormalMap;
  GLuint UUp;
  GLuint UDown;
  GLuint ULeft;
  GLuint URight;
  GLuint UFront;
  GLuint UBack;
};


struct ULight
{
  GLuint UType;
  GLuint UPosition;
  GLuint UDirection;
  GLuint UInnerAngle;
  GLuint UOuterAngle;
  GLuint USpotExponent;
  GLuint UAmbientColor;
  GLuint UDiffuseColor;
  GLuint USpecularColor;
  GLuint UAttenuationC0;
  GLuint UAttenuationC1;
  GLuint UAttenuationC2;
};

/*****************************************************************************/
/*!
\class LineShader
\brief
  Used for drawing single lines.
*/
/*****************************************************************************/
class LineShader : public Shader
{
public:
  virtual void EnableAttributes();
  virtual void DisableAttributes();
public:
  LineShader();
  // Attributes
  GLuint APosition;
  // Uniforms
  GLuint UProjection;
  GLuint UView;
  GLuint UModel;
  GLuint ULineColor;
};



class SolidShader : public Shader
{
public:
  virtual void EnableAttributes();
  virtual void DisableAttributes();
public:
  SolidShader();
  // Attributes
  GLuint APosition;
  // Uniforms
  GLuint UProjection;
  GLuint UView;
  GLuint UModel;
  GLuint UColor;
};

class SkyboxShader : public Shader
{
public:
  virtual void EnableAttributes();
  virtual void DisableAttributes();
public:
  SkyboxShader();
  // Attributes
  GLuint APosition;
  // Uniforms
  GLuint UProjection;
  GLuint UView;
  GLuint UUp;
  GLuint UDown;
  GLuint ULeft;
  GLuint URight;
  GLuint UFront;
  GLuint UBack;
};

/*****************************************************************************/
/*!
\class PhongShader
\brief
  Used for drawing objects that undergo phong shading. Supports lighting
  with up to 10 lights.
*/
/*****************************************************************************/
class PhongShader : public Shader
{
public:
  PhongShader();
  virtual void EnableAttributes();
  virtual void DisableAttributes();
  // Attributes
  GLuint APosition;
  GLuint ANormal;
  GLuint ATangent;
  GLuint ABitangent;
  GLuint AUV;
  // Uniforms
  GLuint UProjection;
  GLuint UView;
  GLuint UModel;
  GLuint UCameraPosition;
  GLuint UEmissiveColor;
  GLuint UGlobalAmbientColor;
  // Material Uniform
  UMaterial UMaterial;
  // Light Uniforms
  GLuint UActiveLights;
  ULight ULights[MAXLIGHTS];
  // Fog Uniforms
  GLuint UFogColor;
  GLuint UNearPlane;
  GLuint UFarPlane;
};

/*****************************************************************************/
/*!
\class GouraudShader
\brief
  Used for drawing objects that undergo gouraud shading. Can be used for
  shading with up to 10 lights.
*/
/*****************************************************************************/
class GouraudShader : public Shader
{
public:
  GouraudShader();
  virtual void EnableAttributes();
  virtual void DisableAttributes();
  // Attributes
  GLuint APosition;
  GLuint ANormal;
  // Uniforms
  GLuint UProjection;
  GLuint UView;
  GLuint UModel;
  GLuint UCameraPosition;
  GLuint UEmissiveColor;
  GLuint UGlobalAmbientColor;
  // Material Uniform
  UMaterial UMaterial;
  // Light Uniforms
  GLuint UActiveLights;
  ULight ULights[MAXLIGHTS];
  // Fog Uniforms
  GLuint UFogColor;
  GLuint UNearPlane;
  GLuint UFarPlane;
};

/*****************************************************************************/
/*!
\class PhongShader
\brief
Used for drawing objects that undergo phong shading. Supports lighting
with up to 10 lights.
*/
/*****************************************************************************/
class BlinnShader : public Shader
{
public:
  BlinnShader();
  virtual void EnableAttributes();
  virtual void DisableAttributes();
  // Attributes
  GLuint APosition;
  GLuint ANormal;
  // Uniforms
  GLuint UProjection;
  GLuint UView;
  GLuint UModel;
  GLuint UCameraPosition;
  GLuint UEmissiveColor;
  GLuint UGlobalAmbientColor;
  // Material Uniform
  UMaterial UMaterial;
  // Light Uniforms
  GLuint UActiveLights;
  ULight ULights[MAXLIGHTS];
  // Fog Uniforms
  GLuint UFogColor;
  GLuint UNearPlane;
  GLuint UFarPlane;
};


/*****************************************************************************/
/*!
\class TextureShader
\brief
  Used for displaying a single texture on a quad.
*/
/*****************************************************************************/
class TextureShader : public Shader
{
public:
  TextureShader();
  // Attributes
  GLuint APosition;
  GLuint ATexCoord;
  // Uniforms
  GLuint UProjection;
  GLuint UView;
  GLuint UModel;
  GLuint UTexture;
};
