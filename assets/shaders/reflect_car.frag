uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_res, u_speed, u_view, u_brake;
varying vec3 v_Vertex;
varying vec3 v_Vertex2;
varying vec3 v_Normal;
varying vec2 v_Coords;
uniform vec4 u_light_diffuse;
uniform float u_light_specular;
uniform float u_light_cut;
uniform float u_light_spot_eff;
uniform vec4 u_light;
uniform vec4 u_light_att;
uniform vec4 u_light_dir;
uniform vec4 u_nearest1;
uniform vec4 u_model_position;

void main()
{
  gl_FragColor = texture2D(color_texture, v_Coords);
  vec3 N = normalize(v_Normal);
  
  //front/brake lights
  if (gl_FragColor.a > 0.99) {
    //player car light
    vec3 lightDir = u_light.xyz - v_Vertex;
    vec3 L = normalize(lightDir);
    //count effectivity(directionaly)
    float eff = dot(normalize(-u_light_dir.xyz), -L) * 2.0;
    //light cutoff
    if (eff >= u_light_cut) {
      //diffuse light
      vec3 color = max(dot(N, L), 0.0) * gl_FragColor.rgb * u_light_diffuse.rgb;
      //specular light
      //vec3 R = normalize(-reflect(L, N));
      //vec3 E = normalize(-v_Vertex);
      //color += pow(max(dot(R, E), 0.0), 128.0) * u_light_specular;
      //light attenuation
      float d = length(lightDir);
      vec3 D = vec3(1.0, d, d * d);
      color *= pow(eff, u_light_spot_eff) / dot(u_light_att.xyz * 0.5, D);
      //apply light color
      gl_FragColor.rgb += 4.0 * color;
    }
  
    //reflect
    gl_FragColor += 8.0 * texture2D(EnvMap1, vec2(0.5 + v_Normal.x * 0.25, 1.5 - gl_FragCoord.z * 1.0 + v_Normal.y * 0.25) * u_view) * clamp(0.0, (v_Vertex2.y - 0.4), 1.0);
  
    //dynamic shadow
    float a = texture2D(EnvMap1, u_model_position.xy).a;
    gl_FragColor *= min(a + 0.05, 0.75);
  } else {
    gl_FragColor.rgb *= (v_Coords.t > 0.3) ? 1.0 : 1.0 + u_brake * 1.5;
  }
  
  //specular light
  vec3 lightDir = u_nearest1.xyz - v_Vertex;
  vec3 L = normalize(lightDir);
  vec3 R = normalize(-reflect(L, N));
  vec3 E = normalize(-v_Vertex);
  gl_FragColor.rgb += max(pow(max(dot(R, E), 0.0), 8.0) * 5.0 - length(lightDir) * 0.5, 0.0);
  
  //blur
  gl_FragColor = (1.0 - u_speed) * gl_FragColor + u_speed * texture2D(EnvMap1, gl_FragCoord.xy * u_res); 
  
  //mark for dynamic shadow on road
  gl_FragColor.a = 1.0;
}
