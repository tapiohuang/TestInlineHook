package o.w.testinlinehook;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

import java.util.Locale;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findViewById(R.id.btn_test_jni).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int value = 0;
                try {
                    value = TestJni.getValue();
                } catch (Throwable throwable) {
                    throwable.printStackTrace();
                }
                Toast.makeText(MainActivity.this, String.format(Locale.CHINA, "value:%d", value), Toast.LENGTH_LONG).show();
            }
        });
        findViewById(R.id.btn_inline_hook).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                try {
                    TestInlineHook.inlineHook();
                } catch (Throwable throwable) {
                    throwable.printStackTrace();
                }
            }
        });
    }
}