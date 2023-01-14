#include "memory.h"
#include <thread>
#include <iostream>

namespace Offsets {
	constexpr auto dwLocalPlayer = 0xDEA964;
	constexpr auto m_fFlags = 0x104;
	constexpr auto dwForceJump = 0x52BBC9C;

	constexpr auto dwEntityList = 0x4DFFF14;
	constexpr auto dwGlowObjectManager = 0x535A9D8;

	constexpr auto m_iTeamNum = 0xF4;
	constexpr auto m_iGlowIndex = 0x10488;

	constexpr auto dwForceAttack = 0x322DD10;
	constexpr auto m_iCrosshairId = 0x11838;
	constexpr auto m_iHealth = 0x100;
}

int main()
{
	auto memory = Memory("csgo.exe");
	//std::cout << "Process ID: " + memory.GetProcessId() << std::endl;


	const auto client = memory.GetModuleAddy("client.dll");
	const auto entity = memory.Read<uintptr_t>(client + Offsets::dwGlowObjectManager);
	

	while (true) {
		const auto localPlayer = memory.Read<uintptr_t>(client + Offsets::dwLocalPlayer);
		const auto glowObjManager = memory.Read<uintptr_t>(client + Offsets::dwGlowObjectManager);
		

		//bhop hack
		if (localPlayer) {
			const auto onGround = memory.Read<bool>(localPlayer + Offsets::m_fFlags);
			if (GetAsyncKeyState(VK_SPACE)) {
				if (onGround & (1 << 0))
					memory.Write<BYTE>(client + Offsets::dwForceJump, 6);
			}
		}

		//trigger bot 
		if (GetAsyncKeyState(0x05)) {
			const auto crosshairID = memory.Read<uintptr_t>(localPlayer + Offsets::m_iCrosshairId);

			if (crosshairID || crosshairID < 64) {
				const auto entityCrosshair = memory.Read<uintptr_t>(client + Offsets::dwEntityList + (crosshairID - 1) * 0x10);
				
				if (memory.Read<int32_t>(entityCrosshair + Offsets::m_iHealth)) {
					if (memory.Read<uintptr_t>(localPlayer + Offsets::m_iTeamNum) != memory.Read<uintptr_t>(entity + Offsets::m_iTeamNum)) {
						memory.Write<uintptr_t>(client + Offsets::dwForceAttack, 6);
						std::this_thread::sleep_for(std::chrono::milliseconds(20));
						memory.Write<uintptr_t>(client + Offsets::dwForceAttack, 4);
					}
				}
			}
		}

		//glow hack 
		for (int ctr = 0; ctr < 64; ctr++) {

			const auto entity = memory.Read<uintptr_t>(client + Offsets::dwEntityList + ctr * 0x10);

			if (memory.Read<uintptr_t>(localPlayer + Offsets::m_iTeamNum) == memory.Read<uintptr_t>(entity + Offsets::m_iTeamNum))
				continue;

			const auto entityGlow = memory.Read<int32_t>(entity + Offsets::m_iGlowIndex);

			memory.Write<float>(glowObjManager + (entityGlow * 0x38) + 0x8, 1.f); //red
			memory.Write<float>(glowObjManager + (entityGlow * 0x38) + 0xC, 0.f); //green
			memory.Write<float>(glowObjManager + (entityGlow * 0x38) + 0x10, 0.f); //blue
			memory.Write<float>(glowObjManager + (entityGlow * 0x38) + 0x14, 1.f); //alpha


			memory.Write<bool>(glowObjManager + (entityGlow * 0x38) + 0x27, true); 
			memory.Write<bool>(glowObjManager + (entityGlow * 0x38) + 0x28, true); 
		}



		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}

	return 0;
} 