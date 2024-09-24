#define RAYGUI_IMPLEMENTATION

#include "player.h"

#include <iostream>
#include <sstream>
#include <filesystem>
#include <vector>

#include "raylib.h"
#include "raygui.h"

void Player::start() {
    InitWindow(800, 600, "Audio Player");
    SetExitKey(0);
    SetTargetFPS(60);

    InitAudioDevice();
    Music music = {};

    float duration = 0;
    std::string formattedDuration = formatSeconds(duration);
    float timePlayed = 0;

    Rectangle sliderBounds = {100, 600 - 25 - 50, 600, 25};

    int listIndex = -1;
    int currentIndex = -1;
    std::string listItems;
    std::vector<std::string> files;

    bool playing = false;
    bool paused = false;
    bool seeked = false;

    while (!WindowShouldClose()) {
        // Update
        if (playing) {
            UpdateMusicStream(music);
        }

        if (listIndex >= 0 && currentIndex != listIndex) {
            currentIndex = listIndex;
            music = LoadMusicStream(files[currentIndex].c_str());
            duration = GetMusicTimeLength(music);
            playing = true;
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(0, 0);

        // Play button
        if (GuiButton({100, 50, 100, 50}, "Play")) {
            if (paused) {
                ResumeMusicStream(music);
                playing = true;
                paused = false;
            } else if (!IsMusicStreamPlaying(music)) {
                PlayMusicStream(music);
                playing = true;
            }
        }

        // Pause button
        if (GuiButton({200 + 10, 50, 100, 50}, "Pause")) {
            if (paused) {
                ResumeMusicStream(music);
                playing = true;
                paused = false;
            } else {
                PauseMusicStream(music);
                playing = false;
                paused = true;
            }
        }

        // Stop button
        if (GuiButton({300 + 20, 50, 100, 50}, "Stop")) {
            // BUG: Doesn't reset the `GetMusicTimePlayed(music)` if stop was called while paused
            // Workaround is to play then stop
            if (paused) {
                PlayMusicStream(music);
            }

            StopMusicStream(music);
            playing = false;
            paused = false;
        }

        // Load music directory button
        if (GuiButton({600 - 50, 50, 150, 50}, "Load Music Directory")) {
            // FIXME: Windows
            const char *music = TextFormat("%s/Music", std::getenv("HOME"));
            if (DirectoryExists(music)) {
                listItems = "";
                files.clear();

                for (const auto &entry : std::filesystem::directory_iterator(music)) {
                    std::filesystem::path path = entry.path();
                    std::string filename = path.filename();
                    std::cout << filename << std::endl;
                    listItems += filename + ";";
                    files.push_back(entry.path());
                }

                // Remove last semi-colon
                if (listItems.length() > 0) {
                    listItems.pop_back();
                }
            }
        }

        // NOTE: Return value was updated in Raygui library
        // Playlist of tracks
        listIndex = GuiListView({100, 100 + 20, 600, 400 - 20}, listItems != "" ? listItems.c_str() : NULL, NULL, &listIndex);

        // Seeking and update slider
        if (seeked) {
            seeked = false;
            SeekMusicStream(music, timePlayed);
        }

        timePlayed = (!paused && !playing) ? 0 : GetMusicTimePlayed(music);
        std::string formattedTimePlayed = formatSeconds(timePlayed);

        // Seekbar and hover values
        Vector2 mousePosition = GetMousePosition();
        bool isHovering = CheckCollisionPointRec(mousePosition, sliderBounds);
        float hoverValue = (mousePosition.x - sliderBounds.x) * duration / sliderBounds.width;

        // Seek bar
        if (GuiSliderBar(sliderBounds, formattedTimePlayed.c_str(), formattedDuration.c_str(), &timePlayed, 0, duration)) {
            seeked = true;
            playing = false;

            if (isHovering) {
                timePlayed = hoverValue;
            }
        } else {
            seeked = false;
            playing = true;
        }

        // Tooltip on hover
        if (isHovering) {
            const char *tooltip = formatSeconds(hoverValue).c_str();
            Vector2 tooltipSize = MeasureTextEx(GetFontDefault(), tooltip, 20, 1);
            Rectangle bounds = {mousePosition.x + 5, mousePosition.y - tooltipSize.y - 15, tooltipSize.x + 15, tooltipSize.y + 10};
            DrawRectangleRec(bounds, DARKGRAY);
            DrawText(tooltip, bounds.x + 5, bounds.y + 5, 20, RAYWHITE);
        }
        EndDrawing();
    }

    StopMusicStream(music);
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
}

std::string Player::formatSeconds(float totalSeconds) {
    int minutes = static_cast<int>(totalSeconds) / 60;
    int seconds = static_cast<int>(totalSeconds) % 60;

    std::stringstream stream;
    stream << std::setw(2) << std::setfill('0') << minutes << ":"
        << std::setw(2) << std::setfill('0') << seconds;

    return stream.str();
}
