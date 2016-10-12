package geometry;

public class AABB
{
  Point3D min;
  Point3D max;
  
  public AABB(Point3D min, Point3D max)
  {
    this.min = min;
    this.max = max;
  }

  public Point3D getMin()
  {
    return min;
  }

  public Point3D getMax()
  {
    return max;
  }
}
