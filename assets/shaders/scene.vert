varying vec2 texture_coordinate;
attribute vec4 v_vertex;
attribute vec2 v_coord;
void main()
{
    texture_coordinate = v_coord.st;
    gl_Position = v_vertex;
}
