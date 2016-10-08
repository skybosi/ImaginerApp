package com.skybosi.imaginer;

import android.ImgSdk.Imaginer;
import android.ImgSdk.Pixels;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.os.Bundle;
import android.support.v7.widget.Toolbar;
import android.support.v7.widget.Toolbar.OnMenuItemClickListener;
import android.util.Log;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;
import android.os.Handler;
import android.os.Message;

import java.util.Random;

import android.graphics.Matrix;

public class MainActivity extends Activity implements View.OnClickListener, View.OnLongClickListener, OnMenuItemClickListener {
    private String bmpFile = null;
    private Handler mHandler = null;
    private final static int REQUEST_CODE = 1;
    private int canvsHight = 0;
    private int canvsWidth = 0;
    private int bmpHight = 0;
    private int bmpWidth = 0;
    private int left = 0;//相对于canvas，不是相对于整个屏幕（左上）
    private int top = 0; //相对于canvas，不是相对于整个屏幕（左上）
    private Bitmap bm = null;
    //private Canvas canvas = null;
    private int focuScale = 4;//foucs scale
    private int nextSpeeds = 200;//ms
    private String TAG = "IMAGINER";
    private static boolean isExit = false;
    private SurfaceHolder holder = null;
    private int resetColor = Color.RED;
    private Canvas canvas = null;
    private Paint paint = null;
    private int nextSteps = 0;
    private int tmpSteps = 1;
    private int startX = -1;
    private int startY = -1;
    private boolean isNewStart = false;
    private boolean isBack = false;
    private int[] location = new int[2];
    private SurfaceView sfv = null;
    private Toolbar toolbar = null;
    private boolean currFoucStatus = false;//false:is src picture;true:is foucs picture
    private int returnValue = -1;
    private Imaginer imaginer = null;
    private int currX = -1;
    private int currY = -1;

