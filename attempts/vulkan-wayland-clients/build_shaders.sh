#!/bin/sh

cd $(dirname $0)/shaders
if [ -z $1 ]
then

glslangValidator -V100 shader.vert -o ../spv/main.vert.spv
glslangValidator -V100 shader.frag -o ../spv/main.frag.spv
glslangValidator -V100 shader.comp -o ../spv/main.comp.spv
for n in *.frag
do 
	glslangValidator -V100 "$n" -o ../spv/"$n".spv
done

else
	glslangValidator -V100 "$1.frag" -o ../spv/"$1.frag".spv
fi
