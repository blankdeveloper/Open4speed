package geometry;

// unit to store texture coord of 3d point
public class Point2D
{

  public float u, v;

  public Point2D(float u, float v)
  {
    this.u = u;
    this.v = v;
  }
  
  public Point2D mix(Point2D p)
  {
    return new Point2D((p.u + u) * 0.5f, (p.v + v) * 0.5f);
  }
}