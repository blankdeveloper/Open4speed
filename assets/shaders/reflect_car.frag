uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_brake;
varying vec3 v_Vertex;
varying vec3 v_Vertex2;
varying vec3 v_Normal;
varying vec2 v_Coords;

vec3 one = vec3(1.0, 1.0, 1.0);

void main()
{
  vec4 diffuse = texture2D(color_texture, v_Coords);
  gl_FragColor.rgb = (u_brake > 0.5) && (diffuse.a < 0.75) ? 3.0 * diffuse.rgb : diffuse.rgb;
  gl_FragColor.a = 1.0;
  
  vec3 N = normalize(v_Normal - 0.25 + 0.5 * diffuse.rgb);
  
  //reflect
  vec4 r = texture2D(EnvMap1, vec2(0.5 + N.x * 0.5, 1.5 - gl_FragCoord.z * 1.0 + N.y * 0.25)) * clamp(0.0, (v_Vertex2.y - 0.4), 1.0);
  float v = dot(diffuse.rgb, one) * 0.33;
  gl_FragColor.rgb += 3.0 * (diffuse.r - v) * r.rgb;
}
