#include <math.h>
#include <glm\glm.hpp>
#include "geometry_helper.h"

void Unit(glm::vec3 input_vector, glm::vec3 &normalized_vector){
	float input_norm;
	input_norm = sqrt(input_vector.x*input_vector.x+input_vector.y*input_vector.y+input_vector.z*input_vector.z);
	normalized_vector.x = input_vector.x / input_norm;
	normalized_vector.y = input_vector.y / input_norm;
	normalized_vector.z = input_vector.z / input_norm;
}

void Normalize(glm::vec3 &p, float rad)
{
	double length, mult;
	length = sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
	if(length != 0) {
		mult = rad/length;//len_perp = 1.0/length;
		p.x *= mult;
		p.y *= mult;
		p.z *= mult;
	}
	else {
		p.x = 0;
		p.y = 0;
		p.z = 0;
	}
}

void find_common_plane(glm::vec3 x0, glm::vec3 x1, glm::vec3 x2, glm::vec4 &coeff)
{
	glm::vec3 coeff_abc;
	float d;
	coeff_abc = glm::cross(x0-x2, x1-x2);
	d = -glm::dot(x2, glm::cross(x0, x1));
	for(int i=0;i<3;i++) coeff[i] = coeff_abc[i];
	coeff[3] = d;
}

bool find_projection(glm::vec3 p, glm::vec3 x0, glm::vec3 x1, glm::vec3 x2, glm::vec3 & q)//, int dbg_idx)
{
	bool ret = true;
	glm::vec4 plane_coeff;
	float t;
	glm::vec3 n;
	find_common_plane(x0, x1, x2, plane_coeff);
	/// Now we found the coefficient for the plane equation ax + by + cz + d = 0;
	n = glm::vec3(plane_coeff[0], plane_coeff[1], plane_coeff[2]);
	t = (glm::dot(n,p)  + plane_coeff[3])/glm::dot(n, n);
	q = p - t*n;
	//if(dbg_idx == 1) {
	//	printf("normal :(%f, %f, %f)\n", n.x, n.y, n.z);
	//}
	return ret;
}

// checks if the input point p is inside a triangle defined by three points x0, x1 and x2
bool IsInsideTriangle(glm::vec3 p, glm::vec3 x0, glm::vec3 x1, glm::vec3 x2, int debug_idx)
{
	bool ret = false;
	const double INTERSECT_EPSILON = 10e-14f;
	glm::vec3 v0, v1, v2;
	float dot00, dot01, dot02, dot11, dot12, invDenom, u, v;
	v0 = x2 - x0;
	v1 = x1 - x0;
	v2 = p - x0;
	dot00 = glm::dot(v0, v0);
	dot01 = glm::dot(v0, v1);
	dot02 = glm::dot(v0, v2);
	dot11 = glm::dot(v1, v1);
	dot12 = glm::dot(v1, v2);
	invDenom = 1.0/(dot00*dot11 - dot01*dot01);
	u = (dot11*dot02 - dot01*dot12)*invDenom;
	v = (dot00*dot12 - dot01*dot02)*invDenom;
	if(  (u >= (0.0-INTERSECT_EPSILON) ) && (v >= (0.0 - INTERSECT_EPSILON)) && ((u+v) <= (1.0 + INTERSECT_EPSILON))  ){
	//	if(debug_idx == 3) printf("[%d] u=%f v=%f u+v=%f\n", debug_idx, u, v, u+v);
		ret = true;
	}
	return ret;
}

glm::mat4 translation_mat(glm::vec3 translation)
{
	glm::mat4 ret_matrix;
	ret_matrix[3][0] = translation[0];
	ret_matrix[3][1] = translation[1];
	ret_matrix[3][2] = translation[2];
	return ret_matrix;
}

glm::mat4 rotation_mat(int xyz, float th)
{
	glm::mat4 ret_matrix;
	if(xyz == 0) {		// x-axis (yaw)
		ret_matrix[1][1] = cos(th);
		ret_matrix[1][2] = sin(th);
		ret_matrix[2][1] = -sin(th);
		ret_matrix[2][2] = cos(th);
	}
	if(xyz == 1) {		// y-axis (pitch)
		ret_matrix[0][0] = cos(th);
		ret_matrix[0][2] = -sin(th);
		ret_matrix[2][0] = sin(th);
		ret_matrix[2][2] = cos(th);
	}
	if(xyz == 2) {		// z-axis (roll)
		ret_matrix[0][0] = cos(th);
		ret_matrix[0][1] = sin(th);
		ret_matrix[1][0] = -sin(th);
		ret_matrix[1][1] = cos(th);
	}
	return ret_matrix;
}

glm::mat4 rotation_mat_diff(int xyz, float th)
{
	glm::mat4 ret_matrix;
	if(xyz == 0) {
		ret_matrix[0][0] = 0.f;
		ret_matrix[1][1] = -sin(th);
		ret_matrix[1][2] = cos(th);
		ret_matrix[2][1] = -cos(th);
		ret_matrix[2][2] = -sin(th);
		ret_matrix[3][3] = 0.f;
	}
	if(xyz == 1) {
		ret_matrix[0][0] = -sin(th);
		ret_matrix[0][2] = -cos(th);
		ret_matrix[1][1] = 0.f;
		ret_matrix[2][0] = cos(th);
		ret_matrix[2][2] = -sin(th);
		ret_matrix[3][3] = 0.f;
	}
	if(xyz == 2) {
		ret_matrix[0][0] = -sin(th);
		ret_matrix[0][1] = cos(th);
		ret_matrix[1][0] = -cos(th);
		ret_matrix[1][1] = -sin(th);
		ret_matrix[2][2] = 0.f;
		ret_matrix[3][3] = 0.f;
	}
	return ret_matrix;
}
///////////////////
//		[   0  -a_z   a_y]
//		[ a_z    0   -a_x]
//		[-a_y   a_x     0]
//////////////////

glm::mat3 get_CrossProductMat (glm::vec3 a)
{
	glm::mat3 ret_mat;
	ret_mat[0][0] = ret_mat[1][1] = ret_mat[2][2] = 0.f;
	ret_mat[0][1] = a[2];
	ret_mat[0][2] = -a[1];
	ret_mat[1][0] = -a[2];
	ret_mat[1][2] = a[0];
	ret_mat[2][0] = a[1];
	ret_mat[2][1] = -a[0];
	return ret_mat;
}

int CreateSphericSurface(Facet3* input, int n_in_faces, int n_iterations, float radius, Facet3* output)//int &n_out)//, int n_out_faces)
{
	int i, it;
	int nt =0, ntold;
	glm::vec3 pa, pb, pc;
	nt = n_in_faces;
//////

	if(n_iterations < 1) {
		return(nt);
	}
	else {
		memcpy(output, input, sizeof(Facet3)*n_in_faces);
		//for(i=0;i<n_in_faces;i++)
		//	output[i] = input[i];
	}
	// Bisect each edge and move to the surface of a unit sphere
	for(it=0;it<n_iterations;it++) {
		ntold = nt;
		for(i=0;i<ntold;i++) {
			pa.x = (output[i].p1.x + output[i].p2.x)*0.5;
			pa.y = (output[i].p1.y + output[i].p2.y)*0.5;
			pa.z = (output[i].p1.z + output[i].p2.z)*0.5;
			////
			pb.x = (output[i].p2.x + output[i].p3.x)*0.5;
			pb.y = (output[i].p2.y + output[i].p3.y)*0.5;
			pb.z = (output[i].p2.z + output[i].p3.z)*0.5;
			////
			pc.x = (output[i].p3.x + output[i].p1.x)*0.5;
			pc.y = (output[i].p3.y + output[i].p1.y)*0.5;
			pc.z = (output[i].p3.z + output[i].p1.z)*0.5;
			///
			//glm::normalize(pa); glm::normalize(pb); glm::normalize(pc);
			Normalize(pa, radius); Normalize(pb, radius); Normalize(pc, radius);
			//Normalise(&pa); Normalise(&pb); Normalise(&pc);
			output[nt].p1 = output[i].p1; output[nt].p2 = pa; output[nt].p3 = pc; nt++;
			output[nt].p1 = pa; output[nt].p2 = output[i].p2; output[nt].p3 = pb; nt++;
			output[nt].p1 = pb; output[nt].p2 = output[i].p3; output[nt].p3 = pc; nt++;
			output[i].p1 = pa; output[i].p2 = pb; output[i].p3 = pc;
//////////////////////
		}
	}
	return nt;
}

Facet3* genSphericSurface(Facet3* input, int n_in_faces, int n_iterations, float radius, int &num_out_facets)
{
	int i, j, nstart, n;
	glm::vec3 p1, p2, p3;
	Facet3* f_out = NULL;
	f_out = (Facet3*)malloc(sizeof(Facet3)*n_in_faces);
	memcpy(f_out, input, sizeof(Facet3)*n_in_faces); //output, input, sizeof(Facet3)*n_in_faces);
	n = n_in_faces;//1;
	for(i=1;i<n_iterations;i++) {
		nstart = n;
		for(j=0;j<nstart;j++) {
			f_out = (Facet3*)realloc(f_out, (n+3)*sizeof(Facet3));//output = (Facet3*)realloc(output, (n+3)*sizeof(Facet3));
			f_out[n]   = f_out[j];//output[n]   = output[j];
			f_out[n+1] = f_out[j];//output[n+1] = output[j];
			f_out[n+2] = f_out[j];//output[n+2] = output[j];
			// Calculate the midpoints
			p1 = (float)0.5*(f_out[j].p1 + f_out[j].p2);//(float)0.5*(output[j].p1 + output[j].p2);
			Normalize(p1, radius);
			p2 = (float)0.5*(f_out[j].p2 + f_out[j].p3);//(float)0.5*(output[j].p2 + output[j].p3);
			Normalize(p2, radius);
			p3 = (float)0.5*(f_out[j].p3 + f_out[j].p1);//(float)0.5*(output[j].p3 + output[j].p1);
			Normalize(p3, radius);
			// Replace the curent facet
			f_out[j].p2 = p1;//output[j].p2 = p1;
			f_out[j].p3 = p3;//output[j].p3 = p3;
			// Create the changed vertices in the new facets
			f_out[n].p1		= p1;//output[n].p1   = p1;
			f_out[n].p3		= p2;//output[n].p3   = p2;
			f_out[n+1].p1	= p3;//output[n+1].p1 = p3;
			f_out[n+1].p2	= p2;//output[n+1].p2 = p2;
			f_out[n+2].p1	= p1;//output[n+2].p1 = p1;
			f_out[n+2].p2	= p2;//output[n+2].p2 = p2;
			f_out[n+2].p3	= p3;//output[n+2].p3 = p3;
			n += 3;
		}
	}
	num_out_facets = n;
	return f_out;//n;
}
