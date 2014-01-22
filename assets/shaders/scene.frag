uniform sampler2D color_texture;
varying vec2 texture_coordinate;

void main() {
  gl_FragColor = texture2D(color_texture, texture_coordinate);
}
