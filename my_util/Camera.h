#pragma once
#ifndef GLM_H
#define GLM_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif
class Camera 
{
private:

public:
	glm::vec3 camDir;
	glm::vec3 camRight;
	glm::vec3 camUp;
	glm::vec3 camPos;
	float yaw;
	float pitch;
	Camera()
	{
		camPos = glm::vec3(0.0f, 0.0f, 0.0f);
		camDir = glm::vec3(0.0f, 0.0f, -1.0f);
		camUp = glm::vec3(0.0f, 1.0f, 0.0f);
		camRight = glm::cross(camDir, camUp);
		yaw = 0;
		pitch = 0;
	}
	Camera(glm::vec3 camPos, glm::vec3 camDir, glm::vec3 camUp, float yaw, float pitch)
	{
		this->camPos = glm::vec3(camPos);
		this->camDir = glm::vec3(camDir);
		this->camUp = glm::vec3(camUp);
		this->camRight = glm::normalize(glm::vec3(glm::cross(this->camDir, this->camUp)));
		this->yaw = yaw;
		this->pitch = pitch;
	}
	Camera(Camera* temp)
	{
		Camera(temp->camPos, temp->camDir, temp->camUp, temp->yaw, temp->pitch);
	}
	void setView(float yaw, float pitch)
	{
		if (yaw > 180)
			yaw = -180+yaw-180;
		else if (yaw < -180)
			yaw = 360+yaw;
		if (pitch > 75)
			pitch = 75;
		else if (pitch < -75)
			pitch = -75;
		glm::vec3 tempCamUp(0.0f, 1.0f, 0.0f);
		glm::vec3 tempCamRight(1.0f, 0.0f, 0.0f);
		glm::vec3 tempCamDir(0.0f, 0.0f, -1.0f);
		glm::mat4 trans(1.0f);
		trans = glm::rotate(trans, glm::radians(yaw), tempCamUp);
		tempCamDir = glm::vec3(trans*glm::vec4(tempCamDir,1.0f));
		tempCamRight = glm::normalize(glm::cross(tempCamDir, tempCamUp));
		trans = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), tempCamRight);
		tempCamDir = glm::vec3(trans*glm::vec4(tempCamDir, 1.0f));
		tempCamUp = glm::normalize(glm::cross(tempCamRight, tempCamDir));
		camUp = tempCamUp;
		camRight = tempCamRight;
		camDir = tempCamDir;
		this->yaw = yaw;
		this->pitch = pitch;
		
	}
	void moveView(float y_offset, float p_offset)
	{
		setView(yaw + y_offset, pitch + p_offset);
	}
};