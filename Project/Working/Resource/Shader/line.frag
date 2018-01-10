/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
#version 330 core

out vec4 FragColor;

uniform vec3 ULineColor = vec3(1.0, 1.0, 1.0);

void main()
{
  FragColor = vec4(ULineColor, 1.0);
}
