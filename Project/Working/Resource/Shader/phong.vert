/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#version 330 core

in vec3 APosition;
in vec3 ANormal;
in vec3 ATangent;
in vec3 ABitangent;
in vec2 AUV;

out vec3 SNormal;
out vec3 STangent;
out vec3 SBitangent;
out vec3 SFragPos;
out vec3 SModelNormal;
out vec3 SModelPos;
out vec2 SUV;

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

void main()
{
  gl_Position = UProjection * UView * UModel * vec4(APosition.xyz, 1.0);
  mat3 orientation = mat3(transpose(inverse(UModel)));
  SNormal = orientation * ANormal;
  STangent = orientation * ATangent;
  SBitangent = orientation * ABitangent;
  SFragPos = vec3(UModel * vec4(APosition, 1.0));
  SModelPos = APosition;
  SModelNormal = ANormal;
  SUV = AUV;
}
