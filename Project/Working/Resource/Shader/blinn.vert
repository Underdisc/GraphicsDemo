/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#version 330 core

in vec3 APosition;
in vec3 ANormal;

out vec3 SNormal;
out vec3 SFragPos;

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
  SNormal = mat3(transpose(inverse(UModel))) * ANormal;
  SFragPos = vec3(UModel * vec4(APosition, 1.0));
}
