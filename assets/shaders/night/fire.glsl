VERT
uniform mat4 u_Matrix;
attribute vec3 v_vertex;
attribute vec2 v_coord;
varying vec2 v_Coords;

void main()
{
    v_Coords = v_coord;
    gl_Position = u_Matrix * vec4(v_vertex, 1.0);
}
END

FRAG
uniform sampler2D color_texture;
varying vec2 v_Coords;

void main()
{
  vec4 diffuse = texture2D(color_texture, v_Coords);
  gl_FragColor.b = 1.5 * (diffuse.g - 0.5);
  gl_FragColor.g = diffuse.b;
  gl_FragColor.r = diffuse.b;
  gl_FragColor.a = diffuse.a * 0.05;
}
END