#include <raylib.h>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

namespace ryanUI{

   
    
    enum class Location {
        TOP_LEFT   ,TOP_MIDDLE   ,TOP_RIGHT,
        LEFT       ,MIDDLE       ,RIGHT,
        BOTTOM_LEFT,BOTTOM_MIDDLE,BOTTOM_RIGHT,
    };

    namespace Keyboard {
        constexpr KeyboardKey allKeys[] = {
            KEY_NULL,
            // Alphanumeric
            KEY_APOSTROPHE,
            KEY_COMMA,
            KEY_MINUS,
            KEY_PERIOD,
            KEY_SLASH,
            KEY_ZERO,
            KEY_ONE,
            KEY_TWO,
            KEY_THREE,
            KEY_FOUR,
            KEY_FIVE,
            KEY_SIX,
            KEY_SEVEN,
            KEY_EIGHT,
            KEY_NINE,
            KEY_SEMICOLON,
            KEY_EQUAL,
            KEY_A,
            KEY_B,
            KEY_C,
            KEY_D,
            KEY_E,
            KEY_F,
            KEY_G,
            KEY_H,
            KEY_I,
            KEY_J,
            KEY_K,
            KEY_L,
            KEY_M,
            KEY_N,
            KEY_O,
            KEY_P,
            KEY_Q,
            KEY_R,
            KEY_S,
            KEY_T,
            KEY_U,
            KEY_V,
            KEY_W,
            KEY_X,
            KEY_Y,
            KEY_Z,
            KEY_LEFT_BRACKET,
            KEY_BACKSLASH,
            KEY_RIGHT_BRACKET,
            KEY_GRAVE,
            // Function keys
            KEY_SPACE,
            KEY_ESCAPE,
            KEY_ENTER,
            KEY_TAB,
            KEY_BACKSPACE,
            KEY_INSERT,
            KEY_DELETE,
            KEY_RIGHT,
            KEY_LEFT,
            KEY_DOWN,
            KEY_UP,
            KEY_PAGE_UP,
            KEY_PAGE_DOWN,
            KEY_HOME,
            KEY_END,
            KEY_CAPS_LOCK,
            KEY_SCROLL_LOCK,
            KEY_NUM_LOCK,
            KEY_PRINT_SCREEN,
            KEY_PAUSE,
            KEY_F1,
            KEY_F2,
            KEY_F3,
            KEY_F4,
            KEY_F5,
            KEY_F6,
            KEY_F7,
            KEY_F8,
            KEY_F9,
            KEY_F10,
            KEY_F11,
            KEY_F12,
            KEY_LEFT_SHIFT,
            KEY_LEFT_CONTROL,
            KEY_LEFT_ALT,
            KEY_LEFT_SUPER,
            KEY_RIGHT_SHIFT,
            KEY_RIGHT_CONTROL,
            KEY_RIGHT_ALT,
            KEY_RIGHT_SUPER,
            KEY_KB_MENU,
            // Keypad
            KEY_KP_0,
            KEY_KP_1,
            KEY_KP_2,
            KEY_KP_3,
            KEY_KP_4,
            KEY_KP_5,
            KEY_KP_6,
            KEY_KP_7,
            KEY_KP_8,
            KEY_KP_9,
            KEY_KP_DECIMAL,
            KEY_KP_DIVIDE,
            KEY_KP_MULTIPLY,
            KEY_KP_SUBTRACT,
            KEY_KP_ADD,
            KEY_KP_ENTER,
            KEY_KP_EQUAL,
            // Android keys
            KEY_BACK,
            KEY_MENU,
            KEY_VOLUME_UP,
            KEY_VOLUME_DOWN
        };

        struct KeyState {
            bool pressed = false;      // is key currently down?
            float timer = 0.0f;        // how long held
        };

        struct Modifiers {
            bool shift = false;
            bool control = false;
            bool alt = false;
            bool super = false;
        };

