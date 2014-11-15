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
    v_Intensity = dot(-v_vertex, u_sun_dir.xyz) * 0.5;
}
END

FRAG
uniform sampler2D color_texture;
varying vec2 v_Coords;
varying float v_Intensity;

vec3 one = vec3(1.0, 1.0, 1.0);

void main()
{
  vec4 diffuse = texture2D(color_texture, v_Coords);
  gl_FragColor = diffuse;
  gl_FragColor.rgb += v_Intensity;
  float v = dot(diffuse.rgb, one) * 0.33;
  gl_FragColor.rgb -= 3.0 * (0.33 - (diffuse.r - v)) * diffuse.rgb;
  gl_FragColor.rgb = 0.25 * gl_FragColor.rgb + one * 0.75;
}
END