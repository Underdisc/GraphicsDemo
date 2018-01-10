#version 330 core

in vec3 APosition;

uniform mat4 UProjection;
uniform mat4 UView;
uniform mat4 UModel;

void main()
{
  gl_Position = UProjection * UView * UModel * vec4(APosition, 1.0);
}
