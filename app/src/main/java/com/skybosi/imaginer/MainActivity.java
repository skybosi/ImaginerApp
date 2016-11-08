package com.skybosi.imaginer;

import android.ImgSdk.Imaginer;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PointF;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.os.Bundle;
import android.support.v7.widget.Toolbar;
import android.support.v7.widget.Toolbar.OnMenuItemClickListener;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import android.os.Handler;
import android.os.Message;
import android.graphics.Matrix;

public class MainActivity extends Activity implements View.OnClickListener, View.OnLongClickListener, OnMenuItemClickListener, View.OnTouchListener {
    //for send message
    private Handler mHandler = null;
    //for save the image
    private final static int REQUEST_CODE = 1;
    private String bmpFile = null;
    private Bitmap bm = null;
    private int bmpHight = 0;
    private int bmpWidth = 0;
    private int curWitdth = 0;
    private int curHeight = 0;
    //for log and exit
    private String TAG = "IMAGINER";
    private static boolean isExit = false;
    //for show image at surfaceview
    private SurfaceHolder holder = null;
    private SurfaceView sfv = null;
    private Canvas canvas = null;
    private Paint paint = null;
    private int canvsHight = 0;
    private int canvsWidth = 0;
    //for highlight the boundary
    private Imaginer imaginer = null;
    private int nextSteps = 0;
    private int tmpSteps = 1;
    private int startX = -1;
    private int startY = -1;
    private int returnValue = -1;
    private int nextSpeeds = 200;//ms
    //for set color at toolbar
    private Toolbar toolbar = null;
    private int ColorA = 0xFF000000;
    private int ColorR = 0;
    private int ColorG = 0;
    private int ColorB = 0;
    private int resetColor = 0;
    private boolean isBack = false;
    //for show a image at center
    private float phoneW = 0;
    private float phoneH = 0;
    //for zoom and drag
    private float distance = 0.0f;
    private float preDistance = 0.0f;
    private PointF mid = new PointF();//两指中点
    private Matrix oldmatrix = new Matrix();
    private Matrix newmatrix = new Matrix();
    private float scale2finger = 1.0f;
    private Bitmap bmp2finger = null;
    private float lastX = -1;
    private float lastY = -1;
    private int currX = -1;
    private int currY = -1;
    private static final int NONE = 0;
    private static final int DRAG = 1;
    private static final int ZOOM = 2;
    private int mode = NONE;
    //for lock the image
    private int clickcount = 0;
    private long firstClick = 0;
    private long secondClick = 0;
    private boolean lockDRAG = false;
    //for make image at position
    private float picleft = -1;
    private float pictop = -1;
    private int[] location = new int[2];
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
        //RelativeLayout animLayout = (RelativeLayout)findViewById(R.id.animmenu);
        //Button ib  = (Button)animLayout.findViewById(R.id.openfile);
        ImageButton ib = (ImageButton) findViewById(R.id.openSD);
        Button bt = (Button) findViewById(R.id.nextPoint);
        isExit = false;
        //animLayout.setOnClickListener(this);
        ib.setOnClickListener(this);
        bt.setOnClickListener(this);
        bt.setOnLongClickListener(this);
        sfv.setOnTouchListener(this);
        toolbar.setOnClickListener(this);
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
                    case 3:
                        Toast.makeText(MainActivity.this, "Get boundry is OK", Toast.LENGTH_SHORT).show();
                        break;
                    case 4:
                        Toast.makeText(MainActivity.this, "Get boundry is finish", Toast.LENGTH_SHORT).show();
                        break;
                    case 5:
                        Toast.makeText(MainActivity.this, "Get boundry is fair", Toast.LENGTH_SHORT).show();
                        break;
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
        WindowManager manager = this.getWindowManager();
        DisplayMetrics outMetrics = new DisplayMetrics();
        manager.getDefaultDisplay().getMetrics(outMetrics);
        phoneW = outMetrics.widthPixels;
        phoneH = outMetrics.heightPixels;
    }

    MyThread thread = null;

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            //case R.id.animmenu:
            //    break;
            case R.id.openSD:
                nextSteps = 1;
                loadFile();
                lockDRAG = false;
                toolbar.setLogo(R.mipmap.imaginer);
                break;
            case R.id.nextPoint:
                if (tmpSteps <= 1)
                    nextSteps = 1;
                else {
                    nextSteps = tmpSteps;
                }
                break;
            case R.id.toolbar:
                if (bm != null) {
                    clickcount++;
                    if (clickcount == 1) {
                        firstClick = System.currentTimeMillis();
                    } else if (clickcount == 2) {
                        secondClick = System.currentTimeMillis();
                        if (secondClick - firstClick < 1000) {
                            if (lockDRAG) {
                                lockDRAG = false;
                                toolbar.setLogo(R.mipmap.imaginer);
                            } else {
                                lockDRAG = true;
                                toolbar.setLogo(R.mipmap.imaginerlock);
                            }
                        }
                        clickcount = 0;
                        firstClick = 0;
                        secondClick = 0;
                    }
                }
                break;
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

    void clearAll()
    {
        newmatrix.reset();
        oldmatrix.reset();
        lastX = lastY = 0;
        scale2finger = 1.0f;
        distance = preDistance = 0;
        mid.set(0, 0);
    }
    //init the picture that will be deal with
    private void init_bmp() {
        if (bm != null) {
            bm.recycle();
            System.gc();
            clearAll();
        }
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inMutable = true;//set Mutable,so can setpixel
        bm = BitmapFactory.decodeFile(bmpFile, options);
        if (bm == null) {
            if (!bmpFile.isEmpty()) {//is not a picture
                mHandler.sendEmptyMessage(0);
            }
        } else {
            bmp2finger = bm;
            if (imaginer != null) {
                imaginer.finalize();//回收Native code分配的内存
                imaginer = null;
                currX = -1; //reset cutrent point
                currY = -1;
            }
            recodeBmp(bm);
            drawBmp(holder, bm, 0, 0);
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
            case R.id.nextSpeed:
                //inputDialog("You Can set next speed(ms)");
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
            case R.id.setColor:
                resetColor = Color.WHITE;
                // 1. 布局文件转换为View对象
                LayoutInflater inflater = LayoutInflater.from(this);
                final LinearLayout layout = (LinearLayout) inflater.inflate(R.layout.color_select, null);
                // 2. 新建对话框对象
                final Dialog dialog = new AlertDialog.Builder(MainActivity.this).create();
                dialog.setCancelable(false);
                dialog.show();
                dialog.getWindow().setContentView(layout);
                View vr = (View) layout.findViewById(R.id.colorCtrlR);
                SeekBar seekBarR = (SeekBar) vr.findViewById(R.id.seek);
                TextView tr = (TextView) vr.findViewById(R.id.text);
                tr.setText("R:");
                //SeekBar seekBarR = (SeekBar) layout.findViewById(R.id.colorCtrlR);
                seekBarR.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                    @Override
                    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                        Button colotbt = (Button) layout.findViewById(R.id.colorShow);
                        ColorR = (progress << 16);
                        colotbt.setBackgroundColor(ColorA | ColorR | ColorG | ColorB);
                    }

                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar) {
                    }

                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar) {
                    }
                });
                View vg = (View) layout.findViewById(R.id.colorCtrlG);
                SeekBar seekBarG = (SeekBar) vg.findViewById(R.id.seek);
                TextView tg = (TextView) vg.findViewById(R.id.text);
                tg.setText("G:");
                //SeekBar seekBarG = (SeekBar) layout.findViewById(R.id.colorCtrlG);
                seekBarG.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                    @Override
                    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                        Button colotbt = (Button) layout.findViewById(R.id.colorShow);
                        ColorG = (progress << 8);
                        colotbt.setBackgroundColor(ColorA | ColorR | ColorG | ColorB);
                    }

                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar) {
                    }

                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar) {
                    }
                });
                View vb = (View) layout.findViewById(R.id.colorCtrlB);
                SeekBar seekBarB = (SeekBar) vb.findViewById(R.id.seek);
                TextView tb = (TextView) vb.findViewById(R.id.text);
                tb.setText("B:");
                //SeekBar seekBarB = (SeekBar) layout.findViewById(R.id.colorCtrlB);
                seekBarB.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                    @Override
                    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                        Button colotbt = (Button) layout.findViewById(R.id.colorShow);
                        ColorB = progress;
                        colotbt.setBackgroundColor(ColorA | ColorR | ColorG | ColorB);
                    }

                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar) {
                    }

                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar) {
                    }
                });
                // 3. 确定按钮
                Button btnOK = (Button) layout.findViewById(R.id.yes);
                btnOK.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        resetColor = ColorA | ColorR | ColorG | ColorB;
                        //ColorA = ColorR = ColorG = ColorB = 0;
                        //Toast.makeText(getApplicationContext(), "ok", Toast.LENGTH_SHORT).show();
                        dialog.dismiss();
                    }
                });
                // 4. 取消按钮
                Button btnCancel = (Button) layout.findViewById(R.id.no);
                btnCancel.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        //Toast.makeText(getApplicationContext(), "cancel", Toast.LENGTH_SHORT).show();
                        dialog.dismiss();
                    }
                });
                break;
            case R.id.cut:
                if(imaginer != null) {
                    int[] cutedImag = imaginer.cutAll(-1, -1);
                    if (cutedImag != null) {
                        Bitmap.Config config = bm.getConfig();
                        Bitmap move = Bitmap.createBitmap(cutedImag, bmpWidth, bmpHight, config);
                        Bitmap picNewRes2 = Bitmap.createBitmap(move, 0, 0, bmpWidth, bmpHight, oldmatrix, true);
                        drawBmp(holder, picNewRes2, 0, 0);
                        if(move != null && !move.isRecycled()){
                            move.recycle();
                            move = null;
                        }
                        if(picNewRes2 != null && !picNewRes2.isRecycled()){
                            picNewRes2.recycle();
                            picNewRes2 = null;
                        }
                        System.gc();
                    }
                }
                break;
            default:
                break;
        }
        return false;
    }

    class MyThread extends Thread {
        SurfaceHolder holder;
        private boolean refresh = true;

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
                if(imaginer.JgetBoundrys()) {
                    mHandler.sendEmptyMessage(3);
                    startX = imaginer.getStartX();
                    startY = imaginer.getStartY();
                    while (refresh) {
                        if (nextSteps-- > 0) {
                            newPoint = imaginer.gotoNextPoint();
                            if (newPoint[0] == 0 && newPoint[1] == 0 && newPoint[2] == 0) {
                                mHandler.sendEmptyMessage(4);
                                Log.d(TAG, "get next point is finished!\n");
                                break;
                            }
                            currX = newPoint[0];
                            currY = newPoint[1];
                            newBmp = highlight(bm, currX, currY);
                            if (newBmp != null)
                                drawBmp(holder, newBmp, 0, 0);
                        }
                        try {
                            Thread.sleep(nextSpeeds / 10);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                }
                else
                {
                    Log.e(TAG, "get Boundrys is fair!\n");
                    mHandler.sendEmptyMessage(5);
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

    //too slow
    Bitmap createBitmap(Bitmap bitmap, float scaleX, float scaleY) {
        if (bitmap == null)
            return null;
        int width = bitmap.getWidth();
        int height = bitmap.getHeight();
        int nwidth = (int) (width * scaleX);
        int nheight = (int) (height * scaleY);
        Bitmap.Config config = bm.getConfig();
        Bitmap picNewRes = Bitmap.createScaledBitmap(bitmap, nwidth, nheight, true);
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                picNewRes.setPixel(x, y, bitmap.getPixel((int) (x / scaleX), (int) (y / scaleY)));
            }
        }
        bmp2finger = picNewRes;
        return picNewRes;
    }

    /*获取两指之间的距离*/
    private float getDistance(MotionEvent event) {
        float x = event.getX(1) - event.getX(0);
        float y = event.getY(1) - event.getY(0);
        float distance = (float) Math.sqrt(x * x + y * y);//两点间的距离
        return distance;
    }

    /*取两指的中心点坐标*/
    public static PointF getMid(MotionEvent event) {
        float midX = (event.getX(1) + event.getX(0)) / 2;
        float midY = (event.getY(1) + event.getY(0)) / 2;
        return new PointF(midX, midY);
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        switch (event.getAction() & MotionEvent.ACTION_MASK) {
            //单个手指触摸
            case MotionEvent.ACTION_DOWN:
                float sx = lastX = event.getRawX();
                float sy = lastY = event.getRawY();
//                String point3 = "(" + lastX + ", " + lastY + ")";
//                if (inPicture(lastX, lastY)) {
//                    Toast.makeText(getApplicationContext(), point3 + "IN PICTURE", 1000).show();
//                    Log.d(TAG, "Ontouch() point in the picture" + point3);
//                }
                mode = DRAG;
                /*
                if(imaginer != null) {
                    if(nextSteps <= 1)
                        nextSteps = 1;
                }*/
                break;
            //两指触摸
            case MotionEvent.ACTION_POINTER_DOWN:
                preDistance = getDistance(event);
                //当两指间距大于10时，计算两指中心点
                if (preDistance > 10f) {
                    mid = getMid(event);
                    newmatrix.set(oldmatrix);
                    mode = ZOOM;
                }
                break;
            case MotionEvent.ACTION_UP:
                mode = NONE;
                break;
            case MotionEvent.ACTION_POINTER_UP:
                mode = NONE;
                break;
            case MotionEvent.ACTION_MOVE:
                //当两指缩放，计算缩放比例
                if (bm == null)
                    break;
                if (mode == ZOOM) {
                    distance = getDistance(event);
                    if (distance > 10f) {
                        Bitmap picNewRes = null;
                        oldmatrix.set(newmatrix);
                        float scale = distance / preDistance;
                        if (scale > 0.01) {
                            scale2finger = scale;
                            oldmatrix.postScale(scale, scale, mid.x, mid.y);//缩放比例和中心点坐标
                            try {
                                picNewRes = Bitmap.createBitmap(bm, 0, 0, bmpWidth, bmpHight, oldmatrix, true);
                                if (picNewRes != null) {
                                    //Bitmap picNewRes = createBitmap(bm,scale2finger,scale2finger);
                                    drawBmp(holder, picNewRes, 0, 0);
                                } else {
                                    Log.e(TAG, "Ontouch()" + scale);
                                }
                            } catch (Exception e) {
                                Log.e(TAG, "scale:" + e.toString() + scale);
                            } finally {
                                if(picNewRes != null && !picNewRes.isRecycled()){
                                    picNewRes.recycle();
                                    picNewRes = null;
                                }
                                System.gc();
                            }
                        }
                    }
                }
                if (mode == DRAG) {
                    float ex = event.getRawX();
                    float ey = event.getRawY();
                    ex -= lastX;
                    ey -= lastY;
                    String point = "(" + ex + ", " + ey + ")";
                    //Toast.makeText(getApplicationContext(), "图片移动被锁！拖动" + point, Toast.LENGTH_SHORT).show();
                    if (ex >= 10 || ey >= 10 || ex <= -10 || ey <= -10) {
                        Bitmap move = bm;
                        Bitmap picNewRes2 = null;
                        if (lockDRAG) {
                            if (inPicture(lastX, lastY)) {
                                //Toast.makeText(getApplicationContext(), point2 + "IN PICTURE", 1000).show();
                                float x = (lastX - picleft)/scale2finger;
                                float y = (lastY - pictop)/scale2finger;
                                String point2 = "(" + x + ", " + x + ")";
                                Log.d(TAG, "Ontouch() point in the picture" + point2);
                                int[] movedImag = imaginer.moveFoucs(x, y, ex, ey);
                                if (movedImag != null) {
                                    Bitmap.Config config = bm.getConfig();
                                    move = Bitmap.createBitmap(movedImag, bmpWidth, bmpHight, config);
                                    picNewRes2 = Bitmap.createBitmap(move, 0, 0, bmpWidth, bmpHight, oldmatrix, true);
                                    drawBmp(holder, picNewRes2, 0, 0);
                                    if(move != null && !move.isRecycled()){
                                        move.recycle();
                                        move = null;
                                    }
                                    if(picNewRes2 != null && !picNewRes2.isRecycled()){
                                        picNewRes2.recycle();
                                        picNewRes2 = null;
                                    }
                                    System.gc();
                                }
                            } else {
                                Log.d(TAG, "Ontouch() point out the picture");
                                //Toast.makeText(getApplicationContext(), point2 + "OUT PICTURE", 1000).show();
                            }
                            lastX = lastY = 0;
                        } else {
                            oldmatrix.postTranslate(ex, ey);
                            lastX = event.getRawX();
                            lastY = event.getRawY();
                            picNewRes2 = Bitmap.createBitmap(move, 0, 0, bmpWidth, bmpHight, oldmatrix, true);
                            drawBmp(holder, picNewRes2, ex, ey);
                        }
                    }
                }
                break;
        }
        return true;
    }

    private Bitmap highlight(Bitmap bitmap, int x, int y) {
        isBack = false;
        int pixelxy = bitmap.getPixel(x, y);
        bitmap.setPixel(x, y, resetColor);
        Bitmap picNewRes = null;
        // 新得到的图片是原图片经过变换填充到整个屏幕的图片
        try {
            picNewRes = Bitmap.createBitmap(bitmap, 0, 0, bmpWidth, bmpHight, oldmatrix, true);
            //picNewRes = Bitmap.createBitmap(bm, cx, cy, newScale, newScale, matrix, true);
        } catch (Exception e) {
            Log.e(TAG, "highlight: createBitmap error" + e.toString());
        } finally {
            return picNewRes;
        }
    }

    private boolean inPicture(float x, float y) {
//        if (x > picleft + location[0])
//            if(x < picleft + curWitdth + location[0])
//                if( y > pictop + location[1])
//                    if(y < pictop + curHeight + location[1])
//                        return true;
        if (x >= picleft + location[0] &&
                x <= picleft + curWitdth + location[0] &&
                y >= pictop + location[1] &&
                y <= pictop + curHeight + location[1])
            return true;
        return false;
    }

    public synchronized void setNextSpeed(int nextSpeed) {
        nextSpeeds = nextSpeed;
    }

    @Override
    public synchronized void onBackPressed() {
        if (!isBack && bm != null && startX != -1) {
            //cleanScreen();
            //drawBmp(holder, bm, 0, 0);
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

    public Bitmap resizeBitmap(Bitmap bitmap) {
        if (bitmap != null) {
            int width = bitmap.getWidth();
            int height = bitmap.getHeight();
            float scaleWight = 0, scaleHeight = 0;
            if (phoneW >= width) {
                scaleWight = 1.0f;
            } else {
                scaleWight = phoneW / width;
            }
            if (phoneH >= height) {
                scaleHeight = 1.0f;
            } else {
                scaleHeight = phoneH / height;
            }
            Matrix matrix = new Matrix();
            matrix.postScale(scaleWight, scaleHeight);
            Bitmap res = Bitmap.createBitmap(bitmap, 0, 0, width, height, matrix, true);
            return res;
        } else {
            return null;
        }
    }

    //surfaceview draw on the canvas center
    private void drawBmp(SurfaceHolder sholder, Bitmap bmp, float x, float y) {
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
            curHeight = bmpHight = bmp.getHeight();
            curWitdth = bmpWidth = bmp.getWidth();

            float left = (canvsWidth - bmpWidth) / 2;
            float top = (canvsHight - bmpHight) / 2;
            if (left < 0)
                left = 0;
            if (top < 0)
                top = 0;
            canvas.drawBitmap(resizeBitmap(bm), left, top, paint);
        } else {
            int picWidth = bmp.getWidth();
            curWitdth = picWidth;
            int picHight = bmp.getHeight();
            curHeight = picHight;
            if (picleft == -1 && pictop == -1 && x == 0 && y == 0) {
                picleft = (canvsWidth - picWidth) / 2;
                pictop = (canvsHight - picHight) / 2;
                if (picleft < 0)
                    picleft = 0;
                if (pictop < 0)
                    pictop = 0;
            } else {
                picleft += x;
                pictop += y;
            }
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
