package com.skybosi.imaginer;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.text.format.Formatter;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AnimationUtils;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.BaseAdapter;
import android.widget.Gallery;
import android.widget.Gallery.LayoutParams;
import android.widget.ImageSwitcher;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ViewSwitcher.ViewFactory;
import android.support.v7.widget.Toolbar;
import android.support.v7.widget.Toolbar.OnMenuItemClickListener;

/**
 * @author Alan
 */
public class ImageViewerActivity extends Activity implements View.OnClickListener, ViewFactory, OnItemSelectedListener, OnMenuItemClickListener {

    public final static int RESULT_CODE = -1;
    private Handler mHandler = null;
    private boolean isExit = false;

    private Toolbar toolbar = null;

    private ImageSwitcher mImgSwitcher;

    private Gallery mGallery;

    private ImageAdapter adapter;

    private ArrayList<String> mImgPathList;

    private static boolean deleteHappen = false;

    //sdcard Path
    private String mSdcardPath;

    //cache for the Bitmap res
    private HashMap<Integer, Bitmap> imgCache = new HashMap<Integer, Bitmap>();

    private static final String LOG_TAG = "ImageViewer";

    //for lock the image
    private int clickcount = 0;
    private long firstClick = 0;
    private long secondClick = 0;
    private String loadPath = null;

    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        toolbar = (Toolbar) findViewById(R.id.toolbar);
        toolbar.inflateMenu(R.menu.menu_main);//设置右上角的填充菜单
        //toolbar.setLogo(R.mipmap.imaginer);
        toolbar.setOnMenuItemClickListener(this);
        hiddenEditMenu(toolbar.getMenu());

        mImgPathList = new ArrayList<String>();
        mSdcardPath = Environment.getExternalStorageDirectory().toString();

        saveImagePathToList(mImgPathList, mSdcardPath);

        //set ImageSwitcher
        mImgSwitcher = (ImageSwitcher) findViewById(R.id.imgswitcher);
        mImgSwitcher.setOnClickListener(this);
        mImgSwitcher.setFactory(this);
        mImgSwitcher.setInAnimation(AnimationUtils.loadAnimation(this, android.R.anim.fade_in));
        mImgSwitcher.setOutAnimation(AnimationUtils.loadAnimation(this, android.R.anim.fade_out));

        //set Gallery
        mGallery = (Gallery) findViewById(R.id.imgallery);
        adapter = new ImageAdapter(this, mImgPathList);
        mGallery.setAdapter(adapter);
        mGallery.setOnItemSelectedListener(this);
        mGallery.setOnItemLongClickListener(new OnItemLongClickListener() {

            public boolean onItemLongClick(AdapterView<?> arg0, View arg1,
                                           final int position, long id) {
                // TODO Auto-generated method stub
                AlertDialog.Builder build = new Builder(ImageViewerActivity.this);
                build.setTitle("你想执行下列哪个操作？")
                        .setPositiveButton("删除选中文件", new OnClickListener() {

                            public void onClick(DialogInterface dialog, int which) {
                                // TODO Auto-generated method stub
                                onDelBtnDown(position);
                            }
                        })
                        .setNegativeButton("详细信息", new OnClickListener() {

                            public void onClick(DialogInterface dialog, int which) {
                                // TODO Auto-generated method stub
                                onDetailBtnDown(position);
                            }
                        });
                build.create().show();
                return false;
            }

        });
        mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case 0:
                        isExit = false;
                    default:
                        break;
                }
            }
        };
    }

    private void hiddenEditMenu(Menu mMenu){
        if(null != mMenu){
            for (int i = 0; i < mMenu.size(); i++){
                if(!mMenu.getItem(i).getTitle().toString().contains("about"))
                    mMenu.getItem(i).setVisible(false);
            }
        }
    }


    public boolean onMenuItemClick(MenuItem item) {
        int menuItemId = item.getItemId();
        switch (menuItemId) {
            case R.id.about:
                new MainActivity.AboutDialog(this,false).show();
                break;
            case R.id.setColor:
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
        //txTitle.setText(geTitle(pos));
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

        AlertDialog.Builder build = new Builder(ImageViewerActivity.this);
        build.setTitle("详细信息")
                .setView(d)
                .show();

    }

    private static void setDetailsValue(View d, String text, int valueId) {
        ((TextView) d.findViewById(valueId)).setText(text);
    }

    private static void hideDetailsRow(View d, int rowId) {
        d.findViewById(rowId).setVisibility(View.GONE);
    }


    private String geTitle(int pos) {

        String title = mImgPathList.get(pos);
        int last_s = title.lastIndexOf("/");
        int last_l = title.lastIndexOf(".");

        title = title.substring(last_s + 1, last_l);
        return title;

    }


    private void onDelBtnDown(int pos) {
        // TODO Auto-generated method stub
        deleteHappen = true;
        Log.d(LOG_TAG, "the pos will deleted is " + pos);
        File current = new File(mImgPathList.get(pos));
        current.delete();

        mImgPathList.remove(pos);
        adapter.notifyDataSetChanged();
        mGallery.setAdapter(adapter);

        if (pos == 0) {
            mImgSwitcher.setImageDrawable(null);
            Toast.makeText(this, "The Picture isn't exist, or Deleted",
                    Toast.LENGTH_SHORT).show();
        }
    }

    //get image path recursively
    private void saveImagePathToList(ArrayList<String> list, String filePath) {
        // TODO Auto-generated method stub
        File mFile = new File(filePath);
        File[] listFile = mFile.listFiles();
        if (listFile != null) {
            for (int i = 0; i < listFile.length; i++) {
                File file = listFile[i];
                if (file.isFile()) {
                    if (isAnImageFile(file.getAbsolutePath())) {
                        list.add(file.getAbsolutePath());
                    }
                } else if (file.isDirectory() && !file.isHidden() && !file.getAbsolutePath().contains("DCIM")) {
                    //get image path recursively
                    this.saveImagePathToList(mImgPathList, file.getAbsolutePath());
                }
                continue;
            }
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

    public View makeView() {
        // TODO Auto-generated method stub
        ImageView iv = new ImageView(this);
        //iv.setBackgroundColor(0xFF000000);   
        iv.setScaleType(ImageView.ScaleType.FIT_CENTER);
        iv.setLayoutParams(new ImageSwitcher.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        return iv;
    }

    @Override
    public void onClick(View v) {
        switch (v.getId())
        {
            case R.id.imgswitcher:
                if (mImgSwitcher != null) {
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
                        }else
                        {
                            if (mGallery.getVisibility() != View.GONE)
                                mGallery.setVisibility(View.GONE);
                            else
                                mGallery.setVisibility(View.VISIBLE);
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


    public class ImageAdapter extends BaseAdapter {

        private Context mContext;
        private int mGalleryBackground;
        private ArrayList<String> mArrayList;

        public ImageAdapter(Context c, ArrayList<String> list) {
            mContext = c;
            mArrayList = list;
            //use attrs.xml to specify the Gallery's attributes
            TypedArray mTypeArray = obtainStyledAttributes(R.styleable.Gallery);
            mGalleryBackground = mTypeArray.getResourceId(R.styleable.Gallery_android_galleryItemBackground, 0);
            mTypeArray.recycle();
        }

        public int getCount() {
            // TODO Auto-generated method stub
            return mArrayList.size();
        }

        public Object getItem(int position) {
            // TODO Auto-generated method stub
            return position;
        }

        public long getItemId(int position) {
            // TODO Auto-generated method stub
            return position;
        }

        public View getView(int position, View convertView, ViewGroup parent) {
            // TODO Auto-generated method stub
            //new a ImageView for the image displaying
            Log.d(LOG_TAG, "getView--->and pos = " + position);
            ImageView imgView = new ImageView(mContext);
            Bitmap current = imgCache.get(position);
            if (deleteHappen || current == null) {
                current = BitmapFactory.decodeFile(mArrayList.get(position));

                Log.e(LOG_TAG, "decodeFile path = " + mArrayList.get(position));

                imgCache.put(position, current);
            }
            ((TextView) findViewById(R.id.toolbar_title)).setText(mImgPathList.get(position));
            imgView.setImageBitmap(current);
            imgView.setScaleType(ImageView.ScaleType.FIT_CENTER);
            int WandH = mGallery.getHeight();
            imgView.setLayoutParams(new Gallery.LayoutParams(WandH, WandH));
            imgView.setBackgroundResource(mGalleryBackground);

            //deleteHappen = false;
            return imgView;
        }

    }

    public void onItemSelected(AdapterView<?> arg0, View arg1, int pos,
                               long id) {
        // TODO Auto-generated method stub
        String imgPath = loadPath = mImgPathList.get(pos);
        Log.e(LOG_TAG, "Selected Img position is " + pos + "Image Path = " + imgPath);
        Drawable drawadle = Drawable.createFromPath(imgPath);
        mImgSwitcher.setImageDrawable(drawadle);
        //release Bitmap resource
        releaseBitmap();
    }

    //just keep the bitmap in Visible range, release the rest
    private void releaseBitmap() {
        // TODO Auto-generated method stub
        int begin = mGallery.getFirstVisiblePosition();
        int end = mGallery.getLastVisiblePosition();

        for (int i = 0; i < begin; i++) {
            Bitmap delBitmap = imgCache.get(i);
            if (delBitmap != null) {
                imgCache.remove(i);
                delBitmap.recycle();
                adapter.notifyDataSetChanged();
            }
        }

        for (int i = end + 1; i < imgCache.size(); i++) {
            Bitmap delBitmap = imgCache.get(i);
            if (delBitmap != null) {
                imgCache.remove(i);
                delBitmap.recycle();
            }
        }

    }

    public void onNothingSelected(AdapterView<?> arg0) {
        // TODO Auto-generated method stub
        Log.e(LOG_TAG, "onNothingSelected...");
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {// 当keyCode等于退出事件值时
            ToQuitTheApp();
            return false;
        } else {
            return super.onKeyDown(keyCode, event);
        }
    }

    //封装ToQuitTheApp方法
    private void ToQuitTheApp() {
        if (isExit) {
            this.finish();
        } else {
            isExit = true;
            Toast.makeText(getApplicationContext(), "再按一次后退键退出主程序！", Toast.LENGTH_SHORT).show();
            mHandler.sendEmptyMessageDelayed(0, 2000);// 2秒后发送消息
        }
    }

    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub
        super.onDestroy();
    }

    @Override
    protected void onPause() {
        // TODO Auto-generated method stub
        super.onPause();
    }

//    public class AboutDialog extends AlertDialog {
//        public AboutDialog(Context context) {
//            super(context);
//            final View view = getLayoutInflater().inflate(R.layout.about, null);
//            setTitle("Imaginer   v1.0.0");
//            TextView tv = (TextView) view.findViewById(R.id.aboutText);
//            tv.setText(R.string.ImageViewAbout);//Thanks for ImageViewer Auther:Alan
//            setView(view);
//        }
//    }
}