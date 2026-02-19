#include "UI.h"
#include <algorithm>
#include <iostream>
#include <cmath>

void ryanUI::Component::Draw(Vector2 offset){ // This is the correct offset already adding the childs offset
    //std::cout << "Drawing Component At: " << offset.x << "," << offset.y << std::endl;
    if(!visible)
        return;
    int start_x = offset.x + margin_left;
    int start_y = offset.y + margin_top;

    // std::cout << "Start : " << start_x << "," << start_y << std::endl;

    

    Vector2 innerSize = GetInnerSize();
    Rectangle rect = {start_x,start_y,innerSize.x,innerSize.y};

    if(corner_radius == 0){
        if(background_color.a != BLANK.a)
            DrawRectangleRec(rect,background_color);
        if(border_size > 0)
            DrawRectangleLinesEx(rect,border_size,border_color);
    } else { // Add rounded corners 
        float roundness = corner_radius / std::min(size.x,size.y);
        if(background_color.a != BLANK.a)
            DrawRectangleRounded(rect,roundness,8,background_color);
        if(border_size > 0)
            DrawRectangleRoundedLinesEx(rect,roundness,8,border_size,border_color);
    }

    
    Vector2 contentOffset = {static_cast<float>(start_x + padding_left + border_size),static_cast<float>(start_y + padding_top + border_size)};
    render(contentOffset);
}

void ryanUI::Component::SetContentSize(Vector2 size){
    resize(size);
    this->size = size;
}

void ryanUI::Component::SetInnerSize(Vector2 innerSize){
    std::cout << "Setting InnerSize: " << innerSize.x << "," << innerSize.y << " CurrentSize: " << size.x << "," << size.y << std::endl; 
    std::cout << "Size:" << size.x << "," << size.y << std::endl;
    float diff_x = innerSize.x - size.x - border_size * 2;
    float diff_y = innerSize.y - size.y - border_size * 2;
    std::cout << "diff_x:" << diff_x << " diff_y" << diff_y << std::endl;

    if( diff_x < 0 || diff_y < 0){
        std::cout << "Undefined operation setting innerSize < size" << std::endl;
        diff_x = 0;
        diff_y = 0;
        Vector2 new_inner_size = {innerSize.x - 2 * border_size, innerSize.y - 2 * border_size};
        SetContentSize(new_inner_size);
    }

    padding_left = static_cast<int>(diff_x / 2);
    padding_right = static_cast<int>(diff_x / 2);
    padding_top = static_cast<int>(diff_y / 2);
    padding_bottom = static_cast<int>(diff_y / 2);
}

void ryanUI::Component::SetTotalSize(Vector2 totalSize){
    std::cout << "Setting TotalSize: " << totalSize.x << "," << totalSize.y << " CurrentSize: " << size.x << "," << size.y << std::endl; 
    Vector2 innerSize = GetInnerSize();
    Vector2 ts = GetTotalSize();
    std::cout << "TotalSize: " << ts.x << "," << ts.y << std::endl;
    std::cout << "InnerSize: " << innerSize.x << "," << innerSize.y << std::endl;
    float diff_x = totalSize.x - innerSize.x;
    float diff_y = totalSize.y - innerSize.y;
    std::cout << "diff_x:" << diff_x << " diff_y" << diff_y << std::endl;

    if( diff_x < 0 || diff_y < 0){
        std::cout << "Undefined operation setting totalSize < size" << std::endl;
        diff_x = 0;
        diff_y = 0;
        SetInnerSize(totalSize);
    }

    margin_left = static_cast<int>(diff_x / 2);
    margin_right = static_cast<int>(diff_x / 2);
    margin_top = static_cast<int>(diff_y / 2);
    margin_bottom = static_cast<int>(diff_y / 2);
}

void ryanUI::Component::OnClick(ryanUI::Event::MouseEvent event){
    Vector2 innerSize = GetInnerSize();
    Rectangle rect = {0,0,innerSize.x,innerSize.y};
    if(CheckCollisionPointRec(event.relative_pos,rect)){
        std::cout << "Clicked" << std::endl;
        if(on_click)
            on_click(this,event);
        if(on_drag)
            dragged_component = this;
        if(on_type){
            text_focus = this;
        }
    }
}

