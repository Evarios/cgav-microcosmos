#pragma once

class bug {
public:
	bug();
	~bug();
	void create();
	void draw(glm::mat4);
	model3D body;
	model3D upperleg, lowerleg;
};
