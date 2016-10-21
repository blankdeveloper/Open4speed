package geometry;

public class Id3D
{
  int x;
  int y;
  int z;
  
  String value = "";
  
  public static final int CULLING_DST = 50;
  
  public boolean set(Triangle t)
  {
    int ox = x;
    int oy = y;
    int oz = z;
    this.x = (int) ((t.a.v.x + t.b.v.x + t.c.v.x) / 3.0f / CULLING_DST);
    this.y = (int) ((t.a.v.y + t.b.v.y + t.c.v.y) / 3.0f / CULLING_DST);
    this.z = (int) ((t.a.v.z + t.b.v.z + t.c.v.z) / 3.0f / CULLING_DST);
    if ((ox == x) && (oy == y) && (oz == z))
      return false;
    value = "." + x + "." + y + "." + z;
    return true;
  }

  public void set(AABB aabb)
  {
    x = (int) ((aabb.min.x + aabb.max.x) / 2 / CULLING_DST);
    y = (int) ((aabb.min.y + aabb.max.y) / 2 / CULLING_DST);
    z = (int) ((aabb.min.z + aabb.max.z) / 2 / CULLING_DST);
    value = "." + x + "." + y + "." + z;
  }
  
  public String value()
  {
    return value;
  }
}
