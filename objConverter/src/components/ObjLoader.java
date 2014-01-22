package components;

import geometry.Graph;
import geometry.Point2D;
import geometry.Point3D;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.StringTokenizer;

public class ObjLoader {

	public static void loadObj(String[] args) throws IOException {

		System.out.print("Loading file into memory...");

		// open input file
		FileInputStream fis;
		try {
			fis = new FileInputStream(args[0]);
		} catch (FileNotFoundException ex) {
			System.err.println("Unable to open input file");
			return;
		}
		Common.sc = new BufferedReader(new InputStreamReader(fis));

		// for broken faces create null material
		Common.materials.add(new MTLLoader("(null)"));
		// set link for saving edges
        Common.graph.add(new Graph());

		while (Common.sc.ready()) {
			// read one line of file
			String line = Common.sc.readLine();

			// set prefix of material
			if (line.startsWith("o ")) {
                //new graph for tracks/lights
                Common.graph.add(new Graph());
                //material prefix
				Common.prefix = line.substring(1);
				if (!Common.prefix.contains("#") && !Common.prefix.contains("$")
						&& !Common.prefix.contains("%") && !Common.prefix.contains("!")) {
					Common.prefix = " ";
				}
			}

			// set material library
			if (line.startsWith("mtllib ")) {
				String mtlPath = args[0].substring(0, args[0].lastIndexOf('/') + 1);
				Common.currentMaterialLib = mtlPath + line.substring(7);
			}

			// change current material
			if (line.startsWith("usemtl ")) {
				// get parameters of material
				String material = MTLLoader.getMaterial(Common.currentMaterialLib,
						line.substring(7));
				material += Common.prefix;
				// check if material already exists
				boolean assigned = false;
				for (int i = 0; i < Common.materials.size(); i++) {
					if (Common.materials.get(i).parameters.compareTo(material) == 0) {
						assigned = true;
					}
				}
				// if material doesn't exist then create it
				if (!assigned) {
					if (MTLLoader.top) {
						Common.materials.add(new MTLLoader(material));
					} else {
						Common.materials.add(1, new MTLLoader(material));
					}
				}
			}

			// add vertex into memory
			if (line.startsWith("v ")) {
				StringTokenizer lsc = new StringTokenizer(line);
				lsc.nextToken();
				// get values
				float x = stringToFloat(lsc.nextToken());
				float y = stringToFloat(lsc.nextToken());
				float z = stringToFloat(lsc.nextToken());
				// update extreme values
				if (x < Common.minx) {
					Common.minx = x;
				}
				if (y < Common.miny) {
					Common.miny = y;
				}
				if (z < Common.minz) {
					Common.minz = z;
				}
				if (x > Common.maxx) {
					Common.maxx = x;
				}
				if (y > Common.maxy) {
					Common.maxy = y;
				}
				if (z > Common.maxz) {
					Common.maxz = z;
				}
				// add it into memory
				Common.vertices.add(new Point3D(x, y, z));
			}

			// add normal into memory
			if (line.startsWith("vn ")) {
				StringTokenizer lsc = new StringTokenizer(line);
				lsc.nextToken();
				// get values
				float x = stringToFloat(lsc.nextToken());
				float y = stringToFloat(lsc.nextToken());
				float z = stringToFloat(lsc.nextToken());
				// add it into memory
				Common.normals.add(new Point3D(x, y, z));
			}

			// add texture coordinate into memory
			if (line.startsWith("vt ")) {
				StringTokenizer lsc = new StringTokenizer(line);
				lsc.nextToken();
				// get values
				float x = stringToFloat(lsc.nextToken());
				float y = stringToFloat(lsc.nextToken());
				// add it into memory
				Common.texCoords.add(new Point2D(x, y));
			}
		}
		fis.close();
		System.out.println("OK");
	}

	// check if file exists
	public static boolean exists(String filename) {
		try {
			FileInputStream fis = new FileInputStream(filename);
			if (fis.available() > 0) {
				fis.close();
				return true;
			} else {
				fis.close();
				return false;
			}
		} catch (IOException e) {
			return false;
		}
	}

	// convert string to float
	public static float stringToFloat(String value) {
		try {
			return Float.valueOf(value);
		} catch (NumberFormatException e) {
			return 0;
		}
	}
}
