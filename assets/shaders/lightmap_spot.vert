uniform mat4 u_MatrixScl;
uniform mat4 u_ModelViewMatrix;
attribute vec3 v_vertex;
attribute vec3 v_normal;
attribute vec2 v_coord;
attribute vec2 v_tid;
varying vec2 v_Coords;
varying vec3 v_Normal;
varying vec3 v_Vertex;
varying vec4 posUV;

void main()
{
    // standart varyings
    v_Coords = v_coord;
    v_Normal = vec3(u_ModelViewMatrix * vec4(v_normal, 0.0));
    vec4 pos = u_ModelViewMatrix * vec4(v_vertex, 1.0);
    v_Vertex = pos.xyz;

    // count shadowmap position
    posUV = u_MatrixScl * vec4(v_vertex, 1.0);
    
    // lightmap coordinates
    gl_Position.xy = v_tid * 2.0 - 1.0;
    gl_Position.zw = vec2(0.0, 1.0);
}
