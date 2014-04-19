uniform sampler2D color_texture;
varying vec2 v_Coords;
void main()
{
  if (texture2D(color_texture, v_Coords).a < 0.9) {
    discard;
  }
}
