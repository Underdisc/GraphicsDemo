#version 330 core

in vec3 SFragPos;

uniform sampler2D UUp;
uniform sampler2D UDown;
uniform sampler2D ULeft;
uniform sampler2D URight;
uniform sampler2D UFront;
uniform sampler2D UBack;

// TODO: FLIP YOUR TEXTURES IN PAINT.NET AND IN FLIP ON LOAD
void main()
{
  vec3 position = abs(SFragPos);
  vec3 final_color = vec3(0.0, 0.0, 0.0);
  vec2 uv = vec2(0.0, 0.0);
  if(position.x >= position.y && position.x >= position.z){
    if(SFragPos.x > 0){
      uv.x = (SFragPos.z / -position.x + 1.0) / 2.0;
      uv.y = (SFragPos.y / -position.x + 1.0) / 2.0;
      final_color = vec3(texture(URight, uv));
    }
    else{
      uv.x = (SFragPos.z /  position.x + 1.0) / 2.0;
      uv.y = (SFragPos.y / -position.x + 1.0) / 2.0;
      final_color = vec3(texture(ULeft, uv));
    }
  }
  else if(position.y >= position.x && position.y >= position.z){
    if(SFragPos.y > 0){
      uv.x = (SFragPos.z / -position.y + 1.0) / 2.0;
      uv.y = (SFragPos.x /  position.y + 1.0) / 2.0;
      final_color = vec3(texture(UUp, uv));
    }
    else{
      uv.x = (SFragPos.z / -position.y + 1.0) / 2.0;
      uv.y = (SFragPos.x / -position.y + 1.0) / 2.0;
      final_color = vec3(texture(UDown, uv));
    }
  }
  else {
    if(SFragPos.z > 0){
      uv.x = (SFragPos.x /  position.z + 1.0) / 2.0;
      uv.y = (SFragPos.y / -position.z + 1.0) / 2.0;
      final_color = vec3(texture(UBack, uv));
    }
    else{
      uv.x = (SFragPos.x / -position.z + 1.0) / 2.0;
      uv.y = (SFragPos.y / -position.z + 1.0) / 2.0;
      final_color = vec3(texture(UFront, uv));
    }
  }

  gl_FragColor = vec4(final_color, 1.0);

  // Use the frag pos to determine the texture

}
