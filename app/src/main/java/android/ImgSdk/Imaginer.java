package android.ImgSdk;

import android.graphics.Bitmap;

/**
 * Created by dejian.fei on 2016/8/30.
 */

public final class Imaginer {
    static {
        System.loadLibrary("Imaginer");//load libimaginer.so
    }

    Bitmap bitmap;
    private int bmpWidth;
    private int bmpHeight;
    private int[] cimageData;
    private Pixels[][] imageData;
    private int currPostion;

    public boolean init()
    {
        return init(cimageData,bmpWidth,bmpHeight);
    }
    public synchronized void addCimgaData(int pixels)
    {
        cimageData[ currPostion++] = pixels;
    }
    public Imaginer(Bitmap bitmap) {
        this.currPostion = 0;
        this.bitmap = bitmap;
        this.bmpWidth = bitmap.getWidth();
        this.bmpHeight = bitmap.getHeight();
        this.imageData = new Pixels[bmpWidth][bmpHeight];
        this.cimageData = new int[bmpWidth*bmpHeight];
    }
    public void finalize()
    {
        cfinalize();
    }

    public void setImageData(Pixels pixels, int x, int y) {
        imageData[x][y] = pixels;
    }

    public void setImageData(int intpixel, int x, int y) {
        imageData[x][y] = new Pixels(x, y, intpixel);
    }

    public void show(int x, int y) {
        System.out.println("x = " + x + " y = " + y);
        System.out.println("R: " + imageData[x][y].getR());
        System.out.println("G: " + imageData[x][y].getG());
        System.out.println("B: " + imageData[x][y].getB());
        System.out.println("A: " + imageData[x][y].getA());
    }

    public native boolean init(int[] bmpData,int width,int height);

    public native boolean isStartPoint();

    public native boolean getNextPoint();

    private native float getSimilarity();

    private native void  cfinalize();
}
