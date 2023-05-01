#include "CLink.h"


#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

/*****************************************
  cLink is a class for a rotational link 
  Has a transformation matrix represented as T*R where is the translation from the parent coordinate origin
  and R is local rotation. 
 ******************************************/
cLink::cLink()
{
	m_disp_plane = 0;
	m_children.clear();
	m_color[0] = m_color[1] = m_color[2] = 1.f;
	m_link_cyl_rad = 0.01;
	m_rot_angle = 0.f;
	m_pParentLink = NULL;
}
cLink::cLink(float color[3], double link_cyl_radius)
{
	unsigned int i;
	for(i=0;i<3;i++) m_color[i] = color[i];
	m_link_cyl_rad = link_cyl_radius;
	m_pParentLink = NULL;
	m_rot_angle = 0.f;
	m_disp_plane = 0;
}
void cLink::initConfig(cLink *pParentLink, glm::vec3 parent_translate, glm::vec3 link_translate, double link_len, int xyz, bool show_axes)//, float color[3])
{
	cGenericLink::initConfig(parent_translate, link_translate, link_len, xyz, m_color, show_axes);
	if(pParentLink != NULL) cGenericLink::setParentLink(pParentLink);
	//m_translate_mat 
	m_parent_transform = translation_mat(parent_translate);
	m_transform_mat = m_parent_transform;//m_translate_mat;
	//for(unsigned int i=0;i<3;i++) {
	//	m_T[3][i] = parent_translate[i];
	//}
	updateTransMatrix(false);
}

void cLink::initConfig(cLink *pParentLink, glm::vec3 parent_translate, double parent_rot[3], glm::vec3 link_translate, double link_len, int xyz, bool show_axes)
{
	cGenericLink::initConfig(parent_translate, parent_rot, link_translate, link_len, xyz, m_color, show_axes);
	if(pParentLink != NULL) cGenericLink::setParentLink(pParentLink);
	//m_translate_mat 
	m_parent_transform = translation_mat(parent_translate) * rotation_mat(2, parent_rot[2])*rotation_mat(1, parent_rot[1])*rotation_mat(0, parent_rot[0]);
	m_transform_mat = m_parent_transform;
	updateTransMatrix(false);
}

void cLink::setParentTransformMatrix(glm::mat3 rot_mat, glm::vec3 translate)
{
	m_parent_transform[3] = glm::vec4(translate[0], translate[1], translate[2], 1);
	m_translate = translate;
	for(unsigned int i=0;i<3;i++)
		m_parent_transform[i] = glm::vec4(rot_mat[i][0], rot_mat[i][1], rot_mat[i][2], 0);
	updateTransMatrix();
}

/********************************************
 * void updateTransMatrix()
 *      updates m_transform_mat variable as T*R   (T: translation matrix, R: rotation matrix)
 ********************************************/
void cLink::updateTransMatrix(bool affectChildren)
{
	unsigned int i, numItems;
	cLink *pParent, *nextChild;
	// updates transformation matrix
	m_transform_mat = m_parent_transform*rotation_mat(m_xyz, m_rot_angle);	//m_translate_mat
	m_transform_mat_diff = m_parent_transform*rotation_mat_diff(m_xyz, m_rot_angle);
	if(m_pParentLink != NULL) {
		pParent = (cLink*)m_pParentLink;		// assumes that the parent link is a cLink object.
		m_g_transform = pParent->m_g_transform*m_transform_mat;
	}
	else {
	//	m_transform_mat = m_parent_transform;
	//	m_transform_mat_diff = m_parent_transform;
		m_g_transform = m_transform_mat;
	}
	m_g_origin = glm::vec3(m_g_transform[3]);
//	if(m_idx ==1) {
	//	printf("[%d][%f %f %f %f] rot(%f)\n", m_idx, m_g_transform[3][0], m_g_transform[3][1], m_g_transform[3][2], m_g_transform[3][3], m_rot_angle);
//	}
	//if(m_g_origin[0] == 1.#INF00 || m_g_origin[0] ==-INF)
	m_g_endPosition = glm::vec3(m_g_transform*glm::vec4(m_link_translate[0], m_link_translate[1], m_link_translate[2], 1.0));
//	printf("m_g_origin=%f %f %f\n", m_g_origin[0], m_g_origin[1], m_g_origin[2]);
	if(affectChildren) {
		numItems = m_children.size();
		for(i=0;i<numItems;i++) {
			nextChild = (cLink*)m_children[i];
			nextChild->updateTransMatrix(affectChildren);
		}
	}
	//for(int i=0;i<4;i++) printf("\t[%f %f %f %f]\n", m_transform_mat[0][i], m_transform_mat[1][i], m_transform_mat[2][i], m_transform_mat[3][i]);
	//printf("---\n");
}

void cLink::setParentTranslationOffset(glm::vec3 parent_translation_offset)
{
//	glm::vec3 prev_trans = m_translate + parent_translation_offset;
	m_translate = m_translate + parent_translation_offset;
	//m_translate_mat
	m_parent_transform = translation_mat(m_translate);
	updateTransMatrix();
}

void cLink::render(bool affectChildren)
{
	unsigned int i, numItems;
	double ang_mult= 180.0/M_PI;
	glColor3f(m_color[0], m_color[1], m_color[2]);
	float mat[16];
	glPushMatrix();
		// from origin to the link end point
		glMultMatrixf(glm::value_ptr(m_parent_transform));
		glRotatef(ang_mult*m_rot_angle, m_xyz==0?1.0:0.0, m_xyz==1?1.0:0.0, m_xyz==2?1.0:0.0);
	//	glRotatef(m_rot_angle, (m_xyz==-1 || m_xyz==1)?(m_xyz==1?1.0:-1.0):0.0, (m_xyz==-2 || m_xyz==2)?(m_xyz==2?1.0:-1.0):0.0, 
	//			(m_xyz==-3 || m_xyz==3)?(m_xyz==3?1.0:-1.0):0.0);
		if(m_link_len> 0.f) {
			DISP_TOOLS::DrawCylinder(glm::vec3(0, 0, 0), m_link_translate, m_link_cyl_rad, m_link_cyl_rad);
		//	DISP_TOOLS::DrawAxes(0.01, .0005);
		}
		if(m_showAxes) DISP_TOOLS::DrawAxes(m_axes_size[0], m_axes_size[1]);
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

