#include "Tetrahedron.h"
CTetrahedron::CTetrahedron()
{} CTetrahedron::~CTetrahedron()
{ 
	Release();
}

void CTetrahedron::Create(string filename)
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

	//z

	

	glm::vec3 planeVertices[12] =
	{
		//back
		glm::vec3(-1, -1, 1),// 0
		glm::vec3(-1, 1, -1),// 2
		glm::vec3(1, -1, -1),// 1
		//{0, 2, -2}
		//{2, 0, -2}
		//(-0.57735, -0.57735, -0.57735)
		//right
		glm::vec3(-1, -1, 1),// 0
		glm::vec3(1, 1, 1),  // 3
		glm::vec3(-1, 1, -1),// 2
		//{2, 2, 0}
		//{0, 2, -2}
		//(-0.57735, 0.57735, 0.57735)
		//down
		glm::vec3(-1, -1, 1),// 0
		glm::vec3(1, -1, -1),// 1
		glm::vec3(1, 1, 1),  // 3
		//{2, 0, -2}
		//{2, 2, 0}
		//(0.57735, -0.57735, 0.57735)
		//left
		glm::vec3(1, -1, -1),// 1
		glm::vec3(-1, 1, -1),// 2
		glm::vec3(1, 1, 1),  // 3
		//{-2, 2, 0}
		//{0, 2, 2}
		//(0.57735, 0.57735, -0.57735)

	};

	// Texture coordinates
	glm::vec2 planeTexCoords[4] =
	{
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),

	};

	/*
	glm::vec3(0, 0, -1),
		glm::vec3(0, 0, 1),
		glm::vec3(1, 0, 0),
		glm::vec3(-1, 0, 0),
	
	*/
	/*
	//normals v2
	glm::vec3(0, 0.89, -0.44),
		glm::vec3(-0.09, 0.9, 0),
		glm::vec3(0.7, 0, 0.7),
		glm::vec3(0.7, -0.07, -0.70),
	
	*/

	/*
	normals v2
	
		glm::vec3(-0.5, 0, -0.5),
		glm::vec3(0, 0.5, 0.5),
		glm::vec3(0.5, -0.5, 0),
		glm::vec3(0, 0.5, -0.5),
	*/

	/*
	glm::vec3(-0.57735, -0.57735, -0.57735),
		glm::vec3(-0.57735, 0.57735, 0.57735),
		glm::vec3(0.57735, -0.57735, 0.57735),
		glm::vec3(0.57735, 0.57735, -0.57735),
	
	*/
	//cross product normilised
	glm::vec3 planeNormals[4] =
	{
		//seem to be working fine
		glm::vec3(-0.33, -0.33, -0.33),
		glm::vec3(-0.33, 0.33, 0.33),
		glm::vec3(0.33, -0.33, 0.33),
		glm::vec3(0.33, 0.33, -0.33),

	};

	// Plane normal
	//glm::vec3 planeNormal = glm::vec3(0.0f, 1.0f, 0.0f);


	// Put the vertex attributes in the VBO
	for (unsigned int i = 0; i < 12; i++) {
		m_vbo.AddData(&planeVertices[i], sizeof(glm::vec3));
		m_vbo.AddData(&planeTexCoords[i%3], sizeof(glm::vec2));
		m_vbo.AddData(&planeNormals[i/3], sizeof(glm::vec3));
		
		//m_vbo.AddData(&planeNormal, sizeof(glm::vec3));
	}
	//---end!


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

void CTetrahedron::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	// Call glDrawArrays to render each side
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 3, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 6, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 9, 3);


} 

void CTetrahedron::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release(); 
}