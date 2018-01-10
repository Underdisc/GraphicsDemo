/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#version 330 core

#define POINT 0
#define DIRECTIONAL 1
#define SPOT 2

in vec3 APosition;
in vec3 ANormal;

out vec4 SFragColor;

uniform mat4 UProjection = mat4(1,0,0,0,
                                0,1,0,0,
                                0,0,1,0,
                                0,0,0,1);

uniform mat4 UView = mat4(1,0,0,0,
                          0,1,0,0,
                          0,0,1,0,
                          0,0,0,1);

uniform mat4 UModel = mat4(1,0,0,0,
                           0,1,0,0,
                           0,0,1,0,
                           0,0,0,1);


// The cameras world position
uniform vec3 UCameraPosition;

// Material values
struct Material
{
  vec3 UColor;
  // Material factors
  float UAmbientFactor;
  float UDiffuseFactor;
  float USpecularFactor;
  float USpecularExponent;
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

vec3 ComputeLight(int light, vec3 normal, vec3 position, vec3 view_dir)
{
  // ambient term
  vec3 ambient_color = UMaterial.UAmbientFactor * ULights[light].UAmbientColor;
  // finding light direction
  vec3 light_vec;
  vec3 light_dir;
  if(ULights[light].UType == DIRECTIONAL)
    light_vec = -normalize(ULights[light].UDirection);
  else
    light_vec = ULights[light].UPosition - position;
  light_dir = normalize(light_vec);
  // diffuse term
  float ndotl = max(dot(normal, light_dir), 0.0);
  vec3 diffuse_color = UMaterial.UDiffuseFactor * ndotl * ULights[light].UDiffuseColor;
  // specular term
  vec3 reflect_dir = 2.0 * dot(normal, light_dir) * normal - light_dir;
  reflect_dir = normalize(reflect_dir);
  float vdotr = max(dot(view_dir, reflect_dir), 0.0);
  float specular_spread = pow(vdotr, UMaterial.USpecularExponent);
  vec3 specular_color = UMaterial.USpecularFactor * ULights[light].USpecularColor * specular_spread;
  // find spotlight effect
  float spotlight_factor;
  if(ULights[light].UType == SPOT){
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
  if(ULights[light].UType == DIRECTIONAL)
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

void main()
{
  gl_Position = UProjection * UView * UModel * vec4(APosition.xyz, 1.0);
  vec3 normal = mat3(transpose(inverse(UModel))) * ANormal;
  vec3 position = vec3(UModel * vec4(APosition, 1.0));
  // precomputations
  normal = normalize(normal);
  vec3 view_vec = UCameraPosition - position;
  vec3 view_dir = normalize(view_vec);
  // summing all light results
  vec3 final_color = vec3(0.0, 0.0, 0.0);
  for (int i = 0; i < UActiveLights; ++i)
    final_color += ComputeLight(i, normal, position, view_dir);
  // accounting for object color
  final_color *= UMaterial.UColor;
  // accounting for emissive and global ambient
  final_color += UMaterial.UAmbientFactor * UGlobalAmbientColor;
  final_color += UEmissiveColor;
  // accounting for fog
  float dist = length(view_vec);
  float fog_factor = (dist - UNearPlane) / (UFarPlane - UNearPlane);
  fog_factor = min(1.0, max(0.0, fog_factor));
  final_color = mix(final_color, UFogColor, fog_factor);
  // final color
  SFragColor = vec4(final_color, 1.0);
}
