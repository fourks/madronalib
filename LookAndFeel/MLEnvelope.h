
// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#ifndef __ML_ENVELOPE_HEADER__
#define __ML_ENVELOPE_HEADER__

#include "MLUI.h"
#include "MLLookAndFeel.h"

class MLEnvelope  : 
	public Component, 
	public MLWidget
{
public:
    MLEnvelope();
    ~MLEnvelope();

	enum ColourIds
    {
        foregroundColor              = 0x10120a01,  
    };
	
	/*
	enum
	{
		delayValue,
		attackValue,
		decayValue,
		sustainValue,
		releaseValue,
		repeatValue
	};
	*/
	
	void setAttribute(MLSymbol attr, float val);

    void setDARMode(bool m) { mDARMode = m; }
	
	/*
	void setSelectedValue (double newValue,
						const int valSelector,
                       const bool sendUpdateMessage,
                       const bool sendMessageSynchronously = 0);
	*/
	
	void triggerChangeMessage (const bool synchronous);

	void resizeWidget(const MLRect& b, const int u);

protected:

    void paint (Graphics& g);
    void lookAndFeelChanged();
    void visibilityChanged();
    void colourChanged();

private:
	bool mDARMode;
	float mOutlineThickness;
	
	/*
	double mDelay;
	double mAttack;
	double mDecay;
	double mSustain;
	double mRelease;
	double mRepeat;
	*/
	
    MLEnvelope (const MLEnvelope&);
    const MLEnvelope& operator= (const MLEnvelope&);
};


#endif  //__ML_ENVELOPE_HEADER__