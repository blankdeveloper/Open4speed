uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_width, u_height;
varying vec3 v_Normal;
varying vec2 v_Coords;

void main()
{
  gl_FragColor.rgb = texture2D(color_texture, v_Coords).rgb * abs(gl_FragCoord.x * u_width - 0.5);
  
  //reflect
  gl_FragColor.rgb += texture2D(color_texture, v_Coords).r * texture2D(EnvMap1, vec2(v_Normal.x * 0.5 + 0.5, gl_FragCoord.y * u_height)).rgb;
  
  //dynamic shadow
  gl_FragColor.rgb *= texture2D(EnvMap1, vec2(gl_FragCoord.x * u_width, max(0.0, gl_FragCoord.y * u_height - 0.3))).a;
  
  gl_FragColor.a = 0.0;
}
