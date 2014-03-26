package components;

import geometry.Edge;
import geometry.Vertex;
import geometry.Point2D;
import geometry.Point3D;
import geometry.Triangle;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.StringTokenizer;

public class Parser {

	public static void parseObj(String args[]) throws IOException {
		
		System.out.print("Parsing OBJ file...");
		ArrayList<Triangle> faces = null;
        int graphIndex = 0;
		FileInputStream fis;
		try {
			fis = new FileInputStream(args[0]);
		} catch (FileNotFoundException ex) {
			System.err.println("Unable to open input file");
			return;
		}
		Common.sc = new BufferedReader(new InputStreamReader(fis));

		// parse all faces
		while (Common.sc.ready()) {
			// read one line of file
			String line = Common.sc.readLine();

			// set prefix of material
			if (line.startsWith("o ")) {
                graphIndex++;
				Common.prefix = line.substring(1);
				if (!Common.prefix.contains("#")
						&& !Common.prefix.contains("$")
						&& !Common.prefix.contains("%")
						&& !Common.prefix.contains("!")) {
					Common.prefix = " ";
				}
			}

			// set material library
			if (line.startsWith("mtllib ")) {
				String mtlPath = args[0].substring(0,
						args[0].lastIndexOf('/') + 1);
				Common.currentMaterialLib = mtlPath + line.substring(7);
			}

			// change current material
			if (line.startsWith("usemtl ")) {
				// get parameters of material
				String material = MTLLoader.getMaterial(
						Common.currentMaterialLib, line.substring(7));
				material += Common.prefix;
				// check if material already exists
				boolean assigned = false;
				for (int i = 0; i < Common.materials.size(); i++) {
					if (Common.materials.get(i).parameters
							.compareTo(material) == 0) {
						faces = Common.materials.get(i).faces;
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
					faces = Common.materials
							.get(Common.materials.size() - 1).faces;
				}
			}

			// add edge into memory
			if (line.startsWith("l ")) {
				StringTokenizer lsc = new StringTokenizer(line);
				lsc.nextToken();
				// create object for points
				Vertex[] point = new Vertex[4];
				// set index of current point
				int pointsCount = 0;
				// parse vertices
				while (lsc.hasMoreTokens()) {
					// get point as scanner
					StringTokenizer vsc = new StringTokenizer(lsc.nextToken()
							.replace('/', ' '));

					// convert value into point
					Point3D vertex = Common.vertices.get(Integer
							.valueOf(vsc.nextToken()) - 1);
					Point2D texCoord = null;
					if (vsc.hasMoreTokens()
							& (Common.texCoords.size() > 0)) {
						int index = Integer.valueOf(vsc.nextToken()) - 1;
						if (index < Common.texCoords.size()) {
							texCoord = Common.texCoords.get(index);
						}
					}
					Point3D normal = null;
					if (vsc.hasMoreTokens() & (Common.normals.size() > 0)) {
						normal = Common.normals.get(Integer.valueOf(vsc
								.nextToken()) - 1);
					}

					// save point
					point[pointsCount++] = new Vertex(normal, texCoord, vertex);
				}
				if (pointsCount == 2) {
					Common.graph.get(graphIndex).edges.add(new Edge(point[0].v, point[1].v));
				}
			}

			// add face into memory
			if (line.startsWith("f ")) {
				StringTokenizer lsc = new StringTokenizer(line);
				lsc.nextToken();
				// create object for points
				Vertex[] point = new Vertex[4];
				// set index of current point
				int pointsCount = 0;
				// parse vertices
				while (lsc.hasMoreTokens()) {
					// get point as scanner
					StringTokenizer vsc = new StringTokenizer(lsc.nextToken()
							.replace('/', ' '));

					// convert value into point
					Point3D vertex = Common.vertices.get(Integer
							.valueOf(vsc.nextToken()) - 1);
					Point2D texCoord = null;
					if (vsc.hasMoreTokens()
							& (Common.texCoords.size() > 0)) {
						int index = Integer.valueOf(vsc.nextToken()) - 1;
						if (index < Common.texCoords.size()) {
							texCoord = Common.texCoords.get(index);
						}
					}
					Point3D normal = null;
					if (vsc.hasMoreTokens() & (Common.normals.size() > 0)) {
						normal = Common.normals.get(Integer.valueOf(vsc
								.nextToken()) - 1);
					}

					// save point
					point[pointsCount++] = new Vertex(normal, texCoord, vertex);
				}

				switch (pointsCount) {
				case (2):
					Common.graph.get(graphIndex).edges.add(new Edge(point[0].v, point[1].v));
					break;
				// save triangle
				case (3):
					faces.add(new Triangle(point[0], point[1], point[2]));
					break;
				// save polygons(separate it to two triangles)
				case (4):
					faces.add(new Triangle(point[3], point[0], point[1]));
					faces.add(new Triangle(point[3], point[1], point[2]));
					break;
				}
			}
		}

		fis.close();
		System.out.println("OK");
	}
}
