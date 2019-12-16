#gcc -static-libgcc -I/usr/include/libdrm -I./ -L/usr/lib -I/usr/lib -O3 -d -c main.c -lOpenGL -lGL -ldrm -lm -lEGL -lgbm
#gcc -static-libgcc -flto -I/usr/include/libdrm -I./ -L/usr/lib -I/usr/lib -O3 main.o -o ds -lOpenGL -lGL -ldrm -lm -lEGL -lgbm
#gcc -static-libgcc -flto -I/usr/include/libdrm -I./ -L/usr/lib -I/usr/lib  -O1 main.c -o ds -lGL -lOpenGL -lgbm -Lminigbm -ldrm -lm -lEGL -g
clang -g -static-libgcc -I/usr/include/libdrm -I./ -L/usr/lib -I/usr/lib -Ofast -flto main.c -o dc -lGL -lOpenGL -ldrm -lm -lEGL -lgbm
#gcc -lm -I/usr/include/libdrm -O1 main.c -o main
#gcc -lm -I/usr/include/libdrm  -I/usr/include -ldrm -lGLESv2 -lgbm -lGL  -lEGL -O1 main.s -o main.o
#gcc -lm -I/usr/include/libdrm  -I/usr/include -ldrm -lGLESv2 -lgbm -lGL  -lEGL -O1 main.o -o main
