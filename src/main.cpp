#include "raylib.h"
#include "UI.h"
#include <iostream>

int main(void)
{
    int window_width = 800;
    int window_height = 600;
    InitWindow(window_width, window_height, "Minimal Rendering Loop");

    SetTargetFPS(60);

    Color hover_color = BLUE;
    Color default_color = WHITE;


    ryanUI::Box rootBox = ryanUI::Box({800,600});

    ryanUI::TextField tf = ryanUI::TextField("a");
    tf.background_color = LIGHTGRAY;
    tf.offset = {200,200};
    tf.fill_width = true;

    ryanUI::CheckBox cb = ryanUI::CheckBox(Vector2{30,30});
    rootBox.AddChild(cb);
    cb.offset = {300,300};

    rootBox.AddChild(tf);

    ryanUI::SetRoot(rootBox);

    rootBox.FlushPending();
    rootBox.Print();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        ryanUI::HandleEvents();
        rootBox.Draw({0,0});


        EndDrawing();
    }

    CloseWindow();

    return 0;
}
