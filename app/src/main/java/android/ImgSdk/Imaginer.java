package android.ImgSdk;

import android.graphics.Bitmap;
import android.util.Log;

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
    private long[][] boundrys;
    private int curX;
    private int curY;
    private String TAG = "IMAGINER";
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
                next[0] = (int)((boundrys[i][curY] & 0xFFFFFFFE00000000L)>>33);
                next[1] = (int)((boundrys[i][curY] & 0x1FFFFFFFCL) >> 2);
                next[2] = (int)((boundrys[i][curY] & 3L));
                switch (next[2])
                {
                    case 0:
                        next[2] = 0;
                        break;
                    case 3:
                        next[2] = -2;
                        break;
                    case 2:
                        next[2] = -1;
                        break;
                    default:
                        next[2] = 0;
                }
                curY++;
                break;
            }
            if(curY < linesize)
                break;
            else {
                curY = 0;
                curX++;
            }
        }
        Log.d(TAG,"next pixle is x:" + next[0] + "\ty:" + next[1] + "\tedge:"+ next[2]);
        return next;
    }

    public int getStartX() {
        return (int)((boundrys[0][0] & 0xFFFFFFFE00000000L)>>33);
    }

    public int getStartY() {
        return (int)((boundrys[0][0] & 0x1FFFFFFFCL) >> 2);
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
    public native long[][] getBoundrys();

    private native void cfinalize();
}
