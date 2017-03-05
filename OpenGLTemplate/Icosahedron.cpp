#include "Icosahedron.h"
CIcosahedron::CIcosahedron()
{} CIcosahedron::~CIcosahedron()
{ 
	Release();
}

glm::vec3 CIcosahedron::CrossAndNorm(glm::vec3 vecA, glm::vec3 vecB) {

	glm::vec3 normal = glm::cross(vecA, vecB);
	glm::normalize(normal);

	return normal;
}


void CIcosahedron::Create(string filename)
{
	m_texture.Load(filename);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();
	// Write the code to add interleaved vertex attributes to the VBO

	glm::vec3 planeVertices[60] =
	{
		glm::vec3(-0.5, 0, 1),// 0
		glm::vec3(0.5, 0, 1),// 1
		glm::vec3(0, 1, 0.5),// 2

		glm::vec3(-0.5, 0, 1),// 0
		glm::vec3(0, 1, 0.5),// 2
		glm::vec3(-1, 0.5, 0),// 3

		glm::vec3(0, 1, 0.5),// 2
		glm::vec3(0, 1, -0.5),// 8
		glm::vec3(-1, 0.5, 0),// 3

		glm::vec3(0, 1, -0.5),// 8
		glm::vec3(-0.5, 0, -1),// 11
		glm::vec3(-1, 0.5, 0),// 3

		glm::vec3(-0.5, 0, -1),// 11
		glm::vec3(-1, -0.5, 0),// 6
		glm::vec3(-1, 0.5, 0),// 3
		//-
		glm::vec3(-0.5, 0, 1),// 0
		glm::vec3(-1, 0.5, 0),// 3
		glm::vec3(-1, -0.5, 0),// 6

		glm::vec3(0, 1, 0.5),// 2
		glm::vec3(0.5, 0, 1),// 1
		glm::vec3(1, 0.5, 0),// 5

		glm::vec3(0.5, 0, 1),// 1
		glm::vec3(1, -0.5, 0),// 7
		glm::vec3(1, 0.5, 0),// 5

		glm::vec3(1, -0.5, 0),// 7
		glm::vec3(0.5, 0, -1),// 10
		glm::vec3(1, 0.5, 0),// 5

		glm::vec3(0.5, 0, -1),// 10
		glm::vec3(0, 1, -0.5),// 8
		glm::vec3(1, 0.5, 0),// 5
		//-
		glm::vec3(0, 1, -0.5),// 8
		glm::vec3(0, 1, 0.5),// 2
		glm::vec3(1, 0.5, 0),// 5

		glm::vec3(0.5, 0, 1),// 1
		glm::vec3(-0.5, 0, 1),// 0
		glm::vec3(0, -1, 0.5),// 4

		glm::vec3(-0.5, 0, 1),// 0
		glm::vec3(-1, -0.5, 0),// 6
		glm::vec3(0, -1, 0.5),// 4

		glm::vec3(-1, -0.5, 0),// 6
		glm::vec3(0, -1, -0.5),// 9
		glm::vec3(0, -1, 0.5),// 4

		glm::vec3(0, -1, -0.5),// 9
		glm::vec3(1, -0.5, 0),// 7
		glm::vec3(0, -1, 0.5),// 4
		//-
		glm::vec3(1, -0.5, 0),// 7
		glm::vec3(0.5, 0, 1),// 1
		glm::vec3(0, -1, 0.5),// 4

		glm::vec3(0, -1, -0.5),// 9
		glm::vec3(-0.5, 0, -1),// 11
		glm::vec3(0.5, 0, -1),// 10

		glm::vec3(-0.5, 0, -1),// 11
		glm::vec3(0, 1, -0.5),// 8
		glm::vec3(0.5, 0, -1),// 10

		glm::vec3(0, -1, -0.5),// 9
		glm::vec3(-1, -0.5, 0),// 6
		glm::vec3(-0.5, 0, -1),// 11

		glm::vec3(1, -0.5, 0),// 7
		glm::vec3(0, -1, -0.5),// 9
		glm::vec3(0.5, 0, -1),// 10

		/*
		//point for icosahedron
		
		glm::vec3(-0.5, 0, 1),// 0
		glm::vec3(0.5, 0, 1),// 1
		glm::vec3(0, 1, 0.5),// 2
		glm::vec3(-1, 0.5, 0),// 3
		glm::vec3(0, -1, 0.5),// 4
		glm::vec3(1, 0.5, 0),// 5
		glm::vec3(-1, -0.5, 0),// 6
		glm::vec3(1, -0.5, 0),// 7
		glm::vec3(0, 1, -0.5),// 8
		glm::vec3(0, -1, -0.5),// 9
		glm::vec3(0.5, 0, -1),// 10
		glm::vec3(-0.5, 0, -1),// 11
		*/
	};

	// Texture coordinates
	glm::vec2 planeTexCoords[4] =
	{
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),

	};

	// Put the vertex attributes in the VBO
	for (unsigned int i = 0; i < 60; i++) {

		m_vbo.AddData(&planeVertices[i], sizeof(glm::vec3));
		m_vbo.AddData(&planeTexCoords[i%3], sizeof(glm::vec2));
		
		//here I took advantage of mod operator to parse vector values into the function( and avoid if statements)

		glm::vec3 p0 = planeVertices[i   - i % 3];
		glm::vec3 p1 = planeVertices[i+1 - i % 3];
		glm::vec3 p2 = planeVertices[i+2 - i % 3];

		glm::vec3 v0 = (p1 - p0);
		glm::vec3 v1 = (p2 - p0);

		m_vbo.AddData(&CrossAndNorm(v0,v1), sizeof(glm::vec3));
	}

	// Upload data to GPU 
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}




void CIcosahedron::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	for (int i = 0; i < 60; i += 3) {
		glDrawArrays(GL_TRIANGLE_STRIP, i, 3);
	}

} 

void CIcosahedron::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release(); 
}