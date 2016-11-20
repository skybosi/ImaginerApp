package com.skybosi.imaginer;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Environment;
import android.os.Message;
import android.support.v7.widget.Toolbar;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.HorizontalScrollView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;
import android.widget.TextView;
import android.app.Activity;
import android.widget.Toast;
import android.os.Handler;

import java.io.File;
import java.util.ArrayList;
import java.util.LinkedList;

public class ImageViewer extends Activity implements View.OnClickListener, Toolbar.OnMenuItemClickListener {
    public final static int RESULT_CODE = -1;
    private Handler mHandler = null;
    private static LinearLayout linearLayout1;
    private boolean isExit = false;
    private ImageView imageViewShow;
    private HorizontalScrollView HSlView = null;
    private Toolbar toolbar = null;
    private TextView toolTV = null;
    //sdcard Path
    private String mSdcardPath = null;
    private static int WandH = 0;
    private static ArrayList<String> mImgPathList = new ArrayList<String>();
    //for lock the image
    private int clickcount = 0;
    private long firstClick = 0;
    private long secondClick = 0;
    private String loadPath = null;
    private int curMaxCount = 0;
    private boolean loadFinish = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.imageview);
        toolbar = (Toolbar) findViewById(R.id.toolbar);
        toolbar.inflateMenu(R.menu.menu_main);//设置右上角的填充菜单
        toolbar.setOnMenuItemClickListener(this);
        toolTV = ((TextView) findViewById(R.id.toolbar_image));
        hiddenEditMenu(toolbar.getMenu());
        // 获取组件
        linearLayout1 = (LinearLayout) findViewById(R.id.linearLayout1);
        imageViewShow = (ImageView) findViewById(R.id.imageViewShow);
        HSlView = (HorizontalScrollView) findViewById(R.id.HSView1);
        imageViewShow.setOnClickListener(this);
        String state = Environment.getExternalStorageState();
        if (Environment.MEDIA_MOUNTED.equals(state)) {
            mSdcardPath = Environment.getExternalStorageDirectory().toString();
        } else {
            Log.e("Imaginer", "SDCARD is not MOUNTED");
        }
        mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 0:
                        isExit = false;
                        break;
                    case 1:
                        loadImage(mImgPathList, 10);
                        toolTV.setEllipsize(TextUtils.TruncateAt.MARQUEE);
                        break;
                    case 2:
                        Toast.makeText(getApplication(), "图片列表为空，加载失败", Toast.LENGTH_LONG).show();
                        break;
                    case 3:
                        String filepath = (String) msg.obj;
                        toolTV.setEllipsize(TextUtils.TruncateAt.END);
                        toolTV.setText(filepath);
                        break;
                    case 4:
                        loadImage(mImgPathList, 10);
                        break;
                    case 5:
                        Toast.makeText(getApplication(), "所有图片加载完成", Toast.LENGTH_LONG).show();
                        break;
                    default:
                        break;
                }
            }
        };
        initAll();
    }

    private void initAll() {
        if (mImgPathList.isEmpty()) {
            String state = Environment.getExternalStorageState();
            if (Environment.MEDIA_MOUNTED.equals(state)) {
                mSdcardPath = Environment.getExternalStorageDirectory().toString();
                //mSdcardPath = "/storage";
            } else {
                Log.e("Imaginer", "SDCARD is not MOUNTED");
            }
            Thread thread = new Thread(new Runnable() {
                @Override
                public void run() {
                    traverseFolder1(mSdcardPath, mImgPathList);
                    if (mImgPathList.size() == 0)
                        mHandler.sendEmptyMessage(2);
                    mHandler.sendEmptyMessage(1);
                }
            });
            thread.start();
        } else {
            loadImage(mImgPathList, 10);
        }
    }

    private synchronized void loadImage(ArrayList<String> imagelist, int count) {
        if (curMaxCount >= imagelist.size() - 1) {
            loadFinish = true;
            mHandler.sendEmptyMessage(5);
            return;
        }
        if (WandH == 0) {
            WandH = HSlView.getHeight();
        }
        (findViewById(R.id.loading)).setVisibility(View.GONE);
        ((ImageView) findViewById(R.id.imageViewShow)).setVisibility(View.VISIBLE);
        File file = null;
        Bitmap bm = null;
        LayoutInflater inflater = LayoutInflater.from(this);
        for (int i = curMaxCount; i <= count + curMaxCount; i++) {
            LinearLayout linearLayout = (LinearLayout) inflater.inflate(R.layout.layout, null);
            ImageView imageView = (ImageView) linearLayout.findViewById(R.id.imageView);
            imageView.setId(i + 1);
            //set image
            file = new File(imagelist.get(i));
            bm = BitmapFactory.decodeFile(imagelist.get(i), getHeapOpts(file));
            if (bm != null) {
                imageView.setImageBitmap(bm);
                imageView.setScaleType(ImageView.ScaleType.FIT_CENTER);
                imageView.setLayoutParams(new LayoutParams(WandH, WandH));
                imageView.setOnClickListener(new OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        loadPath = mImgPathList.get(view.getId() - 1);
                        toolTV.setText(loadPath);
                        imageViewShow.setImageBitmap(BitmapFactory.decodeFile(loadPath));
                    }
                });
                imageView.setOnTouchListener(new View.OnTouchListener() {
                    @Override
                    public boolean onTouch(View v, MotionEvent ev) {
                        //mHandler.sendEmptyMessage(4);
                        if (!loadFinish) {
                            switch (ev.getAction()) {
                                case MotionEvent.ACTION_DOWN:
                                case MotionEvent.ACTION_MOVE:
                                case MotionEvent.ACTION_UP:
                                    mHandler.sendEmptyMessage(4);
                                    break;
                            }
                        }
                        return false;
                    }
                });
                linearLayout1.addView(linearLayout);
            }
        }
        curMaxCount += count;
    }

    public void traverseFolder1(String path, ArrayList<String> imagelist) {
        File file = new File(path);
        if (file.exists()) {
            LinkedList<File> list = new LinkedList<File>();
            File[] files = file.listFiles();
            for (File file2 : files) {
                if (file2.isDirectory() && !file2.isHidden()) {
                    list.add(file2);
                } else {
                    if (isAnImageFile(file2.getAbsolutePath())) {
                        imagelist.add(file2.getAbsolutePath());
                    }
                }
            }
            File temp_file;
            while (!list.isEmpty()) {
                temp_file = list.removeFirst();
                Message msg = Message.obtain();
                msg.what = 3;
                msg.obj = temp_file.getAbsolutePath().toString();
                mHandler.sendMessage(msg);
                try {
                    files = temp_file.listFiles();
                    for (File file2 : files) {
                        if (file2.isDirectory()) {
                            list.add(file2);
                        } else {
                            if (isAnImageFile(file2.getAbsolutePath())) {
                                imagelist.add(file2.getAbsolutePath());
                            }
                        }
                    }
                } catch (Exception e) {
                    Log.e("Imaginer", e.toString() + "this folder " + temp_file.getAbsolutePath().toString() + " is empty");
                }
            }
        } else {
            Log.e("Imaginer", "文件不存在!");
        }
    }

    //check the fils's suffix and tell whether it's an image
    private boolean isAnImageFile(String fileName) {
        String fileNameLowerCase = fileName.toLowerCase();
        if (fileNameLowerCase.endsWith("jpg") || fileNameLowerCase.endsWith("gif")
                || fileNameLowerCase.endsWith("bmp") || fileNameLowerCase.endsWith("jpeg")
                || fileNameLowerCase.endsWith("png")) {
            return true;
        }
        return false;
    }

    // 图片加载的类
    public static BitmapFactory.Options getHeapOpts(File file) {
        BitmapFactory.Options opts = new BitmapFactory.Options();
        // 数字越大读出的图片占用的heap必须越小，不然总是溢出
        if (file.length() < 20480) { // 0-20k
            opts.inSampleSize = 1;// 这里意为缩放的大小
        } else if (file.length() < 51200) { // 20-50k
            opts.inSampleSize = 2;
        } else if (file.length() < 307200) { // 50-300k
            opts.inSampleSize = 4;
        } else if (file.length() < 819200) { // 300-800k
            opts.inSampleSize = 6;
        } else if (file.length() < 1048576) { // 800-1024k
            opts.inSampleSize = 8;
        } else {
            opts.inSampleSize = 10;
        }
        return opts;
    }

    private void hiddenEditMenu(Menu mMenu) {
        if (null != mMenu) {
            for (int i = 0; i < mMenu.size(); i++) {
                String menus = mMenu.getItem(i).getTitle().toString();
                String about = getResources().getString(R.string.app_about);
                if (!menus.toLowerCase().contains(about))
                    mMenu.getItem(i).setVisible(false);
            }
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.imageViewShow:
                if (imageViewShow != null) {
                    clickcount++;
                    if (clickcount == 1) {
                        firstClick = System.currentTimeMillis();
                        if (HSlView.getVisibility() != View.GONE)
                            HSlView.setVisibility(View.GONE);
                        else
                            HSlView.setVisibility(View.VISIBLE);
                    } else if (clickcount == 2) {
                        secondClick = System.currentTimeMillis();
                        if (secondClick - firstClick < 1000) {
                            //select a image to back
                            Intent intent = new Intent();
                            intent.putExtra("filename", loadPath);
                            setResult(RESULT_CODE, intent);
                            finish();
                        }
                        clickcount = 0;
                        firstClick = 0;
                        secondClick = 0;
                    }
                }
                break;
            default:
                break;
        }
    }

    @Override
    public boolean onMenuItemClick(MenuItem item) {
        int menuItemId = item.getItemId();
        switch (menuItemId) {
            case R.id.about:
                //new MainActivity.AboutDialog(this, false).show();
                break;
            case R.id.setColor:
                break;
        }
        return false;
    }
}



