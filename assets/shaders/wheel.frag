uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_res, u_speed;
varying vec3 v_Normal;
varying vec2 v_Coords;

void main()
{
  gl_FragColor.rgb = texture2D(color_texture, v_Coords).rgb * abs(gl_FragCoord.x * u_res - 0.5) * 2.0;
  
  //reflect
  gl_FragColor.rgb += texture2D(color_texture, v_Coords).r * texture2D(EnvMap1, vec2(v_Normal.x * 0.5 + 0.5, gl_FragCoord.y * u_res)).rgb;
  
  //dynamic shadow
  gl_FragColor.rgb *= texture2D(EnvMap1, vec2(gl_FragCoord.x * u_res, max(0.0, gl_FragCoord.y * u_res - 0.3))).a;
  
  //blur
  gl_FragColor = (1.0 - u_speed) * gl_FragColor + u_speed * texture2D(EnvMap1, gl_FragCoord.xy * u_res); 
  
  gl_FragColor.a = 0.0;
}
