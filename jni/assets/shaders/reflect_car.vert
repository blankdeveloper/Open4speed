uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;
attribute vec3 v_vertex;
attribute vec3 v_normal;
attribute vec2 v_coord;
varying vec3 v_Vertex;
varying vec3 v_Vertex2;
varying vec3 v_Normal;
varying vec2 v_Coords;
void main()
{
    v_Vertex2 = v_vertex;
    vec4 view_pos = u_ModelViewMatrix * vec4(v_vertex, 1.0);
    v_Normal = vec3(u_ModelViewMatrix * vec4(v_normal, 0.0));
    v_Vertex = view_pos.xyz;
    v_Coords = v_coord;
    gl_Position = u_ProjectionMatrix * view_pos;
}
