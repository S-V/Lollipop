#pragma once

typedef U4 TaskID;

struct STask
{
	TaskID	id;
	TaskID	parent;
	UINT	numWorkItems;	// zero when the task is completed
	UINT	affinity;
	UINT	priority;
};

struct TaskPool
{

public:
	void Init();

};
