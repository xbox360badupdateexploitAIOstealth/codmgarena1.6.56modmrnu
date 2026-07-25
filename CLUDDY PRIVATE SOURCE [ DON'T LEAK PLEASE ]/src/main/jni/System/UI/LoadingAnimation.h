#pragma once

#include "ImGui/Call_ImGui.h"
#include "../../ImGui/imgui_settings.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

extern ImFont* F50;

namespace font {
    extern ImFont* inter_semibold;
}

namespace ui_loading {

struct TechNode {
    ImVec2 pos;
    float angle;
    float speed;
    float pulse;
    int nodeType;
};

struct CircuitLine {
    ImVec2 start;
    ImVec2 end;
    float progress;
    float speed;
    bool active;
};

struct HexGrid {
    ImVec2 center;
    float size;
    float rotation;
    float glowIntensity;
};

static bool showLoadingAnimation = false;
static float loadingAnimationTimer = 0.0f;
static float loadingAnimationDuration = 1.55f;
static std::vector<TechNode> techNodes;
static std::vector<CircuitLine> circuitLines;
static std::vector<HexGrid> hexGrids;
static float animationTime = 0.0f;
static float rotationAngle = 0.0f;
static float pulseWave = 0.0f;
static bool seededRandom = false;

// ====================== LUXURY GOLD COLOR PALETTE ======================
namespace LuxuryGold {
    const ImVec4 Accent       = ImVec4(0.9608f, 0.7725f, 0.0706f, 1.00f); // #F5C518
    const ImVec4 AccentDark   = ImVec4(0.7843f, 0.5490f, 0.0314f, 1.00f); // Deep Gold
    const ImVec4 AccentSoft   = ImVec4(0.9608f, 0.7725f, 0.0706f, 0.58f);
    const ImVec4 AccentGlow   = ImVec4(0.9804f, 0.8627f, 0.3529f, 0.35f);
    const ImVec4 AccentBright = ImVec4(1.0000f, 0.9412f, 0.65098f, 0.95f);
    const ImVec4 GoldShadow   = ImVec4(0.65098f, 0.45098f, 0.0784f, 0.85f);
}

// =====================================================================

inline float EaseOutCubic(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    const float inv = 1.0f - t;
    return 1.0f - inv * inv * inv;
}

inline float EaseInOutSine(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return -(std::cos(3.14159265f * t) - 1.0f) * 0.5f;
}

inline void SeedRandomOnce() {
    if (!seededRandom) {
        std::srand((unsigned int)std::time(nullptr));
        seededRandom = true;
    }
}

inline void InitParticles(float orbitRadius) {
    SeedRandomOnce();
    techNodes.clear();
    circuitLines.clear();
    hexGrids.clear();

    const int nodeCount = 72;
    for (int i = 0; i < nodeCount; ++i) {
        TechNode node{};
        const float angle = (i / (float)nodeCount) * 2.0f * 3.14159265f;
        const float radialJitter = ((std::rand() % 100) / 100.0f - 0.5f) * orbitRadius * 0.34f;
        const float radius = orbitRadius + radialJitter;
        node.pos = ImVec2(std::cos(angle) * radius, std::sin(angle) * radius);
        node.angle = angle;
        node.speed = 0.55f + ((std::rand() % 100) / 100.0f) * 1.15f;
        node.pulse = (std::rand() % 100) / 100.0f * 6.2831853f;
        node.nodeType = std::rand() % 3;
        techNodes.push_back(node);
    }

    const int lineCount = 56;
    for (int i = 0; i < lineCount; ++i) {
        const int idxA = std::rand() % nodeCount;
        int idxB = std::rand() % nodeCount;
        if (idxA == idxB) {
            idxB = (idxB + 11) % nodeCount;
        }

        CircuitLine line{};
        line.start = techNodes[idxA].pos;
        line.end = techNodes[idxB].pos;
        line.progress = (std::rand() % 100) / 100.0f;
        line.speed = 0.45f + ((std::rand() % 100) / 100.0f) * 1.25f;
        line.active = (std::rand() % 100) < 58;
        circuitLines.push_back(line);
    }

    const int hexCount = 14;
    for (int i = 0; i < hexCount; ++i) {
        HexGrid hex{};
        const float angle = (i / (float)hexCount) * 2.0f * 3.14159265f;
        const float radius = orbitRadius * (0.48f + (i % 4) * 0.18f);
        hex.center = ImVec2(std::cos(angle) * radius, std::sin(angle) * radius);
        hex.size = orbitRadius * (0.10f + (i % 3) * 0.03f);
        hex.rotation = ((std::rand() % 360) / 180.0f) * 3.14159265f;
        hex.glowIntensity = 0.45f + ((std::rand() % 100) / 100.0f) * 0.55f;
        hexGrids.push_back(hex);
    }
}

inline void Start(float durationSeconds = 1.55f) {
    showLoadingAnimation = true;
    loadingAnimationTimer = 0.0f;
    loadingAnimationDuration = std::max(0.4f, durationSeconds);
    animationTime = 0.0f;
    rotationAngle = 0.0f;
    pulseWave = 0.0f;
    InitParticles(132.0f);
}

inline bool IsActive() {
    return showLoadingAnimation;
}

inline float GetProgress() {
    if (loadingAnimationDuration <= 0.0f) return 1.0f;
    return std::clamp(loadingAnimationTimer / loadingAnimationDuration, 0.0f, 1.0f);
}

inline const char* GetStageLabel(float progress) {
    if (progress < 0.34f) return "VERIFYING LICENSE";
    if (progress < 0.68f) return "SYNCING CLUDDY PREMIUM MODULES";
    return "PREPARING MENU";
}

inline void DrawHexRing(ImDrawList* draw, const ImVec2& center, float size, float rotation, ImU32 color, float thickness) {
    for (int i = 0; i < 6; ++i) {
        const float a1 = rotation + (i / 6.0f) * 2.0f * 3.14159265f;
        const float a2 = rotation + ((i + 1) / 6.0f) * 2.0f * 3.14159265f;
        const ImVec2 p1(center.x + std::cos(a1) * size, center.y + std::sin(a1) * size);
        const ImVec2 p2(center.x + std::cos(a2) * size, center.y + std::sin(a2) * size);
        draw->AddLine(p1, p2, color, thickness);
    }
}

inline void DrawAnimation(ImDrawList* draw, const ImVec2& center, float radius, float progress) {
    const float dt = ImGui::GetIO().DeltaTime;
    animationTime += dt;
    rotationAngle += dt * 0.85f;
    pulseWave = std::sin(animationTime * 3.25f) * 0.5f + 0.5f;

    const ImU32 accent       = ImGui::GetColorU32(LuxuryGold::Accent);
    const ImU32 accentDark   = ImGui::GetColorU32(LuxuryGold::AccentDark);
    const ImU32 accentSoft   = ImGui::GetColorU32(LuxuryGold::AccentSoft);
    const ImU32 accentGlow   = ImGui::GetColorU32(LuxuryGold::AccentGlow);
    const ImU32 accentBright = ImGui::GetColorU32(LuxuryGold::AccentBright);
    const ImU32 panelShade   = IM_COL32(15, 15, 20, 245);

    // Background glow layers
    for (int layer = 5; layer >= 0; --layer) {
        const float expand = radius * (0.92f + layer * 0.08f);
        const float alpha = 0.055f - layer * 0.007f;
        draw->AddCircleFilled(center, expand, ImGui::GetColorU32(ImVec4(0.9608f, 0.7725f, 0.0706f, alpha)), 64);
    }
    draw->AddCircleFilled(center, radius * 0.74f, panelShade, 48);

    // Hex Grids
    for (auto& hex : hexGrids) {
        hex.rotation += dt * (0.18f + hex.glowIntensity * 0.12f);
        const float pulse = 0.72f + std::sin(animationTime * 1.9f + hex.glowIntensity * 4.5f) * 0.28f;
        const ImVec2 hexCenter(center.x + hex.center.x * 0.9f, center.y + hex.center.y * 0.9f);
        
        DrawHexRing(draw, hexCenter, hex.size + 6.0f, hex.rotation, 
                    ImGui::GetColorU32(ImVec4(0.9608f, 0.7725f, 0.0706f, 0.11f * pulse)), 1.0f);
        DrawHexRing(draw, hexCenter, hex.size, hex.rotation, 
                    ImGui::GetColorU32(ImVec4(0.9608f, 0.7725f, 0.0706f, 0.45f * pulse)), 1.6f);
    }

    // Circuit Lines
    for (auto& line : circuitLines) {
        if (line.active) {
            line.progress += dt * line.speed * 0.42f;
            if (line.progress >= 1.0f) {
                line.progress = 0.0f;
                line.active = (std::rand() % 100) < 70;
            }
        } else if ((std::rand() % 1000) < 10) {
            line.active = true;
            line.progress = 0.0f;
        }

        const ImVec2 lineStart(center.x + line.start.x, center.y + line.start.y);
        const ImVec2 lineEnd(center.x + line.end.x, center.y + line.end.y);
        draw->AddLine(lineStart, lineEnd, ImGui::GetColorU32(ImVec4(0.96f, 0.77f, 0.07f, 0.10f)), 1.0f);

        if (line.active && line.progress > 0.0f) {
            const ImVec2 current(
                lineStart.x + (lineEnd.x - lineStart.x) * line.progress,
                lineStart.y + (lineEnd.y - lineStart.y) * line.progress
            );
            draw->AddLine(lineStart, current, accentGlow, 1.65f);
            draw->AddCircleFilled(current, 2.2f, accentBright, 12);
        }
    }

    // Tech Nodes
    for (auto& node : techNodes) {
        node.angle += dt * node.speed * 0.18f;
        node.pulse += dt * (1.4f + node.speed * 0.4f);

        const float nodeRadius = radius * 0.86f + std::sin(node.pulse) * radius * 0.08f;
        node.pos.x = std::cos(node.angle) * nodeRadius;
        node.pos.y = std::sin(node.angle) * nodeRadius;

        const ImVec2 nodePos(center.x + node.pos.x, center.y + node.pos.y);
        const float pulse = 0.75f + std::sin(node.pulse) * 0.25f;

        switch (node.nodeType) {
        case 0:
            draw->AddCircleFilled(nodePos, 2.8f * pulse, accent, 10);
            draw->AddCircle(nodePos, 5.0f * pulse, accentSoft, 16, 1.15f);
            break;
        case 1:
            draw->AddRectFilled(
                ImVec2(nodePos.x - 2.4f * pulse, nodePos.y - 2.4f * pulse),
                ImVec2(nodePos.x + 2.4f * pulse, nodePos.y + 2.4f * pulse),
                IM_COL32(245, 215, 120, 190)
            );
            break;
        default:
            draw->AddTriangleFilled(
                ImVec2(nodePos.x, nodePos.y - 3.4f * pulse),
                ImVec2(nodePos.x - 3.0f * pulse, nodePos.y + 2.2f * pulse),
                ImVec2(nodePos.x + 3.0f * pulse, nodePos.y + 2.2f * pulse),
                accentDark
            );
            break;
        }
    }

    // Main Arc Rings
    const int segments = 76;
    const float mainArcLength = 0.72f;
    const float startAngle = rotationAngle;
    const float endAngle = startAngle + (2.0f * 3.14159265f * mainArcLength);

    for (int ring = 0; ring < 3; ++ring) {
        const float ringRadius = radius - ring * 16.0f;
        const float thickness = 3.1f - ring * 0.55f;
        const float alpha = 230.0f - ring * 60.0f;

        for (int i = 0; i < segments; ++i) {
            const float t0 = i / (float)segments;
            const float t1 = (i + 1) / (float)segments;
            const float a0 = startAngle + (endAngle - startAngle) * t0;
            const float a1 = startAngle + (endAngle - startAngle) * t1;
            const ImVec2 p0(center.x + std::cos(a0) * ringRadius, center.y + std::sin(a0) * ringRadius);
            const ImVec2 p1(center.x + std::cos(a1) * ringRadius, center.y + std::sin(a1) * ringRadius);
            const float fade = 0.18f + t0 * 0.82f;
            draw->AddLine(p0, p1, ImGui::GetColorU32(ImVec4(0.96f, 0.77f, 0.07f, (alpha / 255.0f) * fade)), thickness);
        }
    }

    // Progress Ring
    const float progressRadius = radius + 24.0f;
    const float progressSweep = 2.0f * 3.14159265f * (0.1f + 0.9f * EaseInOutSine(progress));
    const float progressStart = -1.5707963f;
    for (int i = 0; i < 54; ++i) {
        const float t0 = i / 54.0f;
        const float t1 = (i + 1) / 54.0f;
        const float a0 = progressStart + progressSweep * t0;
        const float a1 = progressStart + progressSweep * t1;
        const ImVec2 p0(center.x + std::cos(a0) * progressRadius, center.y + std::sin(a0) * progressRadius);
        const ImVec2 p1(center.x + std::cos(a1) * progressRadius, center.y + std::sin(a1) * progressRadius);
        draw->AddLine(p0, p1, ImGui::GetColorU32(ImVec4(1.0f, 0.94f, 0.65f, 0.45f + 0.25f * t0)), 2.2f);
    }

    // Inner Spokes
    for (int i = 0; i < 10; ++i) {
        const float angle = rotationAngle * 0.76f + (i / 10.0f) * 2.0f * 3.14159265f;
        const float inner = radius * 0.48f;
        const float outer = radius * (0.65f + std::sin(animationTime * 1.6f + i) * 0.04f);
        const ImVec2 p0(center.x + std::cos(angle) * inner, center.y + std::sin(angle) * inner);
        const ImVec2 p1(center.x + std::cos(angle) * outer, center.y + std::sin(angle) * outer);
        draw->AddLine(p0, p1, ImGui::GetColorU32(ImVec4(0.96f, 0.77f, 0.07f, 0.45f)), 1.35f);
    }

    // Core
    const float corePulse = 0.76f + pulseWave * 0.24f;
    draw->AddCircleFilled(center, 14.0f * corePulse, IM_COL32(18, 18, 23, 255), 20);
    draw->AddCircle(center, 19.0f * corePulse, accentSoft, 24, 2.2f);
    draw->AddCircleFilled(center, 6.8f * corePulse, accentBright, 20);
}

inline bool RenderWindow(ImTextureID backgroundTexture = nullptr) {
    if (!showLoadingAnimation) return false;
    IM_UNUSED(backgroundTexture);

    ImGuiIO& io = ImGui::GetIO();
    const ImVec2 viewportCenter = ImGui::GetMainViewport()->GetCenter();
    const ImVec2 panelSize(std::min(640.0f, io.DisplaySize.x - 48.0f), std::min(360.0f, io.DisplaySize.y - 72.0f));

    ImGui::SetNextWindowPos(viewportCenter, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(panelSize, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.0f);

    bool finished = false;

    if (ImGui::Begin("##astral_loading_transition", nullptr,
        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove))
    {
        const ImVec2 pos = ImGui::GetWindowPos();
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const ImVec2 innerMin(pos.x + 12.0f, pos.y + 12.0f);
        const ImVec2 innerMax(pos.x + panelSize.x - 12.0f, pos.y + panelSize.y - 12.0f);
        const float innerWidth = innerMax.x - innerMin.x;
        const float leftPaneWidth = innerWidth * 0.36f;
        const float dividerX = innerMin.x + leftPaneWidth;
        const float contentLeft = dividerX + 22.0f;
        const float contentRight = innerMax.x - 26.0f;
        const float progress = GetProgress();

        // Background
        draw->AddRectFilled(pos, ImVec2(pos.x + panelSize.x, pos.y + panelSize.y), IM_COL32(10, 10, 14, 235), 16.0f);
        draw->AddRectFilledMultiColor(pos, ImVec2(pos.x + panelSize.x, pos.y + 140.0f),
            ImGui::GetColorU32(ImVec4(0.96f, 0.77f, 0.07f, 0.09f)),
            ImGui::GetColorU32(ImVec4(0.96f, 0.77f, 0.07f, 0.04f)),
            IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 0));

        draw->AddRectFilled(innerMin, innerMax, IM_COL32(18, 18, 23, 255), 14.0f);

        // Animation
        const ImVec2 animCenter(innerMin.x + leftPaneWidth * 0.5f, innerMin.y + (innerMax.y - innerMin.y) * 0.52f);
        const float animRadius = std::min(leftPaneWidth, innerMax.y - innerMin.y) * 0.29f;
        DrawAnimation(draw, animCenter, animRadius, progress);

        ImFont* titleFont = F50 ? F50 : (font::inter_semibold ? font::inter_semibold : ImGui::GetFont());
        ImFont* labelFont = font::inter_semibold ? font::inter_semibold : ImGui::GetFont();

        const float titleSize       = (titleFont == F50) ? 24.0f : titleFont->FontSize * 1.10f;
        const float sectionLabelSize = labelFont->FontSize * 0.82f;
        const float bodySize        = labelFont->FontSize * 0.90f;

        // Layout constants for step rows and progress bar
        const float stepHeight      = bodySize + 18.0f;
        const float stepGap         = 6.0f;
        const float stepStartY      = innerMin.y + 90.0f;
        const float progressLabelY  = stepStartY + 3 * (stepHeight + stepGap) + 12.0f;
        const float progressBarY    = progressLabelY + sectionLabelSize + 8.0f;
        const float progressBarW    = contentRight - contentLeft;
        const float progressBarH    = 8.0f;

        const char* topLabel = "CLUDDY MOD";
        const char* title = "INITIALIZING PREMIUM HACKS";

        draw->AddText(labelFont, labelFont->FontSize * 0.82f, ImVec2(contentLeft, innerMin.y + 26.0f),
                      ImGui::GetColorU32(ImVec4(0.96f, 0.77f, 0.07f, 0.95f)), topLabel);

        draw->AddText(titleFont, 24.0f, ImVec2(contentLeft, innerMin.y + 52.0f),
                      IM_COL32(255, 245, 200, 255), title);

        auto drawStepRow = [&](int index, const char* label, int state) {
            const float y = stepStartY + index * (stepHeight + stepGap);
            const ImVec2 rowMin(contentLeft, y);
            const ImVec2 rowMax(contentRight, y + stepHeight);
            const ImU32 rowBg = state == 1 ? IM_COL32(0, 0, 0, 158) : IM_COL32(0, 0, 0, 118);
            draw->AddRectFilled(rowMin, rowMax, rowBg, 9.0f);
            draw->AddRectFilledMultiColor(
                rowMin,
                ImVec2(rowMax.x, rowMin.y + (rowMax.y - rowMin.y) * 0.58f),
                ImGui::GetColorU32(ImVec4(0.9608f * 0.18f, 0.7725f * 0.18f, 0.0706f * 0.18f, state == 0 ? 0.04f : 0.12f)),
                ImGui::GetColorU32(ImVec4(0.9608f * 0.12f, 0.7725f * 0.12f, 0.0706f * 0.12f, state == 0 ? 0.02f : 0.06f)),
                IM_COL32(0, 0, 0, 0),
                IM_COL32(0, 0, 0, 0)
            );

            const ImVec2 dotCenter(rowMin.x + 16.0f, rowMin.y + stepHeight * 0.5f);
            if (state == 2) {
                draw->AddCircleFilled(dotCenter, 5.0f, ImGui::GetColorU32(ImVec4(0.9608f, 0.7725f, 0.0706f, 0.95f)), 16);
                draw->AddCircle(dotCenter, 8.0f, ImGui::GetColorU32(ImVec4(0.9608f, 0.7725f, 0.0706f, 0.30f)), 16, 1.2f);
            } else if (state == 1) {
                draw->AddCircleFilled(dotCenter, 4.0f, ImGui::GetColorU32(c::text::text_active), 16);
                draw->AddCircle(dotCenter, 8.0f, ImGui::GetColorU32(ImVec4(0.9608f, 0.7725f, 0.0706f, 0.26f)), 16, 1.2f);
            } else {
                draw->AddCircleFilled(dotCenter, 4.0f, IM_COL32(70, 72, 82, 220), 16);
            }

            draw->AddText(
                labelFont,
                bodySize,
                ImVec2(rowMin.x + 32.0f, rowMin.y + (stepHeight - bodySize) * 0.5f - 1.0f),
                ImGui::GetColorU32(state == 0 ? c::text::text : c::text::text_active),
                label
            );

            const char* stateText = state == 2 ? "DONE" : (state == 1 ? "ACTIVE" : "WAIT");
            const ImVec2 stateTextSize = labelFont->CalcTextSizeA(sectionLabelSize, FLT_MAX, 0.0f, stateText);
            draw->AddText(
                labelFont,
                sectionLabelSize,
                ImVec2(rowMax.x - stateTextSize.x - 14.0f, rowMin.y + (stepHeight - sectionLabelSize) * 0.5f - 1.0f),
                ImGui::GetColorU32(state == 2 ? ImVec4(0.9608f, 0.7725f, 0.0706f, 0.95f) : c::text::text),
                stateText
            );
        };

        drawStepRow(0, "License validation", progress >= 0.34f ? 2 : 1);
        drawStepRow(1, "Module synchronization", progress < 0.34f ? 0 : (progress >= 0.68f ? 2 : 1));
        drawStepRow(2, "Interface bootstrap", progress < 0.68f ? 0 : (progress >= 0.98f ? 2 : 1));

        const char* progressLabel = "Progress";
        draw->AddText(
            labelFont,
            sectionLabelSize,
            ImVec2(contentLeft, progressLabelY),
            ImGui::GetColorU32(c::text::text),
            progressLabel
        );

        char progressText[32] = {};
        std::snprintf(progressText, sizeof(progressText), "%d%%", (int)std::lround(progress * 100.0f));
        const ImVec2 progressTextSize = labelFont->CalcTextSizeA(sectionLabelSize, FLT_MAX, 0.0f, progressText);
        draw->AddText(
            labelFont,
            sectionLabelSize,
            ImVec2(contentRight - progressTextSize.x, progressLabelY),
            ImGui::GetColorU32(c::text::text_active),
            progressText
        );

        draw->AddRectFilled(
            ImVec2(contentLeft, progressBarY),
            ImVec2(contentLeft + progressBarW, progressBarY + progressBarH),
            IM_COL32(0, 0, 0, 214),
            progressBarH * 0.5f
        );
        draw->AddRectFilled(
            ImVec2(contentLeft, progressBarY),
            ImVec2(contentLeft + progressBarW * EaseOutCubic(progress), progressBarY + progressBarH),
            ImGui::GetColorU32(ImVec4(0.9608f, 0.7725f, 0.0706f, 0.92f)),
            progressBarH * 0.5f
        );
        const char* helper = "Securing modules and loading interface";
        draw->AddText(
            ImVec2(contentLeft, progressBarY + 20.0f),
            ImGui::GetColorU32(c::text::text),
            helper
        );

        const char* leftCaption = "Please wait";
        const ImVec2 leftCaptionSize = labelFont->CalcTextSizeA(sectionLabelSize, FLT_MAX, 0.0f, leftCaption);
        draw->AddText(
            labelFont,
            sectionLabelSize,
            ImVec2(innerMin.x + (leftPaneWidth - leftCaptionSize.x) * 0.5f, innerMax.y - 32.0f),
            ImGui::GetColorU32(c::text::text),
            leftCaption
        );
    }
    ImGui::End();

    loadingAnimationTimer += io.DeltaTime;
    if (loadingAnimationTimer >= loadingAnimationDuration) {
        showLoadingAnimation = false;
        loadingAnimationTimer = 0.0f;
        finished = true;
    }

    return finished;
}

} // namespace ui_loading