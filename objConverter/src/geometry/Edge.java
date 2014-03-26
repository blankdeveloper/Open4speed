package geometry;

// 3d points connected into edge
public class Edge {

  Point3D a, b;

  public Edge(Point3D a, Point3D b) {
    this.a = a;
    this.b = b;
  }

  static double dist(float x1, float y1, float z1, float x2, float y2, float z2) {
    return Math.sqrt(sqr(x2 - x1) + sqr(y2 - y1) + sqr(z2 - z1));
  }

  static float sqr(float a) {
    return a * a;
  }

  public static double dist(Vertex a, Vertex b) {
    return dist(a.v.x, a.v.y, a.v.z, b.v.x, b.v.y, b.v.z);
  }

  public static boolean isSame(Vertex k, Vertex l, Vertex r, Vertex s) {
    if ((k.t.u == r.t.u) && (k.t.v == r.t.v)) {
      if ((l.t.u == s.t.u) && (l.t.v == s.t.v)) {
        return true;
      }
    }
    return false;
  }

  // compressed values of edge
  public byte[] value() {
    String output = a.x + " " + a.y + " " + a.z + " ";
    output += b.x + " " + b.y + " " + b.z + "\n";
    return output.getBytes();
  }
}
