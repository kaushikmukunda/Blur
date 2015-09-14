package sketch.km.com.blur;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.ViewTreeObserver;
import android.widget.ImageView;


public class MainActivity extends Activity {
    private ImageView mOrigImg;
    private ImageView mBlurImg;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setupViews();
    }

    private void setupViews() {
        mOrigImg = (ImageView) findViewById(R.id.orig_img);
        mBlurImg = (ImageView) findViewById(R.id.blur_img);

        mOrigImg.getViewTreeObserver(). addOnPreDrawListener(listener);
    }


    private final ViewTreeObserver.OnPreDrawListener listener = new ViewTreeObserver.OnPreDrawListener() {
        @Override
        public boolean onPreDraw() {
            // Wait until screen orients to landscape
            if (mOrigImg.getWidth() > mOrigImg.getHeight()) {
                // Instead of processing the entire bitmap, scale it to the size we need and then process it
                Bitmap fullsizeSrc = BitmapFactory.decodeResource(getResources(), R.drawable.ocean);
                Bitmap scaledSrc = Bitmap.createScaledBitmap(fullsizeSrc, mOrigImg.getWidth(), mOrigImg.getHeight(), false);

                // Destination Buffer
                Bitmap dst = scaledSrc.copy(scaledSrc.getConfig(), true);
                NativeBlur.blurImage(scaledSrc, dst);

                mOrigImg.setImageBitmap(scaledSrc);
                mBlurImg.setImageBitmap(dst);

                mOrigImg.getViewTreeObserver().removeOnPreDrawListener(this);
            }
            return true;
        }
    };
}
