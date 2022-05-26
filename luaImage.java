package io.github.USN484259;

import org.luaj.vm2.*;
import org.luaj.vm2.lib.OneArgFunction;
import org.luaj.vm2.lib.TwoArgFunction;
import org.luaj.vm2.lib.VarArgFunction;
import org.luaj.vm2.lib.ZeroArgFunction;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.io.*;



public class luaImage {
    BufferedImage img = null;
    Rectangle region = null;

    public static int getByte(int val,int id){
        return (val >> (id*8)) & 0xFF;
    }


    public luaImage(int[] buffer, int w,int h) {
        img = new BufferedImage(w,h,BufferedImage.TYPE_INT_RGB);
        img.setRGB(0,0,w,h,buffer,0,w);
        //region = new Rectangle(0,0,w,h);
    }
    public luaImage(String path) throws IOException,badFormatException{
        File file = new File(path);
        if (file.getName().endsWith(".rgb")){
            img = RgbLoader.read(file);
        }
        else{
            img = ImageIO.read(file);
        }
        if (img == null)
            throw new badFormatException();
        //region = new Rectangle(0,0,img.getWidth(),img.getHeight());
    }
    private Rectangle getRegion() {
        if (region == null)
            region = new Rectangle(0,0,img.getWidth(),img.getHeight());
        return region;
    }

    public LuaValue getTable(){
        LuaValue table = LuaValue.tableOf();
        LuaTable meta = LuaValue.tableOf(0,1);
        meta.set("this",LuaValue.userdataOf(this));
        table.setmetatable(meta);
        //table.set("this",LuaValue.userdataOf(this));

        table.set("save", new OneArgFunction() {
            @Override
            public LuaValue call(LuaValue luaValue) {
                File file = new File(luaValue.tojstring());
                String name = file.getName();
                String format = name.substring(1 + name.lastIndexOf('.')).toLowerCase();
                if (format.equals("bmp") || format.equals("png"))
                    ;
                else
                    return error("unknown format: "+format);
                try {
                    ImageIO.write(img, format, file);
                }
                catch(IOException e){
                    return error(e.toString());
                }
                return NIL;
            }
        });

        table.set("view", new ZeroArgFunction() {
            @Override
            public LuaValue call() {
                new viewerWindow(img);
                return NIL;
            }
        });


        table.set("region", new OneArgFunction() {
            @Override
            public LuaValue call(LuaValue luaValue) {
                luaValue.checktable();
                int x = luaValue.get("left").toint();
                int y = luaValue.get("top").toint();
                int w = luaValue.get("right").toint() - x;
                int h = luaValue.get("bottom").toint() - y;
                region = new Rectangle(x,y,w,h);
                return NIL;
            }
        });

        table.set("match", new VarArgFunction() {
            @Override
            public LuaValue invoke(Varargs varargs) {
                luaImage other = (luaImage) varargs.arg1().getmetatable().get("this").checkuserdata(luaImage.class);

                Rectangle rect = (Rectangle) other.getRegion().createIntersection(luaImage.this.getRegion());

                LuaFunction cmp = null;
                if (varargs.isfunction(2))
                    cmp = varargs.checkfunction(2);
                else
                    cmp = new TwoArgFunction() {
                        @Override
                        public LuaValue call(LuaValue luaValue, LuaValue luaValue1) {
                            LuaTable a = luaValue.checktable();
                            LuaTable b = luaValue1.checktable();
                            for (int i = 1;i <= 3;++i){
                                if (a.get(i).toint() != b.get(i).toint())
                                    return FALSE;
                            }
                            return TRUE;
                        }
                    };

                int count = 0;
                LuaTable[] table = {tableOf(3,0),tableOf(3,0)};
                int[] pixel = new int[2];
                for (int y = rect.y;y != rect.y + rect.height;++y){
                    for (int x = rect.x;x != rect.x + rect.width;++x){
                        pixel[0] = luaImage.this.img.getRGB(x,y);
                        pixel[1] = other.img.getRGB(x,y);

                        for(int i =0;i < 6;i++){
                            table[i / 3].set(1 + i % 3,valueOf(getByte(pixel[i / 3],i % 3)));
                        }

                        if (cmp.call(table[0],table[1]).toboolean()){
                            ++count;
                        }
                    }
                }
                return valueOf((double)count / (rect.width*rect.height));
            }
        });

        table.set("pixel", new TwoArgFunction() {
            @Override
            public LuaValue call(LuaValue _x, LuaValue _y) {

                int x = _x.checkint();
                int y = _y.checkint();

                if (x >= img.getWidth() || y >= img.getHeight())
                    error("point out of range");

                LuaValue result = tableOf(3,0);
                int pixel = img.getRGB(x,y);

                for (int i = 0;i < 3;++i){
                    result.set(i+1,valueOf(getByte(pixel,i)));
                }

                return result;
            }
        });

        return table;
    }

}
