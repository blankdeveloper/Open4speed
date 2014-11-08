VERT
uniform mat4 u_Matrix;
attribute vec3 v_vertex;
attribute vec3 v_normal;
attribute vec2 v_coord;
varying vec2 v_Coords;
varying vec3 v_Normal;

void main()
{
    v_Coords = v_coord;
    v_Normal = v_normal;
    gl_Position = u_Matrix * vec4(v_vertex, 1.0);
}
END

FRAG
uniform sampler2D color_texture;
varying vec2 v_Coords;
varying vec3 v_Normal;

const vec4 u_sun_dir = vec4(0.5, -1.0, 0.0, 0.0);

void main()
{
  vec4 diffuse = texture2D(color_texture, v_Coords);
  gl_FragColor = diffuse;
  vec3 N = normalize(v_Normal - 1.0 + 2.0 * diffuse.rgb);
  gl_FragColor.rgb *= dot(N, -u_sun_dir.xyz) + 0.5;
}
END