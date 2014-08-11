uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_width, u_height;
varying vec3 v_Vertex;
varying vec3 v_Normal;
varying vec2 v_Coords;

uniform float u_light_cut;
uniform vec4 u_light;
uniform vec4 u_light_dir;

void main()
{
  vec4 diffuse = texture2D(color_texture, v_Coords);
  gl_FragColor.rgb = diffuse.rgb;
  gl_FragColor.a = 1.0;
  
  //player car light
  //vec3 lightDir = u_light.xyz - v_Vertex;
  //vec3 E = normalize(-v_Vertex);
  vec3 N = normalize(v_Normal - 0.75 + 1.5 * diffuse.rgb);
  /*vec3 L = normalize(lightDir);
  //count effectivity(directionaly)
  float eff = dot(normalize(-u_light_dir.xyz), -L);

    //specular light
    vec3 R = normalize(-reflect(L, N));
    vec3 E = normalize(-v_Vertex);
    float v = min(max(pow(eff * abs(dot(R, E)), 5.0) * 5.0 - length(lightDir) * 0.001, 0.0) * (eff - u_light_cut), 1.0);
    //apply light color
    gl_FragColor.rgb *= 0.75 + 0.25 * v;*/

  //reflect
    float y = 1.0 - gl_FragCoord.y * u_height;
    y += gl_FragCoord.z * 0.1 + gl_FragCoord.z * (1.0 - N.y);
    gl_FragColor.rgb += diffuse.a * 0.5 * texture2D(EnvMap1, vec2(gl_FragCoord.x * u_width, y)).rgb * clamp(gl_FragCoord.y * u_height * 4.5 - 0.75, 0.0, 1.0);

  //gl_FragColor.rgb += clamp(reflect(E, diffuse.rgb).y, 0.0, 0.5);
    
  //dynamic shadow
  /*float a = texture2D(EnvMap1, vec2(gl_FragCoord.x * u_width, gl_FragCoord.y * u_height + 0.5 * (1.0 - gl_FragCoord.z))).a;
  gl_FragColor.rgb *= a == 0.0 ? 0.5 : 1.0;*/
}
