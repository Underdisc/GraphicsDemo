/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
#include "Light.h"

// static initializations
int Light::_activeLights = 1;
const int Light::_typePoint = 0;
const int Light::_typeDirectional = 1;
const int Light::_typeSpot = 2;

Light::Light() : _type(0), _position(0.0f, 2.0f, 0.0f),
  _direction(0.0f, -1.0f, 0.0f), _innerAngle(PI / 12.0f),
  _outerAngle(PI / 6.0f), _spotExponent(1.0f),
  _ambientColor(0.0f, 0.0f, 0.0f), _diffuseColor(0.8f, 0.8f, 0.8f),
  _specularColor(1.0f, 1.0f, 1.0f), _attenuationC0(1.0f),
  _attenuationC1(0.1f), _attenuationC2(0.0f)
{}

Light::Light(unsigned int type, const Math::Vector3 & position,
  const Math::Vector3 & direction, const Color & ambient_color,
  const Color & diffuse_color, const Color & specular_color) :
  _type(type), _position(position), _direction(direction),
  _ambientColor(ambient_color), _diffuseColor(diffuse_color),
  _specularColor(specular_color)
{}

void Light::SetUniforms(unsigned int light_index, PhongShader * phong_shader)
{
  glUniform1i(phong_shader->ULights[light_index].UType, _type);
  glUniform3f(phong_shader->ULights[light_index].UPosition,
    _position.x, _position.y, _position.z);
  glUniform3f(phong_shader->ULights[light_index].UDirection,
    _direction.x, _direction.y, _direction.z);
  glUniform1f(phong_shader->ULights[light_index].UInnerAngle,
    _innerAngle);
  glUniform1f(phong_shader->ULights[light_index].UOuterAngle,
    _outerAngle);
  glUniform1f(phong_shader->ULights[light_index].USpotExponent,
    _spotExponent);
  glUniform3f(phong_shader->ULights[light_index].UAmbientColor,
    _ambientColor._x, _ambientColor._y, _ambientColor._z);
  glUniform3f(phong_shader->ULights[light_index].UDiffuseColor,
    _diffuseColor._x, _diffuseColor._y, _diffuseColor._z);
  glUniform3f(phong_shader->ULights[light_index].USpecularColor,
    _specularColor._x, _specularColor._y, _specularColor._z);
  glUniform1f(phong_shader->ULights[light_index].UAttenuationC0,
    _attenuationC0);
  glUniform1f(phong_shader->ULights[light_index].UAttenuationC1,
    _attenuationC1);
  glUniform1f(phong_shader->ULights[light_index].UAttenuationC2,
    _attenuationC2);
}
void Light::SetUniforms(unsigned int light_index, GouraudShader * gouraud_shader)
{
  glUniform1i(gouraud_shader->ULights[light_index].UType, _type);
  glUniform3f(gouraud_shader->ULights[light_index].UPosition,
    _position.x, _position.y, _position.z);
  glUniform3f(gouraud_shader->ULights[light_index].UDirection,
    _direction.x, _direction.y, _direction.z);
  glUniform1f(gouraud_shader->ULights[light_index].UInnerAngle,
    _innerAngle);
  glUniform1f(gouraud_shader->ULights[light_index].UOuterAngle,
    _outerAngle);
  glUniform1f(gouraud_shader->ULights[light_index].USpotExponent,
    _spotExponent);
  glUniform3f(gouraud_shader->ULights[light_index].UAmbientColor,
    _ambientColor._x, _ambientColor._y, _ambientColor._z);
  glUniform3f(gouraud_shader->ULights[light_index].UDiffuseColor,
    _diffuseColor._x, _diffuseColor._y, _diffuseColor._z);
  glUniform3f(gouraud_shader->ULights[light_index].USpecularColor,
    _specularColor._x, _specularColor._y, _specularColor._z);
  glUniform1f(gouraud_shader->ULights[light_index].UAttenuationC0,
    _attenuationC0);
  glUniform1f(gouraud_shader->ULights[light_index].UAttenuationC1,
    _attenuationC1);
  glUniform1f(gouraud_shader->ULights[light_index].UAttenuationC2,
    _attenuationC2);
}
void Light::SetUniforms(unsigned int light_index, BlinnShader * blinn_shader)
{
  glUniform1i(blinn_shader->ULights[light_index].UType, _type);
  glUniform3f(blinn_shader->ULights[light_index].UPosition,
    _position.x, _position.y, _position.z);
  glUniform3f(blinn_shader->ULights[light_index].UDirection,
    _direction.x, _direction.y, _direction.z);
  glUniform1f(blinn_shader->ULights[light_index].UInnerAngle,
    _innerAngle);
  glUniform1f(blinn_shader->ULights[light_index].UOuterAngle,
    _outerAngle);
  glUniform1f(blinn_shader->ULights[light_index].USpotExponent,
    _spotExponent);
  glUniform3f(blinn_shader->ULights[light_index].UAmbientColor,
    _ambientColor._x, _ambientColor._y, _ambientColor._z);
  glUniform3f(blinn_shader->ULights[light_index].UDiffuseColor,
    _diffuseColor._x, _diffuseColor._y, _diffuseColor._z);
  glUniform3f(blinn_shader->ULights[light_index].USpecularColor,
    _specularColor._x, _specularColor._y, _specularColor._z);
  glUniform1f(blinn_shader->ULights[light_index].UAttenuationC0,
    _attenuationC0);
  glUniform1f(blinn_shader->ULights[light_index].UAttenuationC1,
    _attenuationC1);
  glUniform1f(blinn_shader->ULights[light_index].UAttenuationC2,
    _attenuationC2);
}