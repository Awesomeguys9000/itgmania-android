#ifndef ARCH_HOOKS_ANDROID_H
#define ARCH_HOOKS_ANDROID_H

#include "ArchHooks.h"

class ArchHooks_Android : public ArchHooks
{
public:
	ArchHooks_Android();
	virtual ~ArchHooks_Android();
	virtual void Init();
	virtual RString GetArchName() const { return "Android"; }
	virtual void DumpDebugInfo();

    // Stub implementation for now
    virtual float GetDisplayAspectRatio() { return 1.77778f; } // 16:9 default
};

#ifdef ARCH_HOOKS
#error "More than one ArchHooks selected!"
#endif
#define ARCH_HOOKS ArchHooks_Android

#endif
