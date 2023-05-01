#define _CRT_SECURE_NO_WARNINGS

#include "CExp_Baik_Hand_PSE.h"

#include "CMCI_sound.h"


#include "display_helper.h"
#include "Timer_exp.h"
unsigned __stdcall s826_ReadThread(void *arg);
unsigned __stdcall audioThread(void* arg);
unsigned __stdcall forceCtrlThread(void* arg);


const char m_instruction_msg[][4][128] = {
	{{"Insert your index finger."}, {"To move to the next phase, hit the 'Enter"}},	//INIT
	{{"There are three types of weights."}, {"Grab an object and pull."}, {"It feels like an object is slipping from your hand because it is heavy."},{"Hit 'Enter' to train."}}, // DEVICE_INIT	// adjusts the tendons' tension 
//	{{"Hit 'space bar' to setup the force feedback position"}, {"Hit 'Enter' to move to type the participant information"}},	// PHANTOM_SETUP
	{{"Enter : "},{"Select (A,B,C) and Grab"},{"Hit 'space bar' and pull the virtual object"},{"To move to the next phase, hit the 'Enter'"}},
	{{"subject ID:"}, {"Type subject ID and hit 'Enter' to begin the measurement."}, {""}}, // INFO_INPUT
	{{"Trial No. :"}, {"Hit the spacebar to feel the force feedback to the fingertip."}, {""}, {""}},	// EXP_PHASE1
	{{"Enter : "}, {"Select (A,B,C) and Grab"},{"Hit 'space bar' and pull the virtual object"}, {"Remember which one is heavier. To move to the next phase, hit the 'Enter'"}},	// EXP_PHASE2
	{{"Which one was heavier? ('a' or 'b') : "}, 
		{"If you want to feel the force feedback again, hit the spacebar."}, {"To move to the next trial, hit the 'Enter'."}},		// EXP_PHASE3
	//{{""}, {""}, {""}}, // DATA_ANALYSIS.
	//{{""},},//WRITE_RESULT,
	{{"Experiment Complete. Thank you for your participation."}}//EXP_DONE,
};


static int m_dbg_val[4] = { 0, 0, 0, 0 };
static double m_dbg_dVal[4] = { 0.0, 0.0, 0.0, 0.0 };
HANDLE cExp_Baik_Hand_PSE::m_hAudioEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
HANDLE cExp_Baik_Hand_PSE::m_hArdEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
HANDLE cExp_Baik_Hand_PSE::m_hForceEnable = CreateEvent(NULL, FALSE, FALSE, NULL);

cExp_Baik_Hand_PSE::cExp_Baik_Hand_PSE() {
	int i;
	double trial_pt_force[] = { 0.5, 1.0, 1.5, 0.5, 1.0, 1.5 };
	int trial_joint[] = { 0, 0, 0, 1, 1, 1 };	
	m_exp_phase = EXP_PHASE::INIT;
	m_view_height = -0.04;
	m_textBuf_len = 0;
	m_show_dbg_info = false;
	for (i = 0; i < 4; i++) {
		m_poten_pos[i] = 0.0;
	}
	for (i = 0; i < 2; i++) m_prev_force[i] = 0.0;
	
	m_pwm_val = 150;
	m_disp_coeff = false;
	m_disp_f_coeff = false;
	m_prev_pt_force = 0.0;
	m_enableFctrl = false;
	for (i = 0; i < 2; i++) m_force_d[i] = 0.0;
	for(i=0;i<2;i++) m_f_state[i] = 0;
	m_tot_trial = 6;
	for (i = 0; i < m_tot_trial; i++) {
		m_trial_pt_force[i] = trial_pt_force[i];
		m_trial_joint[i] = trial_joint[i];
		m_trial_force_d[i] = 0.0;
	}
//	m_val_bar = 0.0;
	m_disp_f_bar = false;
	m_fc_opt = true;
	m_uwnd_jnt = -1;
	m_uwnd_dur = 0.7;
	m_min_f_d = 1.0;
	m_max_f_d = 4.0;

	m_disp_train_sphere = false;
	m_disp_test_sphere = false;
	m_sphere_move = false;
	m_sphere_z_pos = 0;

	m_select_sphere_a = false;
	m_select_sphere_b = false;
	m_select_sphere_c = false;

	m_bending_sensor_input = false;

	/*Serial*/
	SP = new Serial("\\\\.\\COM4");

	
	m_FSR_force = 0.0;
	m_act_ready = 0;
//	m_first_pt = true;
}

cExp_Baik_Hand_PSE::~cExp_Baik_Hand_PSE() {
	
}

