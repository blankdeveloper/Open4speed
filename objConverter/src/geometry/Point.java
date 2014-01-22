package geometry;
import components.Common;

// unit for store full 3d point
public class Point {

	public Point2D t;
	public Point3D v;
	public Point3D n;

	public Point(Point3D n, Point2D t, Point3D v) {
		this.n = n;
		this.t = t;
		this.v = v;
	}

	public int getCell() {
		int x = (int) ((v.x - Common.minx) / Common.cellSize);
		int z = (int) ((v.z - Common.minz) / Common.cellSize);
		return z * Common.cellWidth + x;
	}
	
	// compressed values of point
	String value() {
		String output = "";
		if (t == null) {
			output += "0 0 ";
			Common.missingCoord++;
		} else {
			output += t.u + " " + t.v + " ";
		}

		if (n == null) {
			output += "0 0 0 ";
			Common.missingNormal++;
		} else {
			output += n.x + " " + n.y + " " + n.z + " ";
		}

		return output + v.x + " " + v.y + " " + v.z + " ";
	}
}