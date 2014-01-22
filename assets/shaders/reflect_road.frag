uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_res, u_speed, u_view;
varying vec3 v_Vertex;
varying vec3 v_Normal;
varying vec2 v_Coords;
uniform sampler2D Lightmap;
varying vec2 v_T;

uniform vec4 u_light_diffuse;
uniform float u_light_specular;
uniform float u_light_cut;
uniform float u_light_spot_eff;
uniform vec4 u_light;
uniform vec4 u_light_att;
uniform vec4 u_light_dir;

void main()
{
  gl_FragColor = texture2D(color_texture, v_Coords);
  
  //player car light
  vec3 lightDir = u_light.xyz - v_Vertex;
  vec3 N = normalize(v_Normal);
  vec3 L = normalize(lightDir);
  //count effectivity(directionaly)
  float eff = dot(normalize(-u_light_dir.xyz), -L);
  //light cutoff
  if (eff >= u_light_cut) {
    //diffuse light
    vec3 color = max(dot(N, L), 0.0) * gl_FragColor.rgb * u_light_diffuse.rgb;
    //specular light
    //vec3 R = normalize(-reflect(L, N));
    //vec3 E = normalize(-v_Vertex);
    //color += pow(max(dot(R, E), 0.0), 128.0) * u_light_specular;
    //light attenuation
    float d = length(L);
    vec3 D = vec3(1.0, d, d * d);
    color *= pow(eff, u_light_spot_eff) / dot(u_light_att.xyz, D);
    //apply light color
    gl_FragColor.rgb += color;
  }
  
  //reflect
  if (v_Normal.y > 0.0) {
    float y = 4.0 + u_view - gl_FragCoord.y * u_res - 0.2 + gl_FragCoord.z * (u_view * 10.0 + 5.0) * 0.025 - v_Normal.y * 4.0;
    gl_FragColor.rgb += 2.5 * texture2D(EnvMap1, vec2(gl_FragCoord.x * u_res, y)).rgb * max(gl_FragCoord.y * u_res / u_view * 2.0 - 0.5, 0.0);
  }

  //static shadow
  vec4 shadow = texture2D(Lightmap, v_T);
  gl_FragColor.rgb *= shadow.rgb + 0.1;
  gl_FragColor.a *= shadow.g + 0.1;
  gl_FragColor.a = min(gl_FragColor.a, 0.9);
  
  //dynamic shadow
  if (texture2D(EnvMap1, vec2(gl_FragCoord.x * u_res, gl_FragCoord.y * u_res + 0.5 * (1.0 - gl_FragCoord.z))).a == 1.0)
    gl_FragColor.rgb -= 0.05;

  //blur
  gl_FragColor = (1.0 - u_speed) * gl_FragColor + u_speed * texture2D(EnvMap1, gl_FragCoord.xy * u_res); 
}
