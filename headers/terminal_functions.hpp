#ifndef TERMINAL_G
#define TERMINAL_G

#include "program.hpp"



class Terminal_G{
private:
    int w;
    int h;
    int x = 0;
    int y = 0;
    

    int line_spacing = 2;
    int font_size = 10;
    int current_line = 0;
    int max_lines = 0;
    int line_height = 0;

    int cursor_pos = 0;
    int display_his_start = 0;
    int scroll_offset = 0;

    Text prompt = Text("");
    Text input = Text("");

    std::vector<Text> history= {};
    std::vector<Text> display_history = history;
    std::string text = "";
    int opacity = 255;

    int i  = 0;
    bool cursor = true;



    int Get_history_lines(std::vector<Text> his); 
    int Get_displayhis_start_pos();
    std::vector<Text> Get_text_elements(Text text, Text list_last_element);


public:
    Terminal_G(int W, int H);

    void Print_text(Text text);
    void Set_cords(int X, int Y);
    void Draw();
    void Prompt(Text prompt, Text &input, int cursor_pos, bool cursor_indicator = true);
    void Set_font_size(int size);
    void Set_opacity(int amount);
    void Set_dimensions(int width, int height);
    void Update();

};


#endif //TERMINAL_G