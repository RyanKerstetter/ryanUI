
/*
    std::shared_ptr<Box> rootBox = GetRoot();

    std::shared_ptr<ScrollPane> s_box = Create<ScrollPane>(Vector2{ 200, 200 });
    s_box->offset = { 500,100 };
    s_box->SetGrowMode(Box::GrowMode::DOWN);
    s_box->border_size = 1;
    for (int i = 0; i < 30; i++) {
        std::shared_ptr<Text> text_new = Create<Text>("Hello" + std::to_string(i));
        s_box->AddChild(text_new);
    }
    rootBox->AddChild(s_box);

    std::shared_ptr<Box> box = Create<Box>(Vector2{ 100,50 });
    box->SetGrowMode(Box::GrowMode::RIGHT);
    box->fit_non_axis = false;
    box->border_size = 2;
    box->offset = { 100,100 };
    rootBox->AddChild(box);

    std::shared_ptr<Text> text1 = Create<Text>("Hello");
    text1->border_size = 1;
    std::shared_ptr<Text> text2 = Create<Text>("World");
    text2->border_size = 1;
    box->AddChild(text1);
    box->AddChild(text2);

    std::shared_ptr<Text> button = Create<Text>("Create Popup");
    button->border_size = 1;
    button->on_click = [](Component* self, Event::MouseEvent event) {
        std::shared_ptr<Box> box = Create<Box>(Vector2{ 200,50 });
        box->SetTotalSize(Vector2{ 800,600 });
        box->border_size = 1;
        std::shared_ptr<Text> text1 = Create<Text>("Yes");
        text1->border_size = 1;
        text1->on_click = [](Component* self, Event::MouseEvent event) {
            std::cout << "Yes" << std::endl;
            ClearPopup();
            };
        std::shared_ptr<Text> text2 = Create<Text>("No");
        text2->border_size = 1;
        text2->on_click = [](Component* self, Event::MouseEvent event) {
            std::cout << "No" << std::endl;
            ClearPopup();
            };
        box->SetGrowMode(Box::GrowMode::RIGHT);
        box->AddChild(text1);
        box->AddChild(text2);
        SetPopup(PopupEntry{ true,true,box });
        };
    rootBox->AddChild(button);


    std::shared_ptr<Slider> slider = Create<Slider>(200, std::vector<float>{0, 0.25, 0.5, 0.75, 1});
    slider->offset = Vector2{ 500,500 };
    rootBox->AddChild(slider);

    std::shared_ptr<Switch> sw = Create<Switch>(Vector2{ 40,25 }, 0);
    sw->offset = { 100,50 };
    rootBox->AddChild(sw);


    text1->AddAnimation(std::make_shared<Animation::ComponentAnimation>(5, text1.get(), [](Component* self, float t) {
        float x_size = 200;
        float current = x_size * Easing::easeOutExpo(t);
        Vector2 inner_size = self->GetInnerSize();
        self->SetInnerSize({ std::max(current,inner_size.x),inner_size.y });
    }));


    std::shared_ptr<TextField> tf = Create<TextField>("");
    tf->background_color = LIGHTGRAY;
    tf->verifier = Verifier::float_verifier;
    tf->on_submit = [](TextField* self) {
        std::cout << self->text << std::endl;
        self->Clear();
        };
    box->AddChild(tf);

    std::shared_ptr<UIImage> image = Create<UIImage>(texture, Vector2{ 200,200 });
    //box->AddChild(image);

    rootBox->FlushPending();

    //
    // Dropdown Example
    //
    //

    std::shared_ptr<Box> root_box = GetRoot();

    std::shared_ptr<Box> top_bar = Create<Box>(Vector2{ 800,35 });
    top_bar->SetGrowMode(Box::GrowMode::RIGHT);
    top_bar->background_color = LIGHTGRAY;
    root_box->AddChild(top_bar);


    std::vector<std::string> options = { "new","open","clone repository","start window","add","close","close folder","print" };

    std::shared_ptr<TreeSource> file = Create<TreeSource>("file");
    file->background_color = LIGHTGRAY;
    file->border_size = 1;
    top_bar->AddChild(file);


    for (std::string& str : options) {
        std::shared_ptr<TreeNode> node = Create<TreeNode>(str,1,Vector2{250,22});
        node->MakeTree();
        for (std::string& str2 : options) {
            std::string new_str = str + "-" + str2;
            std::shared_ptr<TreeNode> next = Create<TreeNode>(new_str,2, Vector2{ 250,22 });
            node->AddNode(next);
        }
        file->AddNode(node);
    }



    std::shared_ptr<Text> edit = Create<Text>("edit");
    edit->background_color = LIGHTGRAY;
    edit->border_size = 1;
    top_bar->AddChild(edit);

*/