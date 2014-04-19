uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_res, u_speed;
varying vec2 v_Coords;
void main()
{
  //transparency
  vec4 c = texture2D(color_texture, v_Coords);
  if (c.a < 0.5) {
    discard;
  }
  gl_FragColor = c * 1.5;
  gl_FragColor.a = 1.0;
  
  //blur
  gl_FragColor = (1.0 - u_speed) * gl_FragColor + u_speed * texture2D(EnvMap1, gl_FragCoord.xy * u_res); 
}
