#pragma once

#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"


// Class for generating a unit cube
class CIcosahedron
{
public: 
		CIcosahedron();
		~CIcosahedron();
		void Create(string filename); 
		void Render();
		void Release(); 
		glm::vec3 CrossAndNorm(glm::vec3 vecA, glm::vec3 vecB);//my function for getting cross product and normalising
private: 
		GLuint m_vao;
		CVertexBufferObject m_vbo;
		CTexture m_texture;

};