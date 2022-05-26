package io.github.USN484259;

import com.sun.jna.Library;
import com.sun.jna.Native;

public interface cond_clk_lib extends Library{
    int width();
    int height();
    long capture(int[] buffer,long size);
    int type(String str);
    boolean move(int x,int y);
    boolean click(int button);
    boolean scroll(int step);
    void reset();
    void cleanup();

    static cond_clk_lib instance = (cond_clk_lib) Native.load("cond_clk_lib", cond_clk_lib.class);

    static cond_clk_lib getInstance() {
        //Native.setProtected(true);
        return instance;
    }
}

