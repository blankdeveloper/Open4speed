VERT
uniform mat4 u_ModelViewMatrix;
uniform mat4 u_Matrix;
attribute vec3 v_vertex;
attribute vec3 v_normal;
attribute vec2 v_coord;
varying vec3 v_Normal;
varying vec2 v_Coords;
void main()
{
    v_Normal = v_normal;
    v_Coords = v_coord;
    gl_Position = u_Matrix * vec4(v_vertex, 1.0);
}
END

FRAG
uniform sampler2D color_texture;
uniform float u_height;
varying vec3 v_Normal;
varying vec2 v_Coords;

void main()
{
  vec4 diffuse = texture2D(color_texture, v_Coords);
  gl_FragColor = diffuse * 0.25;
  
  //player car light
  vec3 N = v_Normal + diffuse.rgb - 0.5 + 1.0;
  //diffuse light
  gl_FragColor.rgb *= dot(N, N) * diffuse.rgb;
}
END