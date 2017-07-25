package com.example.charlesan.serbappstyle;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.ViewGroup;
import android.widget.TextView;

/**
 * Created by Charles on 7/25/2017.
 */

public class HomeScreen extends AppCompatActivity{
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d("check1","1");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_home_screen);
        Log.d("check2","2");
    }
}
