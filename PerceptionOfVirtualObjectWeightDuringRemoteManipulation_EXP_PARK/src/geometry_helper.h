#ifndef __GEOMETRY_HELPER_H__
#define __GEOMETRY_HELPER_H__
#include <glm\glm.hpp>
#include <vector>
//#include "Eigen\Dense"
//include "CMVertex.h"

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923
#endif

#ifndef M_PI_4
#define M_PI_4     0.785398163397448309616
#endif

#ifndef DTOR
#define DTOR 0.0174532925	// degrees to radians
#endif

using namespace std;

struct joint3D;

typedef struct {
	glm::vec3 p1, p2, p3;
} Facet3;

void Unit(glm::vec3 input_vector, glm::vec3 &normalized_vector);
void Normalize(glm::vec3 &vec, float rad=1.0);
void find_common_plane(glm::vec3 x0, glm::vec3 x1, glm::vec3 x2, glm::vec4 &coeff);
bool find_projection(glm::vec3 p, glm::vec3 x0, glm::vec3 x1, glm::vec3 x2, glm::vec3 & q);//, int dbg_idx);
bool IsInsideTriangle(glm::vec3 p, glm::vec3 x0, glm::vec3 x1, glm::vec3 x2, int debug_idx);
void increaseJointAngle(joint3D &joint);
glm::mat4 rotation_mat(int xyz, float th);
glm::mat4 translation_mat(glm::vec3 translation);//double translation[3]);
glm::mat4 rotation_mat_diff(int xyz, float th);
glm::mat3 get_CrossProductMat (glm::vec3 a);
int CreateSphericSurface(Facet3* input, int n_in_faces, int n_iterations, float radius, Facet3* output);
Facet3* genSphericSurface(Facet3* input, int n_in_faces, int n_iterations, float radius, int &num_out_facets);

////  True if the projection of c to the line connecting a and b is
//// within ab. False otherwise.
inline bool CheckProjectionWithinBoundary(double *a, double *b, double *c)
{
	double inner_product[2];
	//FindProjectionPoint(a, b, c, projection);
	inner_product[0] = (c[0]-b[0])*(b[0]-a[0])+(c[1]-b[1])*(b[1]-a[1]);
	inner_product[1] = (c[0]-a[0])*(b[0]-a[0])+(c[1]-a[1])*(b[1]-a[1]);
	if(inner_product[0] <= 0 && inner_product[1] >= 0)
		return true;
	else return false;
}
struct lineVar {
	// b = slope*t + offset  (0 <= t <= t_max)
	double slope[3];
	double offset[3];
	double t_max;
	void calcVars(double pt0[3], double pt1[3]) {
		double n0=0;
		double vec[3];
		int i;
		for(i=0;i<3;i++) {
			vec[i] = pt1[i] - pt0[i];
			offset[i] = pt0[i];
			n0 += vec[i]*vec[i];
		}
		n0 = sqrt(n0);
		t_max = 0;
		for(i=0;i<3;i++) {
			slope[i] = vec[i]/n0;
			t_max += slope[i] * vec[i];
		}
	}
	double getProjection(double pt[3]) {
		double ret = 0.f;
		double a[3];
		for(int i=0;i<3;i++) {
			a[i] = pt[i] - offset[i];
			ret += a[i]*slope[i];
		}
		return ret;
	}
	double getProjection(double x, double y, double z) {
		double var[3] = {x, y, z};
		return getProjection(var);
	}
	double getProjection(double pt[3], double pt_project[3], double &t_val) {
		double  dist = 0.f, buf;
		t_val = getProjection(pt);
		for(unsigned int i=0;i<3;i++) {
			pt_project[i] = offset[i] + slope[i]*t_val;
			buf = pt_project[i] - pt[i];
			dist += buf*buf;
		}
		dist = sqrt(dist);
		return dist;
	}
	bool inside_segment(double pt[3]) {
		if(getProjection(pt) >= 0.f && getProjection(pt) <= t_max) return true;
		else return false;
	}
	bool inside_segment(double x, double y, double z) {
		double var[3] = {x, y, z};
		return inside_segment(var);
	}
};

#endif
