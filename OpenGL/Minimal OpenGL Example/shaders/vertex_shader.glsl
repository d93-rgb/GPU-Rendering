#version 330 core

const vec2 quadVertices[3] = { vec2(0.0, 1.0), vec2(-1.0, -1.0), vec2(1.0, -1.0)};

out vec3 color;

void main()
{
	color = vec3(gl_VertexID == 0, gl_VertexID == 1, gl_VertexID == 2);
    gl_Position = vec4(quadVertices[gl_VertexID], 0.0, 1.0);
}