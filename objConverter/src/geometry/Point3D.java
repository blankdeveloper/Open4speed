package geometry;

// unit to store vertex or normal of 3d point

public class Point3D
{

  public float x;
  public float y;
  public float z;

  public Point3D(float x, float y, float z)
  {
    this.x = x;
    this.y = y;
    this.z = z;
  }
  
  public Point3D mix(Point3D p)
  {
    return new Point3D((p.x + x) * 0.5f, (p.y + y) * 0.5f, (p.z + z) * 0.5f);
  }
}
