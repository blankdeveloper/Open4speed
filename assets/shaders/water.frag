uniform sampler2D color_texture;
uniform sampler2D EnvMap1;
varying vec2 v_Coords;
void main()
{
    gl_FragColor = texture2D(color_texture, v_Coords);
    gl_FragColor.a *= 0.2;
}
