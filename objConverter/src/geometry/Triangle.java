package geometry;

import java.util.ArrayList;

// 3d points connected into triangle
public class Triangle
{
  private static double SQUARE_DST = sqr(Id3D.CULLING_DST);
  
  public Vertex a, b, c;
  private double ab, bc, ac;

  public Triangle(Vertex a, Vertex b, Vertex c)
  {
    this.a = a;
    this.b = b;
    this.c = c;
    ab = sqr(a.v.x - b.v.x) + sqr(a.v.y - b.v.y) + sqr(a.v.z - b.v.z);
    bc = sqr(b.v.x - c.v.x) + sqr(b.v.y - c.v.y) + sqr(b.v.z - c.v.z);
    ac = sqr(a.v.x - c.v.x) + sqr(a.v.y - c.v.y) + sqr(a.v.z - c.v.z);
  }

  public boolean isTooBig()
  {
    return max(ab, bc, ac) > SQUARE_DST;
  }

  private static double max(double ab, double bc, double ac)
  {
    if ((ab > bc) && (ab > ac))
      return ab;
    else if ((bc > ab) && (bc > ac))
      return bc;
    else
      return ac;
  }
  
  private static double sqr(float v)
  {
    return v * v;
  }

  public static void subdivide(ArrayList<Triangle> output, Triangle t)
  {
    if(max(t.ab, t.bc, t.ac) > SQUARE_DST)
    {
      if ((t.ab > t.bc) && (t.ab > t.ac))
      {
        Vertex a2b = new Vertex(t.a.v.mix(t.b.v), t.a.t.mix(t.b.t), t.a.n.mix(t.b.n));
        output.add(new Triangle(t.c, a2b, t.a));
        output.add(new Triangle(t.b, a2b, t.c));
      }
      else if ((t.bc > t.ab) && (t.bc > t.ac))
      {
        Vertex b2c = new Vertex(t.b.v.mix(t.c.v), t.b.t.mix(t.c.t), t.b.n.mix(t.c.n));
        output.add(new Triangle(t.a, b2c, t.b));
        output.add(new Triangle(t.c, b2c, t.a));
      }
      else
      {
        Vertex a2c = new Vertex(t.a.v.mix(t.c.v), t.a.t.mix(t.c.t), t.a.n.mix(t.c.n));
        output.add(new Triangle(t.b, a2c, t.c));
        output.add(new Triangle(t.a, a2c, t.b));
      }
    }
    else
      output.add(t);
  }
  
  // compressed values of triangle
  public byte[] value(Point3D base)
  {
    String output = a.value(base) + b.value(base) + c.value(base) + "\n";
    return output.getBytes();
  }
}