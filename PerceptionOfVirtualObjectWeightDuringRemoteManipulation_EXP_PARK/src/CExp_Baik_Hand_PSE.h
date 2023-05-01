#pragma once
#include <windows.h>
#include "CAdaptive_PSE.h"
#include <process.h>
#include <glm\glm.hpp>
#include "Serial.h"


enum EXP_PHASE {
	INIT = 0,		// initialize Baik hand winding
	DEVICE_INIT,	// tenson initialization
	FORCE_TEST,		// test the force exertion prior to the measurement
//	PHANTOM_INIT,
	INFO_INPUT,		// subject ID input
	EXP_PHASE1,
	EXP_PHASE2,
	EXP_PHASE3,
//	TRAINING,
	EXP_DONE,
	QUIT_EXP
};

enum AUDIO_PHASE {
	AUDIO_INIT = 0,
	PLAY,
	PAUSE,
	STOP,
	COMPLETE
};

enum RECORD_TYPE {
	REC_INIT = 0,
	REC_TRIAL,
	REC_END
};

typedef unsigned int uint;

class cTimer_exp;
class cExp_Baik_Hand_PSE : public cAdaptive_PSE
{
public:
	//-----------------------------------------------------------------------
	// CONSTRUCTOR & DESTRUCTOR:
	//-----------------------------------------------------------------------
	cExp_Baik_Hand_PSE();
	~cExp_Baik_Hand_PSE();
	//-----------------------------------------------------------------------
	// METHODS:
	//-----------------------------------------------------------------------
	virtual int handleKeyboard(unsigned char key, char* ret_string);	// pure virtual functions
	virtual int moveToNextPhase(char *ret_string = NULL);
	virtual void dataAnalysis();
	virtual void recordResult(int type);
	virtual int getCurrInstructionText(char pDestTxt[3][128]);
	void handleSpecialKeys(int key, int x, int y);
	void init();
	void sub_display();
	int updateAdc();
	void setAudioPhase(int audio_phase);

	void sendArd(int val1, int val2);
	void updateArd(int cmd = 0);
	void windTendon(int idx, int dir);
	void PhantomCallbackSubFunc();
	void enableForceCtrl(bool enable, bool opt, int jnt);
	void trialUnwind();
	//-----------------------------------------------------------------------
	// MEMBERS:
	//-----------------------------------------------------------------------
public:
	static HANDLE m_hAudioEvent;
	static HANDLE m_hArdEvent;
	static HANDLE m_hForceEnable;
	uint m_ch_timer;
	int m_ard_status;	// 0: no action 1: action
	cTimer_exp *m_pTimer;
	bool m_enableFctrl;
	double m_prev_force[2];
	double m_force_d[2];	// desired force
	int m_f_state[2];		// finger force feedback status
							// 0: no action  1: apply the desired force (PWM) 2: apply the negative PWM until sensed force becomes zero
	bool m_fc_opt;			// specifies whether to unwind the tendon after disabling the force control
	double m_min_f_d;		// minimum desired force
	double m_max_f_d;		// maximum desired force

		//==Serial==//
	Serial* SP;
	BYTE* m_pByte;
	int m_readbyte;
	int m_packetmode = 0;
	int m_checkSize = 0;
	int m_delta_T = 0;
	int m_buffer_index = 0;
	int m_sensor_data[4];
	unsigned char m_buffer[11];
	///// sensor data
	double m_FSR_force;
	double m_th_F[2];	// force feedback threshold
	double m_dur_contact;
	//	double m_dur_contact[2];	// contact duration 0: cutaneous 1: force-feedback
	int m_act_ready;	// 0: not ready 1: being ready for feedback; 2: ready to apply feedback; 3: applying feedback 
	float m_unwnd_dur;		// unwind duration
	float m_man_unwnd_dur;	// manual unwind duration
	bool m_b_man_unwnd;		// manual unwind flag
	int m_t_type;	// 0: PIP 1: MCP

	bool m_disp_train_sphere;	//display three train sphere
	bool m_disp_test_sphere;	//display two test shpere
	bool m_sphere_move;		//start sphere move
	double m_sphere_z_pos;
	bool m_select_sphere_a;
	bool m_select_sphere_b;
	bool m_select_sphere_c;
	char serial_txtbuf[10];
	bool m_bending_sensor_input;
	
private:
	double m_view_height;
	char m_txtBuf[65];
	int m_textBuf_len;
	bool m_show_dbg_info;
	double m_poten_pos[4];
	int m_pwm_val;
	bool m_disp_coeff;
	bool m_disp_f_coeff;
	bool m_disp_f_bar;
	float m_prev_pt_force;
	glm::vec3 m_prev_pos;
	char m_rec_filename[128];
	double m_trial_pt_force[16];
	int m_trial_joint[16];	// 0: PIP 1: MCP
	double m_trial_force_d[16];
	int m_tot_trial;
//	float m_val_bar;	// range: 0~1.0; A variable displays the finger joint PWM magnitude
	int m_uwnd_jnt;	// 0: PIP 1: MCP
	float m_uwnd_dur;	// the duration to unwind the string



	

//	bool m_first_pt;
};
