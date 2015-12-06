VERT
uniform mat4 u_ModelMatrix;
uniform mat4 u_ProjViewMatrix;
uniform float u_offset;
uniform float u_pass;
uniform vec4 u_sun_dir;
attribute vec3 v_vertex;
attribute vec3 v_normal;

void main()
{
    vec4 V = u_ModelMatrix * vec4(v_vertex, 1.0);
    gl_Position = u_ProjViewMatrix * (V + u_pass * u_sun_dir);
    gl_Position.z += u_offset;
}
END

FRAG
void main()
{
  gl_FragColor = vec4(0.25, 0.25, 0.25, 0.5);
}
END
