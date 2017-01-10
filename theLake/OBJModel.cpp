#include "OBJModel.h"



OBJModel::OBJModel()
{
	
}


OBJModel::~OBJModel()
{
}


// methods


bool OBJModel::loadOBJ(
	const char * path,
	std::vector < glm::vec3 > & out_vertices,
	std::vector < glm::vec2 > & out_uvs,
	std::vector < glm::vec3 > & out_normals) 
{
	FILE * file = fopen("models/diamond.OBJ", "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader
	
	
	}
}