package android.ImgSdk;

/**
 * Created by dejian.fei on 2016/9/5.
 */

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

public class Pixels {
    private enum Position {
        Up, Down, Left, Right, Front, Back, None;
        String name;
        int index;

        //覆盖方法
        @Override
        public String toString() {
            return this.name + "[" + this.index + "]";
        }
    }
    private int x;
    private int y;
    private int R;
    private int G;
    private int B;
    private int A;
    private int pixel;
    private int bEdge;
    private Position pos;
    public Pixels(int x, int y, int pixel) {
        this.x = x;
        this.y = y;
        this.pixel = pixel;
        this.bEdge = 0;
        pixel2rgba();
        this.pos = Position.None;
    }

    public Pixels(int R, int G, int B, int A) {
        this.R = R;
        this.G = G;
        this.B = B;
        this.A = A;
        this.x = 0;
        this.y = 0;
        this.pixel = rgba2pixel();
        this.bEdge = 0;
        this.pos = Position.None;
    }

    private void pixel2rgba() {
        A = (pixel & 0xFF000000) >> 24;
        R = (pixel & 0x00FF0000) >> 16;
        G = (pixel & 0x0000FF00) >> 8;
        B = (pixel & 0x000000FF);
        int tmp = rgba2pixel();
    }

    private int rgba2pixel() {
        return ((A << 24) | (R << 16) | (G << 8) | B);
    }

    public int getA() {
        return A;
    }

    public int getB() {
        return B;
    }

    public int getR() {
        return R;
    }

    public int getX() {
        return x;
    }

    public int getY() {
        return y;
    }

    public Position getPos() {
        return pos;
    }

    public int getbEdge() {
        return bEdge;
    }

    public int getG() {
        return G;
    }
}
