#include "UI.h"
#include <algorithm>
#include <iostream>
#include <cmath>

ryanUI::Animation::Animation::Animation(float duration) 
    : duration(duration), timer(0) {};

void ryanUI::Animation::Animation::Update(float delta_time) {
    float new_timer = std::min(timer + delta_time, duration);
    float diff = new_timer - timer;
    timer = new_timer;

    if (diff != 0)
        update(timer / duration);
    if (timer == duration)
        finished = true;
}

ryanUI::Animation::ComponentAnimation::ComponentAnimation(float duration,Component* self, std::function<void(Component*,float)> func)
    :Animation(duration), self(self), func(func) {}

void ryanUI::Animation::ComponentAnimation::update(float t) {
    func(self, t);
}


void ryanUI::Component::Draw(Vector2 offset, float opacity){
    
    if(!visible)
        return;
    int start_x = offset.x + this->offset.x + margin_left;
    int start_y = offset.y + this->offset.y + margin_top;

    Vector2 innerSize = GetInnerSize();
    Rectangle rect = {start_x,start_y,innerSize.x,innerSize.y};

    Color bg_color_opacity = { background_color.r,background_color.g,background_color.b,background_color.a * opacity };
    Color border_color_opacity = { border_color.r,border_color.g,border_color.b,border_color.a * opacity };

    if(corner_radius == 0){
        if(background_color.a != BLANK.a)
            DrawRectangleRec(rect,bg_color_opacity);
        if(border_size > 0)
            DrawRectangleLinesEx(rect,border_size,border_color_opacity);
    } else { // Add rounded corners 
        float roundness = corner_radius / std::min(size.x,size.y);
        if(background_color.a != BLANK.a)
            DrawRectangleRounded(rect,roundness,8,bg_color_opacity);
        if(border_size > 0)
            DrawRectangleRoundedLinesEx(rect,roundness,8,border_size,border_color_opacity);
    }

    float new_opacity = opacity * this->opacity;
    Vector2 contentOffset = {static_cast<float>(start_x + padding_left + border_size),static_cast<float>(start_y + padding_top + border_size)};
    
    render(contentOffset,new_opacity);
}

void ryanUI::Component::SetContentSize(Vector2 size){
    resize(size);
    this->size = size;
    if(parent)
        parent->HandleChildSizeChange();
}

void ryanUI::Component::SetInnerSize(Vector2 innerSize){
    //std::cout << "Setting InnerSize: " << innerSize.x << "," << innerSize.y << " CurrentSize: " << size.x << "," << size.y << std::endl; 
    //std::cout << "Size:" << size.x << "," << size.y << std::endl;
    float diff_x = innerSize.x - size.x - border_size * 2;
    float diff_y = innerSize.y - size.y - border_size * 2;
    //std::cout << "diff_x:" << diff_x << " diff_y" << diff_y << std::endl;

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

    if (parent)
        parent->HandleChildSizeChange();
}

void ryanUI::Component::SetTotalSize(Vector2 totalSize){
    //std::cout << "Setting TotalSize: " << totalSize.x << "," << totalSize.y << " CurrentSize: " << size.x << "," << size.y << std::endl; 
    Vector2 innerSize = GetInnerSize();
    Vector2 ts = GetTotalSize();
    //std::cout << "TotalSize: " << ts.x << "," << ts.y << std::endl;
    //std::cout << "InnerSize: " << innerSize.x << "," << innerSize.y << std::endl;
    float diff_x = totalSize.x - innerSize.x;
    float diff_y = totalSize.y - innerSize.y;
    //std::cout << "diff_x:" << diff_x << " diff_y" << diff_y << std::endl;

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

    if (parent)
        parent->HandleChildSizeChange();
}

void ryanUI::Component::HandleChildSizeChange() {}

void ryanUI::Component::AddAnimation(std::shared_ptr<Animation::Animation> animation) {
    animations.push_back(animation);
}

bool ryanUI::Component::OnClick(ryanUI::Event::MouseEvent event){

    if(on_drag)
        dragged_component = this;
    if (on_type) {
        text_focus = this;
    }
    if (on_click) {
        on_click(this, event);
        return true;
    }
    return false;
}

bool ryanUI::Component::OnHold(ryanUI::Event::MouseEvent event){

    if(on_hold){
        on_hold(this,event);
        if(is_drag_listener){
            Vector2 mouse_pos = GetMousePosition();
            drag_start = mouse_pos;
            drag_btn = event.btn;
        }
        return true;
    }
    return false;
}

bool ryanUI::Component::OnRelease(ryanUI::Event::MouseEvent event){
    if (on_release) {
        on_release(this, event);
        return true;
    }
    return false;
}

bool ryanUI::Component::OnHover(ryanUI::Event::MouseEvent event){
    is_hovered = true;
    if(on_hover){
        on_hover(this,event);
        return true;
    }
    return false;
}