int cExp_Baik_Hand_PSE::handleKeyboard(unsigned char key, char* ret_string)	// pure virtual functions
{
	int ret = 0, i;
//	double curr_f_d;
	if (key == 27) {
		m_prev_pt_force = 0.0;
		SetEvent(m_hAudioEvent);
		SetEvent(m_hArdEvent);
		SetEvent(m_hForceEnable);
		m_exp_phase = EXP_PHASE::QUIT_EXP;
		sendArd(0, 0);
		ret = 100;

	}
	else {
		if (m_exp_phase == EXP_PHASE::INIT) {
			if(key == 13) ret = moveToNextPhase();
		}
		else if (m_exp_phase == EXP_PHASE::DEVICE_INIT) {
			if (key == '1') windTendon(0, 1);			// PIP		unwind
			else if (key == '2') windTendon(0, 0);		//			wind
			else if (key == '4') windTendon(1, 1);		// MCP		unwind
			else if (key == '5') windTendon(1, 0);		//			wind
			else if (key == 13) moveToNextPhase();
		}
		else if (m_exp_phase == EXP_PHASE::FORCE_TEST) {		
			if (key == 'm') {
				m_sphere_move = true;
			}
			if (key == 'a' || key == 'b' || key == 'c') {
				m_txtBuf[m_textBuf_len++] = key;
				m_txtBuf[m_textBuf_len] = NULL;
				if (key == 'a') {
					m_select_sphere_a = true;
					m_select_sphere_b = false;
					m_select_sphere_c = false;
					//SP->WriteData("1", 255);
					
				}
				else if (key == 'b') {
					m_select_sphere_a = false;
					m_select_sphere_c = false;
					m_select_sphere_b = true;
				}
				else if (key == 'c') {
					m_select_sphere_a = false;
					m_select_sphere_b = false;
					m_select_sphere_c = true;
				}
			}
			if (key == ' ') {
				for (int i = 0; i < m_textBuf_len; i++)m_txtBuf[i] = NULL;
				m_textBuf_len = 0;
				m_sphere_move = true;
				//SP->WriteData("1", 255);
			}
			if (key == '7') {
				if (m_prev_pt_force > 0.0) m_prev_pt_force -= 0.5;
			}
			else if (key == '8') m_prev_pt_force = 0.0;
			else if (key == '9') {
				if (m_prev_pt_force < 3.0) m_prev_pt_force += 0.5;
			}
			else if (key == 'o' || key == 'O') {
				if (m_force_d[0] > 0.0) m_force_d[0] -= 1.0;
			}
			else if (key == 'p' || key == 'P') {
				if (m_force_d[0] < 5.0) m_force_d[0] += 1.0;
			}
			else if (key == 'k' || key == 'K') {
				if (m_force_d[1] > 0.0) m_force_d[1] -= 1.0;
			}
			else if (key == 'l' || key == 'L') {
				if (m_force_d[1] < 5.0) m_force_d[1] += 1.0;
			}
			else if (key == 'q' || key == 'Q') m_f_state[0] = m_f_state[1] = 0;
			else if (key == 'w' || key == 'W') m_f_state[0] = 1;
			else if (key == 'e' || key == 'E') m_f_state[0] = 2;
			else if (key == 'r' || key == 'R') m_f_state[1] = 1;
			else if (key == 't' || key == 'T') m_f_state[1] = 2;
			else if (key == 13) {
				m_textBuf_len = 0;
				m_txtBuf[m_textBuf_len] = NULL;
				m_disp_train_sphere = false;
				m_select_sphere_a = false;
				m_select_sphere_b = false;
				m_select_sphere_c = false;
				m_sphere_move = false;
				moveToNextPhase();
			}
		}
		else if (m_exp_phase == EXP_PHASE::INFO_INPUT) {
			if (key >= '0' && key <= 'z') {// ASCII 48 ~ 122
				m_txtBuf[m_textBuf_len++] = key;
				m_txtBuf[m_textBuf_len] = NULL;
			}
			else if (key == 8) { // backspace
				if (m_textBuf_len > 0) {
					m_txtBuf[--m_textBuf_len] = NULL;
				}
			}
			else if (key == 13) {
				if (m_textBuf_len == 0) ret = -2;
				else {
					memcpy((void*)m_subjectID, (void*)m_txtBuf, sizeof(char)*m_textBuf_len);
					m_subjectID[m_textBuf_len] = NULL;
					if (strcmp(m_txtBuf, "0000") == 0) {
						printf("test subject %s\n", m_txtBuf);
						m_testSubject = true;
					}
					else {
						char directory_path[128];
						bool directory_check = true;
						sprintf(directory_path, "./z_%s", m_subjectID);
						if (!check_directory(directory_path)) directory_check = false;
						if (!directory_check) {
							MessageBox(NULL, "Failed to locate the logging folder", "Error", MB_OK | MB_ICONERROR);
							m_exp_phase = EXP_PHASE::QUIT_EXP;
							return 100;
						}
						else {
							sprintf(m_rec_filename, "%s/%s.txt", directory_path, m_subjectID);//m_exp_obj_size[0]));	// subject_ID exp_type reference_weight.txt
						}
					}
					for (int i = 0; i < m_textBuf_len; i++)m_txtBuf[i] = NULL;
					m_textBuf_len = 0;
					ret = moveToNextPhase(ret_string);
				}
			}
			else {
				ret = 102;
				sprintf(ret_string, "Type a valid character (0~z)");
			}
		}
		else if (m_exp_phase == EXP_PHASE::EXP_PHASE1) {
			if (key == ' ') moveToNextPhase();
		}
		else if (m_exp_phase == EXP_PHASE::EXP_PHASE2) {
			if (key == 'a' || key == 'b') {
				m_txtBuf[m_textBuf_len++] = key;
				m_txtBuf[m_textBuf_len] = NULL;
				if (key == 'a') {
					m_select_sphere_a = true;
					m_select_sphere_b = false;
				}
				else if (key == 'b') {
					m_select_sphere_a = false;
					m_select_sphere_b = true;
				}
			}
			if (key == ' ') {
				for (int i = 0; i < m_textBuf_len; i++)m_txtBuf[i] = NULL;
				m_textBuf_len = 0;
				m_sphere_move = true;
			}
			else if (key == 13) moveToNextPhase();
		}
		else if (m_exp_phase == EXP_PHASE::EXP_PHASE3) {
			if (key == ' ') {
				m_exp_phase = EXP_PHASE::EXP_PHASE2;
				m_disp_f_bar = false;
				if (m_trial_joint[m_curr_trial_no - 1] == 0){
					m_uwnd_dur = 0.8 + (0.4 / (m_max_f_d - m_min_f_d))*(m_force_d[0] - m_min_f_d);	///
				}
				else {
					m_uwnd_dur = 1.3 + (0.3 / (m_max_f_d - m_min_f_d))*(m_force_d[1] - m_min_f_d);	///
				}
				m_pTimer->setLimit(m_uwnd_dur);// (0.7)
				enableForceCtrl(false, true, m_trial_joint[m_curr_trial_no-1]);
				for (i = 0; i < 2; i++) {
					m_f_state[i] = 0;
					m_force_d[i] = m_min_f_d;
				}
				//if (m_trial_joint[m_curr_trial_no - 1] == 0) {
				//	windTendon(0, 1);
				//}
				//else {
				//	windTendon(1, 1);
				//}
				m_prev_pt_force = m_trial_pt_force[m_curr_trial_no - 1];				
			}
			else if (key == 'a' || key == 'b') {
				m_txtBuf[m_textBuf_len++] = key;
				m_txtBuf[m_textBuf_len] = NULL;
			}
			///save answer 수정 필요
			else if (key == 13) {
				for (int i = 0; i < m_textBuf_len; i++)m_txtBuf[i] = NULL;
				m_textBuf_len = 0;
				moveToNextPhase();
			}
		}
		//else if (m_exp_phase == EXP_PHASE::PHANTOM_INIT) {
		//	if(key)
		//}
	}
	return ret;
}

