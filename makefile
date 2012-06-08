# Generic Makefile that should work with any program you're going to compile.
# Any complaints should be directed at torsten@sfu.ca
#
# To compile and link your program all you have to do is run 'make' in the
#    current directory.
# To clean up object files run 'make clean_object'.
# To delete any compiled files run 'make clean'.
# Originated in 2001 by Haris Teguh
# modified Sep-2011 by Torsten Moeller

# Including of non standard library files:
#   INCLUDEDIR is where the header files can be found
#   LIBDIR is where the library object files can be found
INCLUDEDIR=/usr/include/
LIBDIR=/usr/lib

# If you have more source files add them here 
SOURCE= raytrace.cpp

# The compiler we are using 
CC= g++

# The flags that will be used to compile the object file.
# If you want to debug your program,
# you can add '-g' on the following line
#CFLAGS= -O3 -Wall -pedantic -DGL_GLEXT_PROTOTYPES

# The name of the final executable 
EXECUTABLE=raytrace

# The basic library we are using add the other libraries you want to link
# to your program here 

# If you have other library files in a different directory add them here 
INCLUDEFLAG= -I. -I$(INCLUDEDIR)
LIBFLAG= -L$(LIBDIR)

# Don't touch this one if you don't know what you're doing 
OBJECT= $(SOURCE:.cpp=.o)

# Don't touch any of these either if you don't know what you're doing 
all: $(OBJECT) depend
	$(CC) $(CFLAGS) $(INCLUDEFLAG) $(LIBFLAG) $(LDFLAGS) $(OBJECT) -o $(EXECUTABLE)

depend:
	$(CC) -M $(SOURCE) > depend

$(OBJECT):
	$(CC) $(CFLAGS) $(INCLUDEFLAG) -c -o $@ $(@:.o=.cpp)

clean_object:
	rm -f $(OBJECT)

clean:
	rm -f $(OBJECT) depend $(EXECUTABLE)

test1:
	./raytrace Scenes/example1.scn
test2:
	./raytrace Scenes/example2.scn
test3:
	./raytrace Scenes/example3.scn
test4:
	./raytrace Scenes/example4.scn
test5:
	./raytrace Scenes/example5.scn
testextra:
	./raytrace Scenes/texture_spheres.scn
	./raytrace Scenes/spotlight.scn
testall:
	./raytrace Scenes/example1.scn
	./raytrace Scenes/example2.scn
	./raytrace Scenes/example3.scn
	./raytrace Scenes/example4.scn
	./raytrace Scenes/example5.scn
	./raytrace Scenes/texture_spheres.scn
	./raytrace Scenes/spotlight.scn
include depend
