#pragma once
#include <raylib.h>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

namespace ryanUI{

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
        inline std::unordered_map<char,char> shift_map = { 
            {'1','!'},
            {'2','@'},
            {'3','#'},
            {'4','$'},
            {'5','%'},
            {'6','^'},
            {'7','&'},
            {'8','*'},
            {'9','('},
            {'0',')'},
            {'-','_'},
            {'=','+'},
            {'[','{'},
            {']','}'},
            {'\\','|'},
            {';',':'},
            {'\'','"'},
            {',','<'},
            {'.','>'},
            {'/','?'},
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

    class Component;

    namespace Animation {
        class Animation {
        public:
            float duration = 0.0f;
            float timer = 0.0f;
            bool finished = false;

            Animation(float duration);
            void Update(float delta_time);
            virtual void update(float t) = 0;
            virtual ~Animation() = default;
        };

        class ComponentAnimation : public Animation {
        public:
            std::function<void(Component*, float)> func;
            Component* self;

            ComponentAnimation(float duration, Component* self, std::function<void(Component*, float)> func);
            void update(float t) override;
        };

        template<typename T>
        class PropertyAnimation : public Animation {
        public:
            T* target;
            T start_value;
            T end_value;
            std::function<float(float)> easing;

            PropertyAnimation(T* target, T start, T end, float duration, std::function<float(float)> easing)
                : Animation(duration), target(target), start_value(start), end_value(end), easing(easing) {
            };

            void update(float t) override {
                if (t < 0.0f) return;
                float e = easing(t);
                T diff = end_value - start_value;
                *target = (e * diff) + start_value;
            }
        };
    }

    class Component {
    public:
        std::shared_ptr<Component> parent;

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
        float opacity = 1.0f;
        Color background_color = BLANK;
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

        std::vector<std::shared_ptr<Animation::Animation>> animations;

        std::unordered_map<std::string, std::string> data;

        void Draw(Vector2 offset, float opacity);
        void Update();

        void SetContentSize(Vector2 size);
        void SetInnerSize(Vector2 innerSize);
        void SetTotalSize(Vector2 totalSize);
        virtual void HandleChildSizeChange();

        void AddAnimation(std::shared_ptr<Animation::Animation> animation);

        virtual bool OnClick(Event::MouseEvent event);
        virtual bool OnHold(Event::MouseEvent event);
        virtual bool OnRelease(Event::MouseEvent event);
        virtual bool OnHover(Event::MouseEvent event);
        virtual void OnHoverExit();
        virtual bool OnScroll(Event::ScrollEvent event);
        virtual void OnDrag(Event::DragEvent event);
        virtual void OnType(Event::KeyTypedEvent event);
        virtual bool IsInteractable(Vector2 offset);

        struct CollidingEntry {
            Component* comp = nullptr;
            Vector2 offset = { 0,0 };
        };
        virtual void MatchColliding(std::vector<CollidingEntry>& colliding,Vector2 point);

        virtual void Print();
        
        Vector2 GetTotalSize();
        Vector2 GetInnerSize();
        void SetMargin(int margin);
        void SetPadding(int padding);

        Vector2 GetGlobalOffset();

        virtual void Init() {};

        

        Component();
        Component(Vector2 size);
        Component(Vector2 offset, Vector2 size);
        virtual ~Component() = default;
    protected:
        virtual void render(Vector2 offset, float opacity = 1.0f);
        virtual void resize(Vector2 new_size);
        virtual void update();
    };

    class Box : public Component, public std::enable_shared_from_this<Box>{
    public:
        enum class GrowMode{ // THis is which direction it grows when adding
            FREE,
            RIGHT, 
            LEFT,
            UP,
            DOWN,
        };
       

        // This determines of the Component gets shrunk to fit the box if it is too big along the non growth axis
        // So if it grows right and is too big vertically it would be shrunk to the vertical size of the box if true
        // If false the box would change its y to accomodate the biggest Component
        bool fit_non_axis = true;

        GrowMode grow_mode = GrowMode::FREE;

        
        virtual void AddChild(std::shared_ptr<Component> child);
        virtual void RemoveChild(std::shared_ptr<Component> child);
        virtual void SetGrowMode(GrowMode mode);
        void HandleChildSizeChange() override;

        void Recompute();
        void FlushPending();

        
        void MatchColliding(std::vector<CollidingEntry>& colliding,Vector2 point) override;

        void Print() override;

        void render(Vector2 offset, float opacity) override;
        void resize(Vector2 size) override;
        void update() override;
        

        Box();
        Box(Vector2 offset, Vector2 size);
        Box(Vector2 size);

        std::vector<std::shared_ptr<Component>> children;
    protected:
        
        
        std::vector<std::shared_ptr<Component>> add_pending;
        std::vector<std::shared_ptr<Component>> remove_pending;

        // This is turned on whenever computing the bounds for the children
        // Recompute calls child->SetTotalSize() which calls parent->HandleChildSizeChange() which would result in infinite recursion
        bool computing = false;

    };

    class Text : public Component {
    public:
        std::string text = "";
        Color font_color = BLACK;
        float font_size = 20;

        Vector2 GetTextSize();
        
        void render(Vector2 offset, float opacity) override;

        Text(std::string text = "",float font_size = 20,Color font_color = BLACK);
    };

    class Slider : public Component {
    public:
        float current_progress = 0;
        Color track_left_color = BLUE;
        Color track_right_color = LIGHTGRAY;
        Color dot_color = BLUE;
        Color tick_color = LIGHTGRAY;; // Tickmarks for the discrete slider options
        int tick_width = 4;
        int tick_height = 12;
        int track_height = 6;
        int dot_radius = 10;
        std::vector<float> values;

        Slider(float width, std::vector<float> discrete_values = {});
        Slider(Vector2 size, std::vector<float> discrete_values = {});

        bool IsInteractable(Vector2 offset) override;

        void render(Vector2 offset, float opacity);

        static void onClick(Component* self, ryanUI::Event::MouseEvent click);
        static void onDrag(Component* self, ryanUI::Event::DragEvent drag);
    };

    class ScrollBar : public Component{
    public:
        float current_progress = 0;
        float bar_size = .2; // This is the portion of the size taken up so .5 would be half bar half space
        bool horizontal = false;

        Color bar_color = BLACK;

        void render(Vector2 offset, float opacity) override;

        static void onClick(Component* self,ryanUI::Event::MouseEvent click);
        static void onDrag(Component* self,ryanUI::Event::DragEvent drag);
    
        ScrollBar();
        ScrollBar(float height, bool horizontal = false);
        ScrollBar(Vector2 size, bool horizontal = false);
    };

    class ScrollPane : public Box {
    public:
        std::shared_ptr<Box> content = nullptr;
        std::shared_ptr<ScrollBar> scroll_bar = nullptr;

        void SetGrowMode(GrowMode mode) override;
        void AddChild(std::shared_ptr<Component> child) override;
        void RemoveChild(std::shared_ptr<Component> child) override;
        void render(Vector2 offset, float opacity) override;
        void update() override;
        void Init() override;

        static void onScroll(Component* self, Event::ScrollEvent event);

    public: // Do not use these use ryanUI::Create instead 
        ScrollPane(Vector2 size);
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

    namespace Verifier {
        bool integer_verifier(const std::string& text);
        bool float_verifier(const std::string& text);
    }

    class TextField : public Component {
    public:
        int max_width = -1; // -1 means no max width and to grow dynamically
        bool fill_width = false; // If this is true it will add characters until it would grow past size rather than just to max_width
                                 // This is different for non monospaced fonts where the size is calculated based on biggest character
        int cursor_location = 0;

        std::string text = "";
        float font_size = 20;
        Color font_color = BLACK;
        Color carat_color = WHITE;
        float carat_timer = 0;
        float carat_reset_time = 1; // Will spend 1/2 the time on 1/2 off. Lower half is spent on
        
        std::function<void(TextField*)> on_submit = nullptr;
        std::function<bool(std::string&)> verifier = nullptr;

        TextField(std::string start = "", int max_width = 10,float font_size = 20,Color font_color = BLACK);

        void render(Vector2 offset, float opacity);
        bool canAdd(char c);
        void Clear();

        static void onType(Component* self, ryanUI::Event::KeyTypedEvent event);
    };

    class UIImage : public Component {
    public:
        Texture2D texture;

        UIImage(Texture2D texture,Vector2 size);

        void render(Vector2 offset, float opacity) override;
    };

    class Switch : public Component {
    public:
        bool on = false;
        Color selected_color = BLUE;
        Color deselected_color = DARKGRAY;
        Color dot_color = LIGHTGRAY;
        int dot_radius;
        Switch(Vector2 size = { 40,25 }, float roundness = 1.0f);

        void render(Vector2 offset, float opacity);


        static void onHover(Component* self, Event::MouseEvent event);
        static void onHoverExit(Component* self);
        static void onClick(Component* self, Event::MouseEvent event);
    };

    class Expander : public Box {
    public:
        bool is_expanded = false;

        std::shared_ptr<UIImage> down_arrow = nullptr;
        std::shared_ptr<UIImage> up_arrow = nullptr;
       

        void SetDefaultComponent(std::shared_ptr<Component> default);
        void SetExpandedComponent(std::shared_ptr<Component> default);

        void Expand();
        void Contract();

        void Init() override;
    public: // Do not use this instead use ryanUI::Create
        Expander(Vector2 component_size);
    private:
        std::shared_ptr<Component> default_component = nullptr;
        std::shared_ptr<Component> expanded_component = nullptr;
    };

    class DropdownMenu : public Box {
    public:
        std::shared_ptr<Text> text = nullptr;
        std::shared_ptr<UIImage> down_arrow = nullptr;
        std::vector<std::string> options;

        std::function<void(DropdownMenu*)> on_select = nullptr;

        int selected = 0;
        DropdownMenu(std::vector<std::string> options, float font_size = 20, Color font_color = BLACK);
        
        void SetSelection(int sel);

        void Init() override;

        static void onClick(Component* self, Event::MouseEvent event);
    };

    class TreeSource : public Text {
    public:
        std::shared_ptr<Box> node_list = nullptr;
        TreeSource(std::string);

        static void onClick(Component* self, Event::MouseEvent event);
    };

    class TreeNode : public Text {
    public:
        inline static std::shared_ptr<Box> root = nullptr;
        std::shared_ptr<Box> node_list = nullptr;
        int depth = 0;
        TreeNode(std::string str,int depth);

        static void onClick(Component* self, Event::MouseEvent event);
        static void rootOnClick(Component* self, Event::MouseEvent event);
    };

    struct PopupEntry {
        bool block_inputs = true;
        bool close_on_outside_click = true;
        std::shared_ptr<Component> component = nullptr;
    };


    template <typename T, typename... Args>
    static std::shared_ptr<T> Create(Args&&... args) {

        auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
        ptr->Init();  // Safe: ptr is now managed by shared_ptr
        return ptr;
    }

    
    void Init(int window_width, int window_height);
    std::shared_ptr<Box> GetRoot();
    void HandleEvents();
    void Draw();
    
    void SetPopup(PopupEntry entry);
    void ClearPopup();

    inline static Font default_font = GetFontDefault();
    

    // DO NOT MODIFY ANYTHING BELOW HERE

    inline static std::shared_ptr<Box> root = nullptr;
    inline static Component* dragged_component = nullptr; // Cant use shared ptr because no access to its own shared ptr
    inline static Vector2 drag_start = {0,0};
    inline static Event::MouseButton drag_btn = Event::MouseButton::NONE;

    inline static Component* text_focus = nullptr;

    inline static std::unordered_map<std::string, Texture2D> loaded_textures;

    
    inline static PopupEntry temp_popup; // We need a temp because adding a popup while handling events results in errors
    inline static PopupEntry popup;

    
};