void cExp_Baik_Hand_PSE::handleSpecialKeys(int key, int x, int y)
{
	switch (key) {
	//case GLUT_KEY_LEFT:
	//	if (m_exp_phase == EXP_PHASE::INIT) {
	//		if (m_val_bar > 0.0) m_val_bar -= 0.2;
	//	}
	//	break;
	//case GLUT_KEY_RIGHT:
	//	if (m_exp_phase == EXP_PHASE::INIT) {
	//		if (m_val_bar < 1.0) m_val_bar += 0.2;
	//	}
	//	break;
	case GLUT_KEY_F3:
		if (!m_show_dbg_info) {
			m_show_dbg_info = true;
		}
		else m_show_dbg_info = false;
		break;
	/// <summary>
	/// servo motor control test
	/// </summary>
	case GLUT_KEY_F4:
		SP->WriteData("2", 255);
		break;
	case GLUT_KEY_F5:
		SP->WriteData("3", 255);
		break;
	case GLUT_KEY_F6:
		SP->WriteData("4", 255);
		break;
	case GLUT_KEY_F7:
		SP->WriteData("5", 255);
		Sleep(100);
		break;
	case GLUT_KEY_UP:
		printf("up\n");
		if (m_exp_phase == EXP_PHASE::DEVICE_INIT) {
			if (m_pwm_val < 250) m_pwm_val += 50;
		}
		else if (m_exp_phase == EXP_PHASE::EXP_PHASE3) {
			if (m_trial_joint[m_curr_trial_no - 1] == 0) {
				if(m_force_d[0] < m_max_f_d) m_force_d[0] += 0.5;
			//	m_val_bar = 0.2*m_force_d[0];
			}
			else {
				if (m_force_d[1] < m_max_f_d) m_force_d[1] += 0.5;
			//	m_val_bar = 0.2*m_force_d[1];
			}
		}
		break;
	case GLUT_KEY_DOWN:
		if (m_exp_phase == EXP_PHASE::DEVICE_INIT) {
			if (m_pwm_val > 0) m_pwm_val -= 50;
		}
		//else if (m_exp_phase == EXP_PHASE::EXP_PHASE3) {
		//	if (m_trial_joint[m_curr_trial_no - 1] == 0) {
		//		if (m_force_d[0] > m_min_f_d) {
		//			m_f_state[0] = 3;
		//			m_force_d[0] -= 0.5;
		//		}
		//	//	m_val_bar = 0.2*m_force_d[0];
		//	}
		//	else {
		//		if (m_force_d[1] > m_min_f_d) {
		//			m_f_state[1] = 3;
		//			m_force_d[1] -= 0.5;
		//		}
		//	//	m_val_bar = 0.2*m_force_d[1];
		//	}
		//}
		break;
	/*case GLUT_KEY_F4:
		if (m_enableFctrl) m_enableFctrl = false;
		else m_enableFctrl = true;
		break;
	case GLUT_KEY_F6:
		if (!m_disp_f_bar) m_disp_f_bar = true;
		else m_disp_f_bar = false;
		break;*/
	}
}

