/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#version 330 core

#define LIGHT_POINT 0
#define LIGHT_DIRECTIONAL 1
#define LIGHT_SPOT 2

#define MAP_SPHERICAL 0
#define MAP_CYLINDRICAL 1
#define MAP_PLANAR 2

#define ENVIRONMENT_REFLECT 0
#define ENVIRONMENT_REFRACT 1

in vec3 SNormal;
in vec3 STangent;
in vec3 SBitangent;
in vec3 SFragPos;
in vec3 SModelNormal;
in vec3 SModelPos;
in vec2 SUV;

out vec4 OFragColor;

// Material values
struct Material
{
  vec3 UColor;
  // Material factors
  float UAmbientFactor;
  float UDiffuseFactor;
  float USpecularFactor;
  float USpecularExponent;
  float UEnvironmentFactor;
  float URefractionIndex;
  bool UChromaticAbberation;
  float UChromaticOffset;
  bool UFresnelReflection;
  float UFresnelRatio;
  // texture mapping
  bool UTextureMapping;
  bool USpecularMapping;
  bool UNormalMapping;
  bool UEnvironmentMapping;
  int UMappingType;
  // samplers
  sampler2D UDiffuseMap;  // location 0
  sampler2D USpecularMap; // location 1
  sampler2D UNormalMap;   // location 2
  sampler2D UUp; // location 3
  sampler2D UDown; // location 4
  sampler2D ULeft; // location 5
  sampler2D URight; // location 6
  sampler2D UFront; // location 7
  sampler2D UBack; // location 8

};

// Light values
struct Light
{
  // The type of the type: 0 - Point, 1 - Directional, 2 - Spot
  int UType;
  // The position of the light
  vec3 UPosition;
  // The direction of the light
  vec3 UDirection;
  // The inner and outer angles for a spotlight
  float UInnerAngle;
  float UOuterAngle;
  float USpotExponent;
  // The light colors
  vec3 UAmbientColor;
  vec3 UDiffuseColor;
  vec3 USpecularColor;
  // Attenuation coefficients
  float UAttenuationC0;
  float UAttenuationC1;
  float UAttenuationC2;
};

const int MaxLights = 10;
uniform int UActiveLights = 1;
uniform Material UMaterial;
uniform Light ULights[MaxLights];

uniform vec3 UFogColor;
uniform float UNearPlane;
uniform float UFarPlane;

uniform vec3 UEmissiveColor;
uniform vec3 UGlobalAmbientColor;

uniform vec3 UCameraPosition;

vec2 PerformPlanarMapping(vec3 direction){
  vec3 da = abs(direction);
  vec2 uv;
  // X mapping
  if(da.x > da.y && da.x > da.z){
    if(direction.x > 0){
      uv.x = (direction.z / da.x + 1.0) / 2.0;
      uv.y = (direction.y / da.x + 1.0) / 2.0;
    }
    else{
      uv.x = (direction.z / da.x + 1.0) / 2.0;
      uv.y = (direction.y / da.x + 1.0) / 2.0;
    }
  }
  // Y mapping
  else if(da.y > da.x && da.y > da.z){
    if(direction.y > 0){
      uv.x = (direction.x / da.y + 1.0) / 2.0;
      uv.y = (direction.z / da.y + 1.0) / 2.0;
    }
    else{
      uv.x = (direction.x / da.y + 1.0) / 2.0;
      uv.y = (direction.z / da.y + 1.0) / 2.0;
    }

  }
  // Z mapping
  else if(da.z > da.x && da.z > da.y){
    if(direction.z > 0){
      uv.x = (direction.x / da.z + 1.0) / 2.0;
      uv.y = (direction.y / da.z + 1.0) / 2.0;
    }
    else{
      uv.x = (direction.x / da.z + 1.0) / 2.0;
      uv.y = (direction.y / da.z + 1.0) / 2.0;
    }
  }
  return uv;
}