void ryanUI::Component::OnHold(ryanUI::Event::MouseEvent event){
    Vector2 innerSize = GetInnerSize();
    Rectangle rect = {0,0,innerSize.x,innerSize.y};
    if(on_hold && CheckCollisionPointRec(event.relative_pos,rect)){
        on_hold(this,event);
        if(is_drag_listener){
            Vector2 mouse_pos = GetMousePosition();
            drag_start = mouse_pos;

            drag_btn = event.btn;
        }
    }
}

void ryanUI::Component::OnRelease(ryanUI::Event::MouseEvent event){
    Vector2 innerSize = GetInnerSize();
    Rectangle rect = {0,0,innerSize.x,innerSize.y};
    if(on_release && CheckCollisionPointRec(event.relative_pos,rect))
        on_release(this,event);
}

void ryanUI::Component::OnHover(ryanUI::Event::MouseEvent event){
    Vector2 innerSize = GetInnerSize();
    Rectangle rect = {0,0,innerSize.x,innerSize.y};
    if(on_hover && CheckCollisionPointRec(event.relative_pos,rect)){
        is_hovered = true;
        on_hover(this,event);
    }
}

void ryanUI::Component::OnHoverExit(){
    if(on_hover_exit)
        on_hover_exit(this);
}

void ryanUI::Component::OnScroll(ryanUI::Event::ScrollEvent event){
    Vector2 innerSize = GetInnerSize();
    Rectangle rect = {0,0,innerSize.x,innerSize.y};
    if(on_scroll && CheckCollisionPointRec(event.relative_pos,rect))
        on_scroll(this,event);
}

void ryanUI::Component::OnDrag(ryanUI::Event::DragEvent event){
    if(on_drag)
        on_drag(this,event);
}

void ryanUI::Component::OnType(ryanUI::Event::KeyTypedEvent event){
    if(on_type)
        on_type(this,event);
}

void ryanUI::Component::MatchColliding(std::vector<Component*>& colliding,Vector2 point){
    Vector2 innerSize = GetInnerSize();
    Rectangle rect = {0,0,innerSize.x,innerSize.y};
    if(CheckCollisionPointRec(point,rect)){
        colliding.push_back(this);
    }
}

void ryanUI::Component::Update(){
    if(was_hovered && !is_hovered)
        on_hover_exit(this);
    was_hovered = is_hovered;
    is_hovered = false;
    update();
}

void ryanUI::Component::Print(){
    std::cout << "Offset: " << offset.x << "," << offset.y << std::endl;
    std::cout << "Size: " << size.x << "," << size.y << std::endl;
    std::cout << "Margin: " << margin_left << "," << margin_top << ":" << margin_right << "," << margin_bottom << std::endl;
    std::cout << "Padding: " << padding_left << "," << padding_top << ":" << padding_right << "," << padding_bottom << std::endl;
}

Vector2 ryanUI::Component::GetInnerSize(){
    Vector2 innerSize = size;
    innerSize.x += padding_left + padding_right + border_size * 2;
    innerSize.y += padding_top + padding_bottom + border_size * 2;
    return innerSize;
}

Vector2 ryanUI::Component::GetTotalSize(){
    Vector2 innerSize = GetInnerSize();
    innerSize.x += margin_left + margin_right;
    innerSize.y += margin_top + margin_bottom;
    return innerSize;
}

void ryanUI::Component::SetMargin(int margin){
    margin_left = margin;
    margin_right = margin;
    margin_top = margin;
    margin_bottom = margin;
}

void ryanUI::Component::SetPadding(int padding){
    padding_left = padding;
    padding_right = padding;
    padding_top = padding;
    padding_bottom = padding;
}

void ryanUI::Component::render(Vector2 offset) {}

void ryanUI::Component::resize(Vector2 size){}

void ryanUI::Component::update() {};

ryanUI::Component::Component(){}

ryanUI::Component::Component(Vector2 size) :size(size){

}

ryanUI::Component::Component(Vector2 offset, Vector2 size) : offset(offset), size(size){

}


void ryanUI::Box::AddChild(std::shared_ptr<Component> child){
    children.push_back(child);
    Recompute();
}

void ryanUI::Box::AddChild(Component& child){
    AddChild(std::shared_ptr<Component>(&child));
}

void ryanUI::Box::AddChild(Component* child){
    AddChild(std::shared_ptr<Component>(child));
}

void ryanUI::Box::RemoveChild(std::shared_ptr<Component> child){
    remove_pending.push_back(child);
}

