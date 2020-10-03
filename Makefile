
all: ConvexHull

ConvexHull: ConvexHull.cpp
	g++ -Wall -o $@ $< 

clean:
	rm -rf *.o ConvexHull 

