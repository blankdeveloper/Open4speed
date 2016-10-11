import geometry.Vertex;

import java.io.File;

import components.ObjLoader;

/**
 * Tool for converting OBJ models into O4S
 * 
 * @author Lubos Vonasek
 */
public class ObjConverter
{

  public static void main(String[] args) throws Exception
  {

    // header
    String[] text = { 
        " ___________________________________________________________",
        "|                                                           |",
        "|            Open4speed by L.Vonasek OBJ converter          |",
        "|___________________________________________________________|",
        "usage:   java -jar ObjConverter.jar <input.obj> <xxx/yyy.o4s>", "" };

    for (String text1 : text)
    {
      System.out.println(text1);
    }

    // check arguments
    if (args.length != 2)
    {
      System.err.println("Invalid arguments");
      return;
    }

    // Unix absolute path report
    if ((args[0].charAt(0) == '/') | (args[1].charAt(0) == '/'))
    {
      System.err.println("Do not use absoulute path");
      return;
    }

    // windows absolute path report
    if ((args[1].charAt(1) == ':') | (args[1].charAt(1) == ':'))
    {
      System.err.println("Do not use absoulute path");
      return;
    }

    if (!ObjLoader.exists(args[0]))
    {
      System.err.println("Input file doesn't exist");
      return;
    }

    // get path of output directory
    String path = "";
    try
    {
      path = args[1].substring(0, args[1].lastIndexOf('/'));
      // backup old directory
      new File(path).renameTo(new File("backup-" + System.currentTimeMillis()));
    } catch (Exception e)
    {
      System.err.println("Do not put output file into converter directory");
      return;
    }
    // create output directory
    new File(path).mkdirs();

    // process
    long timestamp = System.currentTimeMillis();
    Vertex.resetStatistics();
    ObjLoader obj = new ObjLoader(path);
    obj.loadObj(args[0]);
    obj.parseObj(args[0]);
    obj.writeO4S(args[1]);
    int time = (int) ((System.currentTimeMillis() - timestamp) / 1000);
    System.out.println("Done in " + time + " seconds.");
  }
}
