#ifndef _CADAPTIVE_PSE_H_
#define _CADAPTIVE_PSE_H_

#include "CGenericExp.h"
#include <glm\glm.hpp>

using std::vector;

//=======================================================================
/*!
	\file	CAdaptive_PSE.h
	
	\brief
	<b> PSE estimation by an adaptive procedure </b>	\n
*/
//=======================================================================

//=======================================================================
/*!
	\class	CAdaptive_PSE
	\author	Jaeyoung Park, 2016
	\brief
	CAdaptive_PSE is a base class for PSE estimation by an adaptive procedure experiment classes
*/
//=======================================================================
struct exp_result {
	int trial_no;
	time_t trial_begin_time;
	time_t trial_end_time;
	float trial_stimulus;
	//tm trial_begin_tm;
	//tm trial_end_tm;
	int trial_user_ans;
	int ad_proc_curr_alt_phase;	// adaptive procedure current alternation phase
	int ad_proc_curr_alt_no;	// adaptive procedure current alternation number
	double mean_coll_depth[2][2];	//mean_coll_depth[0]: for reference mean_coll_depth[1]: 
	double max_coll_depth[2][2];
};

class cAdaptive_PSE : public cGenericExp
{
public:
    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------
	cAdaptive_PSE();
	~cAdaptive_PSE() {};
	//-----------------------------------------------------------------------
    // METHODS:
    //-----------------------------------------------------------------------
	virtual int handleKeyboard(unsigned char key, char* ret_string) { return 0;}	// pure virtual functions
	virtual int moveToNextPhase(char *ret_string) { return 0;}
	virtual void dataAnalysis() {}
	virtual void recordResult(int type) {}
	virtual int getCurrInstructionText(char pDestTxt[4][128]) {return 0;}
	void setExpVariables(double ad_proc_init_stimulus, int ad_proc_num_alternation1, double ad_proc_step_size1, int ad_proc_num_alternation2, double ad_proc_step_size2);
//	double calculate_stimulus(int curr_answer);
	double calculatePSE();
	int calcStimulus(double curr_stimulus, int curr_answer, double *next_stimulus=NULL);
//	void setStimulus(bool type, float stimulus);
public:
	//-----------------------------------------------------------------------
    // MEMBERS:
    //-----------------------------------------------------------------------
	vector<exp_result> m_expResult;
	vector<double> m_alt_val;
//	double m_curr_stimulus;
	double m_init_stimulus;
	double m_step_size1;
	double m_step_size2;
	int m_num_alternation1;
	int m_num_alternation2;
	int m_prev_answer;
//	int m_curr_answer;
	int m_curr_alt_phase;
	int m_curr_alt_no;
	int m_audio_phase;
	double m_PSE_est;
};
#endif

