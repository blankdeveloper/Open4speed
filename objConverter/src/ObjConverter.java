
import components.Common;
import components.ObjLoader;
import components.Parser;
import components.Subdivider;
import components.Unwrapper;
import components.Writer;

/**
 * Tool for converting OBJ models into O4S
 *
 * @author Lubos Vonasek
 */
public class ObjConverter {

  public static void main(String[] args) throws Exception {
    args = new String[2];
    args[0] = "tokyop.obj";
    args[1] = "tracks/tokyo/physics.o4s";

    if (Common.init(args)) {
      ObjLoader.loadObj(args);
      Parser.parseObj(args);
      Unwrapper.unwrap();
      Subdivider.subdivide();
      Writer.write(args);
    }
  }
}