void ryanUI::Box::Recompute(){
    if(grow_mode == GrowMode::FREE){
        return;
    }

    int current_x = grow_mode == GrowMode::LEFT ? size.x : 0; 
    int current_y = grow_mode == GrowMode::UP ? size.y : 0;

    int grow_x = grow_mode == GrowMode::LEFT ? -1 : grow_mode == GrowMode::RIGHT ? 1 : 0; 
    int grow_y = grow_mode == GrowMode::UP ? -1 : grow_mode == GrowMode::DOWN ? 1 : 0;

    for(int i = 0 ;i < children.size();i++){
        std::shared_ptr<Component> child = children[i];
        Vector2 childSize = child->GetTotalSize();
        int pos_x = grow_x == -1 ? current_x - childSize.x : current_x;
        int pos_y = grow_y == -1 ? current_y - childSize.y : current_y;
        

        Vector2 offset = Vector2{ static_cast<float>(pos_x),static_cast<float>(pos_y)};
        child->offset = offset;

        if(grow_x != 0) // This means it grows to the left or right
            child->SetTotalSize({childSize.x,size.y});
        if(grow_y != 0) // This means it grows up or down
            child->SetTotalSize({size.x,childSize.y}); // So we set the width to inherit from the box


        current_x += grow_x * childSize.x;
        current_y += grow_y * childSize.y;
        // If it is out of the bounds grow the box
        if(current_x > size.x)
            size.x = current_x;
        if(current_y > size.y)
            size.y = current_y;
        if(current_x < 0){
            this->offset.x += current_x;
            this->size.x -= current_x;
            Recompute(); // Scary recursion :eyes:
            return;
        }
        if(current_y < 0){
            this->offset.y += current_y;
            this->size.y -= current_y;
            Recompute();
            return;
        }

    }
}

void ryanUI::Box::FlushPending(){
    if(add_pending.size() == 0 && remove_pending.size() == 0)
        return;

    for(std::shared_ptr<Component> pending: add_pending)
        children.push_back(pending);
    add_pending.clear();

    for(std::shared_ptr<Component> pending: remove_pending)
        children.erase(std::remove(children.begin(),children.end(),pending),children.end());
    remove_pending.clear();

    Recompute();
}

void ryanUI::Box::render(Vector2 offset){
    //std::cout << "Box::Render" << std::endl;
    for(std::shared_ptr<Component> child: children){
        Vector2 new_offset = Vector2{
            offset.x + child->offset.x,
            offset.y + child->offset.y};
        child->Draw(new_offset);
    }
}

void ryanUI::Box::resize(Vector2 size){
    this->size = size;
    Recompute();
}

void ryanUI::Box::update(){
    for(std::shared_ptr<Component> child:children){
        child->Update();
    }

    FlushPending();
}

void ryanUI::Box::Print(){
    Component::Print();
    std::cout << "--- Children ---" << std::endl;
    for(std::shared_ptr<Component> child : children){
        child->Print();
    }
    std::cout << "--- End ---" << std::endl;
}

void ryanUI::Box::OnClick(ryanUI::Event::MouseEvent event){
    Component::OnClick(event);
    ryanUI::Event::MouseEvent new_event = event;
    for(std::shared_ptr<Component> child: children){
        Vector2 new_offset = Vector2{
            event.relative_pos.x - child->offset.x - child->margin_left - child->border_size,
            event.relative_pos.y - child->offset.y - child->margin_top - child->border_size};
        new_event.relative_pos = new_offset;

        child->OnClick(new_event);
    }

    FlushPending();
}

void ryanUI::Box::OnRelease(ryanUI::Event::MouseEvent event){
    Component::OnRelease(event);
    ryanUI::Event::MouseEvent new_event = event;
    for(std::shared_ptr<Component> child: children){
        Vector2 new_offset = Vector2{
            event.relative_pos.x - child->offset.x - child->margin_left - child->border_size,
            event.relative_pos.y - child->offset.y - child->margin_top - child->border_size};
        new_event.relative_pos = new_offset;
        child->OnRelease(new_event);
    }

    FlushPending();
}

void ryanUI::Box::OnHold(ryanUI::Event::MouseEvent event){
    Component::OnHold(event);
    ryanUI::Event::MouseEvent new_event = event;
    for(std::shared_ptr<Component> child: children){
        Vector2 new_offset = Vector2{
            event.relative_pos.x - child->offset.x - child->margin_left - child->border_size,
            event.relative_pos.y - child->offset.y - child->margin_top - child->border_size};
        new_event.relative_pos = new_offset;
        child->OnHold(new_event);
    }
    
    FlushPending();
}

