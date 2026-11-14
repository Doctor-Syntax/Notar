#ifndef WINDOW
#define WINDOW

#include "program.hpp"


class GC{
private:
    int w;
    int h;
    int x = 0;
    int y = 0;

public:
    GC(int W, int H);
    void Set_cords(int X, int Y);
    void Draw();
    

};




#endif // WINDOW