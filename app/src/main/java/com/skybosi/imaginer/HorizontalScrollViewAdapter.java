package com.skybosi.imaginer;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;

import java.io.File;
import java.util.ArrayList;

public class HorizontalScrollViewAdapter {

    private Context mContext;
    private LayoutInflater mInflater;
    private ArrayList<String> mDatas;
    private int mWandH = 0;

    public HorizontalScrollViewAdapter(Context context, ArrayList<String> mDatas, int mWandH) {
        this.mContext = context;
        mInflater = LayoutInflater.from(context);
        this.mDatas = mDatas;
        this.mWandH = mWandH;
    }

    public int getCount() {
        return mDatas.size();
    }

    public Object getItem(int position) {
        return mDatas.get(position);
    }

    public long getItemId(int position) {
        return position;
    }

    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder viewHolder = null;
        if (convertView == null) {
            viewHolder = new ViewHolder();
            convertView = mInflater.inflate(
                    R.layout.layout, parent, false);
            viewHolder.mImg = (ImageView) convertView
                    .findViewById(R.id.imageView);
            convertView.setTag(viewHolder);
        } else {
            viewHolder = (ViewHolder) convertView.getTag();
        }
        File file = new File(mDatas.get(position));
        Bitmap bm = BitmapFactory.decodeFile(mDatas.get(position), getHeapOpts(file));
        if (bm != null) {
            viewHolder.mImg.setImageBitmap(bm);
            viewHolder.mImg.setScaleType(ImageView.ScaleType.FIT_CENTER);
            viewHolder.mImg.setLayoutParams(new LinearLayout.LayoutParams(mWandH, mWandH));
        }
        //viewHolder.mImg.setImageResource(mDatas.get(position));
        return convertView;
    }

    private class ViewHolder {
        ImageView mImg;
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

}
