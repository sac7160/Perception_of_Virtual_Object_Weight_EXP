#include "Timer_exp.h"
#include "CExp_Baik_Hand_PSE.h"

cTimer_exp::cTimer_exp() : m_en_inc(false), m_t_limit(0.0)
{
	m_ref_inc = 0.0;// 0.001*(double)m_msTimeout;
	m_ref_val = 0.0;
	m_t_limit = 0.5;
}

cTimer_exp::cTimer_exp(int timeout)
{
	m_msTimeout = timeout;
	m_ref_inc = 0.001*(double)m_msTimeout;
	m_ref_val = 0.0;
	m_t_limit = 0.5;
}

cTimer_exp::~cTimer_exp()
{
}

void cTimer_exp::resetTick()
{
	enableInc(false);
	reset_ref();
}

void cTimer_exp::playTick()
{
	reset_ref();
	enableInc(true);
}

void cTimer_exp::Tick()
{
	if (m_en_inc) {
		m_ref_val += m_ref_inc;
		printf("%f %f\n", m_ref_val, m_t_limit);
		if (m_ref_val >= m_t_limit) {
			m_en_inc = false;
			reset_ref();
			m_pExp->updateArd();			
		}
	}
}