int cExp_Baik_Hand_PSE::moveToNextPhase(char *ret_string)
{
	int ret = 0, i;
	if (m_exp_phase == EXP_PHASE::INIT) {
		m_exp_phase = EXP_PHASE::DEVICE_INIT;
		m_disp_coeff = true;
	}
	else if (m_exp_phase == EXP_PHASE::DEVICE_INIT) {
		m_exp_phase = EXP_PHASE::FORCE_TEST;
		//m_enableFctrl = true;
		enableForceCtrl(true, false, 0);
		m_disp_coeff = false;
		//m_disp_f_coeff = true;
		m_disp_train_sphere = true;
	}
	else if (m_exp_phase == EXP_PHASE::FORCE_TEST) {
		m_exp_phase = EXP_PHASE::INFO_INPUT;
		// set output force to zero
		m_f_state[0] = m_f_state[1] = 0;// 2;
		m_disp_f_coeff = false;
		m_prev_pt_force = 0.0;
		m_pwm_val = 255;
		enableForceCtrl(false, true, -1);
	}
	else if (m_exp_phase == EXP_PHASE::INFO_INPUT) {
		m_curr_trial_no = 1;
		time(&m_expBeginTime);
		time(&m_trialBeginTime);
		setAudioPhase(AUDIO_PHASE::PLAY);
		for (i = 0; i < 2; i++) {
			m_f_state[i] = 0;// 2;
			m_force_d[i] = m_min_f_d;
		}
	//	m_val_bar = 0.2*0.5;
		m_uwnd_dur = 1.0;
		m_pTimer->setLimit(m_uwnd_dur);// (0.7);
		m_disp_f_bar = false;
	//	m_enableFctrl = false;
		///

		time(&m_expBeginTime);
		time(&m_trialBeginTime);
		recordResult(RECORD_TYPE::REC_INIT);
		m_exp_phase = EXP_PHASE::EXP_PHASE1;
	}
	else if (m_exp_phase == EXP_PHASE::EXP_PHASE1) {
		m_prev_pt_force = m_trial_pt_force[m_curr_trial_no-1];
		m_disp_test_sphere = true;
		m_exp_phase = EXP_PHASE::EXP_PHASE2;
	}
	else if (m_exp_phase == EXP_PHASE::EXP_PHASE2) {
		m_prev_pt_force = 0.0;
		if (m_trial_joint[m_curr_trial_no-1] == 0) {
			m_f_state[0] = 1;
			m_f_state[1] = 0;
		}
		else {
			m_f_state[0] = 0;
			m_f_state[1] = 1;
		}
		//m_disp_f_bar = true;
		//enableForceCtrl(true, false, 0);

		m_disp_test_sphere = false;
		m_exp_phase = EXP_PHASE::EXP_PHASE3;
	}
	else if(m_exp_phase == EXP_PHASE::EXP_PHASE3) {
		//// recording
		m_prev_pt_force = 0.0;
		if (m_trial_joint[m_curr_trial_no - 1] == 0) m_trial_force_d[m_curr_trial_no-1] = m_force_d[0];
		else m_trial_force_d[m_curr_trial_no - 1] = m_force_d[1];
		if (m_trial_joint[m_curr_trial_no - 1] == 0) {
			m_uwnd_dur = 0.8 + (0.4 / (m_max_f_d - m_min_f_d))*(m_force_d[0] - m_min_f_d);	///
		}
		else {
			m_uwnd_dur = 1.3 + (0.3 / (m_max_f_d - m_min_f_d))*(m_force_d[1] - m_min_f_d);	///
		}
		//m_uwnd_dur = 0.8 + (0.4 / (m_max_f_d - m_min_f_d))*(m_trial_force_d[m_curr_trial_no - 1] - m_min_f_d);	///
		m_pTimer->setLimit(m_uwnd_dur);// (0.7)
		for (i = 0; i < 2; i++) {
			m_f_state[i] = 0;	// 
			m_force_d[i] = m_min_f_d;
		}
		if(m_curr_trial_no <= 3) enableForceCtrl(false, true, 0);
		else enableForceCtrl(false, true, 1);
		//if (m_trial_joint[m_curr_trial_no - 1] == 0) {
		//	windTendon(0, 1);
		//}
		//else {
		//	windTendon(1, 1);
		//}
	//	m_val_bar = 0.2*0.5;
		recordResult(RECORD_TYPE::REC_TRIAL);
		m_disp_f_bar = false;
		if (m_curr_trial_no == m_tot_trial) {
			// record
			setAudioPhase(AUDIO_PHASE::COMPLETE);
			recordResult(RECORD_TYPE::REC_END);
			m_exp_phase = EXP_PHASE::EXP_DONE;
		}
		else {
			m_curr_trial_no++;
			time(&m_trialBeginTime);
			m_exp_phase = EXP_PHASE::EXP_PHASE1;
		}
	}
	return ret;
}

void cExp_Baik_Hand_PSE::dataAnalysis()
{
}

void cExp_Baik_Hand_PSE::recordResult(int type)
{
	FILE *pFile;
	time_t curr_time, tot_time;
	tm time_tm, time_tm2;
	errno_t err;
	if (type == RECORD_TYPE::REC_INIT) {
		err = _localtime64_s(&time_tm, &m_expBeginTime);
		printf("Subject: %s\n", m_subjectID);
		printf("Experiment began at %02d:%02d:%02d on %04d/%02d/%02d\n", time_tm.tm_hour, time_tm.tm_min, time_tm.tm_sec,
			time_tm.tm_year + 1900, time_tm.tm_mon + 1, time_tm.tm_mday);
		printf("-------------------------------------------------------------------------\n");
		printf("Trial no.\ttrial time (mm:ss)\tjoint_type (PIP/MCP)\tReference force (N)\tPerceived force (N)\n");
		printf("-------------------------------------------------------------------------\n");
		pFile = fopen(m_rec_filename, "a");
		if (pFile != NULL && !m_testSubject) {
			fprintf(pFile, "Subject: %s\n", m_subjectID);
			fprintf(pFile, "Experiment began at %02d:%02d:%02d on %04d/%02d/%02d\n", time_tm.tm_hour, time_tm.tm_min, time_tm.tm_sec,
				time_tm.tm_year + 1900, time_tm.tm_mon + 1, time_tm.tm_mday);
			fprintf(pFile, "-------------------------------------------------------------------------\n");
			fprintf(pFile, "Trial no.\ttrial time (mm:ss)\tjoint_type (PIP/MCP)\tReference force (N)\tPerceived force (N)\n");
			fprintf(pFile, "-------------------------------------------------------------------------\n");
			fclose(pFile);
		}
	}
	else if (type == RECORD_TYPE::REC_TRIAL) {
	//	exp_result last_result = m_expResult.back();
		time(&curr_time);
		tot_time = difftime(curr_time, m_trialBeginTime);
		err = _localtime64_s(&time_tm, &tot_time);
		printf("%d\t%02d:%02d\t%s\t%.1f\t%.1f\n", m_curr_trial_no, time_tm.tm_min, time_tm.tm_sec, (m_trial_joint [m_curr_trial_no-1]==0)?"PIP":"MCP", m_trial_pt_force[m_curr_trial_no-1], m_trial_force_d[m_curr_trial_no-1]);

		pFile = fopen(m_rec_filename, "a");
		if (pFile != NULL && !m_testSubject) {
			fprintf(pFile, "%d\t%02d:%02d\t%s\t%.1f\t%.1f\n", m_curr_trial_no, time_tm.tm_min, time_tm.tm_sec, (m_trial_joint[m_curr_trial_no - 1] == 0) ? "PIP" : "MCP", m_trial_pt_force[m_curr_trial_no - 1], m_trial_force_d[m_curr_trial_no - 1]);
			fclose(pFile);
		}
	}
	else if (type == RECORD_TYPE::REC_END) {
		time(&curr_time);
		err = _localtime64_s(&time_tm, &curr_time);
		tot_time = difftime(curr_time, m_expBeginTime);
		err = _localtime64_s(&time_tm2, &tot_time);
		/////
		printf("===================================\n");
		printf("Experiment ended at %02d:%02d:%02d on %04d/%02d/%02d\n", time_tm.tm_hour,
			time_tm.tm_min, time_tm.tm_sec, time_tm.tm_year + 1900, time_tm.tm_mon + 1, time_tm.tm_mday);
		printf("Total experiment time: %02d:%02d\n", time_tm2.tm_min, time_tm2.tm_sec);
		printf("===================================\n");
		pFile = fopen(m_rec_filename, "a");
		if (pFile != NULL && !m_testSubject) {
			fprintf(pFile, "===================================\n");
			fprintf(pFile, "Experiment ended at %02d:%02d:%02d on %04d/%02d/%02d\n", time_tm.tm_hour,
				time_tm.tm_min, time_tm.tm_sec, time_tm.tm_year + 1900, time_tm.tm_mon + 1, time_tm.tm_mday);
			fprintf(pFile, "Total experiment time: %02d:%02d\n", time_tm2.tm_min, time_tm2.tm_sec);
			fprintf(pFile, "===================================\n");
			fclose(pFile);
		}
	}
}