/******************************************************************************/
/*
  Computes the uv depending on the mapping.
*/
/******************************************************************************/
vec2 ComputeUVs()
{
  vec2 uv;
  float pi = 3.14159265359;
  float theta;
  float phi;
  switch(UMaterial.UMappingType)
  {
    // spherical mapping method
    case MAP_SPHERICAL:
      theta = atan(SModelPos.x, SModelPos.z);
      phi = acos(-1.0 * SModelPos.y);
      uv.x = (theta + pi) / (pi * 2.0);
      uv.y = phi / pi;
      break;
    // cylindrical mapping method
    case MAP_CYLINDRICAL:
      theta = atan(SModelPos.x, SModelPos.z);
      uv.x = (theta + pi) / (pi * 2.0);
      uv.y = (SModelPos.y + 1.0) / 2.0;
      break;
    // planar mapping method
    case MAP_PLANAR:
      uv = PerformPlanarMapping(SModelPos);
      break;
    default:
      break;
  }
  return uv;
}

vec3 GetEnvironmentColor(vec3 direction, vec2 uv)
{
  vec3 da = abs(direction);
  if(da.x > da.y && da.x > da.z){
    if(direction.x > 0)
      return texture(UMaterial.URight, uv).xyz;
    else{
      uv.x = 1.0 - uv.x;
      return texture(UMaterial.ULeft, uv).xyz;
    }
  }
  else if(da.y > da.x && da.y > da.z){
    if(direction.y > 0)
      return texture(UMaterial.UUp, uv).xyz;
    else{
      uv.y = 1.0 - uv.y;
      return texture(UMaterial.UDown, uv).xyz;
    }
  }
  else if(da.z > da.x && da.z > da.y){
    if(direction.z > 0){
      uv.x = 1.0 - uv.x;
      return texture(UMaterial.UBack, uv).xyz;
    }
    else
      return texture(UMaterial.UFront, uv).xyz;
  }
}

vec3 GetRefractColor(float refraction_index, vec3 normal, vec3 view_dir)
{
  float ndotv = dot(normal, view_dir);
  float ndotv_2 = ndotv * ndotv;
  float ir = 1.0 / refraction_index;
  float ir_2 = ir * ir;
  vec3 refract_view_dir = (ir * ndotv - sqrt(1.0 - ir_2 * (1.0 - ndotv_2))) * normal - ir * view_dir;
  vec2 refract_uv = PerformPlanarMapping(refract_view_dir);
  return GetEnvironmentColor(refract_view_dir, refract_uv);
}

vec3 EnvironmentMap(vec3 normal, vec3 view_dir)
{

  vec3 reflect_view_dir = 2.0 * dot(normal, view_dir) * normal - view_dir;
  vec2 reflect_uv = PerformPlanarMapping(reflect_view_dir);
  vec3 reflect_environment_color = GetEnvironmentColor(reflect_view_dir, reflect_uv);

  vec3 refract_environment_color;
  if(UMaterial.UChromaticAbberation){
    refract_environment_color.r = GetRefractColor(UMaterial.URefractionIndex - UMaterial.UChromaticOffset, normal, view_dir).r;
    refract_environment_color.g = GetRefractColor(UMaterial.URefractionIndex, normal, view_dir).g;
    refract_environment_color.b = GetRefractColor(UMaterial.URefractionIndex + UMaterial.UChromaticOffset, normal, view_dir).b;
  }
  else{
    refract_environment_color = GetRefractColor(UMaterial.URefractionIndex, normal, view_dir);
  }

  float fresnel_ratio;
  if(UMaterial.UFresnelReflection){
    // I understand what this is trying to accomplish but I do not fully understand
    // how it works
    float ndotv = dot(normal, view_dir);
    float ir = 1.0 / UMaterial.URefractionIndex;
    float fresnel_value = ((1.0-ir) * (1.0-ir)) / ((1.0+ir) * (1.0+ir));
    fresnel_ratio = fresnel_value + (1.0 - fresnel_value) * pow((1.0 - ndotv), 5.0);
  }
  else{
    fresnel_ratio = UMaterial.UFresnelRatio;
  }


  return mix(refract_environment_color, reflect_environment_color, fresnel_ratio);
}

