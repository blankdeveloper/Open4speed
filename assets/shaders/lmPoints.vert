attribute vec3 v_vertex;
varying float intensity;
void main()
{
    intensity = v_vertex.z;
    gl_Position = vec4(2.0 * v_vertex.xy - 1.0, 0.0, 1.0);
}