    //get surfaceview's location for the location on the picture
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        sfv.getLocationOnScreen(location);
        //sfv.getLocationInWindow(location);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        toolbar = (Toolbar) findViewById(R.id.toolbar);
        toolbar.inflateMenu(R.menu.menu_main);//设置右上角的填充菜单
        toolbar.setLogo(R.mipmap.imaginer);
        toolbar.setOnMenuItemClickListener(this);
        sfv = (SurfaceView) findViewById(R.id.surface);
        ImageButton ib = (ImageButton) findViewById(R.id.openSD);
        Button bt = (Button) findViewById(R.id.nextPoint);
        isExit = false;
        ib.setOnClickListener(this);
        bt.setOnClickListener(this);
        bt.setOnLongClickListener(this);
        mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 0:
                        Toast.makeText(getApplicationContext(), "You choose is not a picture!", Toast.LENGTH_LONG).show();
                    case 1:
                        Toast.makeText(MainActivity.this, "This getBoundray loop is Finish", Toast.LENGTH_SHORT).show();
                        break;
                    case 2:
                        isExit = false;
                    default:
                        break;
                }
            }
        };
        holder = sfv.getHolder();
        holder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceDestroyed(SurfaceHolder arg0) {
                if (thread != null)
                    thread.mystop();
            }

            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                if (bmpFile != null) {
                    thread = new MyThread(holder);
                    thread.start();
                }
            }

            @Override
            public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2, int arg3) {
                // TODO Auto-generated method stub

            }
        });
        Toast.makeText(MainActivity.this, "You can long click to set next steps you Want!", Toast.LENGTH_LONG).show();
    }

    MyThread thread = null;

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.openSD) {
            loadFile();
        } else if (v.getId() == R.id.nextPoint) {
            if (tmpSteps <= 1)
                nextSteps = 1;
            else {
                nextSteps = tmpSteps;
            }
        }
    }

    private void loadFile() {
        isExit = false;
        Intent intent = new Intent("android.intent.action.FILE");
        if (bmpFile != null) {//传入上次查找的路径
            Bundle bundle = new Bundle();
            String lastPath = bmpFile.substring(0, bmpFile.lastIndexOf("/"));
            bundle.putString("lastPath", lastPath);
            intent.putExtras(bundle);
        }
        startActivityForResult(intent, REQUEST_CODE);
    }

    @Override
    public boolean onLongClick(View v) {
        if (v.getId() == R.id.nextPoint) {
            final EditText inputServer = new EditText(this);
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("Next Step Set").setIcon(android.R.drawable.ic_dialog_info).setView(inputServer)
                    .setNegativeButton("CANCEL", null);
            builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int which) {
                    try {
                        tmpSteps = Integer.parseInt(inputServer.getText().toString());
                        if (tmpSteps > 0) {
                            ((Button) findViewById(R.id.nextPoint)).setText("NEXT" + "(" + inputServer.getText().toString() + ")");
                        } else {
                            ((Button) findViewById(R.id.nextPoint)).setText("NEXT");
                        }
                    } catch (Exception e) {
                        Toast.makeText(MainActivity.this, "Sorry;Yours Input type is Error,please try again!", Toast.LENGTH_SHORT).show();
                        Log.e(TAG, "onClick:input type error");
                    }
                }
            });
            builder.show();
            //inputDialog("Next Step Set");
            tmpSteps = returnValue;
            if (tmpSteps > 0) {
                ((Button) findViewById(R.id.nextPoint)).setText("NEXT" + "(" + returnValue + ")");
            } else {
                ((Button) findViewById(R.id.nextPoint)).setText("NEXT");
            }
        }
        return false;
    }

    //init the picture that will be deal with
    private void init_bmp() {
        if (bm != null)
            bm.recycle();
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inMutable = true;//set Mutable,so can setpixel
        bm = BitmapFactory.decodeFile(bmpFile, options);
        if (bm == null) {
            if (!bmpFile.isEmpty()) {//is not a picture
                mHandler.sendEmptyMessage(0);
            }
        } else {
            if (imaginer != null) {
                imaginer.finalize();//回收Native code分配的内存
                imaginer = null;
                currX = -1; //reset cutrent point
                currY = -1;
            }
            recodeBmp(bm);
            drawBmp(holder, bm);
            currFoucStatus = true;
        }
    }

    //set bitmap data to int[],for native code deal with
    private void recodeBmp(Bitmap bm) {
        if (bm != null) {
            if (imaginer == null) {
                imaginer = new Imaginer(bm);
            }
            /*
            int W = bm.getWidth();
            int H = bm.getHeight();
            int x = 0,y = 0;
            int pixel = 0;
            //Pixels pixels = null;
            for (y = 0; y < H; ++y) {
                for (x = 0; x < W; ++x) {
                    pixel = bm.getPixel(x, y);
                    imaginer.addCimgaData(pixel);
                   // imaginer.setImageData(pixel, x, y);
                }
            }
            */
        }
    }

    @Override
    public boolean onMenuItemClick(MenuItem item) {
        int menuItemId = item.getItemId();
        switch (menuItemId) {
            case R.id.about:
                new AboutDialog(this).show();
                break;
            case R.id.setFoucs:
                //inputDialog("You Can set foucs Range");
                //focuScale = returnValue;
                final EditText inputServer = new EditText(this);
                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                builder.setTitle("You Can set foucs Range").setIcon(android.R.drawable.ic_dialog_info).setView(inputServer)
                        .setNegativeButton("CANCEL", null);
                builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        try {
                            focuScale = Integer.parseInt(inputServer.getText().toString());
                        } catch (Exception e) {
                            Toast.makeText(MainActivity.this, "Sorry;Yours Input type is Error,please try again!", Toast.LENGTH_SHORT).show();
                            Log.e(TAG, "onClick:input type error");
                        }
                    }
                });
                builder.show();
                //setfocuScale(focuScale);
                break;
            case R.id.nextSpeed:
                //inputDialog("You Can set next speed(ms)");
                //focuScale = returnValue;
                final EditText nextSpeedServer = new EditText(this);
                AlertDialog.Builder nextSpeedbuilder = new AlertDialog.Builder(this);
                nextSpeedbuilder.setTitle("You Can set next speed(ms)").setIcon(android.R.drawable.ic_dialog_info).setView(nextSpeedServer)
                        .setNegativeButton("CANCEL", null);
                nextSpeedbuilder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        try {
                            nextSpeeds = Integer.parseInt(nextSpeedServer.getText().toString());
                        } catch (Exception e) {
                            Toast.makeText(MainActivity.this, "Sorry;Yours Input type is Error,please try again!", Toast.LENGTH_SHORT).show();
                            Log.e(TAG, "onClick:input type error");
                        }
                    }
                });
                nextSpeedbuilder.show();
                //setNextSpeed(nextSpeeds);
                break;
            default:
                break;
        }
        return false;
    }

    class MyThread extends Thread {
        SurfaceHolder holder;
        private boolean refresh = true;
        Random random = new Random(0);

        public MyThread(SurfaceHolder holder) {
            this.holder = holder;
        }

        @Override
        public void run() {
            super.run();
            init_bmp();
            Bitmap newBmp;
            int[] newPoint = null;
            if (imaginer != null && imaginer.init()) {
                Log.d(TAG, "MyThread run: init Cbitmap finish");
                imaginer.JgetBoundrys();
                startX = imaginer.getStartX();
                startY = imaginer.getStartY();
                newBmp = fullHere(startX, startY);
                if (newBmp != null) {
                    drawBmp(holder, newBmp);
                }
                while (refresh) {
                    if (nextSteps-- > 0 && startX > 0 && startY > 0) {
                        if (isNewStart || currX < 0 && currY < 0) {
                            currX = startX;
                            currY = startY;
                            isNewStart = false;
                            isBack = false;
                            isExit = false;
                        }
                        newPoint = imaginer.gotoNextPoint();
                        if(newPoint[0] == 0 && newPoint[1]==0 && newPoint[1] == 0) {
                            Log.d(TAG, "get next point is finished!\n");
                            break;
                        }
                        if (newPoint != null) {
                            currX = newPoint[0];
                            currY = newPoint[1];
                            if (currFoucStatus) {//draw on the foucs picture
                                newBmp = fullHere(currX, currY);
                                if (newBmp != null) {
                                    drawBmp(holder, newBmp);
                                }
                            } else {//draw on the src picture
                                newBmp = getNext(currX, currY);
                                drawBmp(holder, newBmp);
                            }
                        } else {
                            Log.d(TAG, "MyThread run: gotoNextPoint error");
                        }
                    }
                    try {
                        Thread.sleep(nextSpeeds);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            } else {
                Log.e(TAG, "MyThread run: init Cbitmap not OK!");
            }
        }

        public boolean getRrefresh() {
            return refresh;
        }

        public void setRefresh(boolean status) {
            refresh = status;
        }

        void mystop() {
            refresh = false;
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == REQUEST_CODE) {
            if (resultCode == FileView.RESULT_CODE) {
                Bundle bundle = data.getExtras();
                bmpFile = bundle.getString("filename");
                //Toast.makeText(MainActivity.this, bmpFile, Toast.LENGTH_LONG).show();
                ((TextView) findViewById(R.id.toolbar_title)).setText(bmpFile);
            }
        }
    }

    // 实现onTouchEvent方法
    public boolean onTouchEvent(MotionEvent event) {
        // 如果是按下操作
        if (event.getAction() == MotionEvent.ACTION_DOWN) {
            if (!currFoucStatus) {//at src picture
                if (bm != null) {
                    Bitmap fullBmp = fullHere(currX, currY);
                    if (fullBmp != null)
                        drawBmp(holder, fullBmp);
                    /*
                    float touchX = event.getX();
                    float touchY = event.getY();
                    if (inPicture(touchX, touchY)) {
                        startX = (int) touchX;
                        startY = (int) touchY;
                        isNewStart = true;
                        isExit = false;
                        nextSteps = 0;
                        Bitmap fullBmp = fullHere(startX, startY);
                        if (fullBmp != null)
                            drawBmp(holder, fullBmp);
                        //Toast.makeText(MainActivity.this, "X:" + touchX + " Y:" + touchY, Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(MainActivity.this, "onTouchEvent: Out oyf picture", Toast.LENGTH_SHORT).show();
                        Log.d(TAG, "onTouchEvent: Out oyf picture");
                    }*/
                }
            } else {//at foucs picture
                nextSteps = 1;
            }
        }
        return super.onTouchEvent(event);
    }

    private Bitmap getNext(int x, int y) {
        int px = x; //来自真实图片的x，不需要矫正
        int py = y; //来自真实图片的y，不需要矫正
        /*
        int px = x - left - location[0]; //获取相对于图片左上点的x
        int py = y - top - location[1];  //获取相对于图片左上点的y
        int pixelxy = bm.getPixel(px, py);
        Pixels pixels = new Pixels(px,py,pixelxy);
        */
        int pixelxy = bm.getPixel(px, py);
        bm.setPixel(px, py, 0xFFFFFFFF-pixelxy);
        return bm;
    }

    private Bitmap fullHere(int x, int y) {
        isBack = false;
        int newScale = (focuScale << 1) + 1;
        float scaleWidth = canvsWidth / newScale;
        float scaleHeight = canvsWidth / newScale;
        int px = x; //来自真实图片的x，不需要矫正
        int py = y; //来自真实图片的y，不需要矫正
        /*
        int px = x - left - location[0]; //获取相对于图片左上点的x
        int py = y - top - location[1];  //获取相对于图片左上点的y
        int pixelxy = bm.getPixel(px, py);
        Pixels pixels = new Pixels(px,py,pixelxy);
        //Toast.makeText(getApplicationContext(), "RGB:" + Integer.toHexString(pixelxy),  Toast.LENGTH_SHORT).show();
        */
        int cx = px - focuScale;
        int cy = py - focuScale;
        if (cx < 0) {
            cx = 0;
        }
        if (cy < 0) {
            cy = 0;
        }
        int pixelxy = bm.getPixel(px, py);
        bm.setPixel(px, py, 0xFFFFFFFF-pixelxy);
        //Log.i(TAG, "fullHere: createBitmap error" + pixelxy);
        Matrix matrix = new Matrix();
        // 缩放图片动作
        //matrix.setScale(scaleWidth,scaleHeight);
        matrix.postScale(scaleWidth, scaleHeight);
        Bitmap picNewRes = null;
        // 新得到的图片是原图片经过变换填充到整个屏幕的图片
        try {
            picNewRes = Bitmap.createBitmap(bm, cx, cy, newScale, newScale, matrix, true);
        } catch (Exception e) {
            Log.e(TAG, "fullHere: createBitmap error" + e.toString());
        } finally {
            return picNewRes;
        }
    }

    public synchronized void setfocuScale(int focuscale) {
        focuScale = focuscale;
    }

    public synchronized void setNextSpeed(int nextSpeed) {
        nextSpeeds = nextSpeed;
    }

    private boolean inPicture(float x, float y) {
        if (x > left + location[0] && x < left + bmpWidth + location[0] &&
                y > top + location[1] && y < top + bmpHight + location[1])
            return true;
        return false;
    }

    @Override
    public synchronized void onBackPressed() {
        if (!isBack && bm != null && startX != -1) {
            cleanScreen();
            drawBmp(holder, bm);
            isBack = true;
        } else {
            exit();
        }
    }

    private void exit() {
        if (!isExit) {
            isExit = true;
            Toast.makeText(getApplicationContext(), "再按一次后退键退出主程序！", Toast.LENGTH_SHORT).show();
            // 利用handler延迟发送更改状态信息
            mHandler.sendEmptyMessageDelayed(2, 2000);
        } else {
            if (thread != null)
                thread.mystop();
            this.finish();
        }
    }

    //surfaceview draw on the canvas center
    private void drawBmp(SurfaceHolder sholder, Bitmap bmp) {
        canvas = sholder.lockCanvas();
        canvsHight = canvas.getHeight();
        canvsWidth = canvas.getWidth();
        if (paint == null) {
            paint = new Paint();
        } else {
            paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.CLEAR));
            canvas.drawPaint(paint);
            paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.SRC));
        }
        if (bmp.equals(bm)) {
            currFoucStatus = false;
            bmpHight = bmp.getHeight();
            bmpWidth = bmp.getWidth();
            left = (canvsWidth - bmpWidth) / 2;
            top = (canvsHight - bmpHight) / 2;
            if (left < 0)
                left = 0;
            if (top < 0)
                top = 0;
            canvas.drawBitmap(bm, left, top, paint);
        } else {
            currFoucStatus = true;
            int picWidth = bmp.getWidth();
            int picHight = bmp.getHeight();
            int picleft = (canvsWidth - picWidth) / 2;
            int pictop = (canvsHight - picHight) / 2;
            if (picleft < 0)
                picleft = 0;
            if (pictop < 0)
                pictop = 0;
            canvas.drawBitmap(bmp, picleft, pictop, paint);
        }
        sholder.unlockCanvasAndPost(canvas);
    }

    private void cleanScreen() {
        //clean old draw
        canvas = holder.lockCanvas();
        paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.CLEAR));
        canvas.drawPaint(paint);
        paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.SRC));
        holder.unlockCanvasAndPost(canvas);
    }

    public class AboutDialog extends AlertDialog {
        public AboutDialog(Context context) {
            super(context);
            final View view = getLayoutInflater().inflate(R.layout.about, null);
            setTitle("Imaginer   v1.0.0");
            setView(view);
        }
    }
    /*
    private synchronized void inputDialog(String Tilte)
    {
        Message msg = null;
        final EditText inputServer = new EditText(this);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(Tilte).setIcon(android.R.drawable.ic_dialog_info).setView(inputServer)
                .setNegativeButton("CANCEL", null);
       builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
           public void onClick(DialogInterface dialog, int which) {
               returnValue = Integer.parseInt(inputServer.getText().toString());
           }
       });
       builder.show();
    }
    */
}


            /*
            while(refresh){
                Canvas canvas = holder.lockCanvas();
                int left = random.nextInt(canvas.getWidth());
                int top =  random.nextInt(canvas.getHeight());

                Bitmap bm = BitmapFactory.decodeResource(getResources(), R.mipmap.ic_launcher);
                canvas.drawBitmap(bm, left, top, new Paint());
                holder.unlockCanvasAndPost(canvas);
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            */

        /*
        DisplayMetrics dm = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(dm);
        int screenWidth = dm.widthPixels;                //水平分辨率
        int screenHeight = dm.heightPixels;              //垂直分辨率

        WindowManager wm = this.getWindowManager();
        int width = wm.getDefaultDisplay().getWidth();
        int height = wm.getDefaultDisplay().getHeight();
        */