        static std::vector<KeyboardKey> exclude = { // These are keys we want to exclude sending events for
            KEY_LEFT_ALT,KEY_LEFT_SHIFT,KEY_LEFT_CONTROL,KEY_LEFT_SUPER,KEY_RIGHT_ALT,KEY_RIGHT_SHIFT,KEY_RIGHT_CONTROL,KEY_RIGHT_SUPER,
            KEY_BACK,KEY_MENU,KEY_VOLUME_UP,KEY_VOLUME_DOWN };


        inline float key_repeat_delay = .23;
        inline float key_repeat_rate = 0.05f;    // repeat interval
        inline std::unordered_map<KeyboardKey,KeyState> key_states;   // track all pressed keys
        inline bool initialized = false;

        void Initialize();
    }



    namespace Event {
        enum class MouseButton{
            NONE,
            LEFT,
            RIGHT,
            MIDDLE,
        };

        struct MouseEvent {
            MouseButton btn = MouseButton::NONE;
            Vector2 relative_pos = {0,0};
        };

        struct ScrollEvent {
            Vector2 scroll_dir = {0,0};
            Vector2 relative_pos = {0,0};
        };

        struct DragEvent {
            MouseButton btn = MouseButton::NONE;
            Vector2 click_pos = {0,0};
            Vector2 current_mouse_pos = {0,0};
            Vector2 frame_delta = {0,0};
            Vector2 total_delta = {0,0};
        };

        struct KeyTypedEvent {
            KeyboardKey key;
            Keyboard::Modifiers mods;
        };
    }

    class Component {
    public:
        Vector2 offset = {0,0};
        Vector2 size = {0,0};

        int margin_left = 0;
        int margin_right = 0;
        int margin_top = 0;
        int margin_bottom = 0;
        
        int padding_top = 0;
        int padding_bottom = 0;
        int padding_left = 0;
        int padding_right = 0; 

        bool visible = true;
        Color background_color = WHITE;
        Color border_color = BLACK;
        int border_size = 0;

        float corner_radius = 0.0f;

        bool was_hovered = false;
        bool is_hovered = false;

        std::function<void(Component*,Event::MouseEvent)> on_click = nullptr;
        std::function<void(Component*,Event::MouseEvent)> on_hold = nullptr;
        std::function<void(Component*,Event::MouseEvent)> on_release = nullptr;
        std::function<void(Component*,Event::MouseEvent)> on_hover = nullptr;
        std::function<void(Component*)> on_hover_exit = nullptr;
        std::function<void(Component*,Event::ScrollEvent)> on_scroll = nullptr;
        bool is_drag_listener = false;
        std::function<void(Component*,Event::DragEvent)> on_drag = nullptr;

        std::function<void(Component*,Event::KeyTypedEvent)> on_type = nullptr;

        void Draw(Vector2 offset);
        void Update();

        void SetContentSize(Vector2 size);
        void SetInnerSize(Vector2 innerSize);
        void SetTotalSize(Vector2 totalSize);

        virtual void OnClick(Event::MouseEvent event);
        virtual void OnHold(Event::MouseEvent event);
        virtual void OnRelease(Event::MouseEvent event);
        virtual void OnHover(Event::MouseEvent event);
        virtual void OnHoverExit();
        virtual void OnScroll(Event::ScrollEvent event);
        virtual void OnDrag(Event::DragEvent event);
        virtual void OnType(Event::KeyTypedEvent event);
        virtual void MatchColliding(std::vector<Component*>& colliding,Vector2 point);

        virtual void Print();
        
        Vector2 GetTotalSize();
        Vector2 GetInnerSize();
        void SetMargin(int margin);
        void SetPadding(int padding);

        Component();
        Component(Vector2 size);
        Component(Vector2 offset, Vector2 size);
        virtual ~Component() = default;
    protected:
        virtual void render(Vector2 offset);
        virtual void resize(Vector2 new_size);
        virtual void update();
    };

    class Box : public Component{
    public:
        enum class GrowMode{ // THis is which direction it grows when adding
            FREE,
            RIGHT, 
            LEFT,
            UP,
            DOWN,
        };
        enum class FillMode { // This is how the size is set when adding elements
            FREE, // No change
            EVEN, // Makes each take up 1/n of the open area so 3 elements each take up 33%
            LAST_FILL, // Makes the last area stretch to the end
        };
        GrowMode grow_mode = GrowMode::FREE;
        FillMode fill_mode = FillMode::FREE;
        std::vector<std::shared_ptr<Component>> children;

