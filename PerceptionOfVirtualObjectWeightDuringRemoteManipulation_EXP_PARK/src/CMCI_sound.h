#ifndef _CMCI_SOUND_H_
#define _CMCI_SOUND_H_
#include <string>

class cMCI_sound 
{
public:
	cMCI_sound(const char *filename);
	~cMCI_sound();
	int play(int millisec=0);
	int stop();
	int pause();
	int resume();
	void close();
private:
	bool m_fileOpened;
	std::string m_filename;
};

#endif	// _CMCI_SOUND_H_
