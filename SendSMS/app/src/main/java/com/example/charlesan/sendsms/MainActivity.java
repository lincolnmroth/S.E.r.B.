package com.example.charlesan.sendsms;

import android.support.v7.app.AppCompatActivity;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.app.Activity;

import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.telephony.SmsManager;

import android.util.Log;
import android.view.Menu;
import android.view.View;

import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class MainActivity extends Activity {
    private static final int MY_PERMISSIONS_REQUEST_SEND_SMS =0 ;
    Button sendHelpBtn;
    Button noHelpBtn;
    EditText txtphoneNo;
    EditText txtMessage;
    String phoneNo;
    String message;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //sendBtn = (Button) findViewById(R.id.btnSendSMS);
        //txtphoneNo = (EditText) findViewById(R.id.editText);
        //txtMessage = (EditText) findViewById(R.id.editText2);
        sendHelpBtn = (Button) findViewById(R.id.needhelp);
        noHelpBtn = (Button) findViewById(R.id.doesntneedhelp);

        sendHelpBtn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                Toast.makeText(getApplicationContext(),
                        "Sending", Toast.LENGTH_LONG).show();
                sendSMSMessage();
            }
        });

        noHelpBtn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View view) {
                Toast.makeText(getApplicationContext(),
                        "No message sent. Carry on.", Toast.LENGTH_LONG).show();
            }
        });
    }

    protected void sendSMSMessage() {
        phoneNo = "6093692181";
        message = "I have fallen and I can't get up!";
        Log.d("PHONE NUMBER1:",phoneNo);
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.SEND_SMS)
                != PackageManager.PERMISSION_GRANTED) {
            Log.d("PHONE NUMBER2:",phoneNo);
            if (ActivityCompat.shouldShowRequestPermissionRationale(this,
                    Manifest.permission.SEND_SMS)) {
                Log.d("PHONE NUMBER5:",phoneNo);
            } else {
                Log.d("PHONE NUMBER3:",phoneNo);
                ActivityCompat.requestPermissions(this,
                        new String[]{Manifest.permission.SEND_SMS},
                        MY_PERMISSIONS_REQUEST_SEND_SMS);
            }
        }
        else{
            Log.d("PHONE NUMBER6:",phoneNo);
            if (ActivityCompat.shouldShowRequestPermissionRationale(this,
                    Manifest.permission.SEND_SMS)) {
                Log.d("PHONE NUMBER7:",phoneNo);
            } else {
                Log.d("PHONE NUMBER8:",phoneNo);
                ActivityCompat.requestPermissions(this,
                        new String[]{Manifest.permission.SEND_SMS},
                        MY_PERMISSIONS_REQUEST_SEND_SMS);
            }
        }
        Log.d("PHONE NUMBER4:",phoneNo);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,String permissions[], int[] grantResults) {
        Log.d("PHONE NUMBER:",phoneNo);
        switch (requestCode) {
            case MY_PERMISSIONS_REQUEST_SEND_SMS: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    SmsManager smsManager = SmsManager.getDefault();
                    Log.d("PHONE NUMBER:",phoneNo);
                    smsManager.sendTextMessage(phoneNo, null, message, null, null);
                    Toast.makeText(getApplicationContext(), "SMS sent.",
                            Toast.LENGTH_LONG).show();
                } else {
                    Toast.makeText(getApplicationContext(),
                            "SMS faild, please try again.", Toast.LENGTH_LONG).show();
                    return;
                }
            }
        }

    }
}