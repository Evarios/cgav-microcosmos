#include "bug.h"

bug::bug() {}
bug::~bug() {}
void bug::create() {
	body.loadModel("body.model3D");
	upperleg.loadModel("upperleg.model3D");
	lowerleg.loadModel("lowerleg.model3D");
}

void bug::draw(glm::mat4 M) {
	glm::mat4 u1 = glm::translate(M, glm::vec3(1.0f, 0.2f, -0.2f));;
	u1 = glm::scale(u1, glm::vec3(0.1f, 0.1f, 0.1f));
	u1 = glm::rotate(u1, PI, glm::vec3(0.0f, 1.0f, 0.0f));
	u1 = glm::rotate(u1, PI / 6, glm::vec3(0.0f, 1.0f, 0.0f));
	u1 = glm::rotate(u1, PI / 10, glm::vec3(0.0f, 0.0f, -1.0f));

	glm::mat4 l1 = glm::translate(u1, glm::vec3(-2.0f, 0.1f, -0.35f));
	l1 = glm::scale(l1, glm::vec3(2.0f, 2.0f, 2.0f));
	l1 = glm::rotate(l1, PI, glm::vec3(1.0f, 1.0f, 0.0f));

	glm::mat4 u2 = glm::translate(M, glm::vec3(-1.0f, 0.2f, -0.2f));;
	u2 = glm::scale(u2, glm::vec3(0.1f, 0.1f, 0.1f));
	u2 = glm::rotate(u2, PI / 6, glm::vec3(0.0f, -1.0f, 0.0f));
	u2 = glm::rotate(u2, PI / 10, glm::vec3(0.0f, 0.0f, -1.0f));

	glm::mat4 l2 = glm::translate(u2, glm::vec3(-2.0f, 0.1f, -0.35f));
	l2 = glm::scale(l2, glm::vec3(2.0f, 2.0f, 2.0f));
	l2 = glm::rotate(l2, PI, glm::vec3(1.0f, 1.0f, 0.0f));

	glm::mat4 u3 = glm::translate(M, glm::vec3(1.1f, 0.2f, 0.4f));;
	u3 = glm::scale(u3, glm::vec3(0.1f, 0.1f, 0.1f));
	u3 = glm::rotate(u3, PI, glm::vec3(0.0f, 1.0f, 0.0f));
	u3 = glm::rotate(u3, PI / 10, glm::vec3(0.0f, 0.0f, -1.0f));

	glm::mat4 l3 = glm::translate(u3, glm::vec3(-2.0f, 0.1f, -0.35f));
	l3 = glm::scale(l3, glm::vec3(2.0f, 2.0f, 2.0f));
	l3 = glm::rotate(l3, PI, glm::vec3(1.0f, 1.0f, 0.0f));

	glm::mat4 u4 = glm::translate(M, glm::vec3(-1.1f, 0.2f, 0.4f));;
	u4 = glm::scale(u4, glm::vec3(0.1f, 0.1f, 0.1f));
	u4 = glm::rotate(u4, PI / 10, glm::vec3(0.0f, 0.0f, -1.0f));

	glm::mat4 l4 = glm::translate(u4, glm::vec3(-2.0f, 0.1f, -0.35f));
	l4 = glm::scale(l4, glm::vec3(2.0f, 2.0f, 2.0f));
	l4 = glm::rotate(l4, PI, glm::vec3(1.0f, 1.0f, 0.0f));

	glm::mat4 u5 = glm::translate(M, glm::vec3(1.0f, 0.2f, 1.0f));;
	u5 = glm::scale(u5, glm::vec3(0.1f, 0.1f, 0.1f));
	u5 = glm::rotate(u5, PI, glm::vec3(0.0f, 1.0f, 0.0f));
	u5 = glm::rotate(u5, PI / 6, glm::vec3(0.0f, -1.0f, 0.0f));
	u5 = glm::rotate(u5, PI / 10, glm::vec3(0.0f, 0.0f, -1.0f));

	glm::mat4 l5 = glm::translate(u5, glm::vec3(-2.0f, 0.1f, -0.35f));
	l5 = glm::scale(l5, glm::vec3(2.0f, 2.0f, 2.0f));
	l5 = glm::rotate(l5, PI, glm::vec3(1.0f, 1.0f, 0.0f));

	glm::mat4 u6 = glm::translate(M, glm::vec3(-1.0f, 0.2f, 1.0f));;
	u6 = glm::scale(u6, glm::vec3(0.1f, 0.1f, 0.1f));
	u6 = glm::rotate(u6, PI / 6, glm::vec3(0.0f, 1.0f, 0.0f));
	u6 = glm::rotate(u6, PI / 10, glm::vec3(0.0f, 0.0f, -1.0f));

	glm::mat4 l6 = glm::translate(u6, glm::vec3(-2.0f, 0.1f, -0.35f));
	l6 = glm::scale(l6, glm::vec3(2.0f, 2.0f, 2.0f));
	l6 = glm::rotate(l6, PI, glm::vec3(1.0f, 1.0f, 0.0f));

	glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Compute view matrix
	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f); //Compute projection matrix

	drawModel(P, V, M, this->body);
	drawModel(P, V, u1, this->upperleg);
	drawModel(P, V, l1, this->lowerleg);
	drawModel(P, V, u2, this->upperleg);
	drawModel(P, V, l2, this->lowerleg);
	drawModel(P, V, u3, this->upperleg);
	drawModel(P, V, l3, this->lowerleg);
	drawModel(P, V, u4, this->upperleg);
	drawModel(P, V, l4, this->lowerleg);
	drawModel(P, V, u5, this->upperleg);
	drawModel(P, V, l5, this->lowerleg);
	drawModel(P, V, u6, this->upperleg);
	drawModel(P, V, l6, this->lowerleg);
}
