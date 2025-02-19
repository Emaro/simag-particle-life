#pragma once

#include "ICallback.h"

class IApp : public ICallbackKey
{
public:
	virtual void doWork() = 0;
	virtual void draw() = 0;
	virtual void imGui() = 0;
	virtual const char* name() const = 0;

	// ICallbackKey
	virtual void callbackKey(int key, int scancode, int action, int mods) override = 0;

	// Needed for Mac
	virtual ~IApp() = default;
};

class CApp : public IApp
{
public:
	virtual void doWork() override {};
	virtual void draw() override {};
	virtual void imGui() override {};
	virtual const char* name() const override = 0;
	virtual void callbackKey(int key, int scancode, int action, int mods) override {};
};