int cExp_Baik_Hand_PSE::getCurrInstructionText(char pDestTxt[3][128])
{
	int ret = 0, len[4], i;
	for (i = 0; i < 4; i++) len[i] = strlen(m_instruction_msg[m_exp_phase][i]);
	if (len[0] == 0 && len[1] == 0) ret = -1;
	else {
		/// msg 1
		if (m_exp_phase == EXP_PHASE::INFO_INPUT) {
			sprintf_s(pDestTxt[0], "%s %s", m_instruction_msg[m_exp_phase][0], m_txtBuf);
		}
		else if (m_exp_phase == EXP_PHASE::EXP_PHASE2)
		{
			sprintf_s(pDestTxt[0], "%s %s", m_instruction_msg[m_exp_phase][0], m_txtBuf);
		}
		else if (m_exp_phase == EXP_PHASE::EXP_PHASE3)
		{
			sprintf_s(pDestTxt[0], "%s %s", m_instruction_msg[m_exp_phase][0], m_txtBuf);
		}
		else if (m_exp_phase == EXP_PHASE::FORCE_TEST) {
			sprintf_s(pDestTxt[0], "%s %s", m_instruction_msg[m_exp_phase][0], m_txtBuf);
			//sprintf_s(pDestTxt[0], "%s %s", m_instruction_msg[m_exp_phase][1], m_txtBuf);
		}
		else if (m_exp_phase >= EXP_PHASE::EXP_PHASE1 && m_exp_phase <= EXP_PHASE::EXP_PHASE3) {
			sprintf_s(pDestTxt[0], "%s %d", m_instruction_msg[m_exp_phase][0], m_curr_trial_no);
		}
		else {
			strcpy_s(pDestTxt[0], m_instruction_msg[m_exp_phase][0]);
			pDestTxt[0][len[0]] = NULL;
		}
		/// msg 2
		strcpy_s(pDestTxt[1], m_instruction_msg[m_exp_phase][1]);
		pDestTxt[1][len[1]] = NULL;
		/// msg 3
		strcpy_s(pDestTxt[2], m_instruction_msg[m_exp_phase][2]);
		pDestTxt[2][len[2]] = NULL;
		/// msg 4
		strcpy_s(pDestTxt[3], m_instruction_msg[m_exp_phase][3]);
		pDestTxt[3][len[3]] = NULL;
	}
	return ret;
}

