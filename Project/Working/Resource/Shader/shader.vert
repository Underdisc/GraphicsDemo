#version 330 core

// These two must perfectly match the structure defined in inc/graphics/Vertex.h
layout(location = 0) in vec3 vVertex;
layout(location = 1) in vec3 vNormal;

uniform mat4 ModelViewMatrix; // local->world matrix
uniform mat4 ModelViewProjectionMatrix; // local->NDC matrix [no camera support]

const int MaxLights = 8; // maximum possible lights this shader supports

// only support directional lights for now
struct Light
{
  vec4 direction; // direction the light is directed
  vec4 ambient; // ambient light cast onto objects
  vec4 diffuse; // diffuse light cast onto objects
};

uniform Light Lights[MaxLights]; // support UP TO 8 lights
uniform int LightCount; // number of lights enabled THIS ROUND

// represents material properties of the surface passed by the application
uniform struct
{
  vec4 ambient; // ambient color of the surface/how much ambient light to absorb
  vec4 diffuse; // diffuse color of the surface/how much diffuse light to absorb
} Material;

out vec4 litFragColor;

vec4 computeLightingTerm(in int lightIdx, in vec4 worldNormal)
{
  // grab light
  Light light = Lights[lightIdx];

  // light vector points from the surface toward the light (opposite light dir.)
  vec4 lightVec = normalize(-light.direction);

  // ambient contribution from the light is always constant
  vec4 ambient = light.ambient * Material.ambient;

  // initially, diffuse contribution is black
  vec4 diffuse = vec4(0); // same as vec4(0, 0, 0, 0), or black

  // compute the Lambertian term
  float diffuseFactor = dot(worldNormal, lightVec);


  //// EWWWWWWWWWWWWWWWWWW YOU USED AN IF STATEMENT IN A PHONG
  // SHADER EEEEEEEEEEEEEEEEEEEEEEWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
  if (diffuseFactor > 0) // is there a diffuse contribution?
  {
    // compute diffuse contribution on the surface
    diffuse = diffuseFactor * light.diffuse * Material.diffuse;
  }
  return ambient + diffuse; // total contribution from this light
}

vec4 computeSurfaceColor(in vec4 worldNormal)
{
  // Phong: total contribution of light is sum of all individual light contribs.
  vec4 color = vec4(0, 0, 0, 0); // no light = black
  for (int i = 0; i < LightCount; ++i)
    color += computeLightingTerm(i, worldNormal); // contribution of light i
  return color; // contribution from all lights onto surface
}

void main()
{
  // deal with position and normal in world space
  vec4 worldPos = ModelViewMatrix * vec4(vVertex, 1);

  // vec4(vNormal, 0) because we don't want to translate a normal;
  // NOTE: this code is wrong if we support non-uniform scaling
  vec4 worldNorm = normalize(ModelViewMatrix * vec4(vNormal, 0));

  // compute the final result of passing this vertex through the transformation
  // pipeline and yielding a coordinate in NDC space
  gl_Position = ModelViewProjectionMatrix * vec4(vVertex, 1);

  // compute the contribution of lights onto this vertex and interpolate that
  // color value across the surface of the polygon
  litFragColor = computeSurfaceColor(worldNorm);
}