void ryanUI::Box::OnHover(ryanUI::Event::MouseEvent event){
    Component::OnHover(event);
    ryanUI::Event::MouseEvent new_event = event;
    for(std::shared_ptr<Component> child: children){
        Vector2 new_offset = Vector2{
            event.relative_pos.x - child->offset.x - child->margin_left - child->border_size,
            event.relative_pos.y - child->offset.y - child->margin_top - child->border_size};
        new_event.relative_pos = new_offset;
        child->OnHover(new_event);
    }

    FlushPending();
}

void ryanUI::Box::OnScroll(ryanUI::Event::ScrollEvent event){
    Component::OnScroll(event);
    ryanUI::Event::ScrollEvent new_event = event;
    for(std::shared_ptr<Component> child: children){
        Vector2 new_offset = Vector2{
            event.relative_pos.x - child->offset.x - child->margin_left - child->border_size,
            event.relative_pos.y - child->offset.y - child->margin_top - child->border_size};
        new_event.relative_pos = new_offset;
        child->OnScroll(new_event);
    }

    FlushPending();
}

void ryanUI::Box::MatchColliding(std::vector<Component*>& colliding,Vector2 point ){
    Component::MatchColliding(colliding,point);
    for(std::shared_ptr<Component> child: children){
        Vector2 new_point = Vector2{
            point.x - child->offset.x - child->margin_left - child->border_size,
            point.y - child->offset.y - child->margin_top - child->border_size};

        child->MatchColliding(colliding,point);
    }
}


ryanUI::Box::Box() : Component() {}

ryanUI::Box::Box(Vector2 size) : Component({0,0},size){}

ryanUI::Box::Box(Vector2 offset,Vector2 size) : Component(offset,size){}


ryanUI::Text::Text(std::string text,float font_size,Color font_color) : Component(),text(text),font_size(font_size),font_color(font_color) {
    size = GetTextSize();
}

Vector2 ryanUI::Text::GetTextSize(){
    Font font = GetFontDefault();
    Vector2 measure = MeasureTextEx(font,text.c_str(),font_size,2);
    return measure;
}

void ryanUI::Text::render(Vector2 offset){
    //std::cout << "Text::Render:" << std::endl;
    Font font = GetFontDefault();
    DrawTextPro(font,text.c_str(),offset,{0,0},0,font_size,2,font_color);
}


void ryanUI::ScrollBar::render(Vector2 offset){
    
    if(horizontal){
        Vector2 bar_start = {offset.x + (current_progress * (1 - bar_size) * size.x),offset.y};
        Rectangle bar_rect = {bar_start.x,bar_start.y,bar_size * size.x,size.y};
        DrawRectangleRec(bar_rect,bar_color);
    } else {
        Vector2 bar_start = {offset.x, offset.y + (current_progress * (1 - bar_size) * size.y)};
        Rectangle bar_rect = {bar_start.x,bar_start.y,size.x,bar_size * size.y};
        DrawRectangleRec(bar_rect,bar_color);
    }
    
}

void ryanUI::ScrollBar::onClick(Component* self, ryanUI::Event::MouseEvent mouse){
    ScrollBar* bar = static_cast<ScrollBar*>(self);
    if(bar->horizontal){
        float percent = mouse.relative_pos.x / (bar->size.x * (1 - bar->bar_size));
        if(percent < bar->current_progress || percent > bar->current_progress + bar->bar_size){
            bar->current_progress = std::clamp(percent - (bar->bar_size / 2.0),0.0,1.0);
        }
    } else {
        float percent = mouse.relative_pos.y / (bar->size.y * (1 - bar->bar_size));
        if(percent < bar->current_progress || percent > bar->current_progress + bar->bar_size){
            bar->current_progress = std::clamp(percent - (bar->bar_size / 2.0),0.0,1.0);
        }
    }
    

    
}

void ryanUI::ScrollBar::onDrag(Component* self, ryanUI::Event::DragEvent drag){
    ScrollBar* bar = static_cast<ScrollBar*>(self);
    if(bar->horizontal)
        bar->current_progress = std::clamp(bar->current_progress + (drag.frame_delta.x / (1 - bar->bar_size) / bar->size.x),0.0f,1.0f);
    else
        bar->current_progress = std::clamp(bar->current_progress + (drag.frame_delta.y / (1 - bar->bar_size) / bar->size.y),0.0f,1.0f);
}

