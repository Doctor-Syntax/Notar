
#include "../headers/window_functions.hpp"

GC::GC(int W, int H) : w(W), h(H) {};


void GC::Set_cords(int X, int Y){
    x = X;
    y = Y;
}

void GC::Draw(){
    DrawRectangle(x,y,w,h,{50,50,50,255});
}