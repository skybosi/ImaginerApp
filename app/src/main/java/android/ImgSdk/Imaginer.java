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
    private int[][][] boundrys;
    private int curX;
    private int curY;
    public Imaginer(Bitmap bitmap) {
        this.bitmap = bitmap;
        this.bmpWidth = bitmap.getWidth();
        this.bmpHeight = bitmap.getHeight();
        this.cimageData = new int[bmpWidth * bmpHeight];
        bitmap.getPixels(cimageData, 0, bmpWidth, 0, 0, bmpWidth, bmpHeight);
        this.curX = 0;
        this.curY = 0;
    }

    public synchronized boolean init() {
        return init(cimageData, bmpWidth, bmpHeight);
    }

    public int[] getImageData() {
        return cimageData;
    }
    public void JgetBoundrys() {
       boundrys = getBoundrys();
    }

    public int[] gotoNextPoint() {
        int next[] = new int[3];
        int linesize = 0;
        for(int i = curX;i < boundrys.length;++i )
        {
            linesize = boundrys[i].length;
            for(int j = curY;j < linesize;++j) {
               next = boundrys[curX][curY];
                curY++;
                break;
            }
            if(curY < linesize)
                break;
            curX++;
        }
        return next;
    }

    public int getStartX() {
        return boundrys[0][0][0];
    }

    public int getStartY() {
        return boundrys[0][0][1];
    }

    public synchronized void finalize() {
        cfinalize();
    }
/*
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
	*/

    /**
     * init bitmap data, to provider native .so
     */
    public native boolean init(int[] bmpData, int width, int height);

    /**
     * @return
     */
    public native int[][][] getBoundrys();

    private native void cfinalize();
}
