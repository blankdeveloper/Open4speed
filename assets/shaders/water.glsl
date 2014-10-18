VERT
uniform mat4 u_Matrix;
attribute vec3 v_vertex;
attribute vec2 v_coord;
varying vec2 v_Coords;
void main()
{
    v_Coords = v_coord;
    gl_Position = u_Matrix * vec4(v_vertex, 1.0);
    gl_Position.xy += 2.0 * (v_Coords.xy * 1.0 - 0.5) * (2.75 - min(gl_Position.z, 2.5));
    gl_Position.y += 0.25;
    gl_Position.xy *= (gl_Position.z < 1.0) ? pow(gl_Position.z, 4.0) : 1.0;
    gl_Position.z -= (gl_Position.z < 0.5) ? 1.0 : 0.0;
}
END

FRAG
uniform sampler2D color_texture;
varying vec2 v_Coords;
void main()
{
    gl_FragColor = texture2D(color_texture, v_Coords);
    gl_FragColor.a *= 0.15;
}
END