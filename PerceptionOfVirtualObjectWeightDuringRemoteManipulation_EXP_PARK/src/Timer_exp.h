#pragma once
#include "Timer1.h"


class cExp_Baik_Hand_PSE;

class cTimer_exp : public cTimer1
{
public:
	cTimer_exp();
	cTimer_exp(int timeout);
	virtual ~cTimer_exp();
	void set_ref(double val) { m_ref_val = val; }
	double get_ref() { return m_ref_val; }
	void enableInc(bool val) { m_en_inc = val; }
	bool getEnable() { return m_en_inc; }
	void resetTick();
	void playTick();
	void setLimit(double val) { m_t_limit = val; };
	void reset_ref() { m_ref_val = 0.0f;  }
	void setIncVal(double val) { m_ref_inc = val; }
	void setExpPointer(cExp_Baik_Hand_PSE *pExp) { m_pExp = pExp; }
	cExp_Baik_Hand_PSE *m_pExp;
protected:
	virtual void Tick();
	double m_ref_val;	// refren
	double m_ref_inc;
	double m_t_limit;
	bool m_en_inc;
};