#ifndef __DISPLAY_HELPER_H__
#define __DISPLAY_HELPER_H__
#include <glm\glm.hpp>
#include <GL/glut.h>

namespace DISP_TOOLS {
	/// Draws the cartesisn axes
	void DrawGrid();
	void DrawArrow(glm::vec3 p0, glm::vec3 direction,  double length, double arrow_radius);
	void DrawAxes(double axisLength, double arrow_radius=0.5);
	void DrawCone(glm::vec3 base, glm::vec3 top, double radius);
	void DrawCylinder(glm::vec3 bottom, glm::vec3 top, double bottom_radius, double top_radius);
	void DrawCapsule(glm::vec3 bottom, glm::vec3 top, double radius);
	void DrawSphere(glm::vec3 pos, float sphereRadius);	//float color[4]
	void DrawWireSphere(glm::vec3 pos, float sphereRadius);
	void DrawCircle(glm::vec3 pos, float circleRadius, bool filled = false);
	void DrawCuboid(glm::vec3 center_pos, float side_len[3]);
	void setupGraphicsState();
	void Draw_Text(char *pTxt, float raster_pos_x, float raster_pos_y, float raster_pos_z, void* font=GLUT_BITMAP_TIMES_ROMAN_24);//GLUT_BITMAP_9_BY_15);
	void reshapeSubFunc(int w, int h);
	void mouseDownSubFunc(int button, int s, int x, int y);
	void mouseMoveSubFunc(int x, int y);
	void setProjectionState(bool type);
	void setCameraVariables(int type, glm::vec3 vec);
	extern int window_width;
	extern int window_height;
	extern float dist[3];
	extern double background_color[4];
	extern int oldX;
	extern int oldY;
	extern float rX;
	extern float rY;
	extern int state;
	extern GLdouble MV[16];
	extern GLdouble P[16];
	extern float nearPlane;
	extern float farPlane;
	extern double fovy;
	extern GLint viewport[4];
	extern bool m_projectState;
	extern glm::vec3 camera_pos;
	extern glm::vec3 camera_look;
	extern glm::vec3 camera_up;
};

#endif
