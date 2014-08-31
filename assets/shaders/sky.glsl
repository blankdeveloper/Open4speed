VERT
uniform mat4 u_Matrix;
attribute vec3 v_vertex;
attribute vec3 v_normal;
attribute vec2 v_coord;
varying vec2 v_Coords;
varying vec3 v_Vertex;
void main()
{
    v_Coords = v_coord;
    gl_Position = u_Matrix * vec4(v_vertex, 1.0);
    v_Vertex = v_vertex;
}
END

FRAG
uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_res, u_speed;
varying vec2 v_Coords;
varying vec3 v_Vertex;

vec4 u_sun_dir = normalize(vec4(0.5, -1.0, 0.0, 0.0));

void main()
{
  gl_FragColor = texture2D(color_texture, v_Coords);
  gl_FragColor.rgb += dot(-v_Vertex, u_sun_dir.xyz) * 0.25;
}
END