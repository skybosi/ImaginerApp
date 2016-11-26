package com.skybosi.imaginer;

import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Environment;
import android.os.Message;
import android.support.v7.widget.Toolbar;
import android.text.TextUtils;
import android.text.format.Formatter;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.app.Activity;
import android.widget.Toast;
import android.os.Handler;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.LinkedList;

public class ImageViewer extends Activity implements View.OnClickListener, Toolbar.OnMenuItemClickListener {
    public final static int RESULT_CODE = -1;
    private Handler mHandler = null;
    private static LinearLayout linearLayout1;
    private boolean isExit = false;
    private ImageView imageViewShow;
    private MyHorizontalScrollView HSlView = null;
    private HorizontalScrollViewAdapter mAdapter;
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
    private int curpos = 0;
    private int curMaxCount = 0;
    private boolean loadFinish = false;
    private float firstX = 0;
    private float firstY = 0;
    private float secondX = 0;
    private float secondY = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.imageview);
        toolbar = (Toolbar) findViewById(R.id.toolbar);
        toolbar.inflateMenu(R.menu.menu_main);//设置右上角的填充菜单
        toolbar.setOnMenuItemClickListener(this);
        toolTV = ((TextView) findViewById(R.id.toolbar_image));
        hiddenEditMenu(toolbar.getMenu());
        linearLayout1 = (LinearLayout) findViewById(R.id.linearLayout1);
        imageViewShow = (ImageView) findViewById(R.id.imageViewShow);
        HSlView = (MyHorizontalScrollView) findViewById(R.id.HSView1);
        imageViewShow.setOnClickListener(this);
        //遍历sdcard，获取所有图片的绝对路径
        String state = Environment.getExternalStorageState();
        if (Environment.MEDIA_MOUNTED.equals(state)) {
            mSdcardPath = Environment.getExternalStorageDirectory().toString();
        } else {
            Log.e(MainActivity.TAG, "SDCARD is not MOUNTED");
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
                        (findViewById(R.id.loading)).setVisibility(View.GONE);
                        imageViewShow.setVisibility(View.VISIBLE);
                        loadImage();
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

    private synchronized void loadImage() {
        WandH = HSlView.getHeight();
        if (WandH == 0) {
            WandH = 114;
        }
        mAdapter = new HorizontalScrollViewAdapter(this, mImgPathList, WandH);
        //添加滚动回调
        HSlView.setCurrentImageChangeListener(new MyHorizontalScrollView.CurrentImageChangeListener() {
            @Override
            public void onCurrentImgChanged(int position,
                                            View viewIndicator) {
                loadPath = mImgPathList.get(position);
                toolTV.setText(loadPath);
                imageViewShow.setImageBitmap(BitmapFactory.decodeFile(loadPath));
            }
        });
        //添加点击回调
        HSlView.setOnItemClickListener(new MyHorizontalScrollView.OnItemClickListener() {
            @Override
            public void onClick(View view, int position) {
                loadPath = mImgPathList.get(position);
                toolTV.setText(loadPath);
                imageViewShow.setImageBitmap(BitmapFactory.decodeFile(loadPath));
            }
        });
        //设置适配器
        HSlView.initDatas(mAdapter);
    }

    private void initAll() {
        if (mImgPathList.isEmpty()) {
            String state = Environment.getExternalStorageState();
            if (Environment.MEDIA_MOUNTED.equals(state)) {
                mSdcardPath = Environment.getExternalStorageDirectory().toString();
                //mSdcardPath ="/storage";
            } else {
                Log.e(MainActivity.TAG, "SDCARD is not MOUNTED");
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
            (findViewById(R.id.loading)).setVisibility(View.GONE);
            imageViewShow.setVisibility(View.VISIBLE);
            loadImage();
        }
    }

    public void traverseFolder1(String path, ArrayList<String> imagelist) {
        File file = new File(path);
        if (file.exists()) {
            LinkedList<File> list = new LinkedList<File>();
            try {
                File[] files = file.listFiles();
                for (File file2 : files) {
                    if (file2.isDirectory() && !file2.isHidden() &&
                            !file2.getAbsolutePath().toLowerCase().contains("self")) {
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
                        Log.e(MainActivity.TAG, e.toString() + "this folder " + temp_file.getAbsolutePath().toString() + " is empty");
                        continue;
                    }
                }
            } catch (Exception e) {
                Log.e(MainActivity.TAG, e.toString() + " 文件不存在1");
            }
        } else {
            Log.e(MainActivity.TAG, path + " 文件不存在2");
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
                    if (HSlView.getVisibility() != View.GONE)
                        HSlView.setVisibility(View.GONE);
                    else
                        HSlView.setVisibility(View.VISIBLE);
                    clickcount++;
                    if (clickcount == 1) {
                        firstClick = System.currentTimeMillis();
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
                onDetailBtnDown(curpos);
                break;
        }
        return false;
    }

    //button down callbacks
    private void onDetailBtnDown(int pos) {
        // TODO Auto-generated method stub
        final View d = View.inflate(this, R.layout.detailsview, null);
        //Thumbnail View
        ImageView imageView = (ImageView) d.findViewById(
                R.id.details_thumbnail_image);
        Bitmap bm = BitmapFactory.decodeFile(mImgPathList.get(pos));
        imageView.setImageBitmap(bm);

        //Text View of Image Title
        TextView txTitle = (TextView) d.findViewById(R.id.details_image_path);
        txTitle.setText(mImgPathList.get(pos));

        //the picture resolution value
        int width = bm.getWidth();
        int height = bm.getHeight();
        String pictureSize = width + "*" + height;
        setDetailsValue(d, pictureSize, R.id.details_resolution_value);

        //the picture size
        String size = Formatter.formatFileSize(this, height * width);
        setDetailsValue(d, size, R.id.details_file_size_value);

        //get date
        String value = null;
        File f = new File(mImgPathList.get(pos));
        long lastMod = f.lastModified();
        if (lastMod != 0) {
            Date date = new Date(lastMod);
            SimpleDateFormat dateFormat = new SimpleDateFormat();
            value = dateFormat.format(date);
        }
        if (value != null) {
            setDetailsValue(d, value, R.id.details_date_taken_value);
        } else {
            hideDetailsRow(d, R.id.details_date_taken_row);
        }
        AlertDialog.Builder build = new Builder(ImageViewer.this);
        build.setTitle("详细信息").setView(d).show();
    }

    private static void setDetailsValue(View d, String text, int valueId) {
        ((TextView) d.findViewById(valueId)).setText(text);
    }

    private static void hideDetailsRow(View d, int rowId) {
        d.findViewById(rowId).setVisibility(View.GONE);
    }

}



