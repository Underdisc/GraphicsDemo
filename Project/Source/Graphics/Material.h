/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
#pragma once

#define MAPSPHERICAL 0
#define MAPCYLINDRICAL 1
#define MAPPLANAR 2

#define ENVIRONMENT_REFLECT 0
#define ENVIRONMENT_REFRACT 1

#include "Color.h"
#include "Shader/ShaderLibrary.h"

struct Material
{
  Material();
  void SetUniforms(PhongShader * phong_shader);
  void SetUniforms(GouraudShader * gouraud_shader);
  void SetUniforms(BlinnShader * blinn_shader);
  Color _color;
  // Material factors
  float _ambientFactor;
  float _diffuseFactor;
  float _specularFactor;
  float _specularExponent;
  float _environmentFactor;
  float _refractionIndex;
  bool _chromaticAbberation;
  float _chromaticOffset;
  bool _fresnelReflection;
  float _fresnelRatio;
  // texture mapping
  bool _textureMapping;
  bool _specularMapping;
  bool _normalMapping;
  bool _environmentMapping;
  int _mappingType;
  // samplers
  int _diffuseMap;
  int _specularMap;
  int _normalMap;
  int _up;
  int _down; 
  int _left; 
  int _right;
  int _front;
  int _back;
};