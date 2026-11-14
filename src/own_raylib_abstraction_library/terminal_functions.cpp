#include "../headers/terminal_functions.hpp"

Text::Text(std::string Text, Color COLOR, int Start_x ) : text(Text), color(COLOR), start_x(Start_x){
    amount_of_lines = std::count(text.begin(), text.end(),'\n');
};

int Text::Get_lines(){
    amount_of_lines = std::count(text.begin(), text.end(),'\n');
    if(!Has_last_newline() && amount_of_lines != 0) amount_of_lines++;
    return amount_of_lines;
};

bool Text::Has_last_newline(){
    return (!text.empty() && (text.back() == '\n'));
}





Terminal_G::Terminal_G(int W, int H) : w(W), h(H) {};

void Terminal_G::Set_cords(int X, int Y){
    x = X;
    y = Y;
    std::cout
    << "terminal x: " << x
    << " terminal w: " << w
    << " terminal y: " << y
    << " terminal h: " << h
    << "\n";
}

void Terminal_G::Set_font_size(int size){
    font_size = size;
    line_height = MeasureTextEx(GetFontDefault(),"MMM",font_size,1).y+line_spacing;
    max_lines = h/line_height-2;
}

void Terminal_G::Prompt(Text Prompt, Text &Input, int Cursor_pos, bool cursor_indicator){
    prompt = Prompt;
    input = Input;
    cursor_pos = Cursor_pos;
    cursor = cursor_indicator;
}

void Terminal_G::Set_opacity(int amount){
    if(amount > 255) opacity = 255;
    else if(amount < 0) opacity = 0;
    else opacity = amount;

    for(size_t i = 0; i < history.size(); i++){
        history[i].color.a = opacity;
    }
}

void Terminal_G::Set_dimensions(int width, int height){
    w = width;
    h = height;
}

void Terminal_G::Update(){
    float scroll = GetMouseWheelMove();

    if(scroll > 0) scroll_offset++;
    else if(scroll < 0) scroll_offset--;

    int max_scroll = std::max(0, Get_history_lines(history) - max_lines);

    if(scroll_offset < 0) scroll_offset = 0;
    if(scroll_offset > max_scroll) scroll_offset = max_scroll;

}

void Terminal_G::Draw(){
    DrawRectangle(x,y,w,h,{0,0,0,static_cast<unsigned char>(opacity)});
    DrawLine(((x-1 == 0) ? x-1:x),0,((x-1 == 0) ? x-1:x),h,{255,255,255,static_cast<unsigned char>(opacity)});
    for(size_t i = 0; i < display_history.size(); i++){
        DrawText(display_history[i].text.c_str(), display_history[i].start_x + x, line_height*(Get_history_lines(std::vector<Text>(display_history.begin(), display_history.begin()+i))) + y, font_size, display_history[i].color);
    }

    bool cursor_visible = fmod(GetTime(), 1.0) < 0.5;
    int cursor_x = MeasureText(input.text.substr(0,cursor_pos).c_str(),font_size)+MeasureText(prompt.text.c_str(),font_size)+x;

    bool newline = (!display_history.empty() && !display_history.back().Has_last_newline());
    



    
    if(newline){
        DrawText(prompt.text.c_str(), x,(current_line+1)*line_height+y,font_size,prompt.color);
        DrawText(input.text.c_str(), MeasureText(prompt.text.c_str(),font_size) + x,(current_line+1)*line_height+y,font_size,{0, 112, 163,255});
        if(cursor && cursor_visible)
            DrawText("^", cursor_x,(current_line+2)*line_height+y,font_size,{95, 27, 255,255});
        else if(!cursor && cursor_visible)
            DrawText("|", cursor_x,(current_line+1)*line_height+y,font_size,{255,255,255,255});
    
    }
    else{
        DrawText(prompt.text.c_str(), x,current_line*line_height+y,font_size,prompt.color);
        DrawText(input.text.c_str(), MeasureText(prompt.text.c_str(),font_size)+x,current_line*line_height+y,font_size,{0, 112, 163,255});
        if(cursor && cursor_visible)
            DrawText("^", cursor_x,(current_line+1)*line_height+y,font_size,{95, 27, 255,255});
        else if(!cursor && cursor_visible)
            DrawText("|", cursor_x,(current_line)*line_height+y,font_size,{255,255,255,255});
    
    }
    

    
}

int Terminal_G::Get_history_lines(std::vector<Text> his){
    int amount = 0;

    for(size_t i = 0; i < his.size(); i++){
        amount += his[i].Get_lines();
    }
    return amount;
}

int Terminal_G::Get_displayhis_start_pos(){
    int visual_lines = 0;

    for(int i = history.size()-1; i >= 0; i--){
        visual_lines += history[i].Get_lines();

        if(visual_lines > max_lines){
            return i+1;
        }
    }

    return 0;
}

std::vector<Text> Terminal_G::Get_text_elements(Text text, Text list_last_element){
    std::vector<Text> output = {};
    std::string line = "";
    int start_x_pos = list_last_element.start_x;
    bool first_line = true;

    if(list_last_element.Get_lines() == 0) start_x_pos += MeasureText(list_last_element.text.c_str(), font_size);
    else if(list_last_element.Has_last_newline()) start_x_pos = 0;


    for(size_t i = 0; i < text.text.length(); i++){
        if(text.text[i] == '\n'){
            output.emplace_back(Text(line + "\n", text.color, start_x_pos));
            start_x_pos = 0;
            line = "";
            first_line = true;
        }
        else if(first_line && MeasureText(std::string(line + " ").c_str(), font_size) >= w-(start_x_pos)){
            output.emplace_back(Text(line + "\n", text.color,start_x_pos));
            line = "";
            line += text.text[i];
            first_line = false;
            start_x_pos = 0;
        }
        else if(MeasureText(std::string(line + " ").c_str(), font_size) >= w){
            output.emplace_back(Text(line + "\n", text.color,start_x_pos));
            line = "";
            line += text.text[i];
            start_x_pos = 0;
        }
        else{
            line += text.text[i];
        }
    }
    if(!line.empty()) output.emplace_back(Text(line, text.color,start_x_pos));


    return output;
}

void Terminal_G::Print_text(Text text){
    if(text.text.empty()) return;

    std::vector<Text> elements = {};

    if(!history.empty()) elements = Get_text_elements(text,history.back());
    else elements = Get_text_elements(text,Text(" ",{255,255,255,255},0));

    for(size_t i = 0; i < elements.size(); i++){
        history.emplace_back(elements[i]);
    }

    display_history = history;

    if(Get_history_lines(display_history) > max_lines){
        int start_pos = Get_displayhis_start_pos();
        display_history.erase(display_history.begin(), display_history.begin() + start_pos - scroll_offset);
    }

    current_line = Get_history_lines(display_history);
}

