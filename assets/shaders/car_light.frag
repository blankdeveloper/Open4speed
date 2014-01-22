uniform sampler2D color_texture;
varying vec2 v_Coords;

void main()
{
  gl_FragColor = texture2D(color_texture, v_Coords) * 0.01;
  gl_FragColor.b *= 0.75;
}
