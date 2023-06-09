/*
 * Copyright 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.ece420.lab5;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.Manifest;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;
import android.widget.EditText;
import android.widget.Toast;
import java.util.Random ;

public class MainActivity extends Activity
        implements ActivityCompat.OnRequestPermissionsResultCallback {

    // UI Variables
    Button   trainButton;
    Button identifyButton;
    TextView statusView;
    TextView page_title_view;
    TextView textView;
    //TextView speakerView;
    EditText inputText;
    String  nativeSampleRate;
    String  nativeSampleBufSize;
    int nameID;
    boolean supportRecording;
    Boolean isPlaying = false;

    TextView inferenceView;

    // Static Values
    private static final int AUDIO_ECHO_REQUEST = 0;
    private static final int FRAME_SIZE = 1024;
    private static final int MIN_FREQ = 50;

    /* GLOBAL VARIABLES */
    int process; // flag for what process we are on, 0 for train, 1 for identify
    Boolean isTraining = false;
    int isIdentify = -1; // represents if we stop(-1)/start(1) identifying
    int inference = 0;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_main);
        super.setRequestedOrientation (ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        // define all BUTTONS and VIEWS from .xml vile
        statusView = (TextView)findViewById(R.id.statusView);
        trainButton = (Button)findViewById((R.id.train_button));
        identifyButton = (Button)findViewById((R.id.identify_button));
        textView = (TextView)findViewById((R.id.textView));
        inputText = (EditText)findViewById((R.id.inputText));
        //speakerView = (TextView)findViewById(R.id.speakerView);
        inferenceView = (TextView)findViewById(R.id.inferenceView);

        /* initialize native audio system */
        queryNativeAudioParameters();
        updateNativeAudioUI();
        if (supportRecording) {
            // Native Setting: 48k Hz Sampling Frequency and 128 Frame Size
            createSLEngine(Integer.parseInt(nativeSampleRate), FRAME_SIZE);
        }

        // Setup UI
        page_title_view = (TextView) findViewById(R.id.PageTitle_View);
        page_title_view.setText("Begin Your Training: ");

    }


    @Override
    protected void onDestroy() {
        if (supportRecording) {
            if (isPlaying) {
                stopPlay();
            }
            deleteSLEngine();
            isPlaying = false;
        }
        super.onDestroy();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    public void startEcho() {
        if (!isPlaying) {
            if(!createSLBufferQueueAudioPlayer()) {
                statusView.setText(getString(R.string.error_player));
                return;
            }
            if(!createAudioRecorder()) {
                deleteSLBufferQueueAudioPlayer();
                statusView.setText(getString(R.string.error_recorder));
                return;
            }
            startPlay();   // this must include startRecording()
            statusView.setText(getString(R.string.status_echoing));
        } else {
            stopPlay();  //this must include stopRecording()
            updateNativeAudioUI();
            deleteAudioRecorder();
            deleteSLBufferQueueAudioPlayer();
        }
        isPlaying = !isPlaying;
        if(isPlaying)
        {
            statusView.setText("Recording");
        }
        if(process == 0){
            trainButton.setText(getString((isPlaying) ?
                    R.string.StopTrain: R.string.StartTrain));
        }
        else if(process == 1){
            identifyButton.setText(getString((isPlaying) ? R.string.StopIdentify: R.string.StartIdentify));
        }

    }

    public void onTrainClick(View view) {
        isTraining = !isTraining ;
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) !=
                                               PackageManager.PERMISSION_GRANTED) {
            statusView.setText(getString(R.string.status_record_perm));
            ActivityCompat.requestPermissions(
                    this,
                    new String[] { Manifest.permission.RECORD_AUDIO },
                    AUDIO_ECHO_REQUEST);
            return;
        }
        /* pass nameID and random_recording_id to ece420_main.cpp */
        nameID = Integer.parseInt(inputText.getText().toString());
        if(isTraining){
            inferenceView.setText(getString(R.string.disclaimer));
            writeNameID(nameID);
            setFlags(0, isIdentify);
        }
        else {
            debugLog(); // print mfcc feature map
        }
        process = 0;
        startEcho();
    }

    public void onIdentifyClick(View view){
        isIdentify = -1 * isIdentify;
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) !=
                PackageManager.PERMISSION_GRANTED) {
            statusView.setText(getString(R.string.status_record_perm));
            ActivityCompat.requestPermissions(
                    this,
                    new String[] { Manifest.permission.RECORD_AUDIO },
                    AUDIO_ECHO_REQUEST);
            return;
        }

        if(isIdentify == -1){
            inference = setFlags(1, isIdentify);
            inferenceView.setText("Guess: "+ inference);
        }
        else{
            inferenceView.setText(getString(R.string.disclaimer));
        }
        process = 1;
        startEcho();
    }
    /* */

    public void getLowLatencyParameters(View view) {
        updateNativeAudioUI();
        return;
    }

    public void queryNativeAudioParameters() {
        AudioManager myAudioMgr = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        nativeSampleRate  =  myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
        nativeSampleBufSize =myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
        int recBufSize = AudioRecord.getMinBufferSize(
                Integer.parseInt(nativeSampleRate),
                AudioFormat.CHANNEL_IN_MONO,
                AudioFormat.ENCODING_PCM_16BIT);
        supportRecording = true;
        if (recBufSize == AudioRecord.ERROR ||
            recBufSize == AudioRecord.ERROR_BAD_VALUE) {
            supportRecording = false;
        }
    }

    public void updateNativeAudioUI() {
        if (!supportRecording) {
            statusView.setText(getString(R.string.error_no_mic));
            trainButton.setEnabled(false);
            return;
        }

        statusView.setText("nativeSampleRate    = " + nativeSampleRate + "\n" +
                "nativeSampleBufSize = " + nativeSampleBufSize + "\n");

    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        /*
         * if any permission failed, the sample could not play
         */
        if (AUDIO_ECHO_REQUEST != requestCode) {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
            return;
        }

        if (grantResults.length != 1  ||
            grantResults[0] != PackageManager.PERMISSION_GRANTED) {
            /*
             * When user denied permission, throw a Toast to prompt that RECORD_AUDIO
             * is necessary; also display the status on UI
             * Then application goes back to the original state: it behaves as if the button
             * was not clicked. The assumption is that user will re-click the "start" button
             * (to retry), or shutdown the app in normal way.
             */
            statusView.setText(getString(R.string.error_no_permission));
            Toast.makeText(getApplicationContext(),
                    getString(R.string.prompt_permission),
                    Toast.LENGTH_SHORT).show();
            return;
        }

        /*
         * When permissions are granted, we prompt the user the status. User would
         * re-try the "start" button to perform the normal operation. This saves us the extra
         * logic in code for async processing of the button listener.
         */
        statusView.setText("RECORD_AUDIO permission granted, touch " +
                           getString(R.string.StartTrain) + " to begin");

        // The callback runs on app's thread, so we are safe to resume the action
        startEcho();
    }


    /*
     * Loading our Libs
     */
    static {
        System.loadLibrary("echo");
    }



    /*
     * jni function implementations...
     */
    public static native void createSLEngine(int rate, int framesPerBuf);
    public static native void deleteSLEngine();

    public static native boolean createSLBufferQueueAudioPlayer();
    public static native void deleteSLBufferQueueAudioPlayer();

    public static native boolean createAudioRecorder();
    public static native void deleteAudioRecorder();
    public static native void startPlay();
    public static native void stopPlay();

    public static native void writeNameID(int nameid);

    public static native int setFlags(int process, int identify_action);

    /* for debugging purposes */
    public static native void debugLog();
}
