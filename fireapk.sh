#!/bin/bash

echo "Building FreeMCP Premium..."
./buildapk.sh || exit 1

echo "Checking for connected Android devices..."
DEVICE=$(adb devices | grep -v "List" | grep "device" | head -n1 | cut -f1)

if [ -z "$DEVICE" ]; then
    echo "ERROR: No device found. Please plug in your phone or start emulator."
    exit 1
fi

echo "Found device: $DEVICE. Deploying..."

APK_PATH="android/app/build/outputs/apk/release/app-release.apk"
echo "Installing $APK_PATH..."
adb -s "$DEVICE" install -r "$APK_PATH"

LOCAL_IP=$(ip route get 1.1.1.1 | grep -oP 'src \K\S+')
echo "-------------------------------------------------"
echo "CONNECT TO IP: $LOCAL_IP"
echo "-------------------------------------------------"

echo "Fired up! Check your phone screen!"
