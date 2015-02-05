VERT
uniform mat4 u_Matrix;
attribute vec3 v_vertex;
attribute vec2 v_coord;
varying vec2 v_Coords;
varying float v_Intensity;

const vec4 u_sun_dir = vec4(0.5, -1.0, 0.0, 0.0);

void main()
{
    v_Coords = v_coord;
    gl_Position = u_Matrix * vec4(v_vertex, 1.0);
    v_Intensity = dot(-v_vertex, u_sun_dir.xyz) * 0.1;
}
END

FRAG
uniform sampler2D color_texture;
varying vec2 v_Coords;
varying float v_Intensity;

void main()
{
  gl_FragColor = texture2D(color_texture, v_Coords);
  gl_FragColor.rgb += v_Intensity;
}
END