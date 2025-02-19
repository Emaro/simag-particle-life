#pragma once

#include <memory>

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS 1
#endif

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct GameCameraState;

class Camera
{

public:

	Camera(glm::ivec2 const& sizeFramebuffer, glm::ivec2 const& sizeWindow);
	~Camera();

	// On Mac framebuffer size (in pixel) and window size (screen coordinates) might differ...
    // I.e., framebuffer might be e.g., twice as large as window size
	void setResolution(glm::ivec2 const& sizeFramebuffer, glm::ivec2 const& sizeWindow);
	glm::ivec2 const& sizeWindow() const { return m_sizeWindow; }
	glm::ivec2 const& sizeFramebuffer() const { return m_sizeFramebuffer; }

	void moveFwd(float dx);
	void move(GameCameraState&);

	void updateView(bool bSetViewport = true);
	void setFovY(float f);
	void setNear(float near);
	void setLookAt(glm::vec3 newLookAt);
	void setPosition(glm::vec3 const& pos) { m_position = pos; }
	void setNearFar(float near_, float far_) { m_near = near_; m_far = far_; }
	void set2d() { m_is2d = true; }
	void setScale2d(float scale) { m_scale2d = scale; }
	void mirrorCameraOnFloor(bool mirror, float floorY = 0.0f);

	glm::vec3 getLookAt() const;
	glm::mat4 const& getProjection() const;
	glm::mat4 const& getView() const;
	glm::mat4 getViewProjection() const;
	glm::mat4 getViewProjectionInv() const;
	float getFov() const;
	float getNear() const { return m_near; }
	float getFar() const { return m_far; }
	glm::vec3 getFwd() const;
	float getLookAtDist() const;
	glm::vec3 const& getUp() const;
	glm::vec3 getRight() const;
	glm::vec3 const& getPos() const;
	bool is2d() const { return m_is2d; }
	bool is3d() const { return !m_is2d; }
	glm::vec2 SSToWorld2d(glm::vec2 pos);
	glm::vec2 worldToSS2d(glm::vec2 pos);
	glm::vec3 SSToWorld3d(glm::vec2 posSS, glm::vec3 const& planeP); // intersects camera-oriented plane at planeP
	// glm::vec2 worldToSS3d(glm::vec3 posWS);
	glm::mat4 get2dMatrixScreenResolution() const;
	void drawOrientation() const;

	// Old stuff, might come in handy later
	void adaptFovYtoLookAtAABBFullScreen_NOT_POSITION(glm::vec3 aabb0, glm::vec3 aabb1); // aabb in WS!

	// sets FOVY
	void determineSSBBfromWSBB(glm::vec3 aabb0, glm::vec3 aabb1, glm::vec3& ssMinX, glm::vec3& ssMinY, glm::vec3& ssMinZ, glm::vec3& ssMaxX, glm::vec3& ssMaxY, glm::vec3& ssMaxZ) const;

	// factory
	static std::shared_ptr<Camera> createCamera2d(glm::ivec2 const& sizeFramebuffer, glm::ivec2 const& sizeWindow);
	static std::shared_ptr<Camera> createCamera3d(glm::ivec2 const& sizeFramebuffer, glm::ivec2 const& sizeWindow);

private:

	glm::ivec2 m_sizeFramebuffer;
	glm::ivec2 m_sizeWindow;

	bool m_is2d;
	float m_scale2d = 0.2f;
	float m_fov;
	float m_near;
	float m_far;
	glm::vec3 m_position;
	glm::vec3 m_lookAt;
	glm::vec3 m_up;
	glm::mat4 m_projection;
	glm::mat4 m_view;

	float m_floorY;
	bool m_bMirror;

};

