#pragma once
#include <string>

class ICallBackRestore
{
public:
	virtual void ExecCmdCallBack(const std::string& text) = 0;
protected:
private:
};

