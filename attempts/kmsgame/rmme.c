#include <esTransform.c>
#include <stdio.h>

float verts[4][4]{
	{1,1,0,1},
	{0,1,0,1},
	{0,0,0,1},
	{1,0,1,1}
};
int main(void)
{
	esMatrix mk;
	esMatrixLoadIdentity(&mk);
	esMatrixMultiply
}
