package geometry;

// unit for store full 3d point
public class Vertex
{
  private static int missingCoord  = 0;
  private static int missingNormal = 0;

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
  String value()
  {
    String output = "";
    if (t == null)
    {
      output += "0 0 ";
      missingCoord++;
    } else
    {
      output += t.u + " " + t.v + " ";
    }

    if (n == null)
    {
      output += "0 0 0 ";
      missingNormal++;
    } else
    {
      output += n.x + " " + n.y + " " + n.z + " ";
    }

    return output + v.x + " " + v.y + " " + v.z + " ";
  }
  
  public static void printStatistics()
  {
    if (missingNormal > 0)
    {
      System.out.println(missingNormal + " polygons haven't normal.");
    }
    if (missingCoord > 0)
    {
      System.out.println(missingCoord + " vertices haven't texture coords.");
    }
  }
  
  public static void resetStatistics()
  {
    missingCoord = 0;
    missingNormal = 0;
  }
}