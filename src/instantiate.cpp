#include "../include/TEST.h"
#include "../include/Theme.h"



vec2d View::max_texture_size;

Event* Event::instance = nullptr;

Administrator* Administrator::instance = nullptr;

theme* theme::instance = nullptr;

std::deque<std::string> Error::error_messages;