        std::vector<std::shared_ptr<Component>> add_pending;
        std::vector<std::shared_ptr<Component>> remove_pending;

        virtual void AddChild(std::shared_ptr<Component> child);
        void AddChild(Component& child);
        void AddChild(Component* child);
        virtual void RemoveChild(std::shared_ptr<Component> child);
        void Recompute();
        void FlushPending();

        void OnClick(Event::MouseEvent event) override;
        void OnHold(Event::MouseEvent event) override;
        void OnRelease(Event::MouseEvent event) override;
        void OnHover(Event::MouseEvent event) override;
        void OnScroll(Event::ScrollEvent event) override;
        void MatchColliding(std::vector<Component*>& colliding,Vector2 point);

        void Print() override;

        void render(Vector2 offset) override;
        void resize(Vector2 size) override;
        void update() override;
        

        Box();
        Box(Vector2 offset, Vector2 size);
        Box(Vector2 size);

    };

    class Text : public Component {
    public:
        std::string text = "";
        Color font_color = BLACK;
        float font_size = 20;

        Vector2 GetTextSize();
        
        void render(Vector2 offset) override;

        Text(std::string text = "",float font_size = 20,Color font_color = BLACK);
    };

    class ScrollBar : public Component{
    public:
        float current_progress = 0;
        float bar_size = .2; // This is the portion of the size taken up so .5 would be half bar half space
        bool horizontal = false;

        Color bar_color = BLACK;

        void render(Vector2 offset) override;

        ScrollBar();
        ScrollBar(float height, bool horizontal = false);
        ScrollBar(Vector2 size, bool horizontal = false);

        static void onClick(Component* self,ryanUI::Event::MouseEvent click);
        static void onDrag(Component* self,ryanUI::Event::DragEvent drag);
    };

    class ScrollPane : public Box {
    public:
        Box content = Box();
        ScrollBar scroll_bar = ScrollBar();

        ScrollPane(Vector2 size);

        void AddChild(std::shared_ptr<Component> child);
        void render(Vector2 offset) override;
        void update() override;
    };

    class CheckBox : public Component {
    public:
        Color hover_selected_color = Color{104, 209, 96,255};
        Color selected_color = Color{80, 204, 71,255};
        Color selected_border_color = Color{71, 143, 66,255};
        Color hover_selected_border_color = Color{78, 158, 73,255};
        Color hover_deselected_color = Color{165, 170, 181,255};
        Color deselected_color = Color{183, 189, 201,255};
        Color deselected_border_color = Color{141, 143, 148,255};
        
        Color hover_deselected_border_color = Color{124, 125, 130,255};

        bool checked = false;
        CheckBox(Vector2 size, bool checked = false);
        CheckBox(Vector2 position,Vector2 size, bool checked = false);

        static void onHover(Component* self, Event::MouseEvent event);
        static void onHoverExit(Component* self);
        static void onClick(Component* self, Event::MouseEvent event);
    };

    class TextField : public Component {
    public:
        int max_width = -1; // -1 means no max width and to grow dynamically
        std::string text = "";
        float font_size = 20;
        Color font_color = BLACK;
        Color carat_color = WHITE;
        float carat_timer = 0;
        float carat_reset_time = 1; // Will spend 1/2 the time on 1/2 off Lower half is spend on
        
        std::function<void(Component*)> on_submit;

        TextField(std::string start = "", int max_width = 10,float font_size = 20,Color font_color = BLACK);

        void render(Vector2 offset);

        static void onType(Component* self, ryanUI::Event::KeyTypedEvent event);
    };

    void SetRoot(std::shared_ptr<Box> box);
    void SetRoot(Box& box);
    void HandleEvents();

    inline static std::shared_ptr<Box> root = nullptr;
    inline static Component* dragged_component = nullptr; // Cant use shared ptr because no access to its own shared ptr
    inline static Vector2 drag_start = {0,0};
    inline static Event::MouseButton drag_btn = Event::MouseButton::NONE;

    inline static Component* text_focus = nullptr;
};