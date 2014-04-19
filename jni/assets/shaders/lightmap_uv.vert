attribute vec3 v_vertex;
attribute vec3 v_normal;
attribute vec2 v_coord;
attribute vec2 v_tid;
varying vec2 v_Tid;

void main()
{
    v_Tid = v_coord;
   
    // lightmap coordinates
    gl_Position.xy = v_tid * 2.0 - 1.0;
    gl_Position.zw = vec2(0.0, 1.0);
}