ryanUI::ScrollBar::ScrollBar() : Component() {}

ryanUI::ScrollBar::ScrollBar(Vector2 size, bool horizontal) : Component({0,0},size),horizontal(horizontal) {
    on_drag = onDrag;
    on_click = onClick;
    is_drag_listener = true;
}

ryanUI::ScrollBar::ScrollBar(float size,bool h) : ScrollBar(Vector2{h ? size : 20,h ? 20 : size},h) {}

ryanUI::ScrollPane::ScrollPane(Vector2 size) : Box(Vector2{size.x + 20,size.y}){
    scroll_bar = ScrollBar(size.y);
    scroll_bar.border_size = 1;
    content = Box(size);
    grow_mode = GrowMode::RIGHT;
    Box::AddChild(std::shared_ptr<Component>(&scroll_bar));
    Box::AddChild(std::shared_ptr<Component>(&content));
    FlushPending();
}

void ryanUI::ScrollPane::AddChild(std::shared_ptr<Component> child){
    content.AddChild(child);
}

void ryanUI::ScrollPane::update(){
    float bar_progress = scroll_bar.current_progress;
    float content_y_size = content.size.y;
    float ratio = size.y / content_y_size;
    scroll_bar.bar_size = std::clamp(ratio,.2f,.6f);
    content.offset.y = -1 * (content_y_size - size.y) * scroll_bar.current_progress;
}

void ryanUI::ScrollPane::render(Vector2 offset){
    Rectangle innerRect = {offset.x,offset.y,size.x,size.y};
    BeginScissorMode(innerRect.x,innerRect.y,innerRect.width,innerRect.height);
    Box::render(offset);
    EndScissorMode();
}

ryanUI::CheckBox::CheckBox(Vector2 position,Vector2 size, bool checked) : Component(position,size), checked(checked) {
    on_click = onClick;
    on_hover = onHover;
    on_hover_exit = onHoverExit;
    border_size = 2;
    corner_radius = 12;

    background_color = checked ? selected_color : deselected_color;
    border_color = checked ? selected_color : deselected_border_color;
}

ryanUI::CheckBox::CheckBox(Vector2 size, bool checked) : CheckBox(Vector2{0,0},size,checked){}



void ryanUI::CheckBox::onClick(ryanUI::Component* self, ryanUI::Event::MouseEvent event){
    CheckBox* cb = static_cast<CheckBox*>(self);
    cb->checked = !cb->checked;
    cb->background_color = cb->checked ? cb->selected_color : cb->deselected_color;
}

void ryanUI::CheckBox::onHover(ryanUI::Component* self, ryanUI::Event::MouseEvent event){
    CheckBox* cb = static_cast<CheckBox*>(self);
    cb->background_color = cb->checked ? cb->hover_selected_color : cb->hover_deselected_color;
    cb->border_color = cb->checked ? cb->hover_selected_border_color : cb->hover_deselected_border_color;
}

void ryanUI::CheckBox::onHoverExit(ryanUI::Component* self){
    CheckBox* cb = static_cast<CheckBox*>(self);
    cb->background_color = cb->checked ? cb->selected_color : cb->deselected_color;
    cb->border_color = cb->checked ? cb->selected_border_color : cb->deselected_border_color;
}

ryanUI::TextField::TextField(std::string text,int char_amount,float font_size, Color font_color)
 : Component(),max_width(char_amount),text(text),font_size(font_size),font_color(font_color) {
    on_type = onType;
    Font font = GetFontDefault();
    std::string biggest = "";
    for(int i = 0; i < char_amount;i++){
        biggest += "W"; // Use the widest character in most font sets
    } 
    size = MeasureTextEx(font,biggest.c_str(),font_size,2);
    padding_left = 4;
    padding_right = 4;
}

void ryanUI::TextField::render(Vector2 offset){
    Font font = GetFontDefault();
    DrawTextEx(font,text.c_str(),offset,font_size,2,font_color);
    Vector2 text_size = MeasureTextEx(font,text.c_str(),font_size,2);

    if(carat_timer < carat_reset_time / 2 && text_focus == this)
        DrawLine(offset.x + text_size.x+4, offset.y + 2,offset.x + text_size.x+4, offset.y + size.y - 4,carat_color);
    carat_timer += GetFrameTime();
    while(carat_timer > carat_reset_time)
        carat_timer -= carat_reset_time;
}

