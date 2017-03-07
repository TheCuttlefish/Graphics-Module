#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>



CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;
}

CCatmullRom::~CCatmullRom()
{}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f*p1 - 3.0f*p2 + p3);

	return a + b*t + c*t2 + d*t3;

}


void CCatmullRom::SetControlPoints()
{

	m_controlPoints.push_back(glm::vec3(100, 5, 0));
	m_controlPoints.push_back(glm::vec3(71, 10, 71));
	m_controlPoints.push_back(glm::vec3(0, 5, 100));
	m_controlPoints.push_back(glm::vec3(-71, 5, 71));
	m_controlPoints.push_back(glm::vec3(-100, 5, 0));
	m_controlPoints.push_back(glm::vec3(-71, 15, -71));
	m_controlPoints.push_back(glm::vec3(0, 35, -100));
	m_controlPoints.push_back(glm::vec3(71, 5, -71));
	// Set control points (m_controlPoints) here, or load from disk

	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)
}


// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int) m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i-1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M-1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3 &p, glm::vec3 &up)
{
	if (d < 0)
		return false;

	int M = (int) m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int) (d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size(); i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;

	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j-1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));


	return true;
}



// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}


}



void CCatmullRom::CreateCentreline()
{
	// Call Set Control Points
	SetControlPoints();
	// Call UniformlySampleControlPoints with the number of samples required
	UniformlySampleControlPoints(1000);
	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card
	glGenVertexArrays(1, &m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);

		CVertexBufferObject vbo;

	vbo.Create();
	vbo.Bind();

	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (auto & v : m_centrelinePoints) {
		vbo.AddData(&v, sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));

	}


	vbo.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	glEnableVertexAttribArray(1);//------------------------------------vert distance
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)));

	glEnableVertexAttribArray(2);//----------------------------------vert distance + texture distance
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)+ sizeof(glm::vec2)));




}


void CCatmullRom::CreateOffsetCurves()
{
	// Compute the offset curves, one left, and one right.  Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively
	glm::vec3 p, l, r;
	glm::vec3 pNext;
	int total;
	total = m_centrelinePoints.size();
	//left
	//fspacing = distance/ by all points to get that distance, then mutlyplly every step so it's all equal
	float fSpacing = m_distances[m_distances.size() - 1] / m_centrelinePoints.size();
	for (int i = 0; i < total; i++) {

		
		Sample(i*fSpacing, p);
	
		Sample(((i + 1)% total)*fSpacing, pNext);
		//Frenet
		glm::vec3 t;
		t = glm::normalize(pNext - p);
		glm::vec3 n;
		n = glm::normalize(glm::cross(t, glm::vec3(0, 1, 0)));
		//width = 5 for now...
		l = p - (5.0f * n);
		r = p + (5.0f * n);
		m_leftOffsetPoints.push_back(l);
		m_rightOffsetPoints.push_back(r);
	}
	// Generate two VAOs called m_vaoLeftOffsetCurve and m_vaoRightOffsetCurve, each with a VBO, and get the offset curve points on the graphics card
	//LEFT
	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftOffsetCurve);

	CVertexBufferObject vbol;

	vbol.Create();
	vbol.Bind();

	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (auto & v : m_leftOffsetPoints) {
		vbol.AddData(&v, sizeof(glm::vec3));
		vbol.AddData(&texCoord, sizeof(glm::vec2));
		vbol.AddData(&normal, sizeof(glm::vec3));

	}


	vbol.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	glEnableVertexAttribArray(1);//------------------------------------vert distance
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)));

	glEnableVertexAttribArray(2);//----------------------------------vert distance + texture distance
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	//RIGHT
	//LEFT
	glGenVertexArrays(1, &m_vaoRightOffsetCurve);
	glBindVertexArray(m_vaoRightOffsetCurve);

	CVertexBufferObject vbor;

	vbor.Create();
	vbor.Bind();

	//glm::vec2 texCoord(0.0f, 0.0f);
	//glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (auto & v : m_rightOffsetPoints) {
		vbor.AddData(&v, sizeof(glm::vec3));
		vbor.AddData(&texCoord, sizeof(glm::vec2));
		vbor.AddData(&normal, sizeof(glm::vec3));

	}


	vbor.UploadDataToGPU(GL_STATIC_DRAW);

//	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	glEnableVertexAttribArray(1);//------------------------------------vert distance
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)));

	glEnableVertexAttribArray(2);//----------------------------------vert distance + texture distance
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));






	// Note it is possible to only use one VAO / VBO with all the points instead.


}


void CCatmullRom::CreatePath()
{

	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card

}


void CCatmullRom::RenderCentreline()
{
	// Bind the VAO m_vaoCentreline and render it
	glBindVertexArray(m_vaoCentreline);
	glDrawArrays(GL_POINTS, 0, m_centrelinePoints.size());
	glLineWidth(0.1f);
	glPointSize(5.0f);
	glDrawArrays(GL_LINE_LOOP, 0, m_centrelinePoints.size());
}

void CCatmullRom::RenderOffsetCurves()
{
	// Bind the VAO m_vaoLeftOffsetCurve and render it
	glBindVertexArray(m_vaoLeftOffsetCurve);
	glDrawArrays(GL_POINTS, 0, m_leftOffsetPoints.size());
	//to add right ones start from 500 and move on.... 
	glDrawArrays(GL_LINE_LOOP, 0, m_leftOffsetPoints.size());


	// Bind the VAO m_vaoRightOffsetCurve and render it
	glBindVertexArray(m_vaoRightOffsetCurve);
	glDrawArrays(GL_POINTS, 0, m_rightOffsetPoints.size());
	glDrawArrays(GL_LINE_LOOP, 0, m_rightOffsetPoints.size());
}


void CCatmullRom::RenderPath()
{
	// Bind the VAO m_vaoTrack and render it
}

int CCatmullRom::CurrentLap(float d)
{

	return (int)(d / m_distances.back());

}
