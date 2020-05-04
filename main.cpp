#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<my_util/shader_s.h>
#define STB_IMAGE_IMPLEMENTATION
#include<stb-master/stb_image.h>
#include<iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<my_util/Camera.h>
#include<vector>
#include"MagicCube.h"
#include<cstdlib>

void init_callback();
void init_texture();
void processInput(GLFWwindow* window);
Camera camera(glm::vec3(0, 0, 3), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), 0, 0);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int key, int action,int mod);
glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
instruction my_ins(1, 0, 8);
int replay_ins_all[8] = { 1,0,3,2,5,4,7,6 };
bool firstMouse = true,mouse_right_down=false,mouse_left_down=false,replay=false,disturb=false;
double lastX, lastY,actSX,actSY;
int whichx, whichy,count_disturb=0;
std::vector<instruction*> instructions;
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DECORATED, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Linger", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//====data init====//
	MagicCube magicCube;
	//====data init====//
	//==============set callback====================//
	init_callback();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	//==============set callback====================//
	//====bind texture====//
	init_texture();
	unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	int width, height, nrChannels;
	unsigned char *data = stbi_load("img_out.bmp", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(data);
	//====bind texture====//
	//==============bind VAO========================//
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(magicCube.vertices), magicCube.vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(2);
	//==============bind VAO========================//
	glViewport(0, 0, 1920, 1080);
	glEnable(GL_DEPTH_TEST);
	Shader my_shader("shader.vert", "shader.frag");
	my_shader.use();
	glUniform1i(glGetUniformLocation(my_shader.ID, "texture1"), 0);
	glBindVertexArray(VAO);
	
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
	glm::mat4 ortho = glm::ortho(-14.5f, 1.5f, -7.5f, 1.5f, 0.1f, 100.0f);
	camera.camPos.z = 6;
	glm::mat4 view = glm::lookAt(camera.camPos, camera.camPos + camera.camDir, camera.camUp);
	srand(glfwGetTime());
	float deltaTime = 0;
	int count = 0;
	float coe = 1;
	glUniformMatrix4fv(glGetUniformLocation(my_shader.ID,"model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(my_shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(my_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1f(glGetUniformLocation(my_shader.ID, "coe"), coe);
	
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		float timeValue = glfwGetTime();		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 temp = glm::rotate(glm::mat4(1.0f), glm::radians(sin(timeValue)*180.0f), glm::vec3(1.0f, 0.0f,0.0f));
		glm::mat4 tem2 = glm::rotate(glm::mat4(1.0f), glm::radians(sin(-timeValue)*180.0f), glm::vec3(0.3f, 1.0f, -0.1f));
		if(!replay && !disturb)
			magicCube.rotate(my_ins);
		else if (disturb)
		{
			if (!magicCube.cubeMoving)
			{
				if (count_disturb > 40)
				{
					disturb = false;
					count_disturb = 0;
				}
				else
				{
					++count_disturb;
					int tempx = rand() % 3;
					int tempy = rand() % 3;
					int dir = rand() % 5;
					if (dir == 4)
						dir = rand() % 4 + 4;
					instructions.push_back(new instruction(tempx, tempy, dir));
					instruction tempIns(tempx, tempy, dir);
					magicCube.rotate(tempIns);
				}
			}
			else
			{
				instruction tempIns(0, 0, 8);
				magicCube.rotate(tempIns);
			}
		}
		else
		{
			if (!instructions.empty())
			{
				if (!magicCube.cubeMoving)
				{
					instruction replay_ins(*instructions.back());
					replay_ins.dir = replay_ins_all[replay_ins.dir];
					magicCube.rotate(replay_ins);
					instructions.pop_back();
				}
				else
				{
					instruction tempIns(0, 0, 8);
					magicCube.rotate(tempIns);
				}
			}
			else
				replay = false;
		}

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(magicCube.vertices), magicCube.vertices,GL_DYNAMIC_DRAW);
		for (int x = 0; x < 3; x++)
			for (int y = 0; y < 3; y++)
				for (int z = 0; z < 3; z++)
				{
					int index = (x * 9 + y * 3 + z);
					//glUniformMatrix4fv(glGetUniformLocation(my_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(magicCube.units[index].model));
					glUniformMatrix4fv(glGetUniformLocation(my_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
					glUniform1f(glGetUniformLocation(my_shader.ID, "coe"), 1.0f);
					glUniformMatrix4fv(glGetUniformLocation(my_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
					glDrawArrays(GL_TRIANGLES, index * 36, 36);
					if(magicCube.units[index].x==whichx && magicCube.units[index].y == whichy && magicCube.units[index].z==0)
						glUniform1f(glGetUniformLocation(my_shader.ID, "coe"), 0.6f);
					glUniformMatrix4fv(glGetUniformLocation(my_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(ortho));
					glDrawArrays(GL_TRIANGLES, index * 36, 36);
				}
		glfwSwapBuffers(window);
		glfwPollEvents();
		deltaTime = glfwGetTime()-timeValue;
	}
	glfwTerminate();
	return 0;
}
void init_callback() 
{
}
void init_texture() 
{

}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	int text=glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	lastX = xpos;
	lastY = ypos;

	if (mouse_right_down)
	{
		float xoffset = xpos - actSX;
		float yoffset = actSY - ypos;
		float sensitivity = 0.2;
		xoffset *= sensitivity;
		yoffset *= sensitivity;
		if (abs(xoffset) > 70)xoffset = 70*xoffset/ abs(xoffset);
		if (abs(yoffset) > 70)yoffset = 70 * yoffset / abs(yoffset);
		model = glm::rotate(glm::mat4(1.0f), glm::radians(xoffset), glm::vec3(0.0f, 1.0f,0.0f));
		model = glm::rotate(model, glm::radians(yoffset), glm::vec3(-1.0f, 0.0f, 0.0f));
		
	}
	std::cout << xpos << "-" << ypos << std::endl;
}
void mouse_button_callback(GLFWwindow* window, int key, int action, int mod)
{
	if (key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		actSX = lastX;
		actSY = lastY;
		mouse_left_down = true;
	}
	if (key == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		actSX = lastX;
		actSY = lastY;
		mouse_right_down = true;
	}
	if (key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		float xoffset = lastX - actSX;
		float yoffset = lastY - actSY;
		if (abs(xoffset) + abs(yoffset) > 20)
		{
			if (abs(xoffset) > abs(yoffset))
			{
				if (xoffset > 0)
					my_ins.dir = 7;
				else
					my_ins.dir = 6;
			}
			else
			{
				if (yoffset > 0)
					my_ins.dir = 5;
				else
					my_ins.dir = 4;
			}
			int tempx = ((int)actSX - 1561) / 120;
			int tempy = 2 - ((int)actSY - 1) / 120;
			my_ins.x = tempx;
			my_ins.y = tempy;
			if (tempx >= 0 && tempx <= 2 && tempy >= 0 && tempy <= 2)
				my_ins.dir = my_ins.dir - 4;
		}
		mouse_left_down = false;
		instructions.push_back(new instruction(my_ins));
	}
	if (key == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		model = glm::mat4(1.0f);
		mouse_right_down = false;
	}

}
void processInput(GLFWwindow* window) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		replay = true;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		disturb = true;
	if (!mouse_left_down)
	{
		whichx = ((int)lastX - 1561) / 120;
		whichy = 2 - ((int)lastY - 1) / 120;
	}
	else
	{
		whichx = ((int)actSX - 1561) / 120;
		whichy = 2 - ((int)actSY - 1) / 120;
	}
	std::cout << my_ins.x << "-" << my_ins.y << std::endl;
}