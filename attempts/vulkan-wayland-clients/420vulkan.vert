#version 420

layout(location = 0) out vec2 uv; // [0, 1]
//vec2 uv;

void main() {
	// generate [-1, 1] quad for gl_Position
	uv = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	// generate screen size triangle
	//uv = vec2(2 * vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2) - 1.0f);
} 
