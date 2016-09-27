VERT
uniform mat4 u_Matrix;
attribute vec3 v_vertex;
attribute vec3 v_normal;
varying vec3 v_Color;

void main()
{
    v_Color = v_normal;
    gl_Position = u_Matrix * vec4(v_vertex, 1.0);
}
END

FRAG
varying vec3 v_Color;

void main()
{
  gl_FragColor = vec4(v_Color, 1.0);
}
END
