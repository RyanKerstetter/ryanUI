#include "raylib.h"
#include "UI.h"
#include <iostream>
#include "Easing.h"

using namespace ryanUI;

int main(void)
{
    int window_width = 800;
    int window_height = 600;
    //SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(window_width, window_height, "Minimal Rendering Loop");
    ryanUI::Init(800,600);

    SetTargetFPS(60);

    
    
    std::shared_ptr<Box> root_box = GetRoot();

    std::shared_ptr<Box> top_bar = Create<Box>(Vector2{ 800,35 });
    top_bar->SetGrowMode(Box::GrowMode::RIGHT);
    top_bar->background_color = LIGHTGRAY;
    root_box->AddChild(top_bar);

    std::shared_ptr<Box> file_box = Create<Box>();
    file_box->background_color = LIGHTGRAY;
    file_box->SetGrowMode(Box::GrowMode::DOWN);
    std::vector<std::string> options = { "new","open","clone repository","start window","add","close","close folder","print" };
    file_box->fit_non_axis = false;
    for (std::string& str : options) {
        std::shared_ptr<TreeNode> text = Create<TreeNode>(str,1);
        text->background_color = LIGHTGRAY;
        text->border_color = BLACK;

        std::shared_ptr<Box> next_box = Create<Box>();
        file_box->background_color = LIGHTGRAY;
        file_box->SetGrowMode(Box::GrowMode::DOWN);
        for (std::string& str2 : options) {
            std::string new_str = str + "-" + str2;
            std::shared_ptr<Text> next = Create<Text>(str);
            next->background_color = LIGHTGRAY;
            next->border_color = BLACK;
            next_box->AddChild(next);
        }
        
        file_box->AddChild(text);
    }
    
    std::shared_ptr<TreeSource> file = Create<TreeSource>("file");
    file->background_color = LIGHTGRAY;
    file->border_size = 1;
    file->node_list = file_box;
    top_bar->AddChild(file);

    std::shared_ptr<Text> edit = Create<Text>("edit");
    edit->background_color = LIGHTGRAY;
    edit->border_size = 1;
    top_bar->AddChild(edit);
    
    
    /*
    std::shared_ptr<DropdownMenu> menu = Create<DropdownMenu>(std::vector<std::string>{ "Ryan","Sebastian","Jackson","Braxton","Will","Darius" });
    menu->background_color = LIGHTGRAY;
    menu->offset = { 100,100 };
    menu->on_select = [](DropdownMenu* menu) {
        std::cout << "Menu: " << menu->text->text << std::endl;
        };
    root_box->AddChild(menu);
    */
    
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        ryanUI::HandleEvents();
        ryanUI::Draw();


        EndDrawing();
    }

    CloseWindow();

    return 0;
}
