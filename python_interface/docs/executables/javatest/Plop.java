import java.io.*; 
public class Plop{
  public static void main(String[] args) { 
    BufferedReader in = 
        new BufferedReader(new InputStreamReader(System.in)); 
    String name = "Instructor"; 
    System.out.print("Give your name: "); 
    //try {name = in.readLine();}
    //    catch(Exception e) {
    //       System.out.println("Caught an exception!"); 
    //    }
    System.out.println("Hello roger!"); 
    try {
        String line;
        Process p = Runtime.getRuntime().exec
            ("ls /tmp /home");
        BufferedReader input =
            new BufferedReader
            (new InputStreamReader(p.getInputStream()));
        while ((line = input.readLine()) != null) {
            System.out.println(line);
        }
        input.close();
        String[] cmd = {"/bin/sh", "-c" ,"sleep 10"};
        Process p1 = Runtime.getRuntime().exec(cmd);
        p1.waitFor();
        String[] cmd2 = {"/bin/sh", "-c", "ls > hello"};
        Runtime.getRuntime().exec(cmd2);
    }
    catch (Exception err) {
        err.printStackTrace();
    }
  }
}
