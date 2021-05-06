#version 420
#extension GL_ARB_arrays_of_arrays : require
#extension GL_ARB_explicit_uniform_location : require
uniform vec2 iResolution;
uniform vec2 iMouse;
uniform float iTime;
uniform ivec4 deck[52];//={{400,100,0,11},{0,0,0,12},{0,0,0,13},{0,0,0,14},{0,0,0,15},{0,0,0,16},{0,0,0,17},{0,0,0,18},{0,0,0,19},{0,0,0,110},{0,0,0,111},{0,0,0,112},{0,0,0,113},{0,0,0,21},{0,0,0,22},{0,0,0,23},{0,0,0,24},{0,0,0,25},{0,0,0,26},{0,0,0,27},{0,0,0,28},{0,0,0,29},{0,0,0,210},{0,0,0,211},{0,0,0,212},{0,0,0,213},{0,0,0,31},{0,0,0,32},{0,0,0,33},{0,0,0,34},{0,0,0,35},{0,0,0,36},{0,0,0,37},{0,0,0,38},{0,0,0,39},{0,0,0,310},{0,0,0,311},{0,0,0,312},{0,0,0,313},{0,0,0,41},{0,0,0,42},{0,0,0,43},{0,0,0,44},{0,0,0,45},{0,0,0,46},{0,0,0,47},{0,0,0,48},{0,0,0,49},{0,0,0,410},{0,0,0,411},{0,0,0,412},{0,0,0,413}};
vec3 color=vec3(0.0);
out vec3 fragColor;
void main(){
//	for(int n=0; n<52;n++){
//		if(deck[n][0]-40<gl_FragCoord.x&&deck[n][0]+40>gl_FragCoord.x&&-deck[n][1]-80<gl_FragCoord.y&&-deck[n][1]+80>gl_FragCoord.y){
//			color+=vec3(1.);
//		}
//	}
//	color+=vec3(0.0,iMouse.x-10<gl_FragCoord.x&&iMouse.x+10>gl_FragCoord.x&&-iMouse.y+10>gl_FragCoord.y&&-iMouse.y-10<gl_FragCoord.y,0.0);
	//if(deck[0].x-10<gl_FragCoord.x&&deck[0].x+10>gl_FragCoord.x&&deck[1].y-10<gl_FragCoord.y&&deck[1].y+10>gl_FragCoord.y){
	//	color=vec3(1.0);
	//}
	//color=vec3(0.0,float((deck[0][0]-40<gl_FragCoord.x&&deck[0][0]+40>gl_FragCoord.x&&-deck[0][1]-80<gl_FragCoord.y&&-deck[0][1]+80>gl_FragCoord.y)||(deck[1][0]-40<gl_FragCoord.x&&deck[1][0]+40>gl_FragCoord.x&&-deck[1][1]-80<gl_FragCoord.y&&-deck[1][1]+80>gl_FragCoord.y)||(deck[2][0]-40<gl_FragCoord.x&&deck[2][0]+40>gl_FragCoord.x&&-deck[2][1]-80<gl_FragCoord.y&&-deck[2][1]+80>gl_FragCoord.y)||(deck[3][0]-40<gl_FragCoord.x&&deck[3][0]+40>gl_FragCoord.x&&-deck[3][1]-80<gl_FragCoord.y&&-deck[3][1]+80>gl_FragCoord.y)||(deck[4][0]-40<gl_FragCoord.x&&deck[4][0]+40>gl_FragCoord.x&&-deck[4][1]-80<gl_FragCoord.y&&-deck[4][1]+80>gl_FragCoord.y)||(deck[5][0]-40<gl_FragCoord.x&&deck[5][0]+40>gl_FragCoord.x&&-deck[5][1]-80<gl_FragCoord.y&&-deck[5][1]+80>gl_FragCoord.y)||(deck[7][0]-40<gl_FragCoord.x&&deck[7][0]+40>gl_FragCoord.x&&-deck[7][1]-80<gl_FragCoord.y&&-deck[7][1]+80>gl_FragCoord.y)||(deck[6][0]-40<gl_FragCoord.x&&deck[6][0]+40>gl_FragCoord.x&&-deck[6][1]-80<gl_FragCoord.y&&-deck[6][1]+80>gl_FragCoord.y)||(deck[8][0]-40<gl_FragCoord.x&&deck[8][0]+40>gl_FragCoord.x&&-deck[8][1]-80<gl_FragCoord.y&&-deck[8][1]+80>gl_FragCoord.y)||(deck[9][0]-40<gl_FragCoord.x&&deck[9][0]+40>gl_FragCoord.x&&-deck[9][1]-80<gl_FragCoord.y&&-deck[9][1]+80>gl_FragCoord.y)||(deck[10][0]-40<gl_FragCoord.x&&deck[10][0]+40>gl_FragCoord.x&&-deck[10][1]-80<gl_FragCoord.y&&-deck[10][1]+80>gl_FragCoord.y)||(deck[11][0]-40<gl_FragCoord.x&&deck[11][0]+40>gl_FragCoord.x&&-deck[11][1]-80<gl_FragCoord.y&&-deck[11][1]+80>gl_FragCoord.y)||(deck[12][0]-40<gl_FragCoord.x&&deck[12][0]+40>gl_FragCoord.x&&-deck[12][1]-80<gl_FragCoord.y&&-deck[12][1]+80>gl_FragCoord.y)||(deck[13][0]-40<gl_FragCoord.x&&deck[13][0]+40>gl_FragCoord.x&&-deck[13][1]-80<gl_FragCoord.y&&-deck[13][1]+80>gl_FragCoord.y)||(deck[14][0]-40<gl_FragCoord.x&&deck[14][0]+40>gl_FragCoord.x&&-deck[14][1]-80<gl_FragCoord.y&&-deck[14][1]+80>gl_FragCoord.y)||(deck[15][0]-40<gl_FragCoord.x&&deck[15][0]+40>gl_FragCoord.x&&-deck[15][1]-80<gl_FragCoord.y&&-deck[15][1]+80>gl_FragCoord.y)||(deck[16][0]-40<gl_FragCoord.x&&deck[16][0]+40>gl_FragCoord.x&&-deck[16][1]-80<gl_FragCoord.y&&-deck[16][1]+80>gl_FragCoord.y)||(deck[17][0]-40<gl_FragCoord.x&&deck[17][0]+40>gl_FragCoord.x&&-deck[17][1]-80<gl_FragCoord.y&&-deck[17][1]+80>gl_FragCoord.y)||(deck[18][0]-40<gl_FragCoord.x&&deck[18][0]+40>gl_FragCoord.x&&-deck[18][1]-80<gl_FragCoord.y&&-deck[18][1]+80>gl_FragCoord.y)||(deck[19][0]-40<gl_FragCoord.x&&deck[19][0]+40>gl_FragCoord.x&&-deck[19][1]-80<gl_FragCoord.y&&-deck[19][1]+80>gl_FragCoord.y)||(deck[20][0]-40<gl_FragCoord.x&&deck[20][0]+40>gl_FragCoord.x&&-deck[20][1]-80<gl_FragCoord.y&&-deck[20][1]+80>gl_FragCoord.y)||(deck[21][0]-40<gl_FragCoord.x&&deck[21][0]+40>gl_FragCoord.x&&-deck[21][1]-80<gl_FragCoord.y&&-deck[21][1]+80>gl_FragCoord.y)||(deck[22][0]-40<gl_FragCoord.x&&deck[22][0]+40>gl_FragCoord.x&&-deck[22][1]-80<gl_FragCoord.y&&-deck[22][1]+80>gl_FragCoord.y)||(deck[23][0]-40<gl_FragCoord.x&&deck[23][0]+40>gl_FragCoord.x&&-deck[23][1]-80<gl_FragCoord.y&&-deck[23][1]+80>gl_FragCoord.y)||(deck[24][0]-40<gl_FragCoord.x&&deck[24][0]+40>gl_FragCoord.x&&-deck[24][1]-80<gl_FragCoord.y&&-deck[24][1]+80>gl_FragCoord.y)||(deck[25][0]-40<gl_FragCoord.x&&deck[25][0]+40>gl_FragCoord.x&&-deck[25][1]-80<gl_FragCoord.y&&-deck[25][1]+80>gl_FragCoord.y)||(deck[26][0]-40<gl_FragCoord.x&&deck[26][0]+40>gl_FragCoord.x&&-deck[26][1]-80<gl_FragCoord.y&&-deck[26][1]+80>gl_FragCoord.y)||(deck[27][0]-40<gl_FragCoord.x&&deck[27][0]+40>gl_FragCoord.x&&-deck[27][1]-80<gl_FragCoord.y&&-deck[27][1]+80>gl_FragCoord.y)||(deck[28][0]-40<gl_FragCoord.x&&deck[28][0]+40>gl_FragCoord.x&&-deck[28][1]-80<gl_FragCoord.y&&-deck[28][1]+80>gl_FragCoord.y)||(deck[29][0]-40<gl_FragCoord.x&&deck[29][0]+40>gl_FragCoord.x&&-deck[29][1]-80<gl_FragCoord.y&&-deck[29][1]+80>gl_FragCoord.y)||(deck[30][0]-40<gl_FragCoord.x&&deck[30][0]+40>gl_FragCoord.x&&-deck[30][1]-80<gl_FragCoord.y&&-deck[30][1]+80>gl_FragCoord.y)||(deck[31][0]-40<gl_FragCoord.x&&deck[31][0]+40>gl_FragCoord.x&&-deck[31][1]-80<gl_FragCoord.y&&-deck[31][1]+80>gl_FragCoord.y)||(deck[32][0]-40<gl_FragCoord.x&&deck[32][0]+40>gl_FragCoord.x&&-deck[32][1]-80<gl_FragCoord.y&&-deck[32][1]+80>gl_FragCoord.y)||(deck[33][0]-40<gl_FragCoord.x&&deck[33][0]+40>gl_FragCoord.x&&-deck[33][1]-80<gl_FragCoord.y&&-deck[33][1]+80>gl_FragCoord.y)||(deck[34][0]-40<gl_FragCoord.x&&deck[34][0]+40>gl_FragCoord.x&&-deck[34][1]-80<gl_FragCoord.y&&-deck[34][1]+80>gl_FragCoord.y)||(deck[35][0]-40<gl_FragCoord.x&&deck[35][0]+40>gl_FragCoord.x&&-deck[35][1]-80<gl_FragCoord.y&&-deck[35][1]+80>gl_FragCoord.y)||(deck[36][0]-40<gl_FragCoord.x&&deck[36][0]+40>gl_FragCoord.x&&-deck[36][1]-80<gl_FragCoord.y&&-deck[36][1]+80>gl_FragCoord.y)||(deck[37][0]-40<gl_FragCoord.x&&deck[37][0]+40>gl_FragCoord.x&&-deck[37][1]-80<gl_FragCoord.y&&-deck[37][1]+80>gl_FragCoord.y)||(deck[38][0]-40<gl_FragCoord.x&&deck[38][0]+40>gl_FragCoord.x&&-deck[38][1]-80<gl_FragCoord.y&&-deck[38][1]+80>gl_FragCoord.y)||(deck[39][0]-40<gl_FragCoord.x&&deck[39][0]+40>gl_FragCoord.x&&-deck[39][1]-80<gl_FragCoord.y&&-deck[39][1]+80>gl_FragCoord.y)||(deck[40][0]-40<gl_FragCoord.x&&deck[40][0]+40>gl_FragCoord.x&&-deck[40][1]-80<gl_FragCoord.y&&-deck[40][1]+80>gl_FragCoord.y)||(deck[41][0]-40<gl_FragCoord.x&&deck[41][0]+40>gl_FragCoord.x&&-deck[41][1]-80<gl_FragCoord.y&&-deck[41][1]+80>gl_FragCoord.y)||(deck[42][0]-40<gl_FragCoord.x&&deck[42][0]+40>gl_FragCoord.x&&-deck[42][1]-80<gl_FragCoord.y&&-deck[42][1]+80>gl_FragCoord.y)||(deck[43][0]-40<gl_FragCoord.x&&deck[43][0]+40>gl_FragCoord.x&&-deck[43][1]-80<gl_FragCoord.y&&-deck[43][1]+80>gl_FragCoord.y)||(deck[44][0]-40<gl_FragCoord.x&&deck[44][0]+40>gl_FragCoord.x&&-deck[44][1]-80<gl_FragCoord.y&&-deck[44][1]+80>gl_FragCoord.y)||(deck[45][0]-40<gl_FragCoord.x&&deck[45][0]+40>gl_FragCoord.x&&-deck[45][1]-80<gl_FragCoord.y&&-deck[45][1]+80>gl_FragCoord.y)||(deck[46][0]-40<gl_FragCoord.x&&deck[46][0]+40>gl_FragCoord.x&&-deck[46][1]-80<gl_FragCoord.y&&-deck[46][1]+80>gl_FragCoord.y)||(deck[47][0]-40<gl_FragCoord.x&&deck[47][0]+40>gl_FragCoord.x&&-deck[47][1]-80<gl_FragCoord.y&&-deck[47][1]+80>gl_FragCoord.y)||(deck[48][0]-40<gl_FragCoord.x&&deck[48][0]+40>gl_FragCoord.x&&-deck[48][1]-80<gl_FragCoord.y&&-deck[48][1]+80>gl_FragCoord.y)||(deck[49][0]-40<gl_FragCoord.x&&deck[49][0]+40>gl_FragCoord.x&&-deck[49][1]-80<gl_FragCoord.y&&-deck[49][1]+80>gl_FragCoord.y)||(deck[50][0]-40<gl_FragCoord.x&&deck[50][0]+40>gl_FragCoord.x&&-deck[50][1]-80<gl_FragCoord.y&&-deck[50][1]+80>gl_FragCoord.y)||(deck[51][0]-40<gl_FragCoord.x&&deck[51][0]+40>gl_FragCoord.x&&-deck[51][1]-80<gl_FragCoord.y&&-deck[51][1]+80>gl_FragCoord.y)),float(iMouse.x-10<gl_FragCoord.x&&iMouse.x+10>gl_FragCoord.x&&-iMouse.y+10>gl_FragCoord.y&&-iMouse.y-10<gl_FragCoord.y));
	//color=vec3(0.0,float(uint(deck[0][0]-40<gl_FragCoord.x&&deck[0][0]+40>gl_FragCoord.x&&-deck[0][1]-80<gl_FragCoord.y&&-deck[0][1]+80>gl_FragCoord.y)*deck[0][3]+10000*uint(deck[1][0]-40<gl_FragCoord.x&&deck[1][0]+40>gl_FragCoord.x&&-deck[1][1]-80<gl_FragCoord.y&&-deck[1][1]+80>gl_FragCoord.y)+uint(deck[2][0]-40<gl_FragCoord.x&&deck[2][0]+40>gl_FragCoord.x&&-deck[2][1]-80<gl_FragCoord.y&&-deck[2][1]+80>gl_FragCoord.y)+uint(deck[3][0]-40<gl_FragCoord.x&&deck[3][0]+40>gl_FragCoord.x&&-deck[3][1]-80<gl_FragCoord.y&&-deck[3][1]+80>gl_FragCoord.y)+uint(deck[4][0]-40<gl_FragCoord.x&&deck[4][0]+40>gl_FragCoord.x&&-deck[4][1]-80<gl_FragCoord.y&&-deck[4][1]+80>gl_FragCoord.y)+uint(deck[5][0]-40<gl_FragCoord.x&&deck[5][0]+40>gl_FragCoord.x&&-deck[5][1]-80<gl_FragCoord.y&&-deck[5][1]+80>gl_FragCoord.y)+uint(deck[7][0]-40<gl_FragCoord.x&&deck[7][0]+40>gl_FragCoord.x&&-deck[7][1]-80<gl_FragCoord.y&&-deck[7][1]+80>gl_FragCoord.y)+uint(deck[6][0]-40<gl_FragCoord.x&&deck[6][0]+40>gl_FragCoord.x&&-deck[6][1]-80<gl_FragCoord.y&&-deck[6][1]+80>gl_FragCoord.y)+uint(deck[8][0]-40<gl_FragCoord.x&&deck[8][0]+40>gl_FragCoord.x&&-deck[8][1]-80<gl_FragCoord.y&&-deck[8][1]+80>gl_FragCoord.y)+uint(deck[9][0]-40<gl_FragCoord.x&&deck[9][0]+40>gl_FragCoord.x&&-deck[9][1]-80<gl_FragCoord.y&&-deck[9][1]+80>gl_FragCoord.y)+uint(deck[10][0]-40<gl_FragCoord.x&&deck[10][0]+40>gl_FragCoord.x&&-deck[10][1]-80<gl_FragCoord.y&&-deck[10][1]+80>gl_FragCoord.y)+uint(deck[11][0]-40<gl_FragCoord.x&&deck[11][0]+40>gl_FragCoord.x&&-deck[11][1]-80<gl_FragCoord.y&&-deck[11][1]+80>gl_FragCoord.y)+uint(deck[12][0]-40<gl_FragCoord.x&&deck[12][0]+40>gl_FragCoord.x&&-deck[12][1]-80<gl_FragCoord.y&&-deck[12][1]+80>gl_FragCoord.y)+uint(deck[13][0]-40<gl_FragCoord.x&&deck[13][0]+40>gl_FragCoord.x&&-deck[13][1]-80<gl_FragCoord.y&&-deck[13][1]+80>gl_FragCoord.y)+uint(deck[14][0]-40<gl_FragCoord.x&&deck[14][0]+40>gl_FragCoord.x&&-deck[14][1]-80<gl_FragCoord.y&&-deck[14][1]+80>gl_FragCoord.y)+uint(deck[15][0]-40<gl_FragCoord.x&&deck[15][0]+40>gl_FragCoord.x&&-deck[15][1]-80<gl_FragCoord.y&&-deck[15][1]+80>gl_FragCoord.y)+uint(deck[16][0]-40<gl_FragCoord.x&&deck[16][0]+40>gl_FragCoord.x&&-deck[16][1]-80<gl_FragCoord.y&&-deck[16][1]+80>gl_FragCoord.y)+uint(deck[17][0]-40<gl_FragCoord.x&&deck[17][0]+40>gl_FragCoord.x&&-deck[17][1]-80<gl_FragCoord.y&&-deck[17][1]+80>gl_FragCoord.y)+uint(deck[18][0]-40<gl_FragCoord.x&&deck[18][0]+40>gl_FragCoord.x&&-deck[18][1]-80<gl_FragCoord.y&&-deck[18][1]+80>gl_FragCoord.y)+uint(deck[19][0]-40<gl_FragCoord.x&&deck[19][0]+40>gl_FragCoord.x&&-deck[19][1]-80<gl_FragCoord.y&&-deck[19][1]+80>gl_FragCoord.y)+uint(deck[20][0]-40<gl_FragCoord.x&&deck[20][0]+40>gl_FragCoord.x&&-deck[20][1]-80<gl_FragCoord.y&&-deck[20][1]+80>gl_FragCoord.y)+uint(deck[21][0]-40<gl_FragCoord.x&&deck[21][0]+40>gl_FragCoord.x&&-deck[21][1]-80<gl_FragCoord.y&&-deck[21][1]+80>gl_FragCoord.y)+uint(deck[22][0]-40<gl_FragCoord.x&&deck[22][0]+40>gl_FragCoord.x&&-deck[22][1]-80<gl_FragCoord.y&&-deck[22][1]+80>gl_FragCoord.y)+uint(deck[23][0]-40<gl_FragCoord.x&&deck[23][0]+40>gl_FragCoord.x&&-deck[23][1]-80<gl_FragCoord.y&&-deck[23][1]+80>gl_FragCoord.y)+uint(deck[24][0]-40<gl_FragCoord.x&&deck[24][0]+40>gl_FragCoord.x&&-deck[24][1]-80<gl_FragCoord.y&&-deck[24][1]+80>gl_FragCoord.y)+uint(deck[25][0]-40<gl_FragCoord.x&&deck[25][0]+40>gl_FragCoord.x&&-deck[25][1]-80<gl_FragCoord.y&&-deck[25][1]+80>gl_FragCoord.y)+uint(deck[26][0]-40<gl_FragCoord.x&&deck[26][0]+40>gl_FragCoord.x&&-deck[26][1]-80<gl_FragCoord.y&&-deck[26][1]+80>gl_FragCoord.y)+uint(deck[27][0]-40<gl_FragCoord.x&&deck[27][0]+40>gl_FragCoord.x&&-deck[27][1]-80<gl_FragCoord.y&&-deck[27][1]+80>gl_FragCoord.y)+uint(deck[28][0]-40<gl_FragCoord.x&&deck[28][0]+40>gl_FragCoord.x&&-deck[28][1]-80<gl_FragCoord.y&&-deck[28][1]+80>gl_FragCoord.y)+uint(deck[29][0]-40<gl_FragCoord.x&&deck[29][0]+40>gl_FragCoord.x&&-deck[29][1]-80<gl_FragCoord.y&&-deck[29][1]+80>gl_FragCoord.y)+uint(deck[30][0]-40<gl_FragCoord.x&&deck[30][0]+40>gl_FragCoord.x&&-deck[30][1]-80<gl_FragCoord.y&&-deck[30][1]+80>gl_FragCoord.y)+uint(deck[31][0]-40<gl_FragCoord.x&&deck[31][0]+40>gl_FragCoord.x&&-deck[31][1]-80<gl_FragCoord.y&&-deck[31][1]+80>gl_FragCoord.y)+uint(deck[32][0]-40<gl_FragCoord.x&&deck[32][0]+40>gl_FragCoord.x&&-deck[32][1]-80<gl_FragCoord.y&&-deck[32][1]+80>gl_FragCoord.y)+uint(deck[33][0]-40<gl_FragCoord.x&&deck[33][0]+40>gl_FragCoord.x&&-deck[33][1]-80<gl_FragCoord.y&&-deck[33][1]+80>gl_FragCoord.y)+uint(deck[34][0]-40<gl_FragCoord.x&&deck[34][0]+40>gl_FragCoord.x&&-deck[34][1]-80<gl_FragCoord.y&&-deck[34][1]+80>gl_FragCoord.y)+uint(deck[35][0]-40<gl_FragCoord.x&&deck[35][0]+40>gl_FragCoord.x&&-deck[35][1]-80<gl_FragCoord.y&&-deck[35][1]+80>gl_FragCoord.y)+uint(deck[36][0]-40<gl_FragCoord.x&&deck[36][0]+40>gl_FragCoord.x&&-deck[36][1]-80<gl_FragCoord.y&&-deck[36][1]+80>gl_FragCoord.y)+uint(deck[37][0]-40<gl_FragCoord.x&&deck[37][0]+40>gl_FragCoord.x&&-deck[37][1]-80<gl_FragCoord.y&&-deck[37][1]+80>gl_FragCoord.y)+uint(deck[38][0]-40<gl_FragCoord.x&&deck[38][0]+40>gl_FragCoord.x&&-deck[38][1]-80<gl_FragCoord.y&&-deck[38][1]+80>gl_FragCoord.y)+uint(deck[39][0]-40<gl_FragCoord.x&&deck[39][0]+40>gl_FragCoord.x&&-deck[39][1]-80<gl_FragCoord.y&&-deck[39][1]+80>gl_FragCoord.y)+uint(deck[40][0]-40<gl_FragCoord.x&&deck[40][0]+40>gl_FragCoord.x&&-deck[40][1]-80<gl_FragCoord.y&&-deck[40][1]+80>gl_FragCoord.y)+uint(deck[41][0]-40<gl_FragCoord.x&&deck[41][0]+40>gl_FragCoord.x&&-deck[41][1]-80<gl_FragCoord.y&&-deck[41][1]+80>gl_FragCoord.y)+deck[42][3]*uint(deck[42][0]-40<gl_FragCoord.x&&deck[42][0]+40>gl_FragCoord.x&&-deck[42][1]-80<gl_FragCoord.y&&-deck[42][1]+80>gl_FragCoord.y)+uint(deck[43][0]-40<gl_FragCoord.x&&deck[43][0]+40>gl_FragCoord.x&&-deck[43][1]-80<gl_FragCoord.y&&-deck[43][1]+80>gl_FragCoord.y)+uint(deck[44][0]-40<gl_FragCoord.x&&deck[44][0]+40>gl_FragCoord.x&&-deck[44][1]-80<gl_FragCoord.y&&-deck[44][1]+80>gl_FragCoord.y)+uint(deck[45][0]-40<gl_FragCoord.x&&deck[45][0]+40>gl_FragCoord.x&&-deck[45][1]-80<gl_FragCoord.y&&-deck[45][1]+80>gl_FragCoord.y)+uint(deck[46][0]-40<gl_FragCoord.x&&deck[46][0]+40>gl_FragCoord.x&&-deck[46][1]-80<gl_FragCoord.y&&-deck[46][1]+80>gl_FragCoord.y)+uint(deck[47][0]-40<gl_FragCoord.x&&deck[47][0]+40>gl_FragCoord.x&&-deck[47][1]-80<gl_FragCoord.y&&-deck[47][1]+80>gl_FragCoord.y)+uint(deck[48][0]-40<gl_FragCoord.x&&deck[48][0]+40>gl_FragCoord.x&&-deck[48][1]-80<gl_FragCoord.y&&-deck[48][1]+80>gl_FragCoord.y)+uint(deck[49][0]-40<gl_FragCoord.x&&deck[49][0]+40>gl_FragCoord.x&&-deck[49][1]-80<gl_FragCoord.y&&-deck[49][1]+80>gl_FragCoord.y)+uint(deck[50][0]-40<gl_FragCoord.x&&deck[50][0]+40>gl_FragCoord.x&&-deck[50][1]-80<gl_FragCoord.y&&-deck[50][1]+80>gl_FragCoord.y)+uint(deck[51][0]-40<gl_FragCoord.x&&deck[51][0]+40>gl_FragCoord.x&&-deck[51][1]-80<gl_FragCoord.y&&-deck[51][1]+80>gl_FragCoord.y))/100.,float(iMouse.x-10<gl_FragCoord.x&&iMouse.x+10>gl_FragCoord.x&&-iMouse.y+10>gl_FragCoord.y&&-iMouse.y-10<gl_FragCoord.y));
	
	//fragColor=color;
	switch[
}