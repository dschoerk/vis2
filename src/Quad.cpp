#include "Quad.h"

using namespace GL;


Quad::Quad() {
	init();
}

Quad::~Quad() {
	dispose();
}

void Quad::init() {
	const int vertexCount = 4;
	float vertexData[3*vertexCount] = {
		-1.f, 1.f, 0.f,
		-1.f, -1.f, 0.f,
		1.f, -1.f, 0.f,
		1.f, 1.f, 0.f
	};

	unsigned int vertexIndices[6] = {

		0, 1, 3,
		1, 2, 3

	};

	float uvs[2*vertexCount] = {

		0, 1,
		0, 0,
		1, 0,
		1, 1

	};

	//Init vbo
	glGenBuffers(1, &vbo);

	//bind vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//allocate memory @ the graphics-card + upload data
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * vertexCount, &vertexData, GL_STATIC_DRAW);
	//unbind vbo
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Init IndicesBuffer
	glGenBuffers(1, &indicesBuffer);

	//bind vbo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
	//allocate memory @ the graphics-card + upload data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, &vertexIndices, GL_STATIC_DRAW);
	//unbind vbo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Init UVBuffer
	glGenBuffers(1, &uvBuffer);
	//bind vbo
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	//allocate memory @ the graphics-card + upload data
	glBufferData(GL_ARRAY_BUFFER, vertexCount* 2 * sizeof(float), uvs, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//init vao
	glGenVertexArrays(1, &vao);

	//bind vao
	glBindVertexArray(vao);
	//bind vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//enable position
	glEnableVertexAttribArray(0);

	//set data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


	//bind vbo
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);

	//enable uvs
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);

	//unbind vao & vbo
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	initialized = true;
}

bool Quad::isInitialized() const {
	return initialized;
}

void Quad::dispose() {
	if(initialized) {
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &uvBuffer);
		glDeleteBuffers(1, &indicesBuffer);
		glDeleteVertexArrays(1, &vao);
		initialized = false;
	}
}

void Quad::draw() {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}