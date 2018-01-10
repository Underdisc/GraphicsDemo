/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
#include "Material.h"

Material::Material() : _color(1.0f, 1.0f, 1.0f), _ambientFactor(0.1f),
_diffuseFactor(1.0f), _specularFactor(1.0f), _specularExponent(3.0f),
_environmentFactor(1.0f), _refractionIndex(1.66f), _chromaticAbberation(false),
_chromaticOffset(0.0f), _fresnelReflection(false), _fresnelRatio(0.5),
_textureMapping(false), _specularMapping(false), 
_normalMapping(false), _environmentMapping(true), _mappingType(MAPSPHERICAL), 
_diffuseMap(0), _specularMap(1), 
_normalMap(2), _up(3), _down(4), _left(5), _right(6), _front(7), _back(8)
{}
void Material::SetUniforms(PhongShader * phong_shader)
{
  glUniform3f(phong_shader->UMaterial.UColor,
    _color._r, _color._g, _color._b);
  // Material Factor s
  glUniform1f(phong_shader->UMaterial.UAmbientFactor, _ambientFactor);
  glUniform1f(phong_shader->UMaterial.UDiffuseFactor, _diffuseFactor);
  glUniform1f(phong_shader->UMaterial.USpecularFactor, _specularFactor);
  glUniform1f(phong_shader->UMaterial.USpecularExponent, _specularExponent);
  glUniform1f(phong_shader->UMaterial.UEnvironmentFactor, _environmentFactor);
  glUniform1f(phong_shader->UMaterial.URefractionIndex, _refractionIndex);
  glUniform1i(phong_shader->UMaterial.UChromaticAbberation, _chromaticAbberation);
  glUniform1f(phong_shader->UMaterial.UChromaticOffset, _chromaticOffset);
  glUniform1i(phong_shader->UMaterial.UFresnelReflection, _fresnelReflection);
  glUniform1f(phong_shader->UMaterial.UFresnelRatio, _fresnelRatio);
  // Mapping Types
  glUniform1i(phong_shader->UMaterial.UTextureMapping, _textureMapping);
  glUniform1i(phong_shader->UMaterial.USpecularMapping, _specularMapping);
  glUniform1i(phong_shader->UMaterial.UNormalMapping, _normalMapping);
  glUniform1i(phong_shader->UMaterial.UEnvironmentMapping, _environmentMapping);
  glUniform1i(phong_shader->UMaterial.UMappingType, _mappingType);
  // Samplers
  glUniform1i(phong_shader->UMaterial.UDiffuseMap, _diffuseMap);
  glUniform1i(phong_shader->UMaterial.USpecularMap, _specularMap);
  glUniform1i(phong_shader->UMaterial.UNormalMap, _normalMap);
  glUniform1i(phong_shader->UMaterial.UUp, _up);
  glUniform1i(phong_shader->UMaterial.UDown, _down);
  glUniform1i(phong_shader->UMaterial.ULeft, _left);
  glUniform1i(phong_shader->UMaterial.URight, _right);
  glUniform1i(phong_shader->UMaterial.UFront, _front);
  glUniform1i(phong_shader->UMaterial.UBack, _back);
}
void Material::SetUniforms(GouraudShader * gouraud_shader)
{
  glUniform3f(gouraud_shader->UMaterial.UColor,
    _color._r, _color._g, _color._b);
  glUniform1f(gouraud_shader->UMaterial.UAmbientFactor, _ambientFactor);
  glUniform1f(gouraud_shader->UMaterial.UDiffuseFactor, _diffuseFactor);
  glUniform1f(gouraud_shader->UMaterial.USpecularFactor, _specularFactor);
  glUniform1f(gouraud_shader->UMaterial.USpecularExponent, _specularExponent);
}
void Material::SetUniforms(BlinnShader * blinn_shader)
{
  glUniform3f(blinn_shader->UMaterial.UColor,
    _color._r, _color._g, _color._b);
  glUniform1f(blinn_shader->UMaterial.UAmbientFactor, _ambientFactor);
  glUniform1f(blinn_shader->UMaterial.UDiffuseFactor, _diffuseFactor);
  glUniform1f(blinn_shader->UMaterial.USpecularFactor, _specularFactor);
  glUniform1f(blinn_shader->UMaterial.USpecularExponent, _specularExponent);
}