void cExp_Baik_Hand_PSE::init()
{
	/// display related routines
	DISP_TOOLS::setProjectionState(true);
	DISP_TOOLS::setCameraVariables(0, glm::vec3(0, m_view_height, 0.3));
	DISP_TOOLS::setCameraVariables(1, glm::vec3(0, m_view_height, 0));
	glClearColor(1, 1, 1, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	////826 DAQ board initialization

	/// analog input
//	m_p826->setAin_ch(0, 0);	//
//	m_p826->setAin_ch(1, 1);	//
//	m_p826->setAin_ch(2, 2);	//
	//m_p826->setAin_ch(3, 3);	//
	//m_p826->setAin_ch(4, 4);	//
	//m_p826->setAin_ch(5, 5);	//

//	m_pTimer->setTimeout(500);111
	////
// timer related routine
	m_pTimer = new cTimer_exp(50);	// 50ms update (20Hz)
	m_pTimer->setLimit(0.3);
	m_pTimer->setExpPointer(this);
	m_pTimer->startTicking();
	sendArd(0, 0);
	/// digital output	// only two channels are used for the experiment
	//m_p826->setDIOparams(0, )
	//printf("Sensoray 826 initialization: %d\n", ret);
	_beginthreadex(NULL, 0, s826_ReadThread, (void*)this, 0, NULL);
	///// PHANToM initialization routine
	
///////
	/////audio initialization
	//_beginthreadex(NULL, 0, audioThread, (void*)this, 0, NULL);
	//// force control initialization
	_beginthreadex(NULL, 0, forceCtrlThread, (void*)this, 0, NULL);
}

void cExp_Baik_Hand_PSE::sub_display()
{
	char pTxt[4][128];
	char pTxt2[2][32];
	char txtBuf[64];
	//char txtBuf[128];
	float val_bar;
	double curr_f_d;
	int i;
	float bar_width = 0.12;
	DISP_TOOLS::setupGraphicsState();
	if (0 != getCurrInstructionText(pTxt)) {
		for (i = 0; i < 4; i++) pTxt[i][0] = NULL;
	}
	glColor3f(0, 0, 0);
	DISP_TOOLS::Draw_Text(pTxt[0], -0.15f, m_view_height + 0.08f, 0.f);	//	DISP_TOOLS::Draw_Text(pTxt[0], -7.2f, 3.8f, 0.f);
	DISP_TOOLS::Draw_Text(pTxt[1], -0.15f, m_view_height + 0.07f, 0.f);	// 	DISP_TOOLS::Draw_Text(pTxt[1], -7.2f, 3.2f, 0.f);
	DISP_TOOLS::Draw_Text(pTxt[2], -0.15f, m_view_height + 0.06f, 0.f);	// 	DISP_TOOLS::Draw_Text(pTxt[2], -7.2f, 2.6f, 0.f);
	DISP_TOOLS::Draw_Text(pTxt[3], -0.15F, m_view_height + 0.05f, 0.f);

	/*if (m_disp_coeff) {
		sprintf(pTxt[0], "pos : %.3f mm (%.3f V), %.3f mm (%.3f V), %.3f mm (%.3f V), %.3f mm (%.3f V)", m_poten_pos[0], m_dbg_dVal[0],
			m_poten_pos[1], m_dbg_dVal[1], m_poten_pos[2], m_dbg_dVal[2], m_poten_pos[3], m_dbg_dVal[3]);
		sprintf(pTxt[1], "force : %.3f N (PIP) %.3f N (DIP)", m_prev_force[0], m_prev_force[1]);
		sprintf(pTxt[2], "PWM: %d", m_pwm_val);
		DISP_TOOLS::Draw_Text(pTxt[0], -0.15f, m_view_height - 0.06f, 0.f);
		DISP_TOOLS::Draw_Text(pTxt[1], -0.15f, m_view_height - 0.07f, 0.f);
		DISP_TOOLS::Draw_Text(pTxt[2], -0.15, m_view_height-0.08f, 0.f);

	}*/

	if (m_disp_test_sphere) {
		glColor3f(1, 1, 0);

		if (m_select_sphere_b) {
			DISP_TOOLS::DrawSphere(glm::vec3(0.08, -0.05, m_sphere_z_pos), 0.01);
			if (m_bending_sensor_input) {
				glColor3f(1, 0, 0);
				DISP_TOOLS::DrawSphere(glm::vec3(0.08, -0.05, m_sphere_z_pos), 0.01);
			}
		}
		else {
			DISP_TOOLS::DrawSphere(glm::vec3(0.08, -0.05, 0), 0.01);

		}
		glColor3f(1, 1, 0);
		if (m_select_sphere_a) {
			DISP_TOOLS::DrawSphere(glm::vec3(-0.08, -0.05, m_sphere_z_pos), 0.01);
			if (m_bending_sensor_input) {
				glColor3f(1, 0, 0);
				DISP_TOOLS::DrawSphere(glm::vec3(-0.08, -0.05, m_sphere_z_pos), 0.01);
			}
		}
		else {
			DISP_TOOLS::DrawSphere(glm::vec3(-0.08, -0.05, 0), 0.01);
		}
		glutPostRedisplay();

		glColor3f(0, 0, 0);
		sprintf(pTxt[0], "a");
		sprintf(pTxt[1], "b");
		DISP_TOOLS::Draw_Text(pTxt[0], -0.08f, m_view_height - 0.08f, 0.f);
		DISP_TOOLS::Draw_Text(pTxt[1], 0.08, m_view_height - 0.08f, 0.f);
	}

	if (m_disp_train_sphere) {
		//DISP_TOOLS::DrawAxes(10, 3);
		glColor3f(1, 1, 0);
		
		if (m_select_sphere_c) {
			DISP_TOOLS::DrawSphere(glm::vec3(0.08, -0.05, m_sphere_z_pos), 0.01);
			if (m_bending_sensor_input) {
				glColor3f(1, 0, 0);
				DISP_TOOLS::DrawSphere(glm::vec3(0.08, -0.05, m_sphere_z_pos), 0.01);
			}
		}
		else {
			DISP_TOOLS::DrawSphere(glm::vec3(0.08, -0.05, 0), 0.01);

		}
		glColor3f(1, 1, 0);
		if (m_select_sphere_b) {
			DISP_TOOLS::DrawSphere(glm::vec3(0, -0.05, m_sphere_z_pos), 0.01);
			if (m_bending_sensor_input) {
				glColor3f(1, 0, 0);
				DISP_TOOLS::DrawSphere(glm::vec3(0, -0.05, m_sphere_z_pos), 0.01);
			}
		}
		else {
			DISP_TOOLS::DrawSphere(glm::vec3(0, -0.05, 0), 0.01);
		}
		glColor3f(1, 1, 0);
		if (m_select_sphere_a) {
			DISP_TOOLS::DrawSphere(glm::vec3(-0.08, -0.05, m_sphere_z_pos), 0.01);
			if (m_bending_sensor_input) {
				glColor3f(1, 0, 0);
				DISP_TOOLS::DrawSphere(glm::vec3(-0.08, -0.05, m_sphere_z_pos), 0.01);
			}
		}
		else {
			DISP_TOOLS::DrawSphere(glm::vec3(-0.08, -0.05, 0), 0.01);
		}
		glutPostRedisplay();
		//(m_select_sphere_b) ? DISP_TOOLS::DrawSphere(glm::vec3(0, -0.05, m_sphere_z_pos), 0.01) : DISP_TOOLS::DrawSphere(glm::vec3(0, -0.05, 0), 0.01);
		
		//(m_select_sphere_c) ? DISP_TOOLS::DrawSphere(glm::vec3(-0.07, -0.05, m_sphere_z_pos), 0.01) : DISP_TOOLS::DrawSphere(glm::vec3(-0.07, -0.05, 0), 0.01);


		/// <summary>
		/// display weights
		/// </summary>
		glColor3f(0, 0, 0);
		(m_select_sphere_a && m_sphere_move) ? sprintf(pTxt[0], "a.100g selected") : sprintf(pTxt[0], "a.100g");
		(m_select_sphere_b && m_sphere_move) ? sprintf(pTxt[1], "b.300g selected") : sprintf(pTxt[1], "b.300g");
		(m_select_sphere_c && m_sphere_move) ? sprintf(pTxt[2], "c.500g selected") : sprintf(pTxt[2], "c.500g");
		DISP_TOOLS::Draw_Text(pTxt[0], -0.09f, m_view_height - 0.08f, 0.f);
		DISP_TOOLS::Draw_Text(pTxt[1],-0.010, m_view_height - 0.08f, 0.f);
		DISP_TOOLS::Draw_Text(pTxt[2], 0.07, m_view_height - 0.08f, 0.f);
		
	}
	if (m_disp_f_coeff) {
		sprintf(pTxt[0], "PHANToM force: %.2f N", m_prev_pt_force);
		//////////////////
		sprintf(pTxt[1], "force enable: %s", m_enableFctrl ? "true" : "false");
		for (i = 0; i < 2; i++) {
			switch (m_f_state[i]) {
			case 1:
				sprintf(pTxt2[i], "force exerted");
				break;
			case 2:
				sprintf(pTxt2[i], "negative force");
				break;
			default:
				sprintf(pTxt2[i], "no force");
				break;
			}
		}
		sprintf(pTxt[1], "state: %s, %s", pTxt2[0], pTxt2[1]);
		sprintf(pTxt[2], "F_d: %.3f N, %.3f N", m_force_d[0], m_force_d[1]);
		DISP_TOOLS::Draw_Text(pTxt[0], 0.08, m_view_height - 0.07f, 0.f);
		DISP_TOOLS::Draw_Text(pTxt[1], 0.08, m_view_height - 0.08f, 0.f);
		DISP_TOOLS::Draw_Text(pTxt[2], 0.08, m_view_height - 0.09f, 0.f);
	}
	if (m_disp_f_bar) {
		if (m_trial_joint[m_curr_trial_no - 1] == 0) curr_f_d = m_force_d[0];
		else curr_f_d = m_force_d[1];
		val_bar = (float)((curr_f_d-m_min_f_d)/(m_max_f_d-m_min_f_d));

		glBegin(GL_QUADS);
			glColor3f(1, 0, 0);
			glVertex3f(-0.5*bar_width, m_view_height - 0.03f, 0);
			glColor3f(1, 0, 0);
			glVertex3f(-0.5*bar_width+bar_width*val_bar, m_view_height - 0.03f, 0);
			glColor3f(1, 0, 0);
			glVertex3f(-0.5*bar_width + bar_width * val_bar, m_view_height - 0.01f, 0);
			glColor3f(1, 0, 0);
			glVertex3f(-0.5*bar_width, m_view_height - 0.01f, 0);
			glColor3f(1, 0, 0);
		glEnd();
		//////////////////////////////////////////////////////////////////////////////////////////////
		//sphere rendering "hit 'F6'
		glColor3f(1, 1, 0);
		DISP_TOOLS::DrawSphere(glm::vec3(0, 0, 0), 0.01);
		DISP_TOOLS::DrawAxes(10, 3);
		/// <summary>
		/// /////////////////////////////////////////////////////////////////////////////////////////
		/// </summary>
		
		glLineWidth(1.5);
		glColor3f(0, 0, 0);
		glBegin(GL_LINES);
		glVertex3f(-0.5*bar_width, m_view_height - 0.03f, 0);
		glVertex3f(0.5*bar_width, m_view_height - 0.03f, 0);
		for (i = 0; i <= 4; i++) {
			glVertex3f(-0.5*bar_width + 0.25*bar_width*(float)i, m_view_height - 0.03f, 0);
			glVertex3f(-0.5*bar_width + 0.25*bar_width*(float)i, m_view_height - 0.025f, 0);
		}
		glEnd();
		sprintf(txtBuf, "min");
		DISP_TOOLS::Draw_Text((char*)"min", -0.5*bar_width-0.005, m_view_height - 0.038f, 0.0);
		DISP_TOOLS::Draw_Text((char*)"max", 0.5*bar_width-0.005, m_view_height - 0.038f, 0.0);
	}

}

void cExp_Baik_Hand_PSE::setAudioPhase(int audio_phase)
{
	m_audio_phase = audio_phase;
	SetEvent(m_hAudioEvent);
}

void cExp_Baik_Hand_PSE::windTendon(int idx, int dir)
{
	int input_val;
	if (m_ard_status == 0) {
		m_ard_status = 1;
		if (dir == 0) input_val = m_pwm_val;// 100;
		else input_val = -m_pwm_val;// 100;
		if (idx == 0) sendArd(input_val, 0);
		else sendArd(0, input_val);
		m_pTimer->playTick();
	}
}

void cExp_Baik_Hand_PSE::updateArd(int cmd)
{
	char buf[64];
	memset(buf, 0x00, sizeof(buf));
	if (cmd == 0) {
		m_ard_status = 0;
		sendArd(0, 0);
	}
}

void cExp_Baik_Hand_PSE::PhantomCallbackSubFunc()
{
	
}

int cExp_Baik_Hand_PSE::updateAdc()
{
	return 0;
}

void cExp_Baik_Hand_PSE::enableForceCtrl(bool enable, bool opt, int jnt)
{
	uint i;
	if (enable) {
		m_enableFctrl = true;
		SetEvent(m_hForceEnable);
	}
	else {
		m_fc_opt = opt;
		m_uwnd_jnt = jnt;
		m_enableFctrl = false;
	}
}
void cExp_Baik_Hand_PSE::trialUnwind()
{
//	if (m_trial_joint[m_curr_trial_no - 1] == 0) {
	if(m_uwnd_jnt == 0) {
		windTendon(0, 1);
	}
	else if(m_uwnd_jnt == 1){
		windTendon(1, 1);
	}
}

unsigned __stdcall forceCtrlThread(void* arg)
{
	uint tstamp = 0, i;
	bool first_time = true;
	double curr_time, prev_time, dt;
	double update_freq = 50.0;
	double f_e[2] = { 0.0, 0.0 };	// error force
	double f_r[2], f_d[2];
	int t_pwm[2] = { 0, 0 };
	uint period = (uint)((double)1000000.0 / update_freq);	 // 20ms
	cExp_Baik_Hand_PSE *pExp = (cExp_Baik_Hand_PSE*)arg;
	do {
		
		while (pExp->m_enableFctrl) {
			
			for (i = 0; i < 2; i++) {
				f_r[i] = pExp->m_prev_force[i];
				f_d[i] = pExp->m_force_d[i];
				f_e[i] = f_d[i]-f_r[i];
				if (pExp->m_f_state[i] == 1) {
				//	printf("dbg_1\n");
				//	t_pwm[i] = (int)(51.0*f_d[i]);// (int)(51.0*f_e[i]);		// assuming open-loop
					t_pwm[i] = (int)(51.0*f_d[i]);// (int)(51.0*f_e[i]);		// assuming open-loop
					if (t_pwm[i] > 255) t_pwm[i] = 255;
					else if (t_pwm[i] < -255) t_pwm[i] = -255;
				}
				else if (pExp->m_f_state[i] == 2) {	// negative force
				//	printf("dbg_2\n");
					// state change code
					if (f_r[i] < 0.1) {
						pExp->m_f_state[i] = 0;
						t_pwm[i] = 0;
						printf("dbg_2-1\n");
					}
					else {
					//	printf("dbg_2-2 f_r=%f\n", f_r[i]);
						t_pwm[i] = -250;
					}
				}
				else if (pExp->m_f_state[i] == 3) { // decrease routine
				//	printf("dbg_3\n");
					if (f_r[i] < 0.1) {
						pExp->m_f_state[i] = 1;
						t_pwm[i] = (int)(51.0*f_d[i]);// (int)(51.0*f_e[i]);		// assuming open-loop
						if (t_pwm[i] > 255) t_pwm[i] = 255;
						else if (t_pwm[i] < -255) t_pwm[i] = -255;
					//	printf("dbg_3-1\n");
					}
					else {
					//	printf("dbg_3-2\n");
						t_pwm[i] = -250;
					}

				}
				else {
					t_pwm[i] = 0;
				}

			}
			curr_time = (double)tstamp / 1000000.0;
			if (first_time) {
				dt = 0;
				first_time = false;
			}
			else dt = curr_time - prev_time;
			/////
		//	printf("pwm_out: %d %d\n", t_pwm[0], t_pwm[1]);
			pExp->sendArd(t_pwm[0], t_pwm[1]);
			prev_time = curr_time;
		}
	
		WaitForSingleObject(pExp->m_hForceEnable, INFINITE);
		printf("force enabled\n");
	} while (pExp->m_exp_phase != EXP_PHASE::QUIT_EXP);
	pExp->sendArd(0, 0);
	return 0;
}


unsigned __stdcall s826_ReadThread(void *arg)
{
	cExp_Baik_Hand_PSE *pExp = (cExp_Baik_Hand_PSE*)arg;
	do {
		pExp->updateAdc();
	} while (pExp->m_exp_phase != EXP_PHASE::QUIT_EXP);
	return 0;

}

unsigned __stdcall audioThread(void* arg)
{
	cExp_Baik_Hand_PSE *pExp = (cExp_Baik_Hand_PSE*)arg;
	cMCI_sound fanfare("Fanfare.wav"), noise("WhiteNoise_15min.mp3");
	int curr_audio_phase, prev_audio_phase;
	prev_audio_phase = AUDIO_PHASE::AUDIO_INIT;
	curr_audio_phase = pExp->m_audio_phase;
	do {
		curr_audio_phase = pExp->m_audio_phase;
		if (prev_audio_phase != curr_audio_phase) {
			switch (curr_audio_phase) {
			case AUDIO_PHASE::PLAY:
				noise.play(15000);
				break;
			case AUDIO_PHASE::PAUSE:
				noise.pause();
				break;
			case AUDIO_PHASE::STOP:
				noise.stop();
				break;
			case AUDIO_PHASE::COMPLETE:
				noise.stop();
				fanfare.play();
				break;
			}
			prev_audio_phase = curr_audio_phase;
			WaitForSingleObject(pExp->m_hAudioEvent, INFINITE);
		}
	} while (pExp->m_exp_phase != EXP_PHASE::QUIT_EXP);
	return 0;
}


void cExp_Baik_Hand_PSE::sendArd(int val1, int val2)
{
	char buf[64], buf2[2][5];
	int i;
	memset(buf, 0x00, 10);
	for (i = 0; i < 2; i++) memset(buf2[i], 0x00, 5);
	if (val1 > 0) buf2[0][0] = '1';
	else if(val1 < 0) buf2[0][0] = '2';
	else buf2[0][0] = '0';
	if (abs(val1) <= 255)
		sprintf(buf2[0] + 1, "%03d", abs(val1));
	else sprintf(buf2[0] + 1, "000");
	if (val2 > 0) buf2[1][0] = '1';
	else if(val2 < 0) buf2[1][0] = '2';
	else buf2[1][0] = '0';
	if (abs(val2) <= 255)
		sprintf(buf2[1] + 1, "%03d", abs(val2));
	else sprintf(buf2[1] + 1, "000");
	memcpy(buf, buf2[0], 4);
	memcpy(buf + 4, buf2[1], 4);
	buf[8] = '\n';
	
//	m_pSerial->Send();
}

