#pragma once
#include <raylib.h>
#include "UI.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace ryanUI::Components {
	class FileSelectDialog : public Box {
	public:
		std::shared_ptr<ScrollPane> scroll_pane = nullptr;
		std::shared_ptr<Text> selected_text = nullptr;
		std::shared_ptr<Text> submit_button = nullptr;
		std::function<void(fs::path)> callback = nullptr;
		fs::path path_to_file;
		fs::path current_path;
		void SetPath(fs::path p);

		FileSelectDialog(Vector2 size, fs::path path, std::function<void(fs::path)> cb = nullptr);

		void Init();
	};
}
