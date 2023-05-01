#include "CAdaptive_PSE.h"


cAdaptive_PSE::cAdaptive_PSE() : m_curr_alt_phase(0), m_curr_alt_no(0)
{
	m_alt_val.clear();
}

void cAdaptive_PSE::setExpVariables(double ad_proc_init_stimulus, int ad_proc_num_alternation1, 
	double ad_proc_step_size1, int ad_proc_num_alternation2, double ad_proc_step_size2)
{
	m_init_stimulus = ad_proc_init_stimulus;
	m_step_size1 = ad_proc_step_size1;
	m_step_size2 = ad_proc_step_size2;
	m_num_alternation1 = ad_proc_num_alternation1;
	m_num_alternation2 = ad_proc_num_alternation2;
}
/******************************************************************************************
 * int calcStimulus
	- This function decides whether to continue the experiment and returns the stimulus intensity
	of the next trial.
	- input variables
		curr_stimulus: the intensity of current trial stimulus
		curr_answer: 0 (variable intensity felt stronger, e.g. larger, harder, sharper, etc.), 
					 1 (reference stimulus intensity felt stonger, e.g. larger, harder, sharper, etc.)
	- return variables
		return value: 0 (continue the experiment) 1 (terminate)
		next_stimulus: next stimulus intensity
 ******************************************************************************************/
int cAdaptive_PSE::calcStimulus(double curr_stimulus, int curr_answer, double *next_stimulus)
{
	int ret = 0;	// if ret ==1, experiment is terminated
	if(m_curr_trial_no != 1 && m_prev_answer != curr_answer) // alternation
	{
		printf("alternation %d %d, curr_alt_no: %d\n", m_prev_answer, curr_answer, m_curr_alt_no);
		if(m_curr_alt_phase == 0) {	// first alternation phase with larger step size 
			if((m_curr_alt_no+1) == m_num_alternation1) {
				printf("alternation phase change \n");
				m_curr_alt_phase = 1;
				m_curr_alt_no = 0;
			}
			else {
				m_curr_alt_no++;
			}
		}
		else  { // 2nd alternation phase with larger step size 
			if(m_curr_alt_no > 0)
				m_alt_val.push_back(curr_stimulus);
			if(m_curr_alt_no+1 == m_num_alternation2) {
				ret = 1;	// ends
			}
			else {
				m_curr_alt_no++;
			}
		}
	}
	m_prev_answer = curr_answer;
	if(ret == 0) {
		if(curr_answer == 0) {	// variable intensity felt stronger (e.g. harder, sharper, etc.) --> decrease the variable stimulus intensity
			if(m_curr_alt_phase == 0) {
				if((curr_stimulus - m_step_size1) >= 0.0)
					*next_stimulus = curr_stimulus - m_step_size1;
				else *next_stimulus = 0.0;
			}
			else {
				if((curr_stimulus - m_step_size2) >= 0.0)
					*next_stimulus = curr_stimulus - m_step_size2;
				else *next_stimulus = 0.0;
			}
		}
		else {	// reference stimulus intensity felt stonger (e.g. larger, harder, sharper, etc.) --> increase the variable stimulus intensity
			if(m_curr_alt_phase == 0) *next_stimulus = curr_stimulus + m_step_size1;
			else *next_stimulus = curr_stimulus + m_step_size2;
		}
	}
	return ret;
}

double cAdaptive_PSE::calculatePSE()
{
	double sum_est = 0;
	unsigned int n_est_vals = 0;
	vector<double>::iterator it;
	for(it=m_alt_val.begin(); it!= m_alt_val.end();it++) {
		sum_est += *it;
		n_est_vals++;
	}
	m_PSE_est = sum_est/(double)n_est_vals;
	return m_PSE_est;
}
