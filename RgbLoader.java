package io.github.USN484259;


import javax.imageio.ImageIO;
import javax.imageio.stream.FileImageInputStream;
import java.awt.image.BufferedImage;
import java.io.*;
import java.nio.ByteOrder;

class badFormatException extends Exception{
    public badFormatException(){
        super("bad image format");
    }

}

public class RgbLoader {



    public static BufferedImage read(File in) throws IOException,badFormatException {
        try{
                FileImageInputStream stream = new FileImageInputStream(in);
                stream.setByteOrder(ByteOrder.LITTLE_ENDIAN);
                if (stream.readByte() != 2 || stream.readByte() != 4)
                    throw new badFormatException();
                int width = stream.readUnsignedShort();
                int height = stream.readUnsignedShort();
                if (stream.readUnsignedShort() != 0)
                    throw new badFormatException();

                BufferedImage img = new BufferedImage(width,height,BufferedImage.TYPE_INT_RGB);

                for (int y = 0;y != height;++y){
                    for (int x = 0;x != width;++x){
                        int pixel = stream.readInt();
                        img.setRGB(x,y,pixel);
                    }
                }

                return img;

        }
        catch(EOFException e){
            throw new badFormatException();
        }
    }
}
