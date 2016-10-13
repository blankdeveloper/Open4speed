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
    for (int i = 0; i < faces.size(); i++)
    {
      if (min.x > faces.get(i).a.v.x)
      {
        min.x = faces.get(i).a.v.x;
      }
      if (min.y > faces.get(i).a.v.y)
      {
        min.y = faces.get(i).a.v.y;
      }
      if (min.z > faces.get(i).a.v.z)
      {
        min.z = faces.get(i).a.v.z;
      }

      if (max.x < faces.get(i).a.v.x)
      {
        max.x = faces.get(i).a.v.x;
      }
      if (max.y < faces.get(i).a.v.y)
      {
        max.y = faces.get(i).a.v.y;
      }
      if (max.z < faces.get(i).a.v.z)
      {
        max.z = faces.get(i).a.v.z;
      }

      if (min.x > faces.get(i).b.v.x)
      {
        min.x = faces.get(i).b.v.x;
      }
      if (min.y > faces.get(i).b.v.y)
      {
        min.y = faces.get(i).b.v.y;
      }
      if (min.z > faces.get(i).b.v.z)
      {
        min.z = faces.get(i).b.v.z;
      }

      if (max.x < faces.get(i).b.v.x)
      {
        max.x = faces.get(i).b.v.x;
      }
      if (max.y < faces.get(i).b.v.y)
      {
        max.y = faces.get(i).b.v.y;
      }
      if (max.z < faces.get(i).b.v.z)
      {
        max.z = faces.get(i).b.v.z;
      }

      if (min.x > faces.get(i).c.v.x)
      {
        min.x = faces.get(i).c.v.x;
      }
      if (min.y > faces.get(i).c.v.y)
      {
        min.y = faces.get(i).c.v.y;
      }
      if (min.z > faces.get(i).c.v.z)
      {
        min.z = faces.get(i).c.v.z;
      }

      if (max.x < faces.get(i).c.v.x)
      {
        max.x = faces.get(i).c.v.x;
      }
      if (max.y < faces.get(i).c.v.y)
      {
        max.y = faces.get(i).c.v.y;
      }
      if (max.z < faces.get(i).c.v.z)
      {
        max.z = faces.get(i).c.v.z;
      }
    }
    return new AABB(min, max);
  }
}
