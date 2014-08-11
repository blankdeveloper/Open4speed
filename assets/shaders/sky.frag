uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_res, u_speed;
varying vec2 v_Coords;
void main()
{
  gl_FragColor = texture2D(color_texture, v_Coords);
}
