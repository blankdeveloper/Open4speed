uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
varying vec2 v_Coords;
void main()
{
  //transparency
  vec4 c = texture2D(color_texture, v_Coords);
  gl_FragColor = c * 1.5;
}
