JAVAC=javac
sources = $(wildcard *.java)
classes = $(sources:.java=.class)

default: chatclient.c chatserve.java
	gcc -o chatclient chatclient.c

all: $(classes) chatclient

%.class : %.java
	$(JAVAC) $<

clean:
	rm -f *.class chatclient