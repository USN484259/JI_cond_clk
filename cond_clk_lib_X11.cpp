#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XTest.h>
#include <signal.h>
#include <iostream>
typedef u_int32_t u32;
typedef int32_t s32;
typedef u_int64_t u64;

using namespace std;


static u32 Width = 0,Height = 0;

static Display* display = nullptr;
static Window root_window = None;



static bool initialize(void){
    if (display && root_window)
        return true;
    do{
        display = XOpenDisplay(nullptr);
        if (!display)
            break;
        root_window = XDefaultRootWindow(display);
        if (!root_window)
            break;
        {
            XWindowAttributes attrib;
            if (!XGetWindowAttributes(display,root_window,&attrib))
                break;
            Width = attrib.width;
            Height = attrib.height;

        }

        return true;
    }while(false);
    return false;
}


extern "C"
{
    void cleanup(void){
        Width = Height = 0;
        if (display){
            XCloseDisplay(display);
            display = nullptr;
        }


    }

    u32 width(void){
        initialize();
        return Width;
    }

    u32 height(void){
        initialize();
        return Height;
    }

    u64 capture(void* buffer,u64 limit){
        if (!initialize())
            return 0;
        //raise(SIGINT);
        u64 required_size = 4*Width*Height;
        if (limit < required_size)
            return required_size;

        XImage* img = XGetImage(display,root_window,0,0,Width,Height,AllPlanes,ZPixmap);
        if (!img)
            return 0;
        u32* dst = (u32*)buffer;
        for (auto y = 0;y < Height;++y){
            for (auto x = 0;x < Width;++x){
                u32 pixel = XGetPixel(img,x,y);
                *(dst++) = pixel;
            }
        }
        XFree(img);
        return required_size;
    }

    int click(u32 button){
        if (!initialize())
            return 0;
        if (button > 2)
            return 0;
        XTestFakeButtonEvent(display,button + 1,True,0);
        XTestFakeButtonEvent(display,button + 1,False,0);
        XSync(display,0);
        return 1;
    }

    int move(s32 x,s32 y){
        if (!initialize())
            return 0;
        XTestFakeMotionEvent(display,-1,x,y,0);
        XSync(display,0);
        return 1;  

    }

    int scroll(s32 step){
        if (!initialize())
            return 0;
        int button = 5;
        if (step < 0){
            button = 4;
            step = -step;
        }
        while(step--){
            XTestFakeButtonEvent(display,button,True,0);
            XTestFakeButtonEvent(display,button,False,0);
        }
        XSync(display,0);
        return 1;
    }

    static KeyCode get_keycode(char c,KeyCode& shift){
        if (isprint(c) || c == '\t' || c == '\n' || c == 27)
            ;
        else
            return 0;

        if (islower(c)){
            c -= 0x20;
            shift = 0;
        }
        else if (!isupper(c)){
            switch (c) {
            case '~':
                c = '`';
                break;
            case '!':
                c = '1';
                break;
            case '@':
                c = '2';
                break;
            case '#':
                c = '3';
                break;
            case '$':
                c = '4';
                break;
            case '%':
                c = '5';
                break;
            case '^':
                c = '6';
                break;
            case '&':
                c = '7';
                break;
            case '*':
                c = '8';
                break;
            case '(':
                c = '9';
                break;
            case ')':
                c = '0';
                break;
            case '_':
                c = '-';
                break;
            case '+':
                c = '=';
                break;
            case '<':
                c = ',';
                break;
            case '>':
                c = '.';
                break;
            case '?':
                c = '/';
                break;
            case ':':
                c = ';';
                break;
            case '\"':
                c = '\'';
                break;
            case '{':
                c = '[';
                break;
            case '}':
                c = ']';
                break;
            case '|':
                c = '\\';
                break;
            default:
                shift = 0;
            }
        }
        KeySym sym = c;
        switch(sym){
            case '\t':
            sym = XK_Tab;
            break;
            case '\n':
            sym = XK_Return;
            break;
            case 27:    //esc
            sym = XK_Escape;
            break;
        }

        return XKeysymToKeycode(display,sym);

    }



    int type(const char* str){
        if (!initialize())
            return 0;
        //raise(SIGINT);
        int index = 0;

        XkbStateRec state;
        XkbGetState(display,XkbUseCoreKbd,&state);
        XkbLockGroup(display,XkbUseCoreKbd,0);

        KeyCode shift_key = XKeysymToKeycode(display,XK_Shift_L);

        while(str[index]){
            KeyCode shift = shift_key;
            KeyCode keycode = get_keycode(str[index++],shift);

            if (0 == keycode)
                break;

            if (shift){
                XTestFakeKeyEvent(display,shift,True,0);
            }
            XTestFakeKeyEvent(display,keycode,True,0);
            XTestFakeKeyEvent(display,keycode,False,0);
            if (shift){
                XTestFakeKeyEvent(display,shift,False,0);
            }
        }
        XkbLockGroup(display,XkbUseCoreKbd,state.group);
        XSync(display,0);
        return index;
    }

}