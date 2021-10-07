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
#define GLM_FORCE_SWIZZLE

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
#include "lodepng.h"
#include "shaderprogram.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
using namespace std;
float speed_x = 0;//[radians/s]
GLuint tex;
float legAngle = 0.0f;
float legSpeed = 20.0f;
int legDir = 1;
float speed = 15.0; //speed of a bug
float box = 10.0f; //size of a room
float bugSize = 2.0f; //size of a bug
float aspectRatio = 1;
ShaderProgram* sp;


const GLfloat quadVertices[] = { -10.0f, 0.0f, 10.0f,
		10.0f, 0.0f, 10.0f,
		10.0f,0.0f, -10.0f,
		-10.0f,0.0f, -10.0f
};

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

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
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
	
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));


	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, name.verts.data());

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, name.texCoords.data());

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, name.norms.data());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(sp->u("tex"), 0);

	//glDrawArrays(GL_TRIANGLES, 0, myCubeVertexCount);
	glDrawElements(GL_TRIANGLES, name.indices.size(), GL_UNSIGNED_INT, name.indices.data());

	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("texCoord"));
	glDisableVertexAttribArray(sp->a("normal"));
}

class bug {
public:
	bug();
	~bug();
	float getX();
	float getY();
	float getZ();
	void setX(float newX);
	void setY(float newY);
	void setZ(float newZ);
	void create();
	void draw(glm::mat4, glm::mat4, glm::mat4);

	float getDir();
	void setDir(float);
	int getID();
	void setID(int);
	void move();
	void setStep();
	float getStepX();
	float getStepZ();
	bool collisionWall();
	bool collisionBug();
	void setRotate(bool);
private:
	model3D body, leg;
	float X;
	float Y;
	float Z;
	int ID;
	float direction;
	float stepX;
	float stepZ;
	float tempDistance;
	bool rotate;
};

bug::bug() {
	X = 0.0f;
	Z = 0.0f;
	ID = 0;
	direction = 0.0f;
	stepX = 0.0f;
	stepX = 0.0f;
	tempDistance = 100.0f;
	rotate = false;
}
bug::~bug() {}

void bug::setRotate(bool rot) {
	rotate = rot;
	return;
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

void bug::setStep() {
	float speedX = speed * cos(direction * PI / 180);
	float speedZ = speed * sin(direction * PI / 180);
	stepX = speedX * glfwGetTime() * 1.0f;
	stepZ = speedZ * glfwGetTime() * 1.0f;
	return;
}
float bug::getStepX() {
	return stepX;
}
float bug::getStepZ() {
	return stepZ;
}
int bug::getID() {
	return ID;
}
void bug::setID(int id) {
	ID = id;
	return;
}

void bug::setDir(float dir) {
	direction = dir;
	return;
}
float bug::getDir() {
	return direction;
}

void bug::create() {
	body.loadModel("body.model3D");
	leg.loadModel("leg.model3D");
}

void bug::draw(glm::mat4 M, glm::mat4 P, glm::mat4 V) {
	M = glm::rotate(M, glm::radians(direction), glm::vec3(0.0f, 1.0f, 0.0f));
	if (rotate == true) {
		M = glm::rotate(M, PI, glm::vec3(0.0f, 1.0f, 0.0f));
		rotate = false;
	}
	glm::mat4 u1 = glm::translate(M, glm::vec3(1.0f, 0.2f, -0.2f));
	u1 = glm::rotate(u1, PI / 6, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 u2 = glm::translate(M, glm::vec3(-1.0f, 0.2f, -0.2f));
	u2 = glm::rotate(u2, 5 * PI / 6, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 u3 = glm::translate(M, glm::vec3(1.1f, 0.2f, 0.4f));

	glm::mat4 u4 = glm::translate(M, glm::vec3(-1.1f, 0.2f, 0.4f));
	u4 = glm::rotate(u4, PI, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 u5 = glm::translate(M, glm::vec3(1.0f, 0.2f, 1.0f));
	u5 = glm::rotate(u5, PI / 6, glm::vec3(0.0f, -1.0f, 0.0f));

	glm::mat4 u6 = glm::translate(M, glm::vec3(-1.0f, 0.2f, 1.0f));
	u6 = glm::rotate(u6, 7 * PI / 6, glm::vec3(0.0f, 1.0f, 0.0f));

	u1 = glm::rotate(u1, PI / 10, glm::vec3(0.0f, 0.0f, 1.0f));
	u2 = glm::rotate(u2, PI / 10, glm::vec3(0.0f, 0.0f, 1.0f));
	u3 = glm::rotate(u3, PI / 10, glm::vec3(0.0f, 0.0f, 1.0f));
	u4 = glm::rotate(u4, PI / 10, glm::vec3(0.0f, 0.0f, 1.0f));
	u5 = glm::rotate(u5, PI / 10, glm::vec3(0.0f, 0.0f, 1.0f));
	u6 = glm::rotate(u6, PI / 10, glm::vec3(0.0f, 0.0f, 1.0f));
/*
	u1 = glm::rotate(u1, glm::radians(legAngle * -1), glm::vec3(1.0f, 1.0f, 1.0f));
	u2 = glm::rotate(u2, glm::radians(legAngle), glm::vec3(1.0f, 1.0f, 1.0f));
	u3 = glm::rotate(u3, glm::radians(legAngle), glm::vec3(1.0f, 1.0f, 1.0f));
	if (legAngle > 7.0f || legAngle < -7.0f)
	{
		legAngle = 0.0f;
		legDir = legDir * (-1);
	}
	u4 = glm::rotate(u4, glm::radians(legAngle * -1), glm::vec3(1.0f, 1.0f, 1.0f));
	if (legAngle > 7.0f || legAngle < -7.0f)
	{
		legAngle = 0.0f;
		legDir = legDir * (-1);
	}
	u5 = glm::rotate(u5, glm::radians(legAngle * -1), glm::vec3(0.0f, 0.0f, 1.0f));
	u6 = glm::rotate(u6, glm::radians(legAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	*/
	drawModel(P, V, M, this->body);
	drawModel(P, V, u1, this->leg);
	drawModel(P, V, u2, this->leg);
	drawModel(P, V, u3, this->leg);
	drawModel(P, V, u4, this->leg);
	drawModel(P, V, u5, this->leg);
	drawModel(P, V, u6, this->leg);
}
void bug::move()
{
	this->setStep();
	//std::cout << stepX << " " << stepZ << std::endl;
	if (direction >= 0 && direction <= 90)
	{
		float temp = direction / 90.0f;
		X += (1 - temp) * stepX;
		Z += temp * stepZ;
	}
	else if (direction > 90 && direction < 180)
	{
		float temp = (direction - 90) / 90.0f;
		X += temp * stepX;
		Z += (1 - temp) * stepZ;
	}
	else if (direction >= 180 && direction <= 270)
	{
		float temp = (direction - 180) / 90.0f;
		X += (1 - temp) * stepX;
		Z += temp * stepZ;
	}
	else if (direction > 270 && direction < 360)
	{
		float temp = (direction - 270) / 90.0f;
		X += temp * stepX;
		Z += (1 - temp) * stepZ;
	}
	return;
}

bool bug::collisionWall() {
	if (X >= box && (direction >= 315 || direction <= 45)) {
		return true;
	}
	else if (X <= -1 * box && (direction >= 135 && direction <= 225)) {
		return true;
	}
	else if (Z >= box && (direction >= 45 && direction <= 135)) {
		return true;
	}
	else if (Z <= -1 * box && (direction >= 225 && direction <= 315)) {
		return true;
	}
	else {
		return false;
	}
}

const int numBug = 1;
bug bugi[numBug];

float distance(bug a, bug b) {
	float absx = abs(a.getX() - b.getX());
	float absz = abs(a.getZ() - b.getZ());
	float dist2 = absx * absx + absz * absz;
	float dist = sqrt(dist2);
	return dist;
}

bool bug::collisionBug() {
	bool col = false;
	for (int i = 0; i < numBug; i++) {
		float dist = distance(*this, bugi[i]);
		if (this->getID() != bugi[i].getID() && dist <= bugSize) {
			if (dist > tempDistance) {
				col = false;
				tempDistance = 100.0f;
			}
			else {
				col = true;
				tempDistance = dist;
			}
		}
	}
	return col;
}

bug biedronka;

//Drawing procedure
void drawScene(GLFWwindow* window, float camX, float camZ) {
	//************Place any code here that draws something inside the window******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear color and depth buffers

	glm::mat4 V = glm::lookAt(glm::vec3(camX, 40.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Compute view matrix
	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f); //Compute projection matrix
	sp->use();//Aktywacja programu cieniującego
	glm::mat4 M = glm::mat4(1.0f);
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
	//glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glUniform4f(sp->u("lp1"), 0, 5, 0, 1);
	glUniform4f(sp->u("lp1"), 0, 0, 6, 1);
	/*glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 3, GL_FLOAT, false, 0, quadVertices);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableVertexAttribArray(sp->a("vertex"));*/
	for (int i = 0; i < numBug; i++) {
		glm::mat4 Mi = glm::translate(M, glm::vec3(bugi[i].getX(),0.0f, bugi[i].getZ()));
		//glm::scale(Mi, glm::vec3(0.1f, 0.1f, 0.1f));

		bugi[i].draw(Mi,P,V);
	}
	

	glfwSwapBuffers(window); //Copy back buffer to the front buffer
}

//Initialization code procedure
void initOpenGLProgram(GLFWwindow* window) {
	//************Place any code here that needs to be executed once, at the program start************
	glClearColor(0, 0, 0, 1); //Set color buffer clear color
	glEnable(GL_DEPTH_TEST); //Turn on pixel depth test based on depth buffer
	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	tex = readTexture("bricks.png");
	for (int i = 0; i < numBug; i++) {
		bugi[i].create();
		float x = rand() % 10 * 2.0f;
		float z = rand() % 10 * 2.0f;
		float dir = rand() % 360 * 1.0f;
		bugi[i].setDir(dir);
		bugi[i].setID(i + 1);
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
	glDeleteTextures(1, &tex);
	delete sp;
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

	window = glfwCreateWindow(1500, 1500, "OpenGL", NULL, NULL);  //Create a window 500pxx500px titled "OpenGL" and an OpenGL context associated with it. 

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
		legAngle += (legSpeed * glfwGetTime()) * legDir;
		for (int i = 0; i < numBug; i++)
		{
			if (bugi[i].collisionWall() == true) {
				float dir = bugi[i].getDir();
				if (dir >= 180.0f) {
					bugi[i].setDir(dir - 180.0f);
				}
				else {
					bugi[i].setDir(dir + 180.0f);
				}
				bugi[i].setRotate(true);
				bugi[i].move();
			}
			else if (bugi[i].collisionBug() == true) {
				float dir = bugi[i].getDir();
				if (dir >= 180.0f) {
					bugi[i].setDir(dir - 180.0f);
				}
				else {
					bugi[i].setDir(dir + 180.0f);
				}
				bugi[i].move();
			}
			else {
				bugi[i].move();
			}
		}
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
