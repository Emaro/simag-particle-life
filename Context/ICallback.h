#pragma once

class ICallbackKey
{
public:
	virtual void callbackKey(int key, int scancode, int action, int mods) = 0;
};
