#version 330 core

in vec4 litFragColor;

layout(location = 0) out vec4 vFragColor;

void main()
{
  vFragColor = litFragColor;
}