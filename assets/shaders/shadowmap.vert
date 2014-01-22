uniform mat4 u_Matrix;
attribute vec3 v_vertex;
attribute vec2 v_coord;
varying vec2 v_Coords;

void main()
{
    v_Coords = v_coord;
    gl_Position = u_Matrix * vec4(v_vertex, 1.0);
}
