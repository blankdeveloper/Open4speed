VERT
uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;
attribute vec3 v_vertex;
attribute vec3 v_normal;
attribute vec2 v_coord;
varying vec3 v_Vertex;
varying vec3 v_Normal;
varying vec2 v_Coords;

void main()
{
    vec4 view_pos = u_ModelViewMatrix * vec4(v_vertex, 1.0);
    v_Normal = (u_ModelViewMatrix * vec4(v_normal, 0.0)).xyz;
    v_Vertex = view_pos.xyz;
    v_Coords = v_coord;
    gl_Position = u_ProjectionMatrix * view_pos;
}
END

FRAG
uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_width, u_height;
varying vec3 v_Vertex;
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
  gl_FragColor.rgb =  (1.2 - diffuse.a) * v_Normal.y *  diffuse.rgb + diffuse.a * texture2D(EnvMap1, vec2(gl_FragCoord.x * u_width, y)).rgb * gl_FragCoord.y * u_height;
  gl_FragColor.a = 1.0;
  //fog
  float d = clamp(-0.005 * v_Vertex.z, 0.0, 1.0);
  gl_FragColor.rgb = 0.75 * one * d + (1.0 - d) * gl_FragColor.rgb;
}
END