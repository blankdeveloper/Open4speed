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
varying vec3 v_Vertex;
varying vec3 v_Normal;
varying vec2 v_Coords;

vec3 one = vec3(1.0, 1.0, 1.0);

void main()
{
  vec4 diffuse = texture2D(color_texture, v_Coords);
  vec3 N = normalize(v_Normal - 0.5 + 1.0 * diffuse.rgb);
  vec3 R = normalize(-reflect(-v_Vertex, N));
  gl_FragColor.rgb = diffuse.rgb + 0.5 * clamp(reflect(R, diffuse.rgb).y, -0.75, 0.25);
  gl_FragColor.a = 1.0;
  //fog
  float d = clamp(-0.005 * v_Vertex.z, 0.0, 1.0);
  gl_FragColor.rgb = 0.75 * one * d + (1.0 - d) * gl_FragColor.rgb;
}
END