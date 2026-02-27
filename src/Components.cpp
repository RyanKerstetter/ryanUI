#include "Components.h"
#include <iostream>

namespace ryanUI::Components {
	void FileSelectDialog::SetPath(fs::path p) {
        for (std::shared_ptr<Component> c : scroll_pane->content->children) {
            scroll_pane->content->RemoveChild(c);
        }

        if (!p.parent_path().empty()) {
            std::shared_ptr<Text> up = Create<Text>("..");
            up->border_size = 1;
            up->on_click = [this, p](Component* self, Event::MouseEvent event) {
                SetPath(p.parent_path());
                };
            scroll_pane->AddChild(up);
        }
        float y_accum = 0;
        for (const auto entry : fs::directory_iterator(p)) {
            std::string file_name = entry.path().filename().string();
            std::shared_ptr<Text> text = Create<Text>(file_name);
            if (entry.is_directory()) {
                text->on_click = [p, this, file_name](Component* self, Event::MouseEvent event) {
                    fs::path new_path = p / file_name;
                    SetPath(new_path);
                    };
                text->font_color = BLUE;
            }
            else if (entry.is_regular_file()) {
                text->on_click = [p, this, file_name](Component* self, Event::MouseEvent event) {
                    fs::path new_path = p / file_name;
                    selected_text->text = file_name;
                    path_to_file = new_path;
                    submit_button->background_color = GREEN;
                    };
                text->font_color = GREEN;

            }
            scroll_pane->AddChild(text);
            y_accum += text->size.y;
        }

        scroll_pane->content->size.y = std::max(y_accum, scroll_pane->min_content_size.y);
	}

	FileSelectDialog::FileSelectDialog(Vector2 size, fs::path path, std::function<void(fs::path)> cb)
        : Box(Vector2{ size.x + 35,size.y + 50 }), current_path(path), callback(cb) {
		scroll_pane = Create<ScrollPane>(size);
        scroll_pane->offset = { 15,10 };
        scroll_pane->content->fit_non_axis = false;
        background_color = LIGHTGRAY;
        scroll_pane->SetGrowMode(Box::GrowMode::DOWN);
        scroll_pane->border_size = 1;
        
        selected_text = Create<Text>("",20);
        selected_text->SetContentSize(Vector2{ size.x - 115,20 });
        selected_text->offset = Vector2{ 15,size.y + 20 };
        selected_text->background_color = RAYWHITE;
        selected_text->border_size = 1;

        submit_button = Create<Text>("Submit");
        submit_button->offset = Vector2{ size.x - 100 + (50 - submit_button->size.x / 2),size.y + 20 };
        submit_button->border_size = 1;


        submit_button->background_color = LIGHTGRAY;
        submit_button->on_click = [this](Component* self, Event::MouseEvent event) {
            if (callback)
                callback(path_to_file);
            ClearPopup();
            };
        
	}

    void FileSelectDialog::Init() {
        AddChild(scroll_pane);
        AddChild(selected_text);
        AddChild(submit_button);
        SetPath(current_path);
    }
}