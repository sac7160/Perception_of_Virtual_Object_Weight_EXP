#include <windows.h>		// Header File For Windows
#include <glm/glm.hpp>
#include <float.h>
#include <cstring>
#include "display_helper.h"
#include "geometry_helper.h"
const int GRID_SIZE = 10;

namespace DISP_TOOLS {//DISP_VAR {
	float dist[3] = {0.0, 0.0f, -0.4f};//-15.f};//-4.0f};
	int oldX = 0.0;
	int oldY = 0;
	float rX = 0.0;
	float rY = 0;//-180.0;
	int state = 1;
	GLfloat red[]={1,0,0},green[]={0,1,0}, blue[]={0,0,1};
	GLdouble MV[16];
	GLdouble P[16];
	float nearPlane = 0.1f;
	float farPlane = 1000.f;
	double fovy = 60.0;
	bool m_projectState = false;	// true: perspective false: orthographic
	GLint viewport[4];
	GLuint	base;				// Base Display List For The Font Set
	int m_window_width;
	int m_window_height;
	double background_color[4] = {1, 1, 1, 1};
	glm::vec3 camera_pos(0, 0, 5);
	glm::vec3 camera_look(0, 0, 0);
	glm::vec3 camera_up(0, 1, 0);
	void project() {
		double multiplier = 0.1;//5.0;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if(m_projectState)
			gluPerspective(fovy, GLfloat(m_window_width)/GLfloat(m_window_height), nearPlane, farPlane);
		else {
		//	glOrtho((GLfloat)w/(GLfloat)h,
			gluOrtho2D(-multiplier*(float)m_window_width/(float)m_window_height, multiplier*(float)m_window_width/(float)m_window_height, -multiplier, multiplier);//-1.0, 1.0);
		}
		glGetIntegerv(GL_VIEWPORT, viewport);
		//glGetDoublev(GL_PROJECTION_MATRIX, P);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	void setCameraVariables(int type, glm::vec3 vec)
	{
		switch(type) {
		case 0:
			camera_pos = vec;
			break;
		case 1:
			camera_look = vec; 
			break;
		case 2:
			camera_up = vec;
			break;
		}
	}
	void setupGraphicsState()
	{
		glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
		glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	//	glTranslatef(dist[0], dist[1], dist[2]);
		gluLookAt(camera_pos[0], camera_pos[1], camera_pos[2], camera_look[0], camera_look[1], camera_look[2], camera_up[0], camera_up[1], camera_up[2]);
		glRotatef(rX, 1, 0, 0);
		glRotatef(rY, 0, 1, 0);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);
		glEnable(GL_LIGHT_MODEL_TWO_SIDE);	// two lightings
		glShadeModel(GL_SMOOTH);

		GLfloat lightZeroPosition[]  = {0.0, 3.0, -3.5, 0.0};//{10.0, 4.0, 100.0, 0.0};// 
		GLfloat lightZeroColor[]     = {0.8, 0.8, 0.8, 1.0}; 
		GLfloat lightOnePosition[]   = {0.0, -3.0, 3.5, 0.0};//{-1.0, -2.0, -100.0, 0.0};//
		GLfloat lightOneColor[]      = {0.8, 0.8, 0.8, 1.0};
	
	//	GLfloat light_ambient[] = {0.8, 0.8, 0.8, 1.0}; // white ambient light
	//	GLfloat light_diffuse[] = {0.0, 0.0, 0.0, 1.0}; // black(?) diffuse light
	//	GLfloat light_position[] = {0.0, 0., 100.0, 1.0}; // infinite light location

	//	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
		glLightfv(GL_LIGHT0, GL_POSITION, lightZeroPosition);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
		glLightfv(GL_LIGHT1, GL_POSITION, lightOnePosition);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, lightOneColor);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_COLOR_MATERIAL);

		glEnable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}
	void setProjectionState(bool type)
	{
		m_projectState = type;
		project();
	}

	void reshapeSubFunc(int w, int h)
	{
		m_window_width = w; m_window_height = h;
		glViewport(0, 0, (GLsizei)w, (GLsizei)h);
		project();
	}

	void mouseDownSubFunc(int button, int s, int x, int y)
	{
		if(s==GLUT_DOWN) {
			oldX = x;
			oldY = y;
		}
		if(button == GLUT_MIDDLE_BUTTON) state = 0;
		else state = 1;
	}

	void mouseMoveSubFunc(int x, int y)
	{
		if(state == 0) {
			dist[2] *= (1+(y-oldY)/60.0f);
		}
		else {
			rY += (x-oldX)/5.0f;
			rX += (y-oldY)/5.0f;
		}
		oldX = x;
		oldY = y;
		glutPostRedisplay();
	}

	void DrawGrid()
	{
		glBegin(GL_LINES);
			glColor3f(0.5f, 0.5f, 0.5f);
			for(int i=(-GRID_SIZE);i<GRID_SIZE;i++) {
				glVertex3f((float)i, 0, (float)(-GRID_SIZE));
				glVertex3f((float)i, 0, (float)(GRID_SIZE));

				glVertex3f((float)(-GRID_SIZE), 0, (float)i);
				glVertex3f((float)(GRID_SIZE), 0, (float)i);
			}
		glEnd();
	}

	void DrawCylinder(glm::vec3 bottom, glm::vec3 top, double bottom_radius, double top_radius)
	{
		//printf("top(%f,%f,%f) bottom(%f,%f,%f)\n", bottom[0], bottom[1], bottom[2], top[0], top[1], top[2]);
		glm::vec3 zaxis = glm::vec3(0.0, 0.0, 1.0), direction, norm_direction;
		double coneHgt;
		float mat44[16] =
			{1.0, 0.0, 0.0, 0.0,
			 0.0, 1.0, 0.0, 0.0,
			 0.0, 0.0, 1.0, 0.0,
			 0.0, 0.0, 0.0, 1.0};
		direction = top - bottom;
		Unit(direction, norm_direction);
		coneHgt = glm::length(direction);

		glPushMatrix();
		glPushAttrib(GL_POLYGON_BIT);	// includes GL_CULL_FACE
		glEnable(GL_CULL_FACE);		// draw from all sides

		if( ((norm_direction[0]-zaxis[0]) > 1e-14) && ((norm_direction[1]-zaxis[1]) > 1e-14) && ((norm_direction[2]-zaxis[2]) > 1e-14) ) {
			mat44[0] = 1.0;
			mat44[5] = 1.0;
			mat44[10] = 1.0;
			mat44[15] = 1.0;
		}
		else {
			glm::vec3 axb, norm_axb;
			axb = glm::cross(norm_direction, zaxis);
			//Cross(norm_direction, zaxis, axb);
			Unit(axb, norm_axb);
			if(_isnan(norm_axb.x) || _isnan(norm_axb.y) || _isnan(norm_axb.z)) {
				norm_axb.x = 0.0;
				norm_axb.y = 0.0;
				norm_axb.z = 0.0;
			}
			float ac = acos(glm::dot(norm_direction, zaxis));//Dot(norm_direction, zaxis));
			float cos_th, sin_th, x, y, z, t;
			cos_th = cos(ac); sin_th = sin(ac);t = 1 - cos_th;
	//		printf("cos_th=%f sin_th=%f norm_xyz=(%f, %f, %f)\n", cos_th, sin_th, norm_axb[0], norm_axb[1], norm_axb[2]);
			x = norm_axb[0]; y = norm_axb[1]; z = norm_axb[2];
			///Refer to "matrix_rotation.docx" in "Resources/OpenGL/rotation" folder
			mat44[0] = t*x*x + cos_th;		// x*x+(1-x*x)*cos(-th)
			mat44[1] = t*x*y - z*sin_th;	// x*y*(1-cos(-th)) + z*sin(-th)     
			mat44[2] = t*x*z + y*sin_th;	// x*z*(1-cos(-th)) - z*sin(-th)
			//////
			mat44[4] = t*x*y + z*sin_th;	//				:
			mat44[5] = t*y*y + cos_th;		//				:
			mat44[6] = t*y*z - x*sin_th;	//
			//////
			mat44[8] = t*x*z - y*sin_th;
			mat44[9] = t*y*z + x*sin_th;
			mat44[10] = t*z*z + cos_th;
			///
			mat44[15] = 1.0;
		}
		glTranslatef(bottom[0], bottom[1], bottom[2]);
		glMultMatrixf(mat44);
		//// Drawing a cylinder oriented around the z-axis. Refer to http://www.opengl.org/sdk/docs/man2/xhtml/gluCylinder.xml
		GLUquadric* cone_obj = gluNewQuadric();
		gluCylinder(cone_obj, bottom_radius, top_radius, coneHgt, 8, 1);	// 8 and 1 mean the sudivisions around and along the axis, repectively. 
		glPopAttrib();	// GL_CULL_FACE
		glPopMatrix();
	}

	void DrawCapsule(glm::vec3 bottom, glm::vec3 top, double radius)
	{
		glm::vec3 direction, norm_direction, cylinder_top, cylinder_bottom;
		direction = top - bottom;
		Unit(direction, norm_direction);
		cylinder_bottom = bottom + norm_direction*(float)radius; 
		cylinder_top = top - norm_direction*(float)radius;
		DrawCylinder(cylinder_bottom, cylinder_top, radius, radius);
		DrawSphere(cylinder_bottom, radius);
		DrawSphere(cylinder_top, radius);
	}

	void DrawCone(glm::vec3 base, glm::vec3 top, double radius)
	{
		DrawCylinder(base, top, radius, 0);
	}

	void DrawArrow(glm::vec3 p0, glm::vec3 direction, double length, double arrow_rad)
	{
		glm::vec3 norm_direction, coneBaseLocation, end_pt;
		Unit(direction, norm_direction);
		end_pt = glm::vec3(p0.x+length*norm_direction.x, p0.y+length*norm_direction.y, p0.z+length*norm_direction.z);
		coneBaseLocation = glm::vec3(p0.x+0.95*length*norm_direction.x, p0.y+0.95*length*norm_direction.y, p0.z+0.95*length*norm_direction.z);

		glLineWidth(2.0);
		glBegin(GL_LINES);
		glVertex3f(p0.x, p0.y, p0.z);
		glVertex3f(coneBaseLocation.x, coneBaseLocation.y, coneBaseLocation.z);
		glEnd();	// GL_LINES
		DrawCone(coneBaseLocation, end_pt, arrow_rad);
	}

	void DrawAxes(double axisLength, double arrow_radius)
	{
		glColor4fv(red);
		DrawArrow(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), axisLength, arrow_radius);
		//DrawArrow(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), axisLength);
		glColor4fv(green);
		DrawArrow(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), axisLength, arrow_radius);
		//DrawArrow(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), axisLength);
		glColor4fv(blue);
		DrawArrow(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), axisLength, arrow_radius);
		//DrawArrow(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), axisLength);
	}

	void DrawSphere(glm::vec3 pos, float sphereRadius)
	{
		GLUquadricObj *pQuadObj = gluNewQuadric();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glEnable(GL_LIGHTING);
	//	glColor4fv(color);
		glTranslatef(pos.x, pos.y, pos.z);
		gluSphere(pQuadObj, sphereRadius, 20, 20);
		// glutSolidSphere?
		gluDeleteQuadric(pQuadObj);
		glPopMatrix();
	}

	void DrawWireSphere(glm::vec3 pos, float sphereRadius)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glEnable(GL_LIGHTING);
		glTranslatef(pos.x, pos.y, pos.z);
		glutWireSphere(sphereRadius, 20, 20);
		glPopMatrix();
	}

	void DrawCircle(glm::vec3 pos, float circleRadius, bool filled)
	{
		unsigned int i=0, n=365;
		float th;
		if(filled) glBegin(GL_TRIANGLE_FAN);
		else glBegin(GL_LINE_STRIP);
		for(i=0;i<=n;i++)	{
			th = (2.0*(float)i/(float)n)*M_PI;
			glVertex3f(pos.x+circleRadius*cos(th), pos.y+circleRadius*sin(th), pos.z);
		}
		glEnd();
	}
	void DrawCuboid(glm::vec3 center_pos, float side_len[3])
	{
		glBegin(GL_QUADS);
		// Front
			glVertex3f(center_pos[0]-0.5*side_len[0], center_pos[1]+0.5*side_len[1], center_pos[2]+0.5*side_len[2]);
			glVertex3f(center_pos[0]-0.5*side_len[0], center_pos[1]-0.5*side_len[1], center_pos[2]+0.5*side_len[2]);
			glVertex3f(center_pos[0]+0.5*side_len[0], center_pos[1]-0.5*side_len[1], center_pos[2]+0.5*side_len[2]);
			glVertex3f(center_pos[0]+0.5*side_len[0], center_pos[1]+0.5*side_len[1], center_pos[2]+0.5*side_len[2]);
		// Right
			glVertex3f(center_pos[0]+0.5*side_len[0], center_pos[1]+0.5*side_len[1], center_pos[2]+0.5*side_len[2]);
			glVertex3f(center_pos[0]+0.5*side_len[0], center_pos[1]-0.5*side_len[1], center_pos[2]+0.5*side_len[2]);
			glVertex3f(center_pos[0]+0.5*side_len[0], center_pos[1]-0.5*side_len[1], center_pos[2]-0.5*side_len[2]);
			glVertex3f(center_pos[0]+0.5*side_len[0], center_pos[1]+0.5*side_len[1], center_pos[2]-0.5*side_len[2]);
		// Back
			glVertex3f(center_pos[0]+0.5*side_len[0], center_pos[1]+0.5*side_len[1], center_pos[2]-0.5*side_len[2]);
			glVertex3f(center_pos[0]+0.5*side_len[0], center_pos[1]-0.5*side_len[1], center_pos[2]-0.5*side_len[2]);
			glVertex3f(center_pos[0]-0.5*side_len[0], center_pos[1]-0.5*side_len[1], center_pos[2]-0.5*side_len[2]);
			glVertex3f(center_pos[0]-0.5*side_len[0], center_pos[1]+0.5*side_len[1], center_pos[2]-0.5*side_len[2]);
		// Left
			glVertex3f(center_pos[0]-0.5*side_len[0], center_pos[1]+0.5*side_len[1], center_pos[2]-0.5*side_len[2]);
			glVertex3f(center_pos[0]-0.5*side_len[0], center_pos[1]-0.5*side_len[1], center_pos[2]-0.5*side_len[2]);
			glVertex3f(center_pos[0]-0.5*side_len[0], center_pos[1]-0.5*side_len[1], center_pos[2]+0.5*side_len[2]);
			glVertex3f(center_pos[0]-0.5*side_len[0], center_pos[1]+0.5*side_len[1], center_pos[2]+0.5*side_len[2]);
		// Top
			glVertex3f(center_pos[0]-0.5*side_len[0], center_pos[1]+0.5*side_len[1], center_pos[2]+0.5*side_len[2]);
			glVertex3f(center_pos[0]+0.5*side_len[0], center_pos[1]+0.5*side_len[1], center_pos[2]+0.5*side_len[2]);
			glVertex3f(center_pos[0]+0.5*side_len[0], center_pos[1]+0.5*side_len[1], center_pos[2]-0.5*side_len[2]);
			glVertex3f(center_pos[0]-0.5*side_len[0], center_pos[1]+0.5*side_len[1], center_pos[2]-0.5*side_len[2]);
		// Bottom
			glVertex3f(center_pos[0]+0.5*side_len[0], center_pos[1]-0.5*side_len[1], center_pos[2]+0.5*side_len[2]);
			glVertex3f(center_pos[0]-0.5*side_len[0], center_pos[1]-0.5*side_len[1], center_pos[2]+0.5*side_len[2]);
			glVertex3f(center_pos[0]-0.5*side_len[0], center_pos[1]-0.5*side_len[1], center_pos[2]-0.5*side_len[2]);
			glVertex3f(center_pos[0]+0.5*side_len[0], center_pos[1]-0.5*side_len[1], center_pos[2]-0.5*side_len[2]);
		glEnd();
	}
	void Draw_Text(char *pTxt, float raster_pos_x, float raster_pos_y, float raster_pos_z, void* font)
	{
		char *c;
		glRasterPos3f(raster_pos_x, raster_pos_y, raster_pos_z);
		for(c=pTxt; *c != NULL; c++) {
			glutBitmapCharacter(font, *c);
		}
	}

};