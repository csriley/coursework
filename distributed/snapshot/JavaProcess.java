import java.io.File;
import java.io.IOException;
import java.lang.ProcessBuilder.Redirect;


public final class JavaProcess {

    private JavaProcess() {}     
    public static ProcessBuilder exec(Class klass, String args) throws IOException {
        String javaHome = System.getProperty("java.home");
        String javaBin = javaHome + File.separator + "bin" + File.separator + "java";
        String classPath = System.getProperty("java.class.path");
        String className = klass.getCanonicalName();
        ProcessBuilder builder = new ProcessBuilder(javaBin, "-cp", classPath, className, args);
        builder.redirectErrorStream(true);
        builder.redirectOutput(Redirect.INHERIT);
		return builder;
    }
}