void ryanUI::Component::OnHoverExit(){
    if(on_hover_exit)
        on_hover_exit(this);
}

bool ryanUI::Component::OnScroll(ryanUI::Event::ScrollEvent event){
    if (on_scroll) {
        on_scroll(this, event);
        return true;
    }
    return false;
}

void ryanUI::Component::OnDrag(ryanUI::Event::DragEvent event){
    if(on_drag)
        on_drag(this,event);
}

void ryanUI::Component::OnType(ryanUI::Event::KeyTypedEvent event){
    if(on_type)
        on_type(this,event);
}

bool ryanUI::Component::IsInteractable(Vector2 offset) {
    return CheckCollisionPointRec(offset, { 0,0,size.x,size.y });
}

void ryanUI::Component::MatchColliding(std::vector<CollidingEntry>& colliding,Vector2 point){
    //std::cout << "MatchColliding:" << point.x << "," << point.y << std::endl;
    if (!visible)
        return;
    Vector2 innerSize = GetInnerSize();
    Rectangle rect = {0,0,innerSize.x,innerSize.y};
    if(CheckCollisionPointRec(point,rect)){
        colliding.push_back({ this,point });
    }
}

void ryanUI::Component::Update(){
    if(was_hovered && !is_hovered && on_hover_exit)
        on_hover_exit(this);
    was_hovered = is_hovered;
    is_hovered = false;

    for (std::shared_ptr<Animation::Animation>& animation : animations) {
        animation->Update(GetFrameTime());
    }

    animations.erase(
        std::remove_if(
            animations.begin(),
            animations.end(),
            [](const std::shared_ptr<Animation::Animation>& anim) { return anim->finished; }
        ),
        animations.end()
    );

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

Vector2 ryanUI::Component::GetGlobalOffset() {
    Vector2 self_offset = { offset.x + margin_left + border_size + padding_left, offset.y + margin_top + border_size + padding_top };
    if (!parent) {
        return self_offset;
    }
    Vector2 parent_offset = parent->GetGlobalOffset();
    return { self_offset.x + parent_offset.x,self_offset.y + parent_offset.y };
}

void ryanUI::Component::render(Vector2 offset, float opacity) {}

void ryanUI::Component::resize(Vector2 size){}

void ryanUI::Component::update() {};

ryanUI::Component::Component(){}

ryanUI::Component::Component(Vector2 size) :size(size){

}

ryanUI::Component::Component(Vector2 offset, Vector2 size) : offset(offset), size(size){

}

void ryanUI::Box::HandleChildSizeChange() {
    if (!computing)
        Recompute();
}

void ryanUI::Box::AddChild(std::shared_ptr<Component> child){
    children.push_back(child);
    child->parent = shared_from_this();
    Recompute();
}

void ryanUI::Box::RemoveChild(std::shared_ptr<Component> child){
    remove_pending.push_back(child);
}

void ryanUI::Box::SetGrowMode(GrowMode mode) {
    grow_mode = mode;
}

void ryanUI::Box::Recompute(){
    if(grow_mode == GrowMode::FREE){
        for (int i = 0; i < children.size(); i++) {
            std::shared_ptr<Component> child = children[i];
            Vector2 child_size = child->GetTotalSize();
            int min_x = child->offset.x;
            int max_x = child->offset.x + child_size.x;
            int min_y = child->offset.y;
            int max_y = child->offset.y + child_size.y;
            if (min_x < 0) {
                offset.x += min_x;
                size.x -= min_x;
            }
            if (min_y < 0) {
                offset.y += min_y;
                size.y -= min_y;
            }
            if (max_x > size.x)
                size.x = max_x;
            if (max_y > size.y)
                size.y = max_y;

        }
        return;
    }

    computing = true;

    int current_x = grow_mode == GrowMode::LEFT ? size.x : 0; 
    int current_y = grow_mode == GrowMode::UP ? size.y : 0;

    int grow_x = grow_mode == GrowMode::LEFT ? -1 : grow_mode == GrowMode::RIGHT ? 1 : 0; 
    int grow_y = grow_mode == GrowMode::UP ? -1 : grow_mode == GrowMode::DOWN ? 1 : 0;

    for(int i = 0 ;i < children.size();i++){
        std::shared_ptr<Component> child = children[i];
        Vector2 child_size = child->GetTotalSize();
        int pos_x = grow_x == -1 ? current_x - child_size.x : current_x;
        int pos_y = grow_y == -1 ? current_y - child_size.y : current_y;
        

        Vector2 offset = Vector2{ static_cast<float>(pos_x),static_cast<float>(pos_y)};
        child->offset = offset;

        if (grow_x != 0 && fit_non_axis) // This means it grows to the left or right
            child->SetInnerSize({ child_size.x,size.y });

        if(grow_y != 0 && fit_non_axis) // This means it grows up or down
            child->SetInnerSize({size.x,child_size.y}); // So we set the width to inherit from the box

        if (!fit_non_axis && child_size.x > size.x) {
            SetContentSize(Vector2{ child_size.x,size.y });
            Recompute();
            return;
        }
        if (!fit_non_axis && child_size.y > size.y) {
            SetContentSize(Vector2{ size.x,child_size.y });
            Recompute();
            return;
        }

        current_x += grow_x * child_size.x;
        current_y += grow_y * child_size.y;
        // If it is out of the bounds grow the box
        if(current_x > size.x)
            size.x = current_x;
        if(current_y > size.y)
            size.y = current_y;
        if(current_x < 0){
            this->offset.x += current_x;
            Vector2 new_size = { size.x - current_x,size.y };
            // This is inefficient since it calls parent->HandleChildSizeChange so for nested boxes it would recompute everything
            SetContentSize(new_size);
            Recompute(); // Scary recursion :eyes:
            return;
        }
        if(current_y < 0){
            this->offset.y += current_y;
            Vector2 new_size = { size.x,size.y - current_y };
            SetContentSize(new_size);
            parent->HandleChildSizeChange();
            Recompute();
            return;
        }
    }

    computing = false;
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

void ryanUI::Box::render(Vector2 offset, float opacity){

    //std::cout << "Box::Render" << std::endl;
    for(std::shared_ptr<Component> child: children){
        child->Draw(offset, opacity);
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

void ryanUI::Box::MatchColliding(std::vector<CollidingEntry>& colliding,Vector2 point ){
    Component::MatchColliding(colliding,point);
    for(std::shared_ptr<Component> child: children){
        Vector2 new_point = Vector2{
            point.x - child->offset.x - child->margin_left - child->border_size,
            point.y - child->offset.y - child->margin_top - child->border_size};

        child->MatchColliding(colliding,new_point);
    }
}


ryanUI::Box::Box() : Component() {}

ryanUI::Box::Box(Vector2 size) : Component({0,0},size){}

ryanUI::Box::Box(Vector2 offset,Vector2 size) : Component(offset,size){}


ryanUI::Text::Text(std::string text,float font_size,Color font_color) : Component(),text(text),font_size(font_size),font_color(font_color) {
    size = GetTextSize();
}

Vector2 ryanUI::Text::GetTextSize(){
    Font font = default_font;
    Vector2 measure = MeasureTextEx(font,text.c_str(),font_size,2);
    return measure;
}

void ryanUI::Text::render(Vector2 offset, float opacity){
    Font font = default_font;
    Color font_color_opacity = { font_color.r,font_color.g,font_color.b,font_color.a * opacity };

    DrawTextPro(font,text.c_str(),offset,{0,0},0,font_size,2,font_color_opacity);
}

ryanUI::Slider::Slider(Vector2 size, std::vector<float> discrete_values) : Component(size), values(discrete_values) {
    on_click = onClick;
    on_drag = onDrag;
}

ryanUI::Slider::Slider(float width, std::vector<float> discrete_values) : Slider(Vector2{width,20},discrete_values) {}

bool ryanUI::Slider::IsInteractable(Vector2 offset) {
    //std::cout << "Offset:" << offset.x << "," << offset.y << std::endl;
    Vector2 track_start = { 0,(size.y / 2) - (track_height / 2) };
    Rectangle track_bounds = { track_start.x,track_start.y,size.x,track_height };

    Vector2 dot_middle = { size.x * current_progress,(size.y / 2) };
    Vector2 diff = Vector2{ abs(offset.x - dot_middle.x),abs(offset.y - dot_middle.y) };
    bool collides_rect = CheckCollisionPointRec(offset, track_bounds);
    bool collides_circle = (diff.x * diff.x + diff.y * diff.y <= dot_radius * dot_radius);
    //std::cout << "Rect:" << collides_rect << " Circle:" << collides_circle << std::endl;
    if (!CheckCollisionPointRec(offset, track_bounds) && !(diff.x * diff.x + diff.y * diff.y <= dot_radius * dot_radius)) {
        return false;
    }
    return true;
}

void ryanUI::Slider::render(Vector2 offset, float opacity) {

    Color track_left_color_opacity = { track_left_color.r,track_left_color.g,track_left_color.b,track_left_color.a * opacity };
    Color track_right_color_opacity = { track_right_color.r,track_right_color.g,track_right_color.b,track_right_color.a * opacity };
    Color dot_color_opacity = { dot_color.r,dot_color.g,dot_color.b,dot_color.a * opacity };
    
    Vector2 track_start = { offset.x,offset.y + (size.y / 2) - (track_height / 2) };
    Vector2 track_middle = { offset.x + size.x * current_progress,track_start.y };
    Vector2 track_end = { offset.x + size.x,track_start.y };
    
    Rectangle left_rect = { track_start.x,track_start.y,current_progress * size.x,track_height };
    DrawRectangleRec(left_rect, track_left_color_opacity);
    Rectangle right_rect = { track_middle.x,track_start.y,(1 - current_progress) * size.x,track_height };
    DrawRectangleRec(right_rect, track_right_color_opacity);

    for (float v : values) {
        float x = offset.x + v * (size.x - tick_width);
        float y = offset.y + (size.y / 2) - tick_height / 2;
        Rectangle rect = Rectangle{ x,y,(float)tick_width,(float)tick_height };
        DrawRectangleRec(rect, v <= current_progress ? track_left_color_opacity : track_right_color_opacity);
    }

    Vector2 dot_middle = { offset.x + size.x * current_progress,offset.y + (size.y / 2) };
    DrawCircle(track_middle.x, track_middle.y + (track_height /2),dot_radius, dot_color_opacity);

    
}

void ryanUI::Slider::onClick(Component* self, ryanUI::Event::MouseEvent mouse) {
    std::cout << "OnClick: " << mouse.relative_pos.x << "," << mouse.relative_pos.y << std::endl;
    Slider* bar = static_cast<Slider*>(self);
    Vector2 track_start = { 0,(bar->size.y / 2) - (bar->track_height / 2) };
    Rectangle track_bounds = { track_start.x,track_start.y,bar->size.x ,bar->track_height };
    Vector2 dot_middle = { bar->size.x * bar->current_progress,(bar->size.y / 2) };
    Vector2 diff = Vector2{ abs(mouse.relative_pos.x - dot_middle.x),abs(mouse.relative_pos.y - dot_middle.y) };
    if (!CheckCollisionPointRec(mouse.relative_pos, track_bounds) && !(diff.x * diff.x + diff.y * diff.y < bar->dot_radius * bar->dot_radius)) {
        dragged_component = nullptr;
        return;
    }
    float progress_click = mouse.relative_pos.x / bar->size.x;
    float dot_percent = bar->dot_radius * 2 / bar->size.x;

    // This checks if the dot is clicked
    if (progress_click >= bar->current_progress - dot_percent && progress_click <= bar->current_progress - dot_percent) 
        return;
    bar->current_progress = progress_click;

    
}

void ryanUI::Slider::onDrag(Component* self, ryanUI::Event::DragEvent drag) {
    Slider* bar = static_cast<Slider*>(self);
    Vector2 global_offset = bar->GetGlobalOffset();
    bar->current_progress = std::clamp((drag.current_mouse_pos.x - global_offset.x) / bar->size.x, 0.0f, 1.0f);
    if (bar->values.size() == 0)
        return;
    float closest = bar->values[0];
    for (float v : bar->values) {
        float diff = abs(v - bar->current_progress);
        float closest_diff = abs(closest - bar->current_progress);
        if (diff < closest_diff)
            closest = v;
    }
    bar->current_progress = closest;
}

void ryanUI::ScrollBar::render(Vector2 offset, float opacity){

    Color bar_color_opacity = { bar_color.r,bar_color.g,bar_color.b,bar_color.a * opacity };
    
    if(horizontal){
        Vector2 bar_start = {offset.x + (current_progress * (1 - bar_size) * size.x),offset.y};
        Rectangle bar_rect = {bar_start.x,bar_start.y,bar_size * size.x,size.y};
        DrawRectangleRec(bar_rect,bar_color_opacity);
    } else {
        Vector2 bar_start = {offset.x, offset.y + (current_progress * (1 - bar_size) * size.y)};
        Rectangle bar_rect = {bar_start.x,bar_start.y,size.x,bar_size * size.y};
        DrawRectangleRec(bar_rect,bar_color_opacity);
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


ryanUI::ScrollPane::ScrollPane(Vector2 size) : Box(Vector2{size.x + 22,size.y}){
    scroll_bar = Create<ScrollBar>(size.y-2);
    scroll_bar->border_size = 1;
    content = Create<Box>(size);
    scroll_bar->offset.x = size.x;

    on_scroll = onScroll;
}

void ryanUI::ScrollPane::SetGrowMode(GrowMode mode) {
    content->grow_mode = mode;
}

void ryanUI::ScrollPane::Init() {
    std::cout << "ScrollPane Init" << std::endl;
    Box::AddChild(scroll_bar);
    Box::AddChild(content);
    FlushPending();
}

void ryanUI::ScrollPane::AddChild(std::shared_ptr<Component> child){
    content->AddChild(child);
}

void ryanUI::ScrollPane::RemoveChild(std::shared_ptr<Component> child) {
    content->RemoveChild(child);
}

void ryanUI::ScrollPane::update(){
    float bar_progress = scroll_bar->current_progress;
    float content_y_size = content->size.y;
    float ratio = size.y / content_y_size;
    scroll_bar->bar_size = std::clamp(ratio,.2f,.6f);
    content->offset.y = -1 * (content_y_size - size.y) * scroll_bar->current_progress;
}

void ryanUI::ScrollPane::render(Vector2 offset, float opacity){
    Rectangle innerRect = {offset.x,offset.y,size.x,size.y};
    BeginScissorMode(innerRect.x,innerRect.y,innerRect.width,innerRect.height);
    Box::render(offset, opacity);
    EndScissorMode();
}

void ryanUI::ScrollPane::onScroll(Component* self, Event::ScrollEvent event) {
    std::cout << "onScroll" << event.scroll_dir.y << std::endl;
    ScrollPane* sp = static_cast<ScrollPane*>(self);
    sp->scroll_bar->current_progress += event.scroll_dir.y / sp->content->size.y * -1 * 15;
    sp->scroll_bar->current_progress = std::clamp(sp->scroll_bar->current_progress, 0.0f, 1.0f);
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

bool ryanUI::Verifier::integer_verifier(const std::string& s) {
    if (s.empty()) return true; // allow empty text
    size_t start = 0;
    if (s[0] == '-') {
        if (s.size() == 1) return true; // just "-" is ok while typing
        start = 1;
    }
    for (size_t i = start; i < s.size(); ++i) {
        if (!std::isdigit(s[i])) return false;
    }
    return true;
}

// Verifier for floats (allows optional leading '-' and one '.')
bool ryanUI::Verifier::float_verifier(const std::string& s) {
    if (s.empty()) return true; // allow empty text
    size_t start = 0;
    bool dot_seen = false;
    if (s[0] == '-') {
        if (s.size() == 1) return true; // just "-" is ok while typing
        start = 1;
    }
    for (size_t i = start; i < s.size(); ++i) {
        if (s[i] == '.') {
            if (dot_seen) return false; // only one dot allowed
            dot_seen = true;
        }
        else if (!std::isdigit(s[i])) {
            return false;
        }
    }
    return true;
}

ryanUI::TextField::TextField(std::string text,int char_amount,float font_size, Color font_color)
 : Component(),max_width(char_amount),text(text),font_size(font_size),font_color(font_color) {
    on_type = onType;
    Font font = default_font;
    cursor_location = text.length();
    std::string biggest = "";
    for(int i = 0; i < char_amount;i++){
        biggest += "W"; // Use the widest character in most font sets
    } 
    size = MeasureTextEx(font,biggest.c_str(),font_size,2);
    padding_left = 4;
    padding_right = 4;
}

void ryanUI::TextField::render(Vector2 offset, float opacity){
    Font font = default_font;

    Color font_color_opacity = { font_color.r,font_color.g,font_color.b,font_color.a * opacity };
    DrawTextEx(font,text.c_str(),offset,font_size,2,font_color_opacity);
    std::string cursor_text = text.substr(0, cursor_location);
    Vector2 text_size = MeasureTextEx(font,cursor_text.c_str(),font_size,2);

    int carat_offset = cursor_location == text.size() ? 4 : 2;
    if(carat_timer < carat_reset_time / 2 && text_focus == this)
        DrawLine(offset.x + text_size.x+carat_offset, offset.y + 2,offset.x + text_size.x+carat_offset, offset.y + size.y - 4,carat_color);
    carat_timer += GetFrameTime();
    while(carat_timer > carat_reset_time)
        carat_timer -= carat_reset_time;
}

bool ryanUI::TextField::canAdd(char c) {
    if (fill_width) {
        std::string new_text = text;
        new_text.insert(cursor_location, 1, c);
        Vector2 text_size = MeasureTextEx(default_font, new_text.c_str(), font_size, 2);
        if (text_size.x >= size.x)
            return false;
        if (!verifier)
            return true;
        return verifier(new_text);
    }
    else {
        if (text.size() >= max_width)
            return false;
        if (!verifier)
            return true;
        std::string new_text = text;
        new_text.insert(cursor_location, 1, c);
        return verifier(new_text);
    }
}

void ryanUI::TextField::Clear() {
    text = "";
    cursor_location = 0;
}

void ryanUI::TextField::onType(Component* self, ryanUI::Event::KeyTypedEvent event){
    TextField* tf = static_cast<TextField*>(self);
    tf->carat_timer = 0; // Reset carat when typing
    if(event.key < 256){
        char c = (char) event.key;
        int text_size = tf->text.length();
        if (c >= KEY_A && c <= KEY_Z) {
            // In Upper need to go to lower
            int letter_offset = c - 'A';
            c = event.mods.shift ? letter_offset + 'A' : letter_offset + 'a';
        }

        if(tf->canAdd(c)){
            tf->text.insert(tf->cursor_location, 1, c);
            tf->cursor_location += 1;
        }
    }
    if(event.key == KEY_BACKSPACE){
        if(!tf->text.empty() && tf->cursor_location > 0){
            tf->text.erase(tf->cursor_location - 1, 1);
            tf->cursor_location -= 1;
        }
    }
    if (event.key == KEY_ENTER) {
        if(tf->on_submit)
            tf->on_submit(tf);
    }
    if (event.key == KEY_LEFT) {
        tf->cursor_location = std::clamp(tf->cursor_location - 1, 0,(int) tf->text.size());
    }
    if (event.key == KEY_RIGHT) {
        tf->cursor_location = std::clamp(tf->cursor_location + 1, 0, (int)tf->text.size());
    }
}

ryanUI::UIImage::UIImage(Texture2D texture, Vector2 size) : Component(size), texture(texture) {}

void ryanUI::UIImage::render(Vector2 offset, float opacity) {
    Rectangle rect = { offset.x,offset.y,size.x,size.y };
    Color tint = { 255,255,255,255 * opacity };
    DrawTexturePro(texture, { 0,0,(float)texture.width,(float)texture.height }, rect, { 0,0 }, 0, tint);
}

ryanUI::Switch::Switch(Vector2 size, float roundness) : Component(size) {
    on_hover = onHover;
    on_click = onClick;
    on_hover_exit = onHoverExit;

    background_color = on ? selected_color : deselected_color;

    corner_radius = size.y * roundness;
    dot_radius = (size.y / 2) * .8;
}

void ryanUI::Switch::render(Vector2 offset, float opacity) {
    Vector2 circle_pos = { offset.x,offset.y + size.y / 2 };
    if (on)
        circle_pos.x += size.x - size.y / 2;
    else 
        circle_pos.x += size.y / 2;
    Color dot_color_opacity = { dot_color.r,dot_color.g,dot_color.b,dot_color.a * opacity };
    Rectangle dot_rect = { circle_pos.x - dot_radius, circle_pos.y - dot_radius, dot_radius * 2, dot_radius * 2 };
    DrawRectangleRounded(dot_rect, corner_radius / size.y, 8, dot_color_opacity);
    //DrawCircleV(circle_pos, dot_radius, dot_color_opacity);
}   

void ryanUI::Switch::onClick(ryanUI::Component* self, ryanUI::Event::MouseEvent event) {
    Switch* sw = static_cast<Switch*>(self);
    sw->on = !sw->on;
    sw->background_color = sw->on ? sw->selected_color : sw->deselected_color;
}

void ryanUI::Switch::onHover(ryanUI::Component* self, ryanUI::Event::MouseEvent event) {
    Switch* sw = static_cast<Switch*>(self);

}

void ryanUI::Switch::onHoverExit(ryanUI::Component* self) {
    Switch* cb = static_cast<Switch*>(self);
}

ryanUI::Expander::Expander(Vector2 component_size) : Box(Vector2{ component_size.x + 16,component_size.y }) {
    down_arrow = Create<UIImage>(loaded_textures["down_arrow"], Vector2{ 16,16 });
    down_arrow->offset.x = component_size.x;
    down_arrow->visible = true;

    up_arrow = Create<UIImage>(loaded_textures["up_arrow"], Vector2{ 16,16 });
    up_arrow->offset.x = component_size.x;
    up_arrow->visible = false;

    down_arrow->on_click = [this](Component* self, Event::MouseEvent event) {
        if (!default_component || !expanded_component)
            return;
        Expand();
        };
    up_arrow->on_click = [this](Component* self, Event::MouseEvent event) {
        if (!default_component || !expanded_component)
            return;
        Contract();
        };
}

void ryanUI::Expander::Expand() {
    if(expanded_component)
        expanded_component->visible = true;
    if(default_component)
        default_component->visible = false;
    size.y = expanded_component->size.y;

    down_arrow->visible = false;
    up_arrow->visible = true;
}


void ryanUI::Expander::Contract() {
    if (expanded_component)
        expanded_component->visible = false;
    if (default_component)
        default_component->visible = true;
    size.y = default_component->size.y;

    down_arrow->visible = true;
    up_arrow->visible = false;
}

void ryanUI::Expander::SetDefaultComponent(std::shared_ptr<Component> comp) {
    if (default_component) {
        RemoveChild(default_component);
    }
    default_component = comp;
    AddChild(default_component);
}

void ryanUI::Expander::SetExpandedComponent(std::shared_ptr<Component> comp) {
    if (expanded_component)
        RemoveChild(expanded_component);
    expanded_component = comp;
    expanded_component->visible = false;
    AddChild(expanded_component);
}

void ryanUI::Expander::Init() {
    AddChild(up_arrow);
    AddChild(down_arrow);
}

ryanUI::DropdownMenu::DropdownMenu(std::vector<std::string> options, float font_size, Color font_color)
    : Box(), options(options) {
    float biggest_x = -1;
    for (std::string& str : options) {
        float x = MeasureTextEx(default_font, str.c_str(), 20, 2).x;
        biggest_x = std::max(x, biggest_x);
    }
    SetContentSize(Vector2{ biggest_x + 16,font_size });
    text = Create<Text>(options[0], font_size, font_color);
    SetSelection(0);
    down_arrow = Create<UIImage>(loaded_textures["down_arrow"], Vector2{ 16,16 });
    down_arrow->offset = Vector2{ biggest_x, (font_size - 16) / 2 };
    on_click = onClick;
}

void ryanUI::DropdownMenu::Init() {
    AddChild(text);
    AddChild(down_arrow);
}

void ryanUI::DropdownMenu::SetSelection(int selection) {
    if (selection >= 0 && selection < options.size()) {
        selected = selection;
        text->text = options[selected];
        if (on_select) {
            on_select(this);
        }
    }
}

void ryanUI::DropdownMenu::onClick(ryanUI::Component* self, ryanUI::Event::MouseEvent event) {
    DropdownMenu* dm = static_cast<DropdownMenu*>(self);
    std::shared_ptr<Box> popup = Create<Box>(Vector2{ dm->size.x,dm->size.y * dm->options.size() });
    popup->SetGrowMode(Box::GrowMode::DOWN);
    popup->border_size = 1;
    Vector2 global_offset = dm->GetGlobalOffset();
    popup->offset = { global_offset.x,global_offset.y + dm->size.y };

    for (int i = 0; i < dm->options.size();i++){
        std::string& str = dm->options[i];
        std::shared_ptr<Text> text = Create<Text>(str);
        text->SetInnerSize(Vector2{ dm->size.x,dm->size.y });
        text->background_color = dm->background_color;
        text->on_click = [i, dm](ryanUI::Component* self, ryanUI::Event::MouseEvent event) {
            dm->SetSelection(i);
            ClearPopup();
            };
        text->on_hover = [i, dm](ryanUI::Component* self, ryanUI::Event::MouseEvent event) {
            self->background_color = BLUE;
            };
        text->on_hover_exit = [i, dm](ryanUI::Component* self) {
            self->background_color = LIGHTGRAY;
            };
        popup->AddChild(text);
    }

    PopupEntry entry = PopupEntry{ true, true, popup };
    SetPopup(entry);
}

ryanUI::TreeSource::TreeSource(std::string str) : Text(str) {
    on_click = onClick;
}

void ryanUI::TreeSource::onClick(Component* self, Event::MouseEvent event) {
    TreeSource* ts = static_cast<TreeSource*>(self);
    Vector2 global_offset = ts->GetGlobalOffset();
    Vector2 total_size = ts->GetTotalSize();
    ts->node_list->offset = { global_offset.x, global_offset.y + total_size.y - ts->margin_top - ts->padding_top - ts->border_size };
    TreeNode::root->AddChild(ts->node_list);
    TreeNode::root->offset = Vector2{ 0,0 };
    PopupEntry entry = PopupEntry{ true,true,TreeNode::root };
    SetPopup(entry);
}

ryanUI::TreeNode::TreeNode(std::string str, int depth) : Text(str),depth(depth) {}

void ryanUI::TreeNode::onClick(Component* self, Event::MouseEvent event) {
    TreeNode* tn = static_cast<TreeNode*>(self);
    
    if (root->children.size() > tn->depth) { // Clicked back in the tree
        for (int i = root->children.size() - 1; i > tn->depth; i--) {
            root->children.pop_back();
        }
    }
    if (root->children.size() == tn->depth) { // This means it is the next one to be added
        Vector2 global_offset = tn->GetGlobalOffset();
        tn->node_list->offset = { global_offset.x + tn->size.x,global_offset.y };
        root->AddChild(tn->node_list);
    }
    else {}  // Clicked an option that shouldn't exist. This means there is an error somehwere. Probably set the depth values wrong

}

void ryanUI::TreeNode::rootOnClick(Component* self, Event::MouseEvent event) {
    ClearPopup();
}

void ryanUI::Init(int window_height, int window_width) {
    loaded_textures["down_arrow"] = LoadTexture("ryanUI/down-arrow.png");
    loaded_textures["up_arrow"] = LoadTexture("ryanUI/up-arrow.png");

    default_font = LoadFontEx("ryanUI/IBMPlexMono-Regular.ttf",20,0,0);

    root = Create<ryanUI::Box>(Vector2{ (float)window_height,(float)window_width });

    TreeNode::root = Create<Box>();
}

std::shared_ptr<ryanUI::Box> ryanUI::GetRoot(){
    return root;
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

    MouseCursor cursor = MOUSE_CURSOR_DEFAULT;
    
    static std::vector<Component::CollidingEntry> colliding;
    root->MatchColliding(colliding, mousePos);

    static std::vector<Component::CollidingEntry> popup_colliding;
    if (popup.component)
        popup.component->MatchColliding(popup_colliding, Vector2{ mousePos.x - popup.component->GetGlobalOffset().x,mousePos.y - popup.component->GetGlobalOffset().y });

    for(Component::CollidingEntry c: popup_colliding){
        std::cout << c.comp << std::endl;
        if ((c.comp->on_click || c.comp->on_type || c.comp->on_drag) && c.comp->IsInteractable(c.offset))
            cursor = MOUSE_CURSOR_POINTING_HAND;
    }
    for (Component::CollidingEntry c : colliding) {
        if ((c.comp->on_click || c.comp->on_type || c.comp->on_drag) && c.comp->IsInteractable(c.offset))
            cursor = MOUSE_CURSOR_POINTING_HAND;
    }

    SetMouseCursor(cursor);

    Event::MouseEvent hover_event;
    hover_event.relative_pos = mousePos;
    bool popup_captured = false;
    for (int i = popup_colliding.size() - 1; i >= 0; i--) {
        Component::CollidingEntry& c = popup_colliding[i];
        hover_event.relative_pos = c.offset;
        if (c.comp->OnHover(hover_event)) {
            popup_captured;
            break;
        }
    }
    for (int i = colliding.size() - 1; i >= 0 && !popup_captured; i--) {
        Component::CollidingEntry& c = colliding[i];
        hover_event.relative_pos = c.offset;
        if (c.comp->OnHover(hover_event))
            break;
    }

    Event::MouseEvent click_event;
    click_event.relative_pos = mousePos;
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        click_event.btn = Event::MouseButton::LEFT;
    
    
    if(click_event.btn != Event::MouseButton::NONE){
        text_focus = nullptr;
        bool popup_captured = false;
        for (int i = popup_colliding.size() - 1; i >= 0; i--) {
            Component::CollidingEntry& c = popup_colliding[i];
            click_event.relative_pos = c.offset;
            if (c.comp->OnClick(click_event)) {
                popup_captured = true;
                break;
            }
        }

        for (int i = colliding.size() - 1; i >= 0 && !popup_captured && !(popup.component && popup.block_inputs); i--) {
            Component::CollidingEntry& c = colliding[i];
            click_event.relative_pos = c.offset;
            if (c.comp->OnClick(click_event))
                break;
        }

        if (!popup_captured && popup.component && popup.close_on_outside_click)
            popup.component = nullptr;
    }

    Event::MouseEvent release_event;
    release_event.relative_pos = mousePos;
    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        release_event.btn = Event::MouseButton::LEFT;
    if(release_event.btn != Event::MouseButton::NONE){
        bool popup_captured = false;
        for (int i = popup_colliding.size() - 1; i >= 0; i--) {
            Component::CollidingEntry& c = popup_colliding[i];
            release_event.relative_pos = c.offset;
            if (c.comp->OnRelease(release_event)) {
                popup_captured = true;
                break;
            }
        }

        for (int i = colliding.size() - 1; i >= 0 && !popup_captured && !(popup.component && popup.block_inputs); i--) {
            Component::CollidingEntry& c = colliding[i];
            release_event.relative_pos = c.offset;
            if (c.comp->OnRelease(release_event))
                break;
        }
        
        dragged_component = nullptr;
    }

    Event::MouseEvent hold_event;
    hold_event.relative_pos = mousePos;
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        hold_event.btn = Event::MouseButton::LEFT;
    if (hold_event.btn != Event::MouseButton::NONE) {
        bool popup_captured = false;
        for (int i = popup_colliding.size() - 1; i >= 0; i--) {
            Component::CollidingEntry& c = popup_colliding[i];
            hold_event.relative_pos = c.offset;
            if (c.comp->OnHold(hold_event)){
                popup_captured = true;
                break;
            }
        }
        for (int i = colliding.size() - 1; i >= 0 && !popup_captured && !(popup.component && popup.block_inputs); i--) {
            Component::CollidingEntry& c = colliding[i];
            hold_event.relative_pos = c.offset;
            if (c.comp->OnHold(hold_event))
                break;
        }
    }

    Event::ScrollEvent scroll_event;
    scroll_event.relative_pos = mousePos;
    Vector2 scroll = GetMouseWheelMoveV();
    scroll_event.scroll_dir = scroll;
    if (scroll.x != 0 || scroll.y != 0) {
        bool popup_captured = false;
        for (int i = popup_colliding.size() - 1; i >= 0; i--) {
            Component::CollidingEntry& c = popup_colliding[i];
            click_event.relative_pos = c.offset;
            if (c.comp->OnScroll(scroll_event)){
                popup_captured = true;
                break;
            }
        }
        for (int i = colliding.size() - 1; i >= 0 && !popup_captured && !(popup.component && popup.block_inputs); i--) {
            Component::CollidingEntry& c = colliding[i];
            click_event.relative_pos = c.offset;
            if (c.comp->OnScroll(scroll_event))
                break;
        }
    }

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

    
    popup_colliding.clear();
    colliding.clear();

    if(temp_popup.component)
        popup = temp_popup;
    temp_popup.component = nullptr;
    root->Update();
    if (popup.component)
        popup.component->Update();
}

void ryanUI::Draw() {
    if (root)
        root->Draw({ 0,0 },1);
    if (popup.component) {
        popup.component->Draw({0,0 }, 1);
    }

}

void ryanUI::SetPopup(PopupEntry entry) {
    temp_popup = entry;
}

void ryanUI::ClearPopup() {
    popup = PopupEntry();
}