import org.ow2.proactive.scheduler.common.task.executable.JavaExecutable;
import org.ow2.proactive.scheduler.common.task.TaskResult;
import java.io.Serializable; 
import org.objectweb.proactive.core.util.URIBuilder; 
import org.objectweb.proactive.core.util.ProActiveInet; 

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.*; 

import java.util.Map;

public class MyTask extends JavaExecutable {

    private String localspace;
    private String result = "resultat\n";
    @Override
    public Serializable execute(TaskResult... results) throws Throwable {
        String message;

        try {
            System.err.println("Démarrage de la tache WaitAndPrint");
            System.out.println("Parameters are : ");

            for (TaskResult tRes : results) {
                if (tRes.hadException()) {
                    System.out.println("\t " + tRes.getTaskId() + " : " + tRes.getException().getMessage());
                } else {
                    System.out.println("\t " + tRes.getTaskId() + ": " + tRes.value());
                }
            }

            //message = URIBuilder.getLocalAddress().toString();
            localspace = getLocalSpace().getRealURI().replace("file://","").replace("file://","");
            message = URIBuilder.getHostNameorIP(ProActiveInet.getInstance().getInetAddress()); 
            System.out.println("ok roger");
            String sys = System.getProperty("os.name");
            String arch = System.getProperty("os.arch");
            //PrintWriter ecrivain;

            //ecrivain =  new PrintWriter(new BufferedWriter
            //        (new FileWriter(localspace+"/"+message+".txt")));

            //ecrivain.println("bonjour, comment cela va-t-il ?");
            //ecrivain.println("un peu difficile ?");
            //ecrivain.close();

            PrintWriter out  = new PrintWriter(new FileWriter(localspace+"/"+message+"2.txt"));
            out.println("je suis executé sur "+message+" os : "+System.getProperty("os.name")+" "+System.getProperty("os.arch"));
            out.close();

            /*int i;
            for(i=0;i<10;i++){
                Thread.sleep(1000);
                System.out.println("Iteration "+i+" on machine "+message+" localspace : "+localspace+" osname : "+System.getProperty("os.name")+" "+System.getProperty("os.arch") );
            }
            for (Map.Entry<Object, Object> e : System.getProperties().entrySet()) {
                System.out.println(e);
            }*/

            String executable = "";
            if (sys.contains("Linux")){
                if (arch.contains("86")){
                    executable = "diyabc-comput-linux-i386";
                }
                else if (arch.contains("64")){
                    executable = "diyabc-comput-linux-x64";
                }
                Process pchmod = Runtime.getRuntime().exec("chmod 755 "+localspace+"/"+executable);
            }
            if (sys.contains("Windows")){
                localspace = localspace.substring(1);
                if (arch.contains("86")){
                    executable = "diyabc-comput-win-i386";
                }
                else if (arch.contains("64")){
                    executable = "diyabc-comput-win-x64";
                }
            }

            String line;
            Process p = Runtime.getRuntime().exec
                (localspace+"/"+executable+" -h");
            System.out.println("I launched "+localspace+"/"+executable+" -h");
            //p.waitFor();
            System.out.println("Execution finished");
            BufferedReader input =
                new BufferedReader
                (new InputStreamReader(p.getInputStream()));
            while ((line = input.readLine()) != null) {
                System.out.println(line);
                this.result = this.result+"\n"+line;
            }
            input.close();


        } catch (Exception e) {
            message = "crashed";
            e.printStackTrace();
        }

        System.out.println("Task terminated, localspace : "+localspace);

        return (message + "\n"+this.result);
    }
}
