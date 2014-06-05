uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_width, u_height;
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
  vec4 diffuse = texture2D(color_texture, v_Coords);
  gl_FragColor = 0.01 * diffuse;
  
  //player car light
  vec3 lightDir = u_light.xyz - v_Vertex;
  vec3 N = normalize(v_Normal);
  vec3 L = normalize(lightDir);
  //count effectivity(directionaly)
  float eff = dot(normalize(-u_light_dir.xyz), -L);
  //light cutoff
  if (eff >= u_light_cut) {
    //diffuse light
    vec3 color = max(dot(N, L), 0.0) * diffuse.rgb * u_light_diffuse.rgb;
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

  //dynamic shadow
  if (texture2D(EnvMap1, vec2(gl_FragCoord.x * u_width, gl_FragCoord.y * u_height + 0.5 * (1.0 - gl_FragCoord.z))).a == 1.0)
    diffuse.rgb *= 0.5;

  //static shadow
  vec4 shadow = 1.5 * texture2D(Lightmap, v_T);
  gl_FragColor.rgb += diffuse.rgb * shadow.rgb;
  gl_FragColor.a = min(length(shadow.rgb), 0.9);

  //reflect
  if (v_Normal.y > 0.0) {
    float y = 1.0 - gl_FragCoord.y * u_height;
    y += gl_FragCoord.z * 0.125 + gl_FragCoord.z * (1.0 - v_Normal.y) * 2.0;
    gl_FragColor.rgb += 0.1 * texture2D(EnvMap1, vec2(gl_FragCoord.x * u_width, y)).rgb * clamp(gl_FragCoord.y * u_height * 3.0 - 0.5, 0.0, 1.0);
  }
}
