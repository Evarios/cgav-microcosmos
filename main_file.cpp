/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>

#include <stdio.h>
#include <string>
#include <iostream>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
using namespace std;
float speed_x = 0;//[radians/s]
GLuint tex;


class model3D
{
public:
	model3D();
	~model3D();
	std::vector<glm::vec4> verts;
	std::vector<glm::vec4> norms;
	std::vector<glm::vec2> texCoords;
	std::vector<unsigned int > indices;
	void loadModel(std::string filename);

private:
	
};

model3D::model3D(){}
model3D::~model3D(){}

//Error processing callback procedure
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void key_callback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mod
) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) {
			speed_x = PI;
		}
		if (key == GLFW_KEY_RIGHT) {
			speed_x = -PI;
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
			speed_x = 0;
		}
	}
}



GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Read into computers memory
	std::vector<unsigned char> image;   //Allocate memory 
	unsigned width, height;   //Variables for image size
	//Read the image
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import to graphics card memory
	glGenTextures(1, &tex); //Initialize one handle
	glBindTexture(GL_TEXTURE_2D, tex); //Activate handle
	//Copy image to graphics cards memory reprezented by the active handle
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

void model3D::loadModel(std::string plik) {
	using namespace std;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(plik, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	cout << importer.GetErrorString() << endl;
	
	aiMesh *mesh = scene->mMeshes[0];
	
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i]; //aiVector3D podobny do glm::vec3
		verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));		
		
		aiVector3D normal = mesh->mNormals[i]; // wektory znormalizowane
		norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		//0 to numer zestawu wsp. teksturowania
		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		texCoords.push_back(glm::vec2(texCoord.x, texCoord.y));

		//dla każdego wielokąta składowego
		for (int i = 0; i < mesh->mNumFaces; i++) {
			aiFace& face = mesh->mFaces[i]; //face to jeden z wielokątów siatki
			//dla każdego indeksu->wierzchkołka tworzącego wielokąt
			//dla aiProcess_Triangulate to zawsze będzie 3
			for (int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
	}
	return;
}

void drawModel(glm::mat4 P, glm::mat4 V, glm::mat4 M, model3D name) {
	
	spLambertTextured->use();

	glUniformMatrix4fv(spLambertTextured->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spLambertTextured->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(M));


	glEnableVertexAttribArray(spLambertTextured->a("vertex"));
	glVertexAttribPointer(spLambertTextured->a("vertex"), 4, GL_FLOAT, false, 0, name.verts.data());

	glEnableVertexAttribArray(spLambertTextured->a("texCoord"));
	glVertexAttribPointer(spLambertTextured->a("texCoord"), 2, GL_FLOAT, false, 0, name.texCoords.data());

	glEnableVertexAttribArray(spLambertTextured->a("normal"));
	glVertexAttribPointer(spLambertTextured->a("normal"), 4, GL_FLOAT, false, 0, name.norms.data());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(spLambertTextured->u("tex"), 0);

	//glDrawArrays(GL_TRIANGLES, 0, myCubeVertexCount);
	glDrawElements(GL_TRIANGLES, name.indices.size(), GL_UNSIGNED_INT, name.indices.data());

	glDisableVertexAttribArray(spLambertTextured->a("vertex"));
	glDisableVertexAttribArray(spLambertTextured->a("color"));
	glDisableVertexAttribArray(spLambertTextured->a("normal"));
}

class bug {
public:
	bug();
	bug(float, float);
	~bug();
	float getX();
	float getY();
	float getZ();

	void setX(float newX);
	void setY(float newY);
	void setZ(float newZ);
	void create();
	void draw(glm::mat4, glm::mat4, glm::mat4);
private:
	model3D body;
	model3D upperleg, lowerleg;
	float X;
	float Y;
	float Z;
};

bug::bug() {}
bug::~bug() {}
bug::bug(float x, float y) {
	X = x;
	Y = y;
}

float bug::getX() {
	return X;
}


float bug::getY() {
	return Y;
}

float bug::getZ() {
	return Z;
}

