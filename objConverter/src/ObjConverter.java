
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
    if (Common.init(args)) {
      ObjLoader.loadObj(args);
      Parser.parseObj(args);
      Unwrapper.unwrap();
      Subdivider.subdivide();
      Writer.write(args);
    }
  }
}
