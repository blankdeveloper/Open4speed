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
  gl_FragColor = 0.5 * diffuse;
  
  //player car light
  vec3 lightDir = u_light.xyz - v_Vertex;
  vec3 N = normalize(v_Normal + length(diffuse.rgb));
  vec3 L = normalize(lightDir);
  //count effectivity(directionaly)
  float eff = dot(normalize(-u_light_dir.xyz), -L);

    //specular light
    vec3 R = normalize(-reflect(L, N));
    vec3 E = normalize(-v_Vertex);
    vec3 color = diffuse.rgb * pow(eff * abs(dot(R, E)), 5.0) * 25.0;
    //apply light color
    gl_FragColor.rgb += max(color - length(lightDir) * 0.001, 0.0) * (eff - u_light_cut);

  //reflect
    float y = 1.0 - gl_FragCoord.y * u_height;
    y += gl_FragCoord.z * 0.1 + gl_FragCoord.z * (1.0 - N.y) * 4.0;
    gl_FragColor.rgb += 0.15 * texture2D(EnvMap1, vec2(gl_FragCoord.x * u_width, y)).rgb * clamp(gl_FragCoord.y * u_height * 3.0 - 0.5, 0.0, 1.0);

  //dynamic shadow
  if (texture2D(EnvMap1, vec2(gl_FragCoord.x * u_width, gl_FragCoord.y * u_height + 0.5 * (1.0 - gl_FragCoord.z))).a == 1.0)
    gl_FragColor.rgb *= 0.5;
}
