package o.w.testinlinehook;

public class TestInlineHook {

    static {
        System.loadLibrary("test_inline_hook");
    }

    public static native void inlineHook();
}
