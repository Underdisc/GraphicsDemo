/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
#pragma once


#include "../Math/Vector3.h"
#include "Shader/ShaderLibrary.h"
#include "Color.h"

#define PI 3.141592653589f
#define PI2 6.28318530718f

struct Light
{
  Light();
  Light(unsigned int type, const Math::Vector3 & position,
    const Math::Vector3 & direction, const Color & ambient_color,
    const Color & diffuse_color, const Color & specular_color);
  int _type;
  Math::Vector3 _position;
  Math::Vector3 _direction;
  float _innerAngle;
  float _outerAngle;
  float _spotExponent;
  Color _ambientColor;
  Color _diffuseColor;
  Color _specularColor;
  float _attenuationC0;
  float _attenuationC1;
  float _attenuationC2;

  static const int _typePoint;
  static const int _typeDirectional;
  static const int _typeSpot;

  static int _activeLights;
  void SetUniforms(unsigned int light_index, PhongShader * phong_shader);
  void SetUniforms(unsigned int light_index, GouraudShader * gouraud_shader);
  void SetUniforms(unsigned int light_index, BlinnShader * blinn_shader);
};