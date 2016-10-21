package geometry;

import java.util.ArrayList;

// storage for models
public class Model
{
  public ArrayList<Triangle> faces;
  public String              material;

  public Model(String t)
  {
    faces = new ArrayList<Triangle>();
    material = t;
  }
  
  public boolean isDynamic()
  {
    return material.contains("$");
  }
  
  public AABB getAABB()
  {
    Point3D min = new Point3D(99999, 99999, 99999);
    Point3D max = new Point3D(-99999, -99999, -99999);
    for (Triangle t : faces)
    {
      if (min.x > t.a.v.x)
        min.x = t.a.v.x;
      if (min.y > t.a.v.y)
        min.y = t.a.v.y;
      if (min.z > t.a.v.z)
        min.z = t.a.v.z;
      if (max.x < t.a.v.x)
        max.x = t.a.v.x;
      if (max.y < t.a.v.y)
        max.y = t.a.v.y;
      if (max.z < t.a.v.z)
        max.z = t.a.v.z;

      if (min.x > t.b.v.x)
        min.x = t.b.v.x;
      if (min.y > t.b.v.y)
        min.y = t.b.v.y;
      if (min.z > t.b.v.z)
        min.z = t.b.v.z;
      if (max.x < t.b.v.x)
        max.x = t.b.v.x;
      if (max.y < t.b.v.y)
        max.y = t.b.v.y;
      if (max.z < t.b.v.z)
        max.z = t.b.v.z;

      if (min.x > t.c.v.x)
        min.x = t.c.v.x;
      if (min.y > t.c.v.y)
        min.y = t.c.v.y;
      if (min.z > t.c.v.z)
        min.z = t.c.v.z;
      if (max.x < t.c.v.x)
        max.x = t.c.v.x;
      if (max.y < t.c.v.y)
        max.y = t.c.v.y;
      if (max.z < t.c.v.z)
        max.z = t.c.v.z;
    }
    return new AABB(min, max);
  }
}
