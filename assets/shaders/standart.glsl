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
    v_Normal = vec3(u_ModelViewMatrix * vec4(v_normal, 0.0));
    v_Vertex = view_pos.xyz;
    v_Coords = v_coord;
    gl_Position = u_ProjectionMatrix * view_pos;
}
END

FRAG
uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_width, u_height, u_speed, u_view;
varying vec3 v_Vertex;
varying vec3 v_Normal;
varying vec2 v_Coords;

uniform vec4 u_light_diffuse;
uniform float u_light_cut;
uniform float u_light_spot_eff;
uniform vec4 u_light;
uniform vec4 u_light_att;
uniform vec4 u_light_dir;

void main()
{
  vec4 diffuse = texture2D(color_texture, v_Coords);
  gl_FragColor.rgb = diffuse.rgb;
  
  //player car light
  /*vec3 lightDir = u_light.xyz - v_Vertex;
  vec3 N = normalize(v_Normal - 1.0 + 2.0 * diffuse.rgb);
  vec3 E = normalize(-v_Vertex);
  vec3 L = normalize(lightDir);
  vec3 R = normalize(-reflect(L, N));
  //count effectivity(directionaly)
  float eff = dot(normalize(-u_light_dir.xyz), -L);

    //specular light
    vec3 R = normalize(-reflect(L, N));
    vec3 E = normalize(-v_Vertex);
    float v = min(max(pow(eff * abs(dot(R, E)), 5.0) * 5.0 - length(lightDir) * 0.001, 0.0) * (eff - u_light_cut), 1.0);
    //apply light color
    gl_FragColor.rgb *= 0.75 + 0.125 * v;*/


  //dynamic shadow
  /*float a = texture2D(EnvMap1, vec2(gl_FragCoord.x * u_width, gl_FragCoord.y * u_height + 0.5 * (1.0 - gl_FragCoord.z))).a;
  gl_FragColor.rgb *= a == 0.0 ? 0.5 : 1.0;*/


  vec3 N = normalize(v_Normal - 1.0 + 2.0 * diffuse.rgb);
  vec3 R = normalize(-reflect(-v_Vertex, N));
  gl_FragColor.rgb += 0.5 * clamp(reflect(R, diffuse.rgb).y, -0.75, 0.25);

  gl_FragColor.a = 1.0;
}
END