uniform sampler2D color_texture;
uniform sampler2D Shadowmap;
varying vec2 v_Coords;
varying vec3 v_Normal;
varying vec3 v_Vertex;
varying vec4 posUV;
const float mv = 1.0 / 2048.0;

uniform vec4 u_light_diffuse;
uniform float u_light_near;
uniform vec4 u_light;
uniform vec4 u_light_att;
uniform vec4 u_light_dir;

float lookup(vec3 p) {
   
  // check if fragment is visible
  float shadow = float(texture(Shadowmap, p.xy).x >= p.z - (u_light_near + 0.025) / posUV.w);
  return shadow * float((p.x >= 0.0) && (p.x <= 1.0) && (p.y >= 0.0) && (p.y <= 1.0));
}

void main()
{   
  float visibility = 1.0;

  // transparent textures
  if (texture2D(color_texture, v_Coords).a < 0.9) {
    if (length(posUV.xyz) < 5.00)
      gl_FragColor.rgb = u_light_diffuse.rgb;
    else
      discard;
  } else {

    //count light
    vec3 lightDir = -v_Vertex;
    vec3 N = normalize(v_Normal);
    vec3 L = normalize(lightDir);
    //diffuse light
    vec3 color = max(dot(N, L), 0.0) * u_light_diffuse.rgb;
    //light attenuation
    float d = abs(v_Vertex.z);
    vec3 D = vec3(1.0, d, d * d);
    color *= 1.0 / dot(u_light_att.xyz, D);
    // add to previous lightmap
    gl_FragColor.rgb = color;
      
    // get shadowmap coordinates
    vec3 p = posUV.xyz / posUV.w;

    // PCF
    float shadow = 0.0;
    float x,y;
    //x = 0.0;
    //y = 0.0;
    for (y = -3.5; y <= 3.5; y+=1.0)
      for (x = -3.5; x <= 3.5; x+=1.0)
	shadow += lookup(p + vec3(x, y, 0.0) * mv);	
    visibility *= shadow / 64.0;
    
    gl_FragColor.rgb *= visibility;
  }
  
  //mark as done
  gl_FragColor.a = 1.0;
}
