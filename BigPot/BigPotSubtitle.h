#pragma once
#include "BigPotBase.h"

extern "C"
{
#include "ass/ass.h"
#include "ass/ass_types.h"
}

class BigPotSubtitle :
	public BigPotBase
{
public:
	BigPotSubtitle();
	virtual ~BigPotSubtitle();

private:
	ASS_Library* _lib;
	ASS_Renderer* _ren;
	ASS_Track* _track;
	ASS_Image* _img;
public:
	void init();
};

