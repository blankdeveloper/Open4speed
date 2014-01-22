varying vec2 texture_coordinate;
uniform mat4 u_Matrix;
attribute vec3 v_vertex;
attribute vec3 v_normal;
attribute vec2 v_coord;
void main()
{
    texture_coordinate = v_coord.st;
    gl_Position = u_Matrix * vec4(v_vertex, 1.0);
}