void ryanUI::TextField::onType(Component* self, ryanUI::Event::KeyTypedEvent event){
    std::cout << "OnType" << (int) event.key << std::endl;
    TextField* tf = static_cast<TextField*>(self);

    if(event.key < 256){
        char c = (char) event.key;
        int text_size = tf->text.length();
        if(text_size < tf->max_width){
            tf->text += c;
        }
    }
    if(event.key == KEY_BACKSPACE){
        if(!tf->text.empty()){
            tf->text.pop_back();
        }
    }
}



void ryanUI::SetRoot(std::shared_ptr<Box> box){
    root = box;
}

void ryanUI::SetRoot(Box& box){
    root = std::shared_ptr<Box>(&box);
}

void ryanUI::Keyboard::Initialize(){
    if(initialized)
        return;
    
    for(KeyboardKey key: allKeys){
        if(std::find(exclude.begin(),exclude.end(),key) != exclude.end())
            continue;
        key_states[key] = KeyState();
    }

    initialized = true;
}

void ryanUI::HandleEvents(){
    if(!root)
        return;

    if(!Keyboard::initialized)
        Keyboard::Initialize();

    Vector2 mousePos = GetMousePosition();

    //SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    std::vector<Component*> colliding;
    root->MatchColliding(colliding,mousePos);
    for(Component* c: colliding){
        std::cout << c << std::endl;
        if(c->on_click)
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    }

    Event::MouseEvent hover_event;
    hover_event.relative_pos = mousePos;
    root->OnHover(hover_event);

    Event::MouseEvent click_event;
    click_event.relative_pos = mousePos;
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        click_event.btn = Event::MouseButton::LEFT;
    if(click_event.btn != Event::MouseButton::NONE){

        text_focus = nullptr;
        root->OnClick(click_event);
    }

    Event::MouseEvent release_event;
    release_event.relative_pos = mousePos;
    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        release_event.btn = Event::MouseButton::LEFT;
    if(release_event.btn != Event::MouseButton::NONE){
        root->OnRelease(release_event);
        dragged_component = nullptr;
    }

    Event::MouseEvent hold_event;
    hold_event.relative_pos = mousePos;
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        hold_event.btn = Event::MouseButton::LEFT;
    if(hold_event.btn != Event::MouseButton::NONE)
        root->OnHold(hold_event);

    Event::ScrollEvent scroll_event;
    scroll_event.relative_pos = mousePos;
    Vector2 scroll = GetMouseWheelMoveV();
    if(scroll.x != 0 && scroll.y != 0)
        root->OnScroll(scroll_event);

    if(dragged_component){
        Vector2 frame_delta = GetMouseDelta();
        Vector2 total_delta = {mousePos.x - drag_start.x,mousePos.y - drag_start.y};
        ryanUI::Event::DragEvent drag_event = {drag_btn,drag_start,mousePos,frame_delta,total_delta};
        dragged_component->OnDrag(drag_event);
    }

    if(text_focus){
        Keyboard::Modifiers mods;
        mods.shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        mods.control = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
        mods.alt = IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
        mods.super = IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER);
        std::vector<Event::KeyTypedEvent> events;
        for( const auto& [key,keystate] : Keyboard::key_states){
            bool is_down = IsKeyDown(key);
            if(is_down){
                if(!keystate.pressed){// If the keys wasn't pressed before
                    events.push_back(Event::KeyTypedEvent{key,mods});
                    Keyboard::key_states[key].timer = Keyboard::key_repeat_delay;
                } else {// If its being held
                    float delta_time = GetFrameTime();
                    Keyboard::key_states[key].timer -= delta_time;
                    while(Keyboard::key_states[key].timer < 0){
                        events.push_back(Event::KeyTypedEvent{key,mods});
                        Keyboard::key_states[key].timer += Keyboard::key_repeat_rate;
                    }
                }
            } else {
                Keyboard::key_states[key].timer = 0;
            }

            Keyboard::key_states[key].pressed = is_down;
        }

        for(Event::KeyTypedEvent event: events){
            text_focus->OnType(event);
        }
    }

    
    
    

    root->Update();
}