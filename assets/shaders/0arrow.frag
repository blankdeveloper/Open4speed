uniform sampler2D color_texture;
varying vec2 v_Coords;
vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
void main()
{
  gl_FragColor = (white - texture2D(color_texture, v_Coords) * 2.0) * 0.5;
  gl_FragColor.a = 0.1;
}
