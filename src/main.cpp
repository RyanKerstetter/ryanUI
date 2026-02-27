#include "raylib.h"
#include "UI.h"
#include <iostream>
#include "Easing.h"
#include <filesystem>
#include "Components.h"

namespace fs = std::filesystem;

using namespace ryanUI;
using namespace ryanUI::Components;

void set_sp(std::shared_ptr<ScrollPane> sp, fs::path p) {
    std::cout << "set_sp" << std::endl;
    for (std::shared_ptr<Component> c : sp->content->children) {
        sp->RemoveChild(c);
    }

    if (!p.parent_path().empty()) {
        std::shared_ptr<Text> up = Create<Text>("..");
        up->border_size = 1;
        up->on_click = [sp,p](Component* self, Event::MouseEvent event) {
            set_sp(sp, p.parent_path());
            };
        sp->AddChild(up);
    }
    float y_accum = 0;
    for (const auto entry : fs::directory_iterator(p)) {
        std::string file_name = entry.path().filename().string();
        std::shared_ptr<Text> text = Create<Text>(file_name);
        if (entry.is_directory()) {
            text->on_click = [p, sp, file_name](Component* self, Event::MouseEvent event) {
                fs::path new_path = p / file_name;
                set_sp(sp, new_path);
                };
        }
        sp->AddChild(text);
        y_accum += text->size.y;
    }

    sp->content->size.y = std::max(y_accum, sp->min_content_size.y);
};

int main(void)
{
    int window_width = 800;
    int window_height = 600;
    //SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(window_width, window_height, "Minimal Rendering Loop");
    ryanUI::Init(800,600);

    SetTargetFPS(60);

    
    fs::path path = fs::current_path();

    std::shared_ptr<Box> root_box = GetRoot();


    std::vector<std::string> options = { "new","open","clone repository","start window","add","close","close folder","print" };
    
    /*
    std::shared_ptr<ScrollPane> sp = Create<ScrollPane>(Vector2{ 700,200 });
    sp->offset = { 50,150 };
    sp->content->fit_non_axis = false;

    sp->SetGrowMode(Box::GrowMode::DOWN);
    sp->background_color = LIGHTGRAY;
    
    set_sp(sp, path);
    */

    std::shared_ptr<FileSelectDialog> fsd = Create<FileSelectDialog>(Vector2{ 600,400 }, path,nullptr);

    root_box->AddChild(fsd);
    
    
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLUE);
        ryanUI::HandleEvents();
        ryanUI::Draw();


        EndDrawing();
    }

    CloseWindow();

    return 0;
}
