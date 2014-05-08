package components;

import geometry.Graph;
import geometry.Point2D;
import geometry.Point3D;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;


public class Common {

	public static final int cellSize = 300;
	public static int cellWidth;
	public static int cellHeight;
	public static int missingCoord = 0;
	public static int missingNormal = 0;
	public static String path;
	public static String prefix = " ";
	public static BufferedReader sc;
	public static String currentMaterialLib = "";

	// create time stamp
	public static long timestamp = System.currentTimeMillis();

	// variables to count extreme coordinates of model
	public static float minx = Float.MAX_VALUE;
	public static float miny = Float.MAX_VALUE;
	public static float minz = Float.MAX_VALUE;
	public static float maxx = Float.MIN_VALUE;
	public static float maxy = Float.MIN_VALUE;
	public static float maxz = Float.MIN_VALUE;

	// create storage in memory for OBJ model
	public static ArrayList<Point3D> vertices = new ArrayList<>();
	public static ArrayList<Point3D> normals = new ArrayList<>();
	public static ArrayList<Point2D> texCoords = new ArrayList<>();
	public static ArrayList<MTLLoader> materials = new ArrayList<>();
	public static ArrayList<Graph> graph = new ArrayList<>();

	/**
	 * Program code
	 * 
	 * @param args
	 *            <input.obj> <xxx/yyy.o4s>
   * @return 
	 * @throws IOException
	 */
	public static boolean init(String[] args) throws IOException {

		/*
		 * args = new String[2]; args[0] = "bin/room.obj"; args[1] =
		 * "bin/tracks/room.o4s";
		 */

		// header
		String[] text = {
				" ___________________________________________________________",
				"|                                                           |",
				"|            Open4speed by L.Vonásek OBJ converter          |",
				"|                      version 1.00                         |",
				"|___________________________________________________________|",
				"usage:   java -jar ObjConverter.jar <input.obj> <xxx/yyy.o4s>",
                "           [lightmap precision] [lightmap max. triangle size]",
				"" };
        
        for (String text1 : text) {
            System.out.println(text1);
        }

		// check arguments
		if (args.length < 2) {
			System.err.println("Invalid arguments");
			return false;
		}
        
		if (args.length > 2) {
			Unwrapper.LIGHTMAP_PRECISION = Float.valueOf(args[2]);
		}

		if (args.length > 3) {
			Unwrapper.LIGHTMAP_MAX_TRIANGLE = Integer.valueOf(args[3]);
		}

		// Unix absolute path report
		if ((args[0].charAt(0) == '/') | (args[1].charAt(0) == '/')) {
			System.err.println("Do not use absoulute path");
			return false;
		}

		// windows absolute path report
		if ((args[1].charAt(1) == ':') | (args[1].charAt(1) == ':')) {
			System.err.println("Do not use absoulute path");
			return false;
		}

		if (!ObjLoader.exists(args[0])) {
			System.err.println("Input file doesn't exist");
			return false;
		}
		
		// get path of output directory
		try {
			path = args[1].substring(0, args[1].lastIndexOf('/'));
			// backup old directory
			new File(path).renameTo(new File("backup-"
					+ System.currentTimeMillis()));
		} catch (Exception e) {
			System.err
					.println("Do not put output file into converter directory");
			return false;
		}
		// create output directory
		new File(path).mkdirs();
		
		return true;
	}
}