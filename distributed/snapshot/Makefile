JFLAGS = -g
JC = javac
.SUFFIXES: .java .class
.java.class:
		$(JC) $(JFLAGS) $*.java

CLASSES = \
		Controller.java \
		Node.java \
		JavaProcess.java \
 		PortConfiguration.java 

default: classes

classes: $(CLASSES:.java=.class)

clean:
		$(RM) *.class
