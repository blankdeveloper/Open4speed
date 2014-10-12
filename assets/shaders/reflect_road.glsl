VERT
uniform mat4 u_Matrix;
attribute vec3 v_vertex;
attribute vec3 v_normal;
attribute vec2 v_coord;
varying vec3 v_Normal;
varying vec2 v_Coords;

void main()
{
    v_Normal = v_normal;
    v_Coords = v_coord;
    gl_Position = u_Matrix * vec4(v_vertex, 1.0);
}
END

FRAG
uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_width, u_height;
varying vec3 v_Normal;
varying vec2 v_Coords;
varying vec3 v_Sun;

void main()
{
  vec4 diffuse = texture2D(color_texture, v_Coords);
  
  //reflect
  vec3 N = normalize(v_Normal - 0.5 + 1.0 * diffuse.rgb);
  float y = 1.0 - gl_FragCoord.y * u_height;
  y += gl_FragCoord.z * 0.1 + gl_FragCoord.z * (1.1 - N.y);
  gl_FragColor.rgb = (1.25 - diffuse.a) *  diffuse.rgb + diffuse.a * texture2D(EnvMap1, vec2(gl_FragCoord.x * u_width, y)).rgb * max(0.0, gl_FragCoord.y * u_height - 0.2);
  gl_FragColor.a = 1.0;
}
END