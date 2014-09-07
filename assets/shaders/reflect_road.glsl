VERT
uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;
attribute vec3 v_vertex;
attribute vec3 v_normal;
attribute vec2 v_coord;
varying vec3 v_Vertex;
varying vec3 v_Normal;
varying vec2 v_Coords;

void main()
{
    vec4 view_pos = u_ModelViewMatrix * vec4(v_vertex, 1.0);
    v_Normal = vec3(u_ModelViewMatrix * vec4(v_normal, 0.0));
    v_Vertex = view_pos.xyz;
    v_Coords = v_coord;
    gl_Position = u_ProjectionMatrix * view_pos;
}
END

FRAG
uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
uniform float u_width, u_height;
varying vec3 v_Vertex;
varying vec3 v_Normal;
varying vec2 v_Coords;
varying vec3 v_Sun;

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
  vec3 N = normalize(v_Normal - 1.0 + 2.0 * diffuse.rgb);
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
    y += gl_FragCoord.z * 0.1 + gl_FragCoord.z * (1.1 - N.y);
    gl_FragColor.rgb += diffuse.a < 0.75 ? vec3(0.0, 0.0, 0.0) : -0.0 + 1.5 * texture2D(EnvMap1, vec2(gl_FragCoord.x * u_width, y)).rgb * max(0.0, gl_FragCoord.y * u_height - 0.2);


  //vec3 N = normalize(v_Normal - 1.0 + 2.0 * diffuse.rgb);
  vec3 R = normalize(-reflect(-v_Vertex, N));
  gl_FragColor.rgb += 0.5 * clamp(reflect(R, diffuse.rgb).y, diffuse.a < 0.75 ? -0.75 : 0.0, 0.25);
}
END