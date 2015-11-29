VERT
uniform mat4 u_Matrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjViewMatrix;
uniform float u_offset;
attribute vec3 v_vertex;
attribute vec3 v_normal;

const vec4 u_sun_dir = vec4(-1.5, -1.0, 0.0, 0.0);

void main()
{
    float v = dot((u_ModelViewMatrix * vec4(v_normal, 0.0)).xyz, (u_ViewMatrix * u_sun_dir).xyz) > 0.0 ? 0.005 : 65535.0;
    gl_Position = u_Matrix * (vec4(v_vertex, 1.0)) + v * u_ProjViewMatrix * u_sun_dir;
    gl_Position.z += u_offset;
}
END

FRAG
void main()
{
  gl_FragColor = vec4(0.25, 0.25, 0.25, 0.5);
}
END