/******************************************************************************/
/*
  Computes the final vec3 produced by a light.
*/
/******************************************************************************/
vec3 ComputeLight(int light, vec3 normal, vec3 view_dir, vec2 uv)
{
  // ambient term
  vec3 ambient_color = UMaterial.UAmbientFactor * ULights[light].UAmbientColor;
  // finding light direction
  vec3 light_vec;
  vec3 light_dir;
  if(ULights[light].UType == LIGHT_DIRECTIONAL)
    light_vec = -normalize(ULights[light].UDirection);
  else
    light_vec = ULights[light].UPosition - SFragPos;
  light_dir = normalize(light_vec);

  // diffuse term
  float ndotl = max(dot(normal, light_dir), 0.0);
  vec3 diffuse_color;
  if(UMaterial.UTextureMapping){
    diffuse_color = ndotl * texture(UMaterial.UDiffuseMap, uv).xyz *
      ULights[light].UDiffuseColor;
  }
  else {
    diffuse_color = ndotl * UMaterial.UDiffuseFactor *
      ULights[light].UDiffuseColor;
  }

  // specular term
  vec3 reflect_dir = 2.0 * dot(normal, light_dir) * normal - light_dir;
  reflect_dir = normalize(reflect_dir);
  float vdotr = max(dot(view_dir, reflect_dir), 0.0);
  float specular_spread = pow(vdotr, UMaterial.USpecularExponent);
  vec3 specular_color;
  if(UMaterial.USpecularMapping){
    specular_color = texture(UMaterial.USpecularMap,uv).xyz *
      ULights[light].USpecularColor * specular_spread;
  }
  else{
    specular_color = UMaterial.USpecularFactor *
      ULights[light].USpecularColor * specular_spread;
  }

  // find spotlight effect
  float spotlight_factor;
  if(ULights[light].UType == LIGHT_SPOT){
    float cos_inner = cos(ULights[light].UInnerAngle);
    float cos_outer = cos(ULights[light].UOuterAngle);
    vec3 spot_dir = -normalize(ULights[light].UDirection);
    float ldots = dot(light_dir, spot_dir);
    spotlight_factor = (ldots - cos_outer) / (cos_inner - cos_outer);
    spotlight_factor = pow(spotlight_factor, ULights[light].USpotExponent);
    spotlight_factor = max(0.0, spotlight_factor);
    spotlight_factor = min(1.0, spotlight_factor);
  }
  else{
    spotlight_factor = 1.0f;
  }

  // find attenuation
  float attenuation;
  if(ULights[light].UType == LIGHT_DIRECTIONAL)
    attenuation = 1.0;
  else{
    float light_dist = length(light_vec);
    attenuation = ULights[light].UAttenuationC0 +
      ULights[light].UAttenuationC1 * light_dist +
      ULights[light].UAttenuationC2 * light_dist * light_dist;
    attenuation = min(1.0 / attenuation, 1.0);
  }
  // final color
  vec3 spot_color = spotlight_factor * (diffuse_color + specular_color);
  vec3 light_color = (ambient_color + spot_color);
  light_color = attenuation * light_color;
  return light_color;
}

/******************************************************************************/
/*
  Entry point
*/
/******************************************************************************/
void main()
{
  // perform texture mapping
  vec2 uv;
  if(UMaterial.UTextureMapping || UMaterial.USpecularMapping ||
    UMaterial.UNormalMapping)
    uv = ComputeUVs();
  // lighting
  // precomputations
  vec3 normal;
  if(UMaterial.UNormalMapping){
    mat3 tbn = mat3(STangent, SBitangent, SNormal);
    normal = texture(UMaterial.UNormalMap, uv).xyz;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(tbn * normal);
  }
  else{
    normal = normalize(SNormal);
  }
  vec3 view_vec = UCameraPosition - SFragPos;
  vec3 view_dir = normalize(view_vec);
  // summing all light results
  vec3 final_color = vec3(0.0, 0.0, 0.0);
  for (int i = 0; i < UActiveLights; ++i)
    final_color += ComputeLight(i, normal, view_dir, uv);
  // accounting for object color
  final_color *= UMaterial.UColor;
  // accounting for emissive and global ambient
  final_color += UMaterial.UAmbientFactor * UGlobalAmbientColor;
  final_color += UEmissiveColor;
  // account for environment mapping
  if(UMaterial.UEnvironmentMapping){
    vec3 environment_color = EnvironmentMap(normal, view_dir);
    final_color = mix(final_color, environment_color, UMaterial.UEnvironmentFactor);
  }
  // accounting for fog
  float dist = length(view_vec);
  float fog_factor = (dist - UNearPlane) / (UFarPlane - UNearPlane);
  fog_factor = min(1.0, max(0.0, fog_factor));
  final_color = mix(final_color, UFogColor, fog_factor);
  // final color
  OFragColor = vec4(final_color, 1.0);
  //OFragColor = texture(UMaterial.UNormalMap, uv);
  //OFragColor = vec4(STangent, 1.0);
}
