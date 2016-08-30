package android.ImgSdk;

/**
 * Created by dejian.fei on 2016/8/30.
 */

public final class Imaginer {
    static
    {
        System.loadLibrary("imginer");//load libimaginer.so
    }

    private int mNativePerson;

    public Imaginer()
    {
        mNativePerson = init();
    }
    protected void finalize()
    {
        try {
            finalizer(mNativePerson);
        } finally {
            try {
                super.finalize();
            } catch (Throwable e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
    }
    public int getAge()
    {
        return native_getAge(mNativePerson);
    }

    public void setAge(int age)
    {
        native_setAge(mNativePerson, age);
    }


    private native void finalizer(int nPerson);
    public 	native int	add(int a, int b);
    public	native int  sub(int a, int b);
    private native int  init();

    private native int   native_getAge(int nPerson);
    private native void  native_setAge(int nPerson, int age);
}
