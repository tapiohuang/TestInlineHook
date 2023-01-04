package o.w.testinlinehook;

import android.util.Log;

public class TestJni {
    static {
        System.loadLibrary("test_jni");
    }

    public static native int getValue();


    public static int realValue() {
        Log.i("TestJNI", "realValue");
        return 77788;
    }
}
