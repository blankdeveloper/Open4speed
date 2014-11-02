VERT
uniform mat4 u_Matrix;
uniform mat4 u_ModelViewMatrix;
attribute vec3 v_vertex;
attribute vec3 v_normal;
attribute vec2 v_coord;
varying vec3 v_Normal;
varying vec2 v_Coords;

void main()
{
    v_Normal = (u_ModelViewMatrix * vec4(v_normal, 0.0)).xyz;
    v_Coords = v_coord;
    gl_Position = u_Matrix * vec4(v_vertex, 1.0);
}
END

FRAG
uniform sampler2D color_texture;
varying vec3 v_Normal;
varying vec2 v_Coords;

void main()
{
  vec4 diffuse = texture2D(color_texture, v_Coords);
  diffuse.rgb *= 1.1;
  
  //snow
  vec3 N = v_Normal - 0.5 + 1.0 * diffuse.rgb;
  gl_FragColor.rgb =  v_Normal.y * diffuse.rgb;
  gl_FragColor.a = 1.0;
}
END