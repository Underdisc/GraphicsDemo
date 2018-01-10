#version 330 core

in vec3 APosition;
in vec2 ATexCoord;

out vec2 STexCoord;

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
  STexCoord = ATexCoord;
}
