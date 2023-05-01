#ifndef _CGENERIC_EXP_H_
#define _CGENERIC_EXP_H_

#include <time.h>
#include <vector>
#include <Windows.h>
//=======================================================================
/*!
	\file	CGenericExp.h
	
	\brief
	<b> Psychophyics Experiment </b>	\n
	Base Class.
*/
//=======================================================================

//=======================================================================
/*!
	\class	cGenericExp
	\author	Jaeyoung Park, 2014
	\brief
	cGenericExp is an abstract class for psychophysics experiments
*/
//=======================================================================
//typedef void (*resultRecordFuncPtr)(int type);
using std::vector;

class cGenericExp {
public:
    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------
	cGenericExp();
	virtual ~cGenericExp() {};
	//-----------------------------------------------------------------------
    // METHODS:
    //-----------------------------------------------------------------------
//	void setResultRecordFunc(resultRecordFuncPtr result_func) { m_resultRecordFunc = result_func;};
	void getCurrTime(tm &currTime);
	bool check_directory(char *directory_path);
	virtual int handleKeyboard(unsigned char key, char* ret_string) = 0;	// pure virtual functions
	virtual int moveToNextPhase(char *ret_string) = 0;
	virtual void dataAnalysis() = 0;
	virtual void recordResult(int type) = 0;
public:
	//-----------------------------------------------------------------------
    // MEMBERS:
    //-----------------------------------------------------------------------
	char m_subjectID[65];
	bool m_testSubject;
	int m_exp_phase;
	int m_curr_trial_no;	// current trial no.
	int m_tot_trial_number;
	time_t m_expBeginTime; 
	time_t m_trialBeginTime;
//	resultRecordFuncPtr m_resultRecordFunc;
};

#endif	// _CGENERIC_EXP_H_