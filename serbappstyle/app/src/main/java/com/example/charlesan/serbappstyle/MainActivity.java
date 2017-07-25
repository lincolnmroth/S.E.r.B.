package com.example.charlesan.serbappstyle;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

public class MainActivity extends AppCompatActivity {
    private String username="milos";
    private String password="fakeserb";
    private String enteredusername="";
    private String enteredpassword="";
    Button enterbutton;
    EditText userenter;
    EditText passenter;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        enterbutton = (Button) findViewById(R.id.mainenterbutton);
        userenter = (EditText)findViewById(R.id.enterusername);
        passenter = (EditText)findViewById(R.id.enterpassword);

        enterbutton.setOnClickListener(
            new View.OnClickListener(){
                public void onClick(View view){
                    enteredusername=userenter.getText().toString();
                    enteredpassword=passenter.getText().toString();
                    if(username.equals(enteredusername)){
                        if(password.equals(enteredpassword)){
                            Log.d("check3","3");
                            Intent intent = new Intent(view.getContext(),HomeScreen.class);
                            startActivity(intent);
                            Log.d("check4","4");
                        }
                    }
                }
            }
        );
    }

}
