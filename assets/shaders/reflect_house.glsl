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
uniform float u_height, u_speed, u_view, u_gamma;
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
  gl_FragColor = 0.5 * diffuse;
  
  //player car light
  vec3 lightDir = u_light.xyz - v_Vertex;
  vec3 N = normalize(v_Normal);
  vec3 L = normalize(lightDir);
  //count effectivity(directionaly)
  float eff = dot(normalize(-u_light_dir.xyz), -L);

    //diffuse light
    vec3 color = max(dot(N, L), 0.0) * diffuse.rgb * 10.0;
    //specular light
    vec3 R = normalize(-reflect(L, N));
    vec3 E = normalize(-v_Vertex);
    gl_FragColor.rgb += diffuse.rgb * pow(abs(dot(R, E) * dot(N, diffuse.rgb)), 2.0) * 1.0;
    color *= pow(eff, u_light_spot_eff);
    //apply light color
    gl_FragColor.rgb += max(color - length(lightDir) * 0.001, 0.0) * max(eff - u_light_cut, 0.0);

  //dynamic shadow
//  if (texture2D(EnvMap1, vec2(gl_FragCoord.x * u_res, gl_FragCoord.y * u_res + 0.5 * (1.0 - gl_FragCoord.z))).a == 1.0)
//    diffuse.rgb *= 0.5;

  float d = length(lightDir);
  gl_FragColor.rgb *= u_gamma;
  gl_FragColor.rgb += (pow(length(diffuse.rgb) + 1.0, 2.0) - 1.0) * (min(abs(v_Vertex.z), 12.0) - 12.0) * abs(dot(N, diffuse.rgb)) * 0.03;


  vec4 c = texture2D(EnvMap1, vec2((v_Normal * length(diffuse.rgb)).x * 0.5 + 0.5, gl_FragCoord.y * u_height));
  gl_FragColor = gl_FragColor + (1.0 - abs(v_Normal.y)) * 0.25 * (1.0 - (1.0 - c * c));
}
END