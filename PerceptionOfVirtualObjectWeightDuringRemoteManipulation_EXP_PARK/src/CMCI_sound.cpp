#include "CMCI_sound.h"
#include <windows.h>
#include <MMSystem.h>
#include <sstream>

cMCI_sound::cMCI_sound(const char *filename) : m_fileOpened(false)
{
	m_filename.assign(filename);
	/// initialize

	std::string szCommand = "open \""+m_filename+"\" type mpegvideo alias "+m_filename;
	int error = mciSendString(szCommand.c_str(), NULL, 0, 0);
	if(error == 0) m_fileOpened = true;
}

cMCI_sound::~cMCI_sound()
{
	if(m_fileOpened) close();
}

int cMCI_sound::play(int millisec)
{
	int ret = -1;
	std::stringstream sstr;
	if(m_fileOpened) {
		sstr << "play " << m_filename << " from " << millisec;
		//std::string f_str = std::to_string(millisec)
		//std::string szCommand = "play "+m_filename+" from "+ std::to_string(millisec);
		//ret = mciSendString(szCommand.c_str(), NULL, 0, 0);
		ret = mciSendString(sstr.str().c_str(), NULL, 0, 0);
	}
	return ret;
}

int cMCI_sound::stop()
{
	int ret = -1;
	if(m_fileOpened) {
		std::string szCommand = "stop "+m_filename;
		ret = mciSendString(szCommand.c_str(), NULL, 0, 0);
	}
	return ret;
}

int cMCI_sound::pause()
{
	int ret = -1;
	if(m_fileOpened) {
		std::string szCommand = "pause "+m_filename;
		ret = mciSendString(szCommand.c_str(), NULL, 0, 0);
	}
	return ret;
}

int cMCI_sound::resume()
{
	int ret = -1;
	if(m_fileOpened) {
		std::string szCommand = "resume "+ m_filename;
		ret = mciSendString(szCommand.c_str(), NULL, 0, 0);
	}
	return ret;
}

void cMCI_sound::close()
{
	if(m_fileOpened) {
		int error;
		std::string szCommand = "close "+ m_filename;
		error = mciSendString(szCommand.c_str(), NULL, 0, 0);
		m_fileOpened = false;
	}
}
