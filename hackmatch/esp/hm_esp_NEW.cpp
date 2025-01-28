#include "../hm_helper.h"
#include "hm_esp.h"

// Main
void HMEsp::RenderEsp()
{
	if (!hmHelper->m_pConfig->playerEsp.enabled)
		return;
	RenderNameplates();
	RenderBox();
}


// Rendering
void HMEsp::RenderNameplates()
{
	if (!hmHelper->m_pConfig->playerEsp.nameplates)
		return;

	PlayerController_o* localPlayer = hmHelper->m_pSdk->GetLocalPlayer();
	if (localPlayer == nullptr)
		return;

	hmHelper->m_pSdk->playersMutex.lock();
	for (const auto& player : hmHelper->m_pSdk->players)
	{
		if (!player)
			continue;
		ImVec2 screen;

		if (!hmHelper->m_pIl2cpp->world_to_screen(player->fields.lastNonLocalPos, screen))
			continue;

		std::string name = hmHelper->m_pIl2cpp->to_string(player->fields._identity_k__BackingField->fields._Owner_k__BackingField->fields._name);
		screen.x -= ImGui::CalcTextSize(name.c_str()).x * 0.5f;
		ImGui::GetBackgroundDrawList()->AddText(screen, IM_COL32(255, 0, 0, 255), name.c_str());

	}
	hmHelper->m_pSdk->playersMutex.unlock();
}

// 定义一个函数用于渲染玩家的ESP框
void HMEsp::RenderBox()
{
    // 检查配置是否启用玩家ESP框，如果未启用则直接返回
	if (!hmHelper->m_pConfig->playerEsp.box)
		return;

	// 获取本地玩家控制器，如果为空则直接返回
	PlayerController_o* localPlayer = hmHelper->m_pSdk->GetLocalPlayer();
	if (localPlayer == nullptr)
		return;

	// 锁定玩家列表的互斥锁，以防止在遍历时被修改
	hmHelper->m_pSdk->playersMutex.lock();
	for (const auto& player : hmHelper->m_pSdk->players)
	{
		// 如果玩家对象为空，则跳过当前循环
		if (!player)
			continue;

		// 定义玩家的高度大约为2个单位
		constexpr float appxHeight = 2.f;
		// 获取玩家的最后非本地位置
		UnityEngine_Vector3_o pos = player->fields.lastNonLocalPos;

		// 计算玩家的脚部位置
		UnityEngine_Vector3_o feet = { pos.fields.x, pos.fields.y - appxHeight * 0.5f, pos.fields.z };
		// 计算玩家的头部位置
		UnityEngine_Vector3_o head = { pos.fields.x, pos.fields.y + appxHeight * 0.5f, pos.fields.z };

		// 将头部位置从世界坐标转换为屏幕坐标
		ImVec2 tmpHead;
		if (!hmHelper->m_pIl2cpp->world_to_screen(head, tmpHead))
			continue;

		// 将脚部位置从世界坐标转换为屏幕坐标
		ImVec2 tmpFeet;
		if (!hmHelper->m_pIl2cpp->world_to_screen(feet, tmpFeet))
			continue;

		// 计算ESP框的高度
		const float height = GetPlayerHeight(tmpHead, tmpFeet);
		// 计算ESP框的宽度，宽度为高度的一半
		const float width = height * 0.5f;

		// 计算ESP框的左上角和右下角坐标
		const ImVec2 min = { tmpFeet.x - width * 0.5f, tmpFeet.y - height };		   // upper-left
		const ImVec2 max = { tmpFeet.x + width * 0.5f, tmpFeet.y };                     // lower-right

		// 将玩家的位置从世界坐标转换为屏幕坐标
		ImVec2 screen;
		if (!hmHelper->m_pIl2cpp->world_to_screen(player->fields.lastNonLocalPos, screen))
			continue;

		// 使用ImGui绘制ESP框，外层为半透明黑色，内层为红色
		ImGui::GetBackgroundDrawList()->AddRect(min,
			max,
			ColorRGBA{ 0.f, 0.f, 0.f, 225.f }.imGui(), // 半透明黑色
			0.f,
			ImDrawCornerFlags_All,
			2.5f); // 外层边框宽度
		ImGui::GetBackgroundDrawList()->AddRect(min, max, IM_COL32(255, 0, 0, 255), 0.f,ImDrawCornerFlags_All, 1.5f); // 内层红色边框宽度
	}
	// 解锁玩家列表的互斥锁
	hmHelper->m_pSdk->playersMutex.unlock();
}

void HMEsp::RenderPlayersEspLine()
{
    // 检查配置是否启用玩家 ESP 线条
    if (!hmHelper->m_pConfig->playerEsp.PlayersEspLine)
        return;

    // 获取本地玩家
    PlayerController_o* localPlayer = hmHelper->m_pSdk->GetLocalPlayer();
    if (localPlayer == nullptr)
        return;

    // 锁定玩家列表以确保线程安全
    hmHelper->m_pSdk->playersMutex.lock();
    for (const auto& player : hmHelper->m_pSdk->players)
    {
        if (!player)
            continue;

        // 计算玩家脚部位置
        constexpr float appxHeight = 2.f;
        UnityEngine_Vector3_o pos = player->fields.lastNonLocalPos;
        UnityEngine_Vector3_o feet = { pos.fields.x, pos.fields.y - appxHeight * 0.5f, pos.fields.z };

        // 将脚部位置转换为屏幕坐标
        ImVec2 feetScreen;
        if (!hmHelper->m_pIl2cpp->world_to_screen(feet, feetScreen))
            continue; // 如果转换失败，跳过当前玩家

        // 获取 ImGui 窗口尺寸并计算屏幕中心
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 screenCenter = { windowSize.x * 0.5f, windowSize.y * 0.5f };

        // 绘制从玩家脚部到屏幕中心的红色线条
        ImGui::GetBackgroundDrawList()->AddLine(
            feetScreen,                // 起点：玩家脚部屏幕坐标
            screenCenter,             // 终点：屏幕中心
            IM_COL32(255, 0, 0, 255),  // 线条颜色：红色
            1.5f                     // 线条粗细
            // 0.0f                      // 弧度（直线）
        );
    }
    hmHelper->m_pSdk->playersMutex.unlock();
}
