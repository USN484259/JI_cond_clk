package io.github.USN484259;



import org.luaj.vm2.*;
import org.luaj.vm2.lib.*;

import java.io.IOException;
import java.nio.BufferOverflowException;


public class cond_clk extends TwoArgFunction {
    static cond_clk_lib library = null;
    int[] buffer = null;

    public cond_clk(){
        if (library == null)
            library=cond_clk_lib.getInstance();
    }

    @Override
    public LuaValue call(LuaValue modname, LuaValue env) {
        LuaValue table = tableOf();
        table.set("width", new ZeroArgFunction() {
            @Override
            public LuaValue call() {
                return valueOf(library.width());
            }
        });
        table.set("height", new ZeroArgFunction() {
            @Override
            public LuaValue call() {
                return valueOf(library.height());
            }
        });
        table.set("capture", new ZeroArgFunction() {
            @Override
            public LuaValue call() {
                if (buffer == null) {
                    buffer = new int[library.width() * library.height()];
                }
                long size = library.capture(buffer,4*buffer.length);
                if (size == 0)
                    return error("failed capturing screen");
                int w = library.width();
                int h = library.height();
                if (size != 4*buffer.length || size != 4*w*h)
                    throw new BufferOverflowException();
                return new luaImage(buffer,w,h).getTable();
            }
        });
        table.set("type", new OneArgFunction() {
            @Override
            public LuaValue call(LuaValue luaValue) {
                String str = luaValue.tojstring();
                int pos = library.type(str);
                if (pos != str.length())
                    return error("invalid character at "+pos);
                return valueOf(pos);
            }
        });
        table.set("move", new TwoArgFunction() {
            @Override
            public LuaValue call(LuaValue x,LuaValue y) {
                if (!library.move(x.toint(),y.toint()))
                    return error("failed moving mouse");
                return NIL;
            }

        });
        /*
        table.set("click", new ThreeArgFunction() {
            @Override
            public LuaValue call(LuaValue x,LuaValue y,LuaValue button) {
                if (!library.click(x.toint(),y.toint(),button.optint(0)))
                    return error("failed clicking mouse");
                return NIL;
            }
        });
        */
        table.set("click", new VarArgFunction() {
            @Override
            public Varargs invoke(Varargs varargs) {
                int button = 0;
                switch(varargs.narg()){
                    case 3:
                        button = varargs.checkint(3);
                    case 2:
                    {
                        int x = varargs.checkint(1);
                        int y = varargs.checkint(2);
                        if (library.move(x,y) && library.click(button))
                            return NIL;
                        break;
                    }
                    case 1:
                        button = varargs.checkint(1);
                    case 0:
                        if (library.click(button))
                            return NIL;
                        break;
                }
                return error("failed clicking mouse");
            }
        });

        table.set("scroll", new OneArgFunction() {
            @Override
            public LuaValue call(LuaValue luaValue) {
                if (!library.scroll(luaValue.toint()))
                    return error("failed scrolling mouse");
                return NIL;
            }
        });
        table.set("reset", new ZeroArgFunction() {
            @Override
            public LuaValue call() {
                if (library != null)
                    library.reset();
                buffer = null;
                return null;
            }
        });

        table.set("image", new OneArgFunction() {
            @Override
            public LuaValue call(LuaValue luaValue) {
                try {
                    return new luaImage(luaValue.tojstring()).getTable();
                }
                catch(IOException | badFormatException e){
                    return error(e.toString());
                }
            }
        });
        table.set("sleep", new OneArgFunction() {
            @Override
            public LuaValue call(LuaValue luaValue) {
                try {
                    Thread.sleep(luaValue.toint());
                }
                catch(InterruptedException | IllegalArgumentException e){
                    return error(e.toString());
                }
                return NIL;
            }
        });
        
        //env.set("cond_clk",table);
        return table;
    }
}
