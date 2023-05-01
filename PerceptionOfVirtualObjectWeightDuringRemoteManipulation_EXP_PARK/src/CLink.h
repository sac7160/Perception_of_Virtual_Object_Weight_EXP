#ifndef _CLINK_H_
#define _CLINK_H_
#include "geometry_helper.h"
#include "cGenericLink.h"
#include "display_helper.h"
#include "glm\glm.hpp"

class cLink : public cGenericLink
{
public:
	cLink();
	cLink(float color[3], double link_cyl_radius);
	~cLink() {};
	void render(bool affectChildren);
	void updateTransMatrix(bool affectChildren = true);
	void initConfig(cLink *pParent, glm::vec3 parent_translate, glm::vec3 link_translate, double link_len, int xyz, bool show_axes=false);//, float color[3]);
	void initConfig(cLink *pParent, glm::vec3 parent_translate, double parent_rot[3], glm::vec3 link_translate, double link_len, int xyz, bool show_axes=false);
	void setParentTranslationOffset(glm::vec3 parent_translation_offset);	// To set an offset when using multiple devices.

	void setParentTransformMatrix(glm::mat3 rot_mat, glm::vec3 translate);
	
	inline void setJointAngle(double th) {
		m_rot_angle = th;
		updateTransMatrix();
	}
	inline void setDispPlane(int disp_plane) { m_disp_plane = disp_plane; }
//	inline void setTransMatrix(glm::mat4x4 transformation_mat) { m_transform_mat = transformation_mat; }
	inline glm::vec3 getGlobalOriginPosition() { return m_g_origin;}
	inline glm::vec3 getGlobalLinkEndPosition() { return m_g_endPosition; };
	inline glm::mat4 getTransformMatrix() {return m_transform_mat;}
	inline glm::mat4 getTransformDiffMatrix() {return m_transform_mat_diff;}
	inline void setParentTransformMatrix(glm::vec3 parent_translate) {
		m_parent_transform = translation_mat(parent_translate);
		updateTransMatrix();
	}
	inline glm::mat4 getParentTransform() { return m_parent_transform; }
	inline void setAxesSize(float length, float arr_radius) {
		m_axes_size[0] = length; m_axes_size[1] = arr_radius;
	}
public:
	//-------------------------------------------
	// MEMBERS
	//-------------------------------------------
//	glm::mat4 m_g_transform;
	glm::mat4 m_g_transform;
	int m_idx;	// variable for debugging
private:
	//-------------------------------------------
	// MEMBERS
	//-------------------------------------------
//	glm::mat4 m_translate_mat;
	glm::mat4 m_parent_transform;
	glm::vec3 m_g_origin;
	glm::vec3 m_g_endPosition;
	glm::mat4 m_transform_mat;
	glm::mat4 m_transform_mat_diff;
	int m_disp_plane;	// 0 :  no-plane 1: xy-plane 2: yz-plane; 3: zx-palne
	float m_axes_size[2];
};

#endif
