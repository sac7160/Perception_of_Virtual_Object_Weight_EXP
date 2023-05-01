#include "CGenericExp.h"

cGenericExp::cGenericExp() 
{
	m_subjectID[0] = NULL;
	m_curr_trial_no = -1;
	m_testSubject = false;
}

void cGenericExp::getCurrTime(tm &currTime)
{
	time_t ltime;
	errno_t err;
	time(&ltime);
	err = _localtime64_s(&currTime, &ltime);
}

bool cGenericExp::check_directory(char *directory_path) {
	bool ret = true;
	if(!CreateDirectory(directory_path, NULL)) {
		if(GetLastError() != ERROR_ALREADY_EXISTS) {
			ret = false;
		}
	}
	return ret;
}