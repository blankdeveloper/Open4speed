uniform sampler2D color_texture;
varying vec3 v_Vertex;
varying vec3 v_Normal;
varying vec2 v_Coords;

void main()
{
  vec4 diffuse = texture2D(color_texture, v_Coords);
//  if (diffuse.a < 0.5)
//    discard;
  gl_FragColor = diffuse;
}
