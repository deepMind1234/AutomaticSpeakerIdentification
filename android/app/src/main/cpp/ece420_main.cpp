//
// Created by daran on 1/12/2017 to be used in ECE420 Sp17 for the first time.
// Modified by dwang49 on 1/1/2018 to adapt to Android 7.0 and Shield Tablet updates.
//

#include <jni.h>
#include "ece420_main.h"
#include "ece420_lib.h"
#include "kiss_fft/kiss_fft.h"
#include <android/log.h>


/* apart of Deepak's code */
#include "ASI_UTILS.h"
#include "./kiss_fft/kiss_fft.h"
#define NUM_SAMPLES 8192
#define FRAME_SIZE 1024
#define ZP_FACTOR 1024


// JNI Function
extern "C" {
JNIEXPORT void JNICALL
Java_com_ece420_lab5_MainActivity_writeNameID(JNIEnv *env, jclass, jint);
}


// Student Variables
#define EPOCH_PEAK_REGION_WIGGLE 30
#define VOICED_THRESHOLD 200000000
#define FRAME_SIZE 1024
#define BUFFER_SIZE (3 * FRAME_SIZE)
#define F_S 48000
float bufferIn[BUFFER_SIZE] = {};
float bufferOut[BUFFER_SIZE] = {};
int newEpochIdx = FRAME_SIZE;

// We have two variables here to ensure that we never change the desired frequency while
// processing a frame. Thread synchronization, etc. Setting to 300 is only an initializer.
int FREQ_NEW_ANDROID = 300;
int FREQ_NEW = 300;
int name_ID;

void ece420ProcessFrame(sample_buf *dataBuf) {
    __android_log_print(ANDROID_LOG_DEBUG, "ID", "%d", name_ID);
    // Keep in mind, we only have 20ms to process each buffer!
    struct timeval start;
    struct timeval end;
    gettimeofday(&start, NULL);

    // Get the new desired frequency from android
    FREQ_NEW = FREQ_NEW_ANDROID;

    // Data is encoded in signed PCM-16, little-endian, mono
    int16_t data[FRAME_SIZE];
    for (int i = 0; i < FRAME_SIZE; i++) {
        data[i] = ((uint16_t) dataBuf->buf_[2 * i]) | (((uint16_t) dataBuf->buf_[2 * i + 1]) << 8);
    }

    // Shift our old data back to make room for the new data
    for (int i = 0; i < 2 * FRAME_SIZE; i++) {
        bufferIn[i] = bufferIn[i + FRAME_SIZE - 1];
    }

    // Finally, put in our new data.
    for (int i = 0; i < FRAME_SIZE; i++) {
        bufferIn[i + 2 * FRAME_SIZE - 1] = (float) data[i];
    }
    /* MFCC calculation ! */
    unsigned int coeff_i;
    double mfcc_result;
    //float audio_data[NUM_SAMPLES];
    kiss_fft_cpx fft_input[NUM_SAMPLES];
    kiss_fft_cpx fft_output[NUM_SAMPLES];
    double spectrum[NUM_SAMPLES];

    for (int i = 0; i < NUM_SAMPLES; i++) {
        fft_input[i].r = dataBuf->buf_[i];
        fft_input[i].i = 0;
        printf("%f \n",fft_input[i].r);
    }

    kiss_fft_cfg cfg = kiss_fft_alloc(NUM_SAMPLES,0,NULL,NULL);
    kiss_fft(cfg,fft_input,fft_output); // perform fft using the kissfft cfg ds

    for (int i = 0; i < NUM_SAMPLES; i++) {
        spectrum[i] = (double) fft_output[i].r;
    };

    for(coeff_i = 0; coeff_i < 13; coeff_i++)
        {
            mfcc_result = GetCoefficient(spectrum, 44100, 48, 128, coeff_i);
            printf("%i %f\n", coeff_i, mfcc_result);
            __android_log_print(ANDROID_LOG_DEBUG, "TRACKERS", "%f", mfcc_result);
    }
    // The whole kit and kaboodle !

    gettimeofday(&end, NULL);
    LOGD("Time delay: %ld us",  ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));

}


JNIEXPORT void JNICALL
Java_com_ece420_lab5_MainActivity_writeNameID(JNIEnv *env, jclass, jint newnameid) {
    name_ID = (int) newnameid;
    return;
}

JNIEXPORT void JNICALL Java_com_example_MyClass_myFunction(JNIEnv* env, jobject obj, jstring inputString) {
    const char* inputCString = env->GetStringUTFChars(inputString, 0);
    // Do something with the input string here...
    env->ReleaseStringUTFChars(inputString, inputCString);
}