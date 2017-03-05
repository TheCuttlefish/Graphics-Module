#include "Cube.h"
CCube::CCube()
{} CCube::~CCube()
{ 
	Release();
}

void CCube::Create(string filename)
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


	/*
	
	glm::vec3 p0 = glm::vec3(-1, -1, -1);
	glm::vec3 p1 = glm::vec3(1, -1, -1);
	glm::vec3 p2 = glm::vec3(-1, 1, -1);
	glm::vec3 p3 = glm::vec3(1,1, -1);
	
	m_vbo.AddData(&p0, sizeof(glm::vec3));
	m_vbo.AddData(&p1, sizeof(glm::vec3));
	m_vbo.AddData(&p2, sizeof(glm::vec3));
	
	m_vbo.AddData(&p1, sizeof(glm::vec3));
	m_vbo.AddData(&p3, sizeof(glm::vec3));
	m_vbo.AddData(&p2, sizeof(glm::vec3));
	
	*/

	// Vertex positions

	/*
	//wrong bindings
	glm::vec3(-1, -1, -1),//0
		glm::vec3(1, -1, -1),//1
		glm::vec3(-1, 1, -1),//2
		glm::vec3(1, 1, -1),//3

		glm::vec3(-1, -1, 1),//4
		glm::vec3(1, -1, 1),//5
		glm::vec3(-1, 1, 1),//6
		glm::vec3(1, 1, 1),//7

		glm::vec3(1, -1, 1),//5
		glm::vec3(1, -1, -1),//1
		glm::vec3(1, 1, 1),//7
		glm::vec3(1, 1, -1),//3

		glm::vec3(-1, -1, 1),//4
		glm::vec3(-1, -1, -1),//0
		glm::vec3(-1, 1, 1),//6
		glm::vec3(-1, 1, -1),//2

		glm::vec3(1, 1, 1),//7
		glm::vec3(1, 1, -1),//3
		glm::vec3(-1, 1, 1),//6
		glm::vec3(-1, 1, -1),//2

		glm::vec3(1, -1, 1),//5
		glm::vec3(1, -1, -1),//1
		glm::vec3(-1, -1, 1),//4
		glm::vec3(-1, -1, -1),//0
	
	*/

	/*
	*/
	

	glm::vec3 planeVertices[24] =
	{

		
		//-z
		glm::vec3(1, -1, -1),//1
		glm::vec3(-1, -1, -1),//0
		glm::vec3(1, 1, -1),//3
		glm::vec3(-1, 1, -1),//2

		//+z
		glm::vec3(-1, -1, 1),//4
		glm::vec3(1, -1, 1),//5
		glm::vec3(-1, 1, 1),//6
		glm::vec3(1, 1, 1),//7

		//+x
		glm::vec3(1, -1, 1),//5
		glm::vec3(1, -1, -1),//1
		glm::vec3(1, 1, 1),//7
		glm::vec3(1, 1, -1),//3

		//-x
		glm::vec3(-1, -1, -1),//0
		glm::vec3(-1, -1, 1),//4
		glm::vec3(-1, 1, -1),//2
		glm::vec3(-1, 1, 1),//6

		//+y
		glm::vec3(1, 1, 1),//7
		glm::vec3(1, 1, -1),//3
		glm::vec3(-1, 1, 1),//6
		glm::vec3(-1, 1, -1),//2

		//-y
		glm::vec3(1, -1, -1),//1
		glm::vec3(1, -1, 1),//5
		glm::vec3(-1, -1, -1),//0
		glm::vec3(-1, -1, 1),//4
		
	};

	// Texture coordinates
	glm::vec2 planeTexCoords[4] =
	{
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),

	};




	glm::vec3 planeNormals[6] =
	{
		//z
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, 1),
		//x
		glm::vec3(1, 0, 0),
		glm::vec3(-1, 0, 0),
		//y
		glm::vec3(0, 1, 0),
		glm::vec3(0, -1, 0),

	};

	// Plane normal
	//glm::vec3 planeNormal = glm::vec3(0.0f, 1.0f, 0.0f);


	// Put the vertex attributes in the VBO
	for (unsigned int i = 0; i < 24; i++) {
		m_vbo.AddData(&planeVertices[i], sizeof(glm::vec3));
		m_vbo.AddData(&planeTexCoords[i%4], sizeof(glm::vec2));
		m_vbo.AddData(&planeNormals[i/4], sizeof(glm::vec3));
		
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

void CCube::Render() 
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	// Call glDrawArrays to render each side
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

	glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);

	glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

} 

void CCube::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release(); 
}