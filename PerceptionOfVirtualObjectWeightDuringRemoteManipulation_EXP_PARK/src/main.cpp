#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
//#include <826api.h>
#include <GL/glut.h>
#include "geometry_helper.h"
#include "display_helper.h"
#include "CExp_Baik_Hand_PSE.h"

#define _TITLE "Baik-Hand Experiment part1: PSE measurement"


void keyboard(unsigned char key, int x, int y);
void display();
void myInit();
void specialKeys(int key, int x, int y);
void idle();

const int WIDTH = 1000;//400;
const int HEIGHT = 600;//400;
int width = WIDTH, height = HEIGHT;

cExp_Baik_Hand_PSE m_expBaik;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA);
	glutInitWindowSize(width, height);
	glutCreateWindow(_TITLE);
	glutDisplayFunc(display);
	glutReshapeFunc(DISP_TOOLS::reshapeSubFunc);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeys);
	glutIdleFunc(idle);
	glutMouseFunc(DISP_TOOLS::mouseDownSubFunc);
	glutMotionFunc(DISP_TOOLS::mouseMoveSubFunc);
	myInit();
	glutMainLoop();
	return 0;
}

void keyboard(unsigned char key, int x, int y)
{
	char pTxt[256];
	int key_ret;
	key_ret = m_expBaik.handleKeyboard(key, pTxt);
	if (key_ret == 100) {
		exit(0);
	}
	else if (key_ret == 102) {	// Error message box
		MessageBox(NULL, pTxt, "Error", MB_OK | MB_ICONERROR);
	}
}
void display()
{
	m_expBaik.sub_display();
	glFlush();
	glutSwapBuffers();
}

void myInit()
{
	m_expBaik.init();
}

void specialKeys(int key, int x, int y)
{
	m_expBaik.handleSpecialKeys(key, x, y);
}

bool send_once = false;
void idle()
{
	Sleep(10);
	glutPostRedisplay();

	if ((m_expBaik.m_exp_phase == FORCE_TEST || m_expBaik.m_exp_phase == EXP_PHASE2)) m_expBaik.SP->ReadData(m_expBaik.serial_txtbuf, 255);
	/// <summary>
	/// bending sensor 
	/// output 0 : finger bent
	/// output 1 : finger stretched
	/// </summary>
	if ((m_expBaik.m_exp_phase == FORCE_TEST || m_expBaik.m_exp_phase == EXP_PHASE2) && m_expBaik.serial_txtbuf[0] == '0') {
		//printf("%c", m_expBaik.serial_txtbuf[0]);
		m_expBaik.m_bending_sensor_input = true;
	}
	else if ((m_expBaik.m_exp_phase == FORCE_TEST || m_expBaik.m_exp_phase == EXP_PHASE2) && m_expBaik.serial_txtbuf[0] == '1') {
		//printf("%c", m_expBaik.serial_txtbuf[0]);
		m_expBaik.m_bending_sensor_input = false;
		m_expBaik.SP->WriteData("2", 255);
	}

	/// <summary>
	/// FORCE_TEST PHASE
	/// serial
	/// </summary>
	if (m_expBaik.m_exp_phase == FORCE_TEST && m_expBaik.m_sphere_move )
	{
		//Serial read!!!!!!!!!!!!!!!!!!!!
		
		//printf("%c", m_expBaik.serial_txtbuf[0]);
		//idle 에 read 넣으면 안될듯 수정 필요 0425
		if (m_expBaik.m_bending_sensor_input)
		{
			
			/// <summary>
			/// writedata 2 : servo motor degree 30
			/// writedata 3 : servo motor degree 40
			/// writedata 4 : servo motor degree 60
			/// writedata 5 : servo motor degree 80
			/// </summary>
			if (m_expBaik.m_select_sphere_a) m_expBaik.SP->WriteData("3", 255);
			else if (m_expBaik.m_select_sphere_b) m_expBaik.SP->WriteData("4", 255);
			else if (m_expBaik.m_select_sphere_c) m_expBaik.SP->WriteData("5", 255);

			if (m_expBaik.m_sphere_z_pos < 0.15) {
				if (m_expBaik.m_select_sphere_a)m_expBaik.m_sphere_z_pos += 0.005;
				else if (m_expBaik.m_select_sphere_b)m_expBaik.m_sphere_z_pos += 0.003;
				else m_expBaik.m_sphere_z_pos += 0.001;
				
			}
			else if (m_expBaik.m_sphere_z_pos > 0.15) {
				m_expBaik.m_sphere_z_pos = 0;
				m_expBaik.m_sphere_move = false;
				m_expBaik.m_bending_sensor_input = false;
				m_expBaik.m_select_sphere_a = false;
				m_expBaik.m_select_sphere_b = false;
				m_expBaik.m_select_sphere_c = false;
			}
		}
	}
	/// <summary>
	/// FORCE_TEST PHASE
	/// serial
	/// </summary>
	if (m_expBaik.m_exp_phase == EXP_PHASE2 && m_expBaik.m_sphere_move)
	{
		if (m_expBaik.m_bending_sensor_input)
		{
			//랜덤값 수정 필요

			/// <summary>
			/// writedata 2 : servo motor degree 30
			/// writedata 3 : servo motor degree 40
			/// writedata 4 : servo motor degree 60
			/// writedata 5 : servo motor degree 80
			/// </summary>
			if (m_expBaik.m_select_sphere_a) m_expBaik.SP->WriteData("3", 255);
			else if (m_expBaik.m_select_sphere_b) m_expBaik.SP->WriteData("4", 255);

			if (m_expBaik.m_sphere_z_pos < 0.15) {
				if (m_expBaik.m_select_sphere_a)m_expBaik.m_sphere_z_pos += 0.005;
				else if (m_expBaik.m_select_sphere_b)m_expBaik.m_sphere_z_pos += 0.003;
				else m_expBaik.m_sphere_z_pos += 0.001;

			}
			else if (m_expBaik.m_sphere_z_pos > 0.15) {
				m_expBaik.m_sphere_z_pos = 0;
				m_expBaik.m_sphere_move = false;
				m_expBaik.m_bending_sensor_input = false;
				m_expBaik.m_select_sphere_a = false;
				m_expBaik.m_select_sphere_b = false;
			}
		}
	}
}