void bug::setX(float newX) {
	X = newX;
	return;
}

void bug::setY(float newY) {
	Y = newY;
	return;
}

void bug::setZ(float newZ) {
	Z = newZ;
	return;
}


void bug::create() {
	body.loadModel("body.model3D");
	upperleg.loadModel("upperleg.model3D");
	lowerleg.loadModel("lowerleg.model3D");
}

void bug::draw(glm::mat4 M, glm::mat4 P, glm::mat4 V) {

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

bug biedronka;
int numBug = 6;
bug bugi[6];

//Drawing procedure
void drawScene(GLFWwindow* window, float camX, float camZ) {
	//************Place any code here that draws something inside the window******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear color and depth buffers

	glm::mat4 V = glm::lookAt(glm::vec3(camX, 10.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Compute view matrix
	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f); //Compute projection matrix
	spLambert->use();//Aktywacja programu cieniującego
	glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V));
	glm::mat4 M = glm::mat4(1.0f);
	for (int i = 0; i < numBug; i++) {
		glm::mat4 Mi = glm::translate(M, glm::vec3(bugi[i].getX(),0.0f, bugi[i].getZ()));
		//glm::scale(Mi, glm::vec3(0.1f, 0.1f, 0.1f));

		bugi[i].draw(Mi,P,V);

	}
	glfwSwapBuffers(window); //Copy back buffer to the front buffer
}

//Initialization code procedure
void initOpenGLProgram(GLFWwindow* window) {
	initShaders();
	//************Place any code here that needs to be executed once, at the program start************
	glClearColor(0, 0, 0, 1); //Set color buffer clear color
	glEnable(GL_DEPTH_TEST); //Turn on pixel depth test based on depth buffer
	glfwSetKeyCallback(window, key_callback);
	tex = readTexture("bricks.png");
	for (int i = 0; i < numBug; i++) {
		bugi[i].create();
		float x = rand() % 20 * 1.0f;
		float z = rand() % 20 * 1.0f;
		if (i % 4 == 0) {
			bugi[i].setX(-1 * x);
			bugi[i].setZ(-1 * z);
		}

		else if (i % 4 == 1) {
			bugi[i].setX(x);
			bugi[i].setZ(-1 * z);
		}
		else if (i % 4 == 2) {
			bugi[i].setX(-1 * x);
			bugi[i].setZ(z);
		}
		else if (i % 4 == 3) {
			bugi[i].setX(x);
			bugi[i].setZ(z);
		}
	}

}

//Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow* window) {
	freeShaders();
	glDeleteTextures(1, &tex);
	//************Place any code here that needs to be executed once, after the main loop ends************
}

int main(void)
{
	GLFWwindow* window; //Pointer to object that represents the application window

	glfwSetErrorCallback(error_callback);//Register error processing callback procedure

	if (!glfwInit()) { //Initialize GLFW library
		fprintf(stderr, "Can't initialize GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Create a window 500pxx500px titled "OpenGL" and an OpenGL context associated with it. 

	if (!window) //If no window is opened then close the program
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Since this moment OpenGL context corresponding to the window is active and all OpenGL calls will refer to this context.
	glfwSwapInterval(1); //During vsync wait for the first refresh

	GLenum err;
	if ((err = glewInit()) != GLEW_OK) { //Initialize GLEW library
		fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Call initialization procedure

	//Main application loop
	float camX = 0.0f;
	float camZ = 0.0f;
	double timestamp = 0.0f;
	const float radius = 10.0f;
	const int speedFactor = 200000;
	glfwSetTime(0); //clear internal timer
	while (!glfwWindowShouldClose(window)) //As long as the window shouldnt be closed yet...
	{
		glfwSetTime(0);
		timestamp += speed_x * glfwGetTime();
		camX = sin(timestamp*speedFactor) * radius; //Compute camera X
		camZ = cos(timestamp*speedFactor) * radius; //Compute camera Z
		glfwSetTime(0);
		drawScene(window, camX, camZ); //Execute drawing procedure
		glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
		
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
