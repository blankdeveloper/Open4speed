package geometry;
// unit to store vertex or normal of 3d point

public class Point3D {

  public float x;
  public float y;
  public float z;

  public Point3D(float x, float y, float z) {
    this.x = x;
    this.y = y;
    this.z = z;
  }

  public Point3D cross(Point3D v) {
    return new Point3D((y * v.z) - (z * v.y),
            (z * v.x) - (x * v.z),
            (x * v.y) - (y * v.x));
  }

  public Point3D minus(Point3D p) {
    return new Point3D(x - p.x, y - p.y, z - p.z);
  }

  public double size() {
    return Math.sqrt(x * x + y * y + z * z);
  }
}
