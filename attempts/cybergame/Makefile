FLAGS=`pkg-config --cflags --libs libdrm`
FLAGS+=-Wall -Ofast
FLAGS+=-D_FILE_OFFSET_BITS=64
FLAGS+=-lm
FLAGS+=-funroll-all-loops
FLAGS+=-mxsave -mtune=intel -maes -madx -mabm -mbmi -mbmi2 
FLAGS+=-mfancy-math-387 -mfp-ret-in-387 -mpc32 -mfpmath=387,sse #-m80387
#FLAGS+=-DCURSOR_SMOOTHING


all:
	#gcc -o rmmedrm rmmedrm.c $(FLAGS) -nostdlib
	gcc -o modeset modeset.c $(FLAGS)
	#gcc -o gbmmodeset gbmmodeset.c $(FLAGS)
	#gcc -o modeset-double-buffered modeset-double-buffered.c $(FLAGS)
	#gcc -o modeset-vsync modeset-vsync.c $(FLAGS)
