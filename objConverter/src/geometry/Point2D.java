package geometry;
// unit to store texture coord of 3d point
public class Point2D {

	public float u, v;

	public Point2D(float u, float v) {
		this.u = u;
		this.v = v;
	}
	
	public static double dist(Point a, Point b) {
		return Edge.dist(a.t.u, a.t.v, 1, b.t.u, b.t.v, 1);
	}
	
}