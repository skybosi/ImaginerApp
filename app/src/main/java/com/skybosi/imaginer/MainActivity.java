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

/* PoritionView 类*/

class PoritionView extends View {

    private Bitmap showPic = null;
    private int startX = 0;
    private int startY = 0;

    public PoritionView(Context context) {
        super(context);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        canvas.drawBitmap(showPic, startX, startY, null);
    }

    public void setBitmapShow(Bitmap b, int x, int y) {
        showPic = b;
        startX = x;
        startY = y;
    }
}

public class MainActivity extends Activity implements View.OnClickListener, View.OnLongClickListener {
    private String bmpFile = null;
    private Handler mHandler = null;
    private final static int REQUEST_CODE = 1;
    private boolean isPictue = false;
    private PoritionView poritonView = null;
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

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        SurfaceView sfv = (SurfaceView) findViewById(R.id.surface);
        ImageButton ib = (ImageButton) findViewById(R.id.openSD);
        Button bt = (Button) findViewById(R.id.nextPoint);
        ib.setOnClickListener(this);
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
        }
    }

    private void loadFile() {
        Intent intent = new Intent("android.intent.action.FILE");
        startActivityForResult(intent, REQUEST_CODE);
    }

    @Override
    public boolean onLongClick(View v) {
        if (v.getId() == R.id.nextPoint) {
            //String step = null;
            final EditText inputServer = new EditText(this);
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("Next Step Set").setIcon(android.R.drawable.ic_dialog_info).setView(inputServer)
                    .setNegativeButton("CANCEL", null);
            builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int which) {
                    inputServer.getText().toString();
                    ((Button) findViewById(R.id.nextPoint)).setText("NEXT" + "(" + inputServer.getText().toString() + ")");
                }
            });
            builder.show();
        }

        return false;
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
            float x = event.getX();
            float y = event.getY();
            if (inPicture(x, y)) {
                fullHere(x, y);
                Toast.makeText(MainActivity.this, "X:" + x + " Y:" + y, Toast.LENGTH_SHORT).show();
            } else {
                Toast.makeText(MainActivity.this, "onTouchEvent: Out oyf picture", Toast.LENGTH_SHORT).show();
                Log.d("Imaginer", "onTouchEvent: Out oyf picture");
            }
        }
        return super.onTouchEvent(event);
    }

    private void fullHere(float x, float y) {
        isExit = false;
        int newScale = (focuScale << 1) + 1;
        float scaleWidth = canvsWidth / newScale;
        float scaleHeight = canvsWidth / newScale;
        int px = (int) x - left;
        int py = (int) y - top;
        int pixelxy = bm.getPixel(px, py);
        int cx = px - focuScale;
        int cy = py - focuScale;
        if (cx < 0) {cx = 0;}
        if (cy < 0) {cy = 0;}
        bm.setPixel(px, py, resetColor);
        Log.i(TAG, "fullHere: createBitmap error" + pixelxy);
        Matrix matrix = new Matrix();
        // 缩放图片动作
        matrix.postScale(scaleWidth, scaleHeight);
        // 新得到的图片是原图片经过变换填充到整个屏幕的图片
        try {
            Bitmap picNewRes = Bitmap.createBitmap(bm, cx, cy, newScale, newScale, matrix, true);
            int picWidth = picNewRes.getWidth();
            int picHight = picNewRes.getHeight();
            Bitmap showPic = Bitmap.createBitmap(picNewRes, 0, 0, picWidth, picHight);
            poritonView = new PoritionView(this);
            int picleft = (canvsWidth - picWidth) / 2;
            int pictop = (canvsHight - picHight) / 2;
            poritonView.setBitmapShow(showPic, picleft, pictop);
            setContentView(poritonView);
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
        poritonView.setBitmapShow(bm, left, top);
        setContentView(poritonView);
        exit();
    }

    private void exit() {
        if (!isExit) {
            isExit = true;
            Toast.makeText(getApplicationContext(), "再按一次后退键退出主程序！", Toast.LENGTH_SHORT).show();
        } else {
            this.finish();
        }
    }

    //surfaceview draw on the canvas
    private void drawBmp(SurfaceHolder holder, Bitmap bmp) {
        Canvas canvas = holder.lockCanvas();
        canvsHight = canvas.getHeight();
        canvsWidth = canvas.getWidth();
        bmpHight = bmp.getHeight();
        bmpWidth = bmp.getWidth();
        left = (canvsWidth - bmpWidth) / 2;
        top = (canvsHight - bmpHight) / 2;
        if(left < 0)
            left = 0;
        if(top < 0)
            top = 0;
        canvas.drawBitmap(bm, left, top, new Paint());
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
