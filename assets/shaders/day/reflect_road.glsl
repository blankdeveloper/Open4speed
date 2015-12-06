VERT
uniform mat4 u_Matrix;
uniform mat4 u_ModelViewMatrix;
attribute vec3 v_vertex;
attribute vec3 v_normal;
attribute vec2 v_coord;
varying vec3 v_Normal;
varying vec2 v_Coords;

void main()
{
    v_Normal = (u_ModelViewMatrix * vec4(v_normal, 0.0)).xyz;
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

vec3 one = vec3(1.0, 1.0, 1.0);

void main()
{
  vec4 diffuse = texture2D(color_texture, v_Coords);
  
  //reflect
  vec3 N = v_Normal - 0.5 + 1.0 * diffuse.rgb;
  float y = 1.0 - gl_FragCoord.y * u_height + gl_FragCoord.z * 0.1 + gl_FragCoord.z * (0.9 - N.y);
  gl_FragColor.rgb =  (1.0 - diffuse.a) * v_Normal.y *  diffuse.rgb + diffuse.a * texture2D(EnvMap1, vec2(gl_FragCoord.x * u_width, y)).rgb * gl_FragCoord.y * u_height;
  gl_FragColor.a = 1.0;
}
END