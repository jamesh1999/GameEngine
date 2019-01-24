#ifndef __GARBAGE_COLLECTOR_INCLUDED__
#define __GARBAGE_COLLECTOR_INCLUDED__

#include "GCObject.h"
#include "GCPointer.h"
#include <vector>
#include "GCSettings.h"

class GarbageCollector
{
private:
	static GCSettings settings;

	static std::vector<GCObject*> objects;
	static std::vector<GCObject*> base;

	static int curID;

	static void recurseObjectTree(GCObject*);

public:
	static void RegisterBaseObject(GCObject*);
	static void RemoveBaseObject(GCObject*);
	static void RegisterObject(GCObject*);
	static void RunCollector();
	static void RefreshTracked();
	static void UpdateSettings(GCSettings*);
	static const GCSettings* GetSettings();
};

#endif