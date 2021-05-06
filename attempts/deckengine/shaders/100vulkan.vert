#version 100

//layout(location = 0) out vec2 uv; // [0, 1]
//vec2 uv;

void main() {
	// generate [-1, 1] quad for gl_Position
	//uv = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(2 * vec2((gl_VertexID << 1) & 2, gl_VertexID & 2) - 1.0f, 0.0f, 1.0f);
} 
