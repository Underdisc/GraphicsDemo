/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#version 330 core

in vec3 APosition;

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
  mat4 transformation = UProjection * UView * UModel;
  gl_Position = transformation * vec4(APosition, 1.0);
}
