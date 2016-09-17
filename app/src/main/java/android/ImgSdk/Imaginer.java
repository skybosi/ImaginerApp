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
    //private Pixels[][] imageData;
    private int currPostion;
    private int startX;
    private int startY;

    public Imaginer(Bitmap bitmap) {
        this.currPostion = 0;
        this.bitmap = bitmap;
        this.bmpWidth = bitmap.getWidth();
        this.bmpHeight = bitmap.getHeight();
        //this.imageData = new Pixels[bmpWidth][bmpHeight];
        this.cimageData = new int[bmpWidth * bmpHeight];
        bitmap.getPixels(cimageData, 0, bmpWidth, 0, 0, bmpWidth, bmpHeight);
        this.startX = 0;
        this.startY = 0;
    }

    public synchronized boolean init() {
        return init(cimageData, bmpWidth, bmpHeight);
    }

    public int[] getImageData() {
        return cimageData;
    }

    public synchronized void addCimgaData(int pixels) {
        cimageData[currPostion++] = pixels;
    }

    public int[] getStartPoint(int x, int y) {
        int[] startpoint = new int[2];
        startpoint = isStartPoint(x, y);
        if (startpoint != null) {
            startX = startpoint[0];
            startY = startpoint[1];
            return startpoint;
        } else {
            return null;
        }
    }

    public int[] gotoNextPoint(int x, int y) {
        int[] nextpoint = new int[3];
        nextpoint = getNextPoint(x, y);
        if (nextpoint[0] == startX && nextpoint[1] == startY) {
            //next point add start point,will get next start point from current point
            if (getStartPoint(startX + 1, startY) != null) {
                nextpoint[0] = startX;
                nextpoint[1] = startY;
                nextpoint[2] = 6;
            }
        }
        return nextpoint;
    }

    public int getStartX() {
        return startX;
    }

    public int getStartY() {
        return startY;
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
     * get the boudray's start point
     */
    public native int[] isStartPoint(int x, int y);

    /**
     * After getStartPoint will from here to get next boudray point
     */
    public native int[] getNextPoint(int curX, int curY);

    private native void cfinalize();
}
