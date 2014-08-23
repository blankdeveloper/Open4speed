VERT
varying vec2 texture_coordinate;
attribute vec3 v_vertex;
attribute vec2 v_coord;
void main()
{
    texture_coordinate = v_coord.st;
    gl_Position = vec4(v_vertex.xy, 0.0, 1.0);
}
END

FRAG
uniform sampler2D color_texture;
varying vec2 texture_coordinate;

void main() {
  gl_FragColor = texture2D(color_texture, texture_coordinate);
}
END
