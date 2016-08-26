package com.skybosi.imaginer;

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
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Menu;
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
import android.view.WindowManager;

import java.util.Random;

import android.graphics.Matrix;

public class MainActivity extends Activity implements View.OnClickListener, View.OnLongClickListener {
    private String bmpFile = null;
    private Handler mHandler = null;
    private final static int REQUEST_CODE = 1;
    private boolean isPictue = false;
    private int canvsHight = 0;
    private int canvsWidth = 0;
    private int bmpHight = 0;
    private int bmpWidth = 0;
    private int left = 0;
    private int top = 0;
    private Bitmap bm = null;
    //private Canvas canvas = null;
    private int focuScale = 4;//foucs scale
    private String TAG = "Imaginer";
    private static boolean isExit = false;
    private SurfaceHolder holder = null;
    private int resetColor = Color.RED;
    private Canvas canvas = null;
    private Paint paint = null;
    private int nextSteps = 0;
    private int tmpSteps = 1;
    private int startX = -1;
    private int startY = -1;
    private boolean isBack = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        SurfaceView sfv = (SurfaceView) findViewById(R.id.surface);
        ImageButton ib = (ImageButton) findViewById(R.id.openSD);
        Button bt = (Button) findViewById(R.id.nextPoint);
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
        Intent intent = new Intent("android.intent.action.FILE");
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
                    tmpSteps  = Integer.parseInt(inputServer.getText().toString());
                    if(tmpSteps > 0) {
                        ((Button) findViewById(R.id.nextPoint)).setText("NEXT" + "(" + inputServer.getText().toString() + ")");
                    }else{
                        ((Button) findViewById(R.id.nextPoint)).setText("NEXT");
                    }
                }
            });
            builder.show();
        }
        return false;
    }
    //init the picture that will be deal with
    private void init_bmp(){
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inMutable = true;//set Mutable,so can setpixel
        bm = BitmapFactory.decodeFile(bmpFile, options);
        if (bm == null) {
            if (!bmpFile.isEmpty()) {//is not a picture
                mHandler.sendEmptyMessage(0);
            }
        } else {
            drawBmp(holder, bm);
            isPictue = true;
        }
    }

    class MyThread extends Thread {
        SurfaceHolder holder;
        boolean refresh = true;
        Random random = new Random(0);
        public MyThread(SurfaceHolder holder) {
            this.holder = holder;
        }
        @Override
        public void run() {
            super.run();
            init_bmp();
            int ranLeft = -1;
            int ranTop =  -1;
            while(refresh){
                if(nextSteps-- > 0 && startX > 0 && startX > 0) {
                    if(isBack || ranLeft < 0 && ranTop < 0) {
                        ranLeft = startX;
                        ranTop = startY;
                        isBack = false;
                    }
                    ranLeft += random.nextInt(2);
                    ranTop += random.nextInt(2);
                    //can not out of picture
                    if (inPicture(ranLeft, ranTop)) {
                        fullHere(ranLeft, ranTop);
                    } else {
                        break;
                    }
                }
                try {
                    Thread.sleep(200);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }

        void mystop() {
            refresh = false;
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        //getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == REQUEST_CODE) {
            if (resultCode == FileView.RESULT_CODE) {
                Bundle bundle = data.getExtras();
                bmpFile = bundle.getString("filename");
                //Toast.makeText(MainActivity.this, bmpFile, Toast.LENGTH_LONG).show();
                ((TextView) findViewById(R.id.showFilepath)).setText(bmpFile);
            }
        }
    }

    // 实现onTouchEvent方法
    public boolean onTouchEvent(MotionEvent event) {
        // 如果是按下操作
        if (event.getAction() == MotionEvent.ACTION_DOWN && isPictue) {
            float touchX = event.getX();
            float touchY = event.getY();
            if (inPicture(touchX, touchY)) {
                startX = (int)touchX;
                startY = (int)touchY;
                fullHere(startX,startY);
                Toast.makeText(MainActivity.this, "X:" + touchX + " Y:" +touchY, Toast.LENGTH_SHORT).show();
            } else {
                Toast.makeText(MainActivity.this, "onTouchEvent: Out oyf picture", Toast.LENGTH_SHORT).show();
                Log.d(TAG, "onTouchEvent: Out oyf picture");
            }
        }
        return super.onTouchEvent(event);
    }

    private void fullHere(int x, int y) {
        isExit = false;
        int newScale = (focuScale << 1) + 1;
        float scaleWidth = canvsWidth / newScale;
        float scaleHeight = canvsWidth / newScale;
        int px = (int) x - left;
        int py = (int) y - top;
        int pixelxy = bm.getPixel(px, py);
        //Toast.makeText(getApplicationContext(), "RGB:" + Integer.toHexString(pixelxy),  Toast.LENGTH_SHORT).show();
        int cx = px - focuScale;
        int cy = py - focuScale;
        if (cx < 0) {cx = 0;}
        if (cy < 0) {cy = 0;}
        bm.setPixel(px, py, resetColor);
        Log.i(TAG, "fullHere: createBitmap error" + pixelxy);
        Matrix matrix = new Matrix();
        // 缩放图片动作
        //matrix.setScale(scaleWidth,scaleHeight);
        matrix.postScale(scaleWidth, scaleHeight);
        // 新得到的图片是原图片经过变换填充到整个屏幕的图片
        try {
            Bitmap picNewRes = Bitmap.createBitmap(bm, cx, cy, newScale, newScale, matrix, true);
            drawBmp(holder, picNewRes);
        } catch (Exception e) {
            Log.e(TAG, "fullHere: createBitmap error" + e.toString());
        }
    }

    public void setfocuScale(int focuscale){
        focuScale = focuscale;
    }

    private boolean inPicture(float x, float y) {
        if (x > left && x < left + bmpWidth &&
                y > top && y < top + bmpHight)
            return true;
        return false;
    }

    @Override
    public synchronized void onBackPressed() {
        if (bm != null) {
            cleanScreen();
            drawBmp(holder,bm);
            startX = startY = -1;
            isBack = true;
        }
        exit();
    }

    private void exit() {
        if (!isExit) {
            isExit = true;
            Toast.makeText(getApplicationContext(), "再按一次后退键退出主程序！", Toast.LENGTH_SHORT).show();
        } else {
            thread.mystop();
            this.finish();
        }
    }

    //surfaceview draw on the canvas center
    private void drawBmp(SurfaceHolder sholder, Bitmap bmp) {
        canvas = sholder.lockCanvas();
        canvsHight = canvas.getHeight();
        canvsWidth = canvas.getWidth();
        if(paint == null)
        {
            paint = new Paint();
        }else{
            paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.CLEAR));
            canvas.drawPaint(paint);
            paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.SRC));
        }
        if(bmp.equals(bm)) {
            bmpHight = bmp.getHeight();
            bmpWidth = bmp.getWidth();
            left = (canvsWidth - bmpWidth) / 2;
            top = (canvsHight - bmpHight) / 2;
            if (left < 0)
                left = 0;
            if (top < 0)
                top = 0;
            canvas.drawBitmap(bm, left, top, paint);
        }else
        {
            int picWidth = bmp.getWidth();
            int picHight = bmp.getHeight();
            int picleft = (canvsWidth - picWidth) / 2;
            int pictop = (canvsHight - picHight) / 2;
            if(picleft < 0)
                picleft = 0;
            if(pictop < 0)
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
        */
