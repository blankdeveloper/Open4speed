package geometry;

// unit for store full 3d point
public class Vertex
{
  public Point2D     t;
  public Point3D     v;
  public Point3D     n;

  public Vertex(Point3D n, Point2D t, Point3D v)
  {
    this.n = n;
    this.t = t;
    this.v = v;
  }

  // compressed values of point
  String value(Point3D base)
  {
    String output = "";
    if (t == null)
      output += "0 0 ";
    else
      output += t.u + " " + t.v + " ";

    if (n == null)
      output += "0 0 0 ";
    else
      output += n.x + " " + n.y + " " + n.z + " ";

    return output + (v.x - base.x) + " " + (v.y - base.y) + " " + (v.z - base.z) + " ";
  }
}