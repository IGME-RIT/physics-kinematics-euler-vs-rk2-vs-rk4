/*
File Name: main.cpp
Copyright © 2018
Original authors: Srinivasan Thiagarajan, Sanketh Bhat
Refactored by Sanketh Bhat
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Description:
Euler-RK2-RK4 comparison using projectile motion
*/


#include "GLRender.h"
#include "GameObject.h"






#pragma region program specific Data members
// Large timestep for this simulation only
float timestep = .5;
//the number of disvision used to make the structure of the circle
int NumberOfDivisions = 20;


//init values for proj. motion
float angle = PI / 2.5f;
float initVelocity = 0.09;
bool nextStep = false;
GameObject circleEU(MVP, glm::vec4(1, 0, 0, 1));
GameObject circleRK2(MVP, glm::vec4(0, 1, 0, 1));
GameObject circleRK4(MVP, glm::vec4(0, 0, 1, 1));


// vector of scene bodies
std::vector<GameObject*> bodies;

glm::vec3 spawn = glm::vec3(-2.4, -2.5, 0);

glm::vec3 prevPos1 = spawn;
glm::vec3 prevPos2 = spawn;
glm::vec3 prevPos3 = spawn;

std::vector<VertexFormat> lines;

VertexBuffer lineBuffer;

#pragma endregion



//Fires the simulated projectiles
void shoot()
{
	circleEU.Velocity(glm::vec3());
	circleEU.Position(spawn);

	circleRK2.Velocity(glm::vec3());
	circleRK2.Position(spawn);

	circleRK4.Velocity(glm::vec3());
	circleRK4.Position(spawn);

	float vX = cos(angle)*initVelocity;

	float vY = sin(angle)*initVelocity;

	circleEU.Velocity(glm::vec3(vX, vY, 0));

	circleRK2.Velocity(glm::vec3(vX, vY, 0));

	circleRK4.Velocity(glm::vec3(vX, vY, 0));
}


#pragma region util_functions

//Similar code to renderBody(), however here we use GL_LINES instead of GL_TRIANGLES
void renderLines()
{

	glUniformMatrix4fv(uniMVP, 1, GL_FALSE, glm::value_ptr(MVP));
	glBindBuffer(GL_ARRAY_BUFFER, lineBuffer.vbo);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)16);

	glDrawArrays(GL_LINES, 0, lineBuffer.numberOfVertices);



}

//Adds the position of the gameobject to the vevtor of vertex format and then feeds the buffer
void loadLines()
{
	//Clearing buffer so we dont overflow it
	glDeleteBuffers(1, &lineBuffer.vbo);

	//Line segment for object 1
	lines.push_back(VertexFormat(prevPos1, circleEU.color));
	lines.push_back(VertexFormat(circleEU.Position(), circleEU.color));

	//Line segment for object 2
	lines.push_back(VertexFormat(prevPos2, circleRK2.color));
	lines.push_back(VertexFormat(circleRK2.Position(), circleRK2.color));

	//Line segment for object 3
	lines.push_back(VertexFormat(prevPos3, circleRK4.color));
	lines.push_back(VertexFormat(circleRK4.Position(), circleRK4.color));

	//Storing current position to be used in the next frame
	prevPos1 = circleEU.Position();
	prevPos2 = circleRK2.Position();
	prevPos3 = circleRK4.Position();

	lineBuffer.initBuffer(lines.size(), &lines[0]);



}


void update()
{
	//Progresses timestep
	if (nextStep)
	{
		for each (GameObject *body in bodies)
			body->update(timestep, PV);

		
		loadLines();


	}
	nextStep = false;

}



//Resets simulation and updates by 1 tick
void restart()
{
	shoot();
	lines.clear(); //Emptying vector

	prevPos1 = prevPos2 = prevPos3 = spawn; //Resetting positions
	nextStep = true;
	update();

}

// This function is used to handle key inputs.
// It is a callback funciton. i.e. glfw takes the pointer to this function (via function pointer) and calls this function every time a key is pressed in the during event polling.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//Sets the current window to a close flag when ESC is pressed
	if (key == GLFW_KEY_ESCAPE && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		glfwSetWindowShouldClose(window, 1);
	}

	if (key == GLFW_KEY_R && (action == GLFW_PRESS))
	{

		restart();
	}

	if (key == GLFW_KEY_SPACE && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		nextStep = true;

	}


}
#pragma endregion


void  sceneSetup()
{
	//Sets up bodies in the scene
	circleEU.setupCircle(.005f, NumberOfDivisions);
	circleEU.integType = 1; 
	circleEU.Position(glm::vec3(-20, -20, 0)); //Spawning off screen
	bodies.push_back(&circleEU);


	circleRK2.setupCircle(.005f, NumberOfDivisions);
	circleRK2.integType = 2;
	circleRK2.Position(glm::vec3(-20, -20, 0)); //Spawning off screen
	bodies.push_back(&circleRK2);

	circleRK4.setupCircle(.005f, NumberOfDivisions);
	circleRK4.integType = 3;
	circleRK4.Position(glm::vec3(-20, -20, 0)); //Spawning off screen
	bodies.push_back(&circleRK4);


	shoot();
	loadLines();




}

void main()
{
	// Initializes most things needed before the main loop
	init();

	//Sets up scene objects 
	sceneSetup();

	// Sends the funtion as a funtion pointer along with the window to which it should be applied to.
	glfwSetKeyCallback(window, key_callback);

	std::cout << "Hold Space to update timestep\n";
	std::cout << "Press R to reset\n";
	std::cout << "Red is Euler Integration, Green is RK2 and Blue is RK4 \n";



	// Enter the main loop.
	while (!glfwWindowShouldClose(window))
	{

		// Call to update() which will update the gameobjects.
		update();

		// Call the render function(s).
		renderScene();


		//Rendering each body after the scene
		//In this simulation we do not need to render them
		for each (GameObject *body in bodies)
			renderBody(body);

		//Rendering GL lines
		renderLines();


		// Swaps the back buffer to the front buffer
		// Remember, you're rendering to the back buffer, then once rendering is complete, you're moving the back buffer to the front so it can be displayed.
		glfwSwapBuffers(window);

		// Checks to see if any events are pending and then processes them.
		glfwPollEvents();


	}

	//Cleans shaders and the program and frees up GLFW memory
	cleanup();

	return;
}