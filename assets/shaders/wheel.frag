uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_width, u_height;
varying vec3 v_Normal;
varying vec2 v_Coords;

void main()
{
  vec4 diffuse = texture2D(color_texture, v_Coords);

  gl_FragColor.rgb = diffuse.rgb * abs(gl_FragCoord.x * u_width - 0.5);
  
  //reflect
  vec3 N = normalize(v_Normal - 0.75 + 1.5 * diffuse.rgb);
  gl_FragColor.rgb += diffuse.r * texture2D(EnvMap1, vec2(N.x * 0.75, gl_FragCoord.y * u_height)).rgb;
  
  gl_FragColor.a = 1.0;
}
