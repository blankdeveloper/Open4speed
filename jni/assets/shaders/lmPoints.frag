uniform vec4 color;
varying float intensity;

void main()
{
  gl_FragColor = color * intensity;
}
