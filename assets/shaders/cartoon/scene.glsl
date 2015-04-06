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
uniform float u_speed;
uniform sampler2D color_texture;
varying vec2 texture_coordinate;

void main()
{
  float x = 2.0 * (texture_coordinate.x - 0.5);
  float y = 2.0 * (texture_coordinate.y - 0.5);
  float f = (1.25 - abs(x)) * (1.25 - abs(y));
  float b = u_speed * max(0.0, 1.25 - f);
  vec4 c = texture2D(color_texture, texture_coordinate)*0.125;
  c += texture2D(color_texture, texture_coordinate + vec2(0.001 * b, -0.007 * b))*0.0625;
  c += texture2D(color_texture, texture_coordinate + vec2(-0.002 * b, -0.006 * b))*0.0625;
  c += texture2D(color_texture, texture_coordinate + vec2(0.003 * b, -0.005 * b))*0.0625;
  c += texture2D(color_texture, texture_coordinate + vec2(-0.004 * b, -0.004 * b))*0.0625;
  c += texture2D(color_texture, texture_coordinate + vec2(0.005 * b, -0.003 * b))*0.0625;
  c += texture2D(color_texture, texture_coordinate + vec2(-0.006 * b, -0.002 * b))*0.0625;
  c += texture2D(color_texture, texture_coordinate + vec2(0.007 * b, -0.001 * b))*0.0625;
  c += texture2D(color_texture, texture_coordinate + vec2(-0.007 * b, 0.001 * b))*0.0625;
  c += texture2D(color_texture, texture_coordinate + vec2(0.006 * b, 0.002 * b))*0.0625;
  c += texture2D(color_texture, texture_coordinate + vec2(-0.005 * b, 0.003 * b))*0.0625;
  c += texture2D(color_texture, texture_coordinate + vec2(0.004 * b, 0.004 * b))*0.0625;
  c += texture2D(color_texture, texture_coordinate + vec2(-0.003 * b, 0.005 * b))*0.0625;
  c += texture2D(color_texture, texture_coordinate + vec2(0.002 * b, 0.006 * b))*0.0625;
  c += texture2D(color_texture, texture_coordinate + vec2(-0.001 * b, 0.007 * b))*0.0625;
  gl_FragColor = 1.0 * vec4(c.r * f * f, c.g * f * f, c.b * f * f, 1.0);
}
END
