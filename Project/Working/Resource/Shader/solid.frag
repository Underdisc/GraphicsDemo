#version 330 core

out vec4 OFragColor;

uniform vec3 UColor;

void main()
{
  OFragColor = vec4(UColor, 1.0);
}
