package io.github.USN484259;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.BufferedImage;

public class viewerWindow extends JFrame {
    BufferedImage image;
    public viewerWindow(BufferedImage img) {
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        image = img;
        JLabel label = new JLabel(new ImageIcon(img));
        label.addMouseListener(new MouseListener() {
            @Override
            public void mouseClicked(MouseEvent mouseEvent) {
                Point p=mouseEvent.getPoint();
                int pixel = image.getRGB(p.x,p.y);
                StringBuilder str = new StringBuilder();
                for (int i = 0;i < 3;++i) {
                    str.append(luaImage.getByte(pixel,i));
                    str.append(' ');
                }
                str.append("at ");
                str.append(p.x);
                str.append(',');
                str.append(p.y);
                viewerWindow.this.setTitle(str.toString());
            }

            @Override
            public void mousePressed(MouseEvent mouseEvent) {

            }

            @Override
            public void mouseReleased(MouseEvent mouseEvent) {

            }

            @Override
            public void mouseEntered(MouseEvent mouseEvent) {

            }

            @Override
            public void mouseExited(MouseEvent mouseEvent) {

            }
        });

        JScrollPane pane = new JScrollPane(label);

        //pane.setPreferredSize(new Dimension(img.getWidth(),img.getHeight()));

        add(pane);

        pack();
        setVisible(true);
    }
}
