
// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#include "MLChangeList.h"

MLChangeList::MLChangeList() : mSize(0), mChanges(0), mValue(0.f)
{	
	// setup glide time = 1 sample
	mGlideCounter = 0;
	mGlideTimeInSamples = 1;
	mInvGlideTimeInSamples = 1.f;
	mGlideTime = 0.f;
	mGlideStartValue = mGlideEndValue = 0.f;
	
	mSampleRate = 44100;
	temp = 0.f;
	mValue = 0.f;
}

MLChangeList::~MLChangeList()
{
}

// mValueSignal and mTimeSignal are able to hold one change per output sample.
// A typical vector might have zero or one changes, but in an extreme case may have
// as many as one per output sample.  And we never want to reallocate it (or anything)
// in the process() method.
MLProc::err MLChangeList::setDims(int size)
{
	MLProc::err e = MLProc::OK;	
	mSize = size;
	MLSample* a = mValueSignal.setDims(size);
	MLSample* b = mTimeSignal.setDims(size);
	if (!a || !b)
	{
		e = MLProc::memErr;
	}
	return e;
}

void MLChangeList::clearChanges()
{
	mChanges = 0;
}

void MLChangeList::zero()
{
	clearChanges();
	addChange(0.f, 0);
}

void MLChangeList::calcGlide()
{
	const unsigned prevGlideTimeInSamples = mGlideTimeInSamples;
	mGlideTimeInSamples = max(1, (int)(mGlideTime * (float)mSampleRate));
	mInvGlideTimeInSamples = 1.f / (float)mGlideTimeInSamples;
	const float glideFrac = (float)mGlideCounter / (float)prevGlideTimeInSamples;
	mGlideCounter = (unsigned)(glideFrac*(float)mGlideTimeInSamples);
}

void MLChangeList::setGlideTime(float time)
{
	mGlideTime = time;
	calcGlide();
}

void MLChangeList::setSampleRate(unsigned rate)
{
	mSampleRate = rate;
	calcGlide();
}

void MLChangeList::addChange(MLSample val, int time)
{
	temp = val;
	if (mChanges < mSize)
	{
		if (mChanges > 0)
		{
			int prevTime = (int)mTimeSignal[mChanges - 1];			
			if (time == prevTime)
			{
				mChanges--;
			}
		}

		mValueSignal[mChanges] = val;
		mTimeSignal[mChanges] = time;
//debug() << "MLChangeList(" << static_cast<void*>(this) << ") add changes:" << mChanges << " time:" << time  << " val:" << val << " glide:" << mGlideTimeInSamples << "\n";
		mChanges++;
	}
}

inline void MLChangeList::setGlideTarget(float target)
{
	mGlideStartValue = mValue;
	mGlideEndValue = target;
	mGlideCounter = mGlideTimeInSamples;
}

// write the input change list from the given offset into the output signal y .
// if ignoreZeroes is on, don't allow the value to change to 0.
// 
void MLChangeList::writeToSignal(MLSignal& y, unsigned readOffset, unsigned frames, bool ignoreZeroes)
{
	unsigned size = min(y.getWidth(), mValueSignal.getWidth());
	size = min(size, frames);
	unsigned t=0;
	unsigned i, changeTime, localChangeTime;
	
	// no changes, no glide?  mark constant and bail.
	if (!mChanges && (mGlideCounter <= 0)) 
	{
		y.setToConstant(mValue);
	}
	else if (!mChanges) // just gliding to target
	{
		for(; t<size; ++t)
		{
			// tick glide
			if (mGlideCounter > 0) 
			{
				mGlideCounter--;
				float x = (float)(mGlideTimeInSamples - mGlideCounter) * mInvGlideTimeInSamples;
				mValue = lerp(mGlideStartValue, mGlideEndValue, x);
			}
			y[t] = mValue;
		}
	}
	else
	{
		y.setConstant(false);
	
		// skip changes until change time is >= offset
		for(i=0; i<mChanges; ++i)
		{
			changeTime = (int)mTimeSignal[i];
			if (changeTime >= readOffset) break;
		}
		
		// write current value up to each change time, then change current value
		for(; i<mChanges; ++i)
		{
			changeTime = (int)mTimeSignal[i];
			localChangeTime = changeTime - readOffset;
			if (localChangeTime >= size) break;
			
			// write current glide up to change
			for(; t<localChangeTime; ++t)
			{
				// tick glide
				if (mGlideCounter > 0) 
				{
					mGlideCounter--;
					float x = (float)(mGlideTimeInSamples - mGlideCounter) * mInvGlideTimeInSamples;
					mValue = lerp(mGlideStartValue, mGlideEndValue, x);
				}				
				y[t] = mValue;			
			}
			
			float nextTarget = mValueSignal[i];
			bool doSet = ignoreZeroes ? (nextTarget != 0.f) : true;
			if (doSet)
			{
				setGlideTarget(nextTarget);
//debug() << "set glide target " << nextTarget << " i = " << i << "\n";
			}
		}
		
		// tick out to end
		for(; t<size; ++t)
		{
			// tick glide
			if (mGlideCounter > 0) 
			{
				mGlideCounter--;
				float x = (float)(mGlideTimeInSamples - mGlideCounter) * mInvGlideTimeInSamples;
				mValue = lerp(mGlideStartValue, mGlideEndValue, x);
			}				
			y[t] = mValue;
		}
		
		mChanges = 0;
	}
}

void MLChangeList::dump(void)
{
	debug() << "MLChangeList:   changes " << mChanges << ", c[0] " << mValueSignal[0] << ", counter " << mGlideCounter << ", value " << mValue << "\n";
//	debug() << "glide time: " << mGlideTimeInSamples << "inv:" << mInvGlideTimeInSamples << "\n";
}


