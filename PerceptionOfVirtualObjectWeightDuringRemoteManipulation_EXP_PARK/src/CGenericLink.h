#ifndef _CGENERIC_LINK_H_
#define _CGENERIC_LINK_H_
#include <windows.h>
#include <vector>
#include "geometry_helper.h"
#include "display_helper.h"

#include <GL\glut.h>
#include <glm\glm.hpp>

using std::vector;

class cGenericLink {
public:
	cGenericLink();
	~cGenericLink() {};
	inline void addChild(cGenericLink *a_link) {
		a_link->m_pParentLink = this;
		m_children.push_back(a_link);
	}
	inline void setJointAngle(double th) {
		m_rot_angle = th;
	//	updateTransMatrix();
	}
	//inline void setParentTranslate(glm::vec3 parent_translate)
	//{
	//	m_translate = parent_translate;
	//	
	//}
	inline void setColor(const double &R, const double &G, const double &B) {
		m_color[0] = R; m_color[1] = G; m_color[2] = B;
	}
	inline void setLinkRadius(double link_cyl_radius) {
		m_link_cyl_rad = link_cyl_radius;
	}
	void setParentLink(cGenericLink *a_parentLink);// { m_pParentLink = a_parentLink; };
	virtual void render(bool affectChildren);
	virtual void setParentTransformMatrix(glm::mat3 rot_mat, glm::vec3 translate) {};
	void initConfig(glm::vec3 parent_translate, glm::vec3 link_translate, double link_len, int xyz, float color[3], bool show_axes=false);
	void initConfig(glm::vec3 parent_translate, double parent_rot[3], glm::vec3 link_translate, double link_len, int xyz, float color[3], bool show_axes=false);
	void initConfig(glm::vec3 parent_translate, glm::vec3 link_translate, double link_len, int xyz, float color[3]);
public:
	//-------------------------------------------
	// MEMBERS
	//-------------------------------------------
	bool m_showAxes;
	double m_link_len;
	cGenericLink *m_pParentLink;
	vector<cGenericLink*> m_children;
	double m_rot_angle;
	double m_parent_rot[3];
	double m_link_cyl_rad;
	glm::vec3 m_translate;		// translation from parent link origin
	glm::vec3 m_link_translate;	// 
//	glm::mat4 m_refMatrix;		// m_transMatrix = m_refMatrix * rotation_matrix; 
//	glm::mat4 m_transMatrix;
	// m_xyz : joint rotation direction
	//	-1, 1: x-axis rotation  
	//  -2, 2: y-axis rotation
	//  -3, 3: z-axis rotation
	int m_xyz;
	GLfloat m_color[3];
	lineVar m_line;
};

#endif	// _CGENERIC_H_

