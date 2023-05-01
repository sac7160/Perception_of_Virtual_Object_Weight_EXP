#include "CGenericLink.h"

cGenericLink::cGenericLink() {
	m_showAxes = false;
	for(unsigned int i=0;i<3;i++) m_parent_rot[i] =0.f;
}

void cGenericLink::initConfig(glm::vec3 parent_translate, glm::vec3 link_translate, double link_len, int xyz, float color[3])
{
	m_translate = parent_translate;
	m_link_translate = link_translate;
	m_link_len = link_len;
	m_xyz = xyz;
	for(unsigned int i=0;i<3;i++) m_color[i] = color[i];
//	updateTransMatrix();
}

void cGenericLink::initConfig(glm::vec3 parent_translate, glm::vec3 link_translate, double link_len, int xyz, float color[3], bool show_axes)
{
	m_translate = parent_translate;
	m_link_translate = link_translate;
	m_link_len = link_len;
	m_xyz = xyz;
	m_showAxes = show_axes;
	for(unsigned int i=0;i<3;i++) m_color[i] = color[i];
//	updateTransMatrix();
}

void cGenericLink::initConfig(glm::vec3 parent_translate, double parent_rot[3], glm::vec3 link_translate, double link_len, int xyz, float color[3], bool show_axes)
{
	for(unsigned int i=0;i<3;i++) m_parent_rot[i] = parent_rot[i];
	initConfig(parent_translate, link_translate, link_len, xyz, color, show_axes);
}

void cGenericLink::setParentLink(cGenericLink *a_parentLink) 
{
	m_pParentLink = a_parentLink; 
	if(a_parentLink != NULL) {
		a_parentLink->addChild(this);
	}
};

void cGenericLink::render(bool affectChildren)
{
	unsigned int i, numItems;
	double ang_mult= 180.0/M_PI;
	glColor3f(m_color[0], m_color[1], m_color[2]);
	glPushMatrix();
	// from origin to the link end point
	glTranslatef(m_translate[0], m_translate[1], m_translate[2]);
	glRotatef(ang_mult*m_rot_angle, m_xyz==0?1.0:0.0, m_xyz==1?1.0:0.0, m_xyz==2?1.0:0.0);
//	glRotatef(m_rot_angle, (m_xyz==-1 || m_xyz==1)?(m_xyz==1?1.0:-1.0):0.0, (m_xyz==-2 || m_xyz==2)?(m_xyz==2?1.0:-1.0):0.0, 
//			(m_xyz==-3 || m_xyz==3)?(m_xyz==3?1.0:-1.0):0.0);
	if(m_link_len> 0.f) {
		DISP_TOOLS::DrawCylinder(glm::vec3(0, 0, 0), m_link_translate, m_link_cyl_rad, m_link_cyl_rad);
		DISP_TOOLS::DrawAxes(0.03*100.0, 0.01*100.0);
	}
	if(affectChildren) {
		//glPushMatrix();
		numItems = m_children.size();
		for(i=0;i<numItems;i++) {
			cGenericLink *nextChild = m_children[i];
			nextChild->render(affectChildren);
		}
		//glPopMatrix();
	}
	glPopMatrix();
}
