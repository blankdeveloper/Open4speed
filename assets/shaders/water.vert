uniform mat4 u_Matrix;
attribute vec3 v_vertex;
attribute vec3 v_normal;
attribute vec2 v_coord;
varying vec2 v_Coords;
void main()
{
    v_Coords = v_coord;
    gl_Position = u_Matrix * vec4(v_vertex, 1.0);
    gl_Position.xy += 4.0 * (v_Coords.xy * 1.0 - 0.5) / max(gl_Position.z, 2.5);
    gl_Position.y += 0.5;
}
