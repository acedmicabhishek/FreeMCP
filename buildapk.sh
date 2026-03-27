#!/bin/bash

export ANDROID_HOME=/opt/android-sdk
export ANDROID_NDK_HOME=/opt/android-sdk/ndk/27.1.12297006

mkdir -p ./android
cd android

echo "sdk.dir=$ANDROID_HOME" > local.properties
echo "ndk.dir=$ANDROID_NDK_HOME" >> local.properties

echo "Starting Release Build..."
./gradlew assembleRelease || gradle assembleRelease

if [ $? -eq 0 ]; then
    echo "================================================="
    echo "BUILD SUCCESSFUL (Signed with Debug Key)"
    echo "APK Location: android/app/build/outputs/apk/release/app-release.apk"
    echo "================================================="
    echo "TIP: Use 'adb install -r android/app/build/outputs/apk/release/app-release.apk' to deploy."
else
    echo "BUILD FAILED"
    exit 1
fi
