#version 330 core

in vec2 STexCoord;

out vec4 OFragColor;

uniform sampler2D UTexture;

void main()
{
  OFragColor = texture(UTexture, STexCoord);
}
