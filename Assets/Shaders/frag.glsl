#version 460 core

out vec4 fragColor;

in vec2 TextureCoord;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_diffuse4;
uniform sampler2D texture_diffuse5;
uniform sampler2D texture_diffuse6;
uniform sampler2D texture_diffuse7;

uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_specular3;
uniform sampler2D texture_specular4;
uniform sampler2D texture_specular5;
uniform sampler2D texture_specular6;

void main(){
  vec4 td1 = texture(texture_diffuse1,TextureCoord);
  vec4 td2 = texture(texture_diffuse2,TextureCoord);
  vec4 td3 = texture(texture_diffuse3,TextureCoord);
  vec4 td4 = texture(texture_diffuse4,TextureCoord);
  vec4 td5 = texture(texture_diffuse5,TextureCoord);
  vec4 td6 = texture(texture_diffuse6,TextureCoord);
  vec4 td7 = texture(texture_diffuse7,TextureCoord);

  vec4 ts1 = texture(texture_specular1,TextureCoord);
  vec4 ts2 = texture(texture_specular2,TextureCoord);
  vec4 ts3 = texture(texture_specular3,TextureCoord);
  vec4 ts4 = texture(texture_specular4,TextureCoord);
  vec4 ts5 = texture(texture_specular5,TextureCoord);
  vec4 ts6 = texture(texture_specular6,TextureCoord);

  vec4 color_diffuse = (td1 + td2 + td3 + td4 + td5 + td6 + td7)/7.0;
  vec4 color_specular = (ts1 + ts2 + ts3 + ts4 + ts5 + ts6)/6.0;
  vec4 color_final = (color_diffuse + color_specular)/2.0;

  fragColor = color_final;
}

