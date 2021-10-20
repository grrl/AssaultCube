#pragma once
#include "vector3.h"

typedef struct _KERNEL_MEMORY
{
	DWORD ProcessId;

	QWORD Address;
	void* Buffer;
	SIZE_T Size;

} KERNEL_MEMORY, *PKERNEL_MEMORY;

typedef struct info_t {
	int pid = 0;
	DWORD_PTR address;
	void* value;
	SIZE_T size;
	void* data;
}info, *p_info;

struct ViewMatrix {

	float vm0;
	float vm1;
	float vm2;
	float vm3;
	float vm4;
	float vm5;
	float vm6;
	float vm7;
	float vm8;
	float vm9;
	float vm10;
	float vm11;
	float vm12;
	float vm13;
	float vm14;
	float vm15;

};

struct ObjectName
{
	char data[127];
};

struct sPlayerName
{
	char data[64];
};

struct USkillCheck
{
public:
	unsigned char                                      UnknownData00[0x4];                                       // 0x0290(0x0004) MISSED OFFSET
	float                                              ProgressRate;                                             // 0x0294(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	float                                              SuccessZoneStart;                                         // 0x0298(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	float                                              SuccessZoneEnd;                                           // 0x029C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	float                                              BonusZoneLength;                                          // 0x02A0(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	int                                                SkillCheckCount;                                          // 0x02A4(0x0004) (ZeroConstructor, Transient, IsPlainOldData
};
