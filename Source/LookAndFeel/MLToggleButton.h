
// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#ifndef __ML_TOGGLE_BUTTON_HEADER__
#define __ML_TOGGLE_BUTTON_HEADER__

#include "MLUI.h"
#include "MLButton.h"

typedef std::tr1::shared_ptr<Drawable> DrawablePtr;


class MLToggleButton : 
	public MLButton
{
public:
     MLToggleButton();
    ~MLToggleButton();
	
	void setAttribute(MLSymbol attr, float val);
	void paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown);
	void resizeWidget(const MLRect& b, const int);

protected:
	float mLineThickness;
private:
 	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MLToggleButton);
};

#endif   // __ML_TOGGLE_BUTTON_HEADER__