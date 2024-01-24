# Makefile for ChenH.CS5600.LearnC.c

# Default target 'all'
all: ChenH.CS5600.LearnC.c mpg2km.c mpg2km.h 
	gcc -o CS5600.LearnC ChenH.CS5600.LearnC.c mpg2km.c 

clean: 
	rm -f CS5600.LearnC
	touch ChenH.CS5600.LearnC.c mpg2km.c