package com.skybosi.imaginer;


import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.ContextMenu;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;


import java.io.File;
import java.io.FileFilter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

/**
 * Created by dejian.fei on 2016/7/27.
 */
//
//public class FileView extends Activity implements View.OnClickListener{
public class FileView extends Activity{
    public final static int RESULT_CODE =  -1;
    private List<String> mfiles = null;
    MyAdapter mAdapter = null;
    String currentBack = null;
    String SDpath = null;
    String currentPath = null;
    //    EditText tvCurrpath = null;
    TextView tvCurrpath = null;
    ListView fileList = null;
    String fullfilePath = null;
    private static boolean isExit = false;
    final  static  FileFilter Filter = new FileFilter() {
        public boolean accept(File pathname) {
            return !pathname.isHidden();//过滤隐藏文件
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.file_listview);
        init();
    }
    protected  List<String> loadPathFile(String path){
        List<String> files = new ArrayList<String>();
        List<String> dirs = new ArrayList<String>();
        if(path != null) {
            try {
                currentPath = path;
                tvCurrpath.setText(currentPath);
                File lsFile = new File(path);
                //控制最高目录路径（这里设置为/storage
                if (!path.equals("/storage")) {
                    dirs.add("..");
                }
                if (lsFile.listFiles().length > 0) {
                    for (File file : lsFile.listFiles(Filter)) {
                        if(file.isDirectory())
                            dirs.add(file.getName());
                        else
                            files.add(file.getName());
                    }
                }

                //sort by name
                Collections.sort(dirs, new Comparator<String>() {
                    @Override
                    public int compare(String o1, String o2) {
                        return o1.compareTo(o2);
                    }
                });
                Collections.sort(files, new Comparator<String>() {
                    @Override
                    public int compare(String o1, String o2) {
                        return o1.compareTo(o2);
                    }
                });

            } catch (Exception e) {
                //
            }
        }
        else
            return mfiles;
            dirs.addAll(files);
        return dirs;
    }

    private boolean checkSdpath() {
        String sDStateString = Environment.getExternalStorageState();
        if (sDStateString.equals(Environment.MEDIA_MOUNTED)) {
            return true;
        }
        return false;
    }

    private void getParentFile()
    {
        try {
            if (currentBack != null) {
                // 获取上一级目录
//                currentBack = currentBack.getParentFile();
//                currentPath = currentBack.toString();
//                // 列出当前目录下的所有文件
                mfiles = loadPathFile(currentBack);
                isExit = false;
            }
        } catch (Exception e) {
            // TODO: handle exception
        }
    }
    private void getBackFile()
    {
        try {
            if (currentBack != null) {
//                // 获取Back目录
//                currentBack = currentBack.getParentFile();
//                currentPath = currentBack.toString();
                mfiles = loadPathFile(currentBack);
                currentBack = null;
                isExit = false;
            }
        } catch (Exception e) {
            // TODO: handle exception
        }
    }
    private void init() {
        tvCurrpath = (TextView) findViewById(R.id.currPath);
        if(checkSdpath()) {
            SDpath = Environment.getExternalStorageDirectory().getAbsolutePath();
            mfiles = loadPathFile(SDpath);
        }
        fileList = (ListView) findViewById(R.id.listview);
        mAdapter = new MyAdapter();
        fileList.setAdapter(mAdapter);
        fileList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> arg0, View arg1, int arg2, long arg3) {
                String filename  = mAdapter.getItem(arg2).toString();
                itemClickDeal(filename);
            }
        });
        fileList.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener(){
            @Override
            public boolean onItemLongClick(AdapterView<?> arg0, View view,int arg2, long arg3) {
                if( mAdapter.getItem(arg2).toString() == null)
                    return true;
                fullfilePath = currentPath + "/" + mAdapter.getItem(arg2).toString();
                return false;
            }
        });
        registerForContextMenu(fileList);
    }

    private void itemClickDeal(String filename) {
        // 获取用户单击的列表项对应的文件夹，设为当前的返回点
        currentBack = currentPath;
        String loadPath = null;
        // 获取用户点击,如果是"..",返回上一级
        if (filename.equals("..")) {
            loadPath = (new File(currentPath)).getParent();
        } else {
            loadPath = currentPath + "/" + filename;
        }
        if (new File(loadPath).isDirectory()) {
            // 获取用户点击的文件夹 下的所有文件
            List<String> newfiles = loadPathFile(loadPath);
            if (newfiles == null) {
                Toast.makeText(FileView.this, "当前路径不可访问", Toast.LENGTH_LONG).show();
            } else {
                // 再次更新ListView
                mfiles = newfiles;
                mAdapter.notifyDataSetChanged();
            }
        } else {
            Intent intent = new Intent();
            intent.putExtra("filename", loadPath);
            setResult(RESULT_CODE, intent);
            finish();
        }
    }

    /**
     * 监听Back键按下事件,方法1:
     * 注意:
     * super.onBackPressed()会自动调用finish()方法,关闭
     * 当前Activity.
     * 若要屏蔽Back键盘,注释该行代码即可
     */
    @Override
    public void onBackPressed() {
        if(currentBack != null) {
            getBackFile();
            mAdapter.notifyDataSetChanged();
        }else {
            exit();
        }
    }

    private void exit() {
        if (!isExit) {
            isExit = true;
            Toast.makeText(getApplicationContext(), "再按一次后退键退出文件浏览界面！", Toast.LENGTH_SHORT).show();
        }
        else {
            this.finish();
        }
    }
//
//    @Override
//    public void onClick(View v) {
//        switch (v.getId())
//        {
//            case R.id.GotoPath:
//                if(tvCurrpath != null) {
//                    String goPath = tvCurrpath.getText().toString();
//                    gotoFilePath(goPath);
//                }
//                break;
//            case R.id.BackPath:
//                getParentFile();
//                mAdapter.notifyDataSetChanged();
//                break;
//            default:
//                break;
//        }
//    }

    private void gotoFilePath(String goPath ) {
        mfiles = loadPathFile(goPath);
        mAdapter.notifyDataSetChanged();
    }


    class MyAdapter extends BaseAdapter {

        @Override
        public int getCount() {
            return mfiles.size();
        }

        @Override
        public Object getItem(int position) {
            return mfiles.get(position);
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            View row = convertView;
            Wrapper wrapper = null;
            if(row == null){
                row = LayoutInflater.from(FileView.this).inflate(R.layout.fileitem, parent,false);
                wrapper = new Wrapper(row);
                row.setTag(wrapper);
            }else {
                wrapper = (Wrapper) row.getTag();
            }
            TextView FileName = wrapper.getFileName();
            TextView FileNum = wrapper.getFileNum();
            String file_name = mfiles.get(position);
            FileName.setText(file_name);
            if( new File(currentPath + "/" + file_name).isDirectory()) {
                FileName.setTextColor(getResources().getColor(R.color.colorDir));
                if(currentPath.length() > 0) {
                    if (!file_name.equals("..")) {
                        if(currentPath.equals("/"))
                            FileNum.setText(new File(currentPath  + file_name).listFiles().length + " 项    ");
                        else {
                            try {
                                File lsFile = new File(currentPath + "/" + file_name);
                                int lenth = lsFile.listFiles().length;
                                if ( lenth > 0) {
                                    FileNum.setText(lenth +" 项    ");
                                }else {
                                    FileNum.setText("0 项    ");
                                }
                            }catch(Exception e){
                                Log.e("error", "getView: "+e.toString());
                            }
                        }
                    }
                    else {
                        FileNum.setText(new File(currentPath).getParentFile().listFiles().length + " 项    ");
                    }
                }
            }else
            {
                FileName.setTextColor(getResources().getColor(R.color.colorFile));
                FileNum.setText("");
            }
            return row;
        }
    }
    class Wrapper {
        TextView FileName;
        TextView FileNum;
        View row;

        public Wrapper(View row) {
            this.row = row;
        }

        public TextView getFileName(){
            if(FileName == null){
                FileName = (TextView)row.findViewById(R.id.fileItem);
            }
            return FileName;
        }
        public TextView getFileNum(){
            if(FileNum == null){
                FileNum = (TextView)row.findViewById(R.id.fileNum);
            }
            return FileNum;
        }
    }

    //创建上下文菜单
    @Override
    public void onCreateContextMenu(ContextMenu menu, View v, ContextMenu.ContextMenuInfo menuInfo) {
        super.onCreateContextMenu(menu, v, menuInfo);
        // ContextMenu的Item不支持Icon，所以不用再资源文件中，为它们设定图标
        if (v.getId() == fileList.getId()) {
            //加载xml菜单布局文件
            this.getMenuInflater().inflate(R.menu.menu_browser, menu);
            // 设定头部图标
            //menu.setHeaderIcon(R.drawable.aaa);
            // 设定头部标题
            menu.setHeaderTitle("File Operate Option");
        }
    }
    //选择菜单项后的响应
    @Override
    public boolean onContextItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.copy:
                if (fullfilePath != null) {
                    Toast.makeText(FileView.this, "Copying......" + fullfilePath, Toast.LENGTH_LONG).show();
                }
                break;
            case R.id.rename:
                if (fullfilePath != null) {
                    Toast.makeText(FileView.this, "Rename......" + fullfilePath, Toast.LENGTH_LONG).show();
                }
                break;
            case R.id.delete:
                if (fullfilePath != null) {
                    Toast.makeText(FileView.this, "Delete......" + fullfilePath, Toast.LENGTH_LONG).show();
                    //deleteFile(fullfilePath);
                }
                break;
            default:
                break;
        }
        return super.onContextItemSelected(item);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_browser, menu);
        return true;
    }

    @Override
    public boolean deleteFile(String name) {
        return super.deleteFile(name);
    }

    public boolean renameFile(String name) {
        if(new File(name).exists()){

        }
        return super.deleteFile(name);
    }
}
