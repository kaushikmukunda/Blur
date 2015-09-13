package sketch.km.com.blur;

import android.graphics.Bitmap;

public class NativeBlur {
    static native String loadFromJni();
    static native void blurImage(Bitmap src, Bitmap dst);

    static {
        System.loadLibrary("native");
    }
}
