package com.freemcp.app;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;
import android.view.View;
import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("native-lib");
    }

    private native String sendPromptToLaptop(String ip, String prompt);

    private boolean isConnected = false;
    private Timer pingTimer = null;

    private void log(TextView tv, String msg) {
        runOnUiThread(() -> {
            tv.append(msg + "\n");
            final ScrollView scroller = findViewById(R.id.log_scroll);
            scroller.post(() -> scroller.fullScroll(ScrollView.FOCUS_DOWN));
        });
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        EditText ipInput = findViewById(R.id.ip_input);
        EditText promptInput = findViewById(R.id.prompt_input);
        com.google.android.material.button.MaterialButton sendButton = findViewById(R.id.send_button);
        com.google.android.material.button.MaterialButton connectButton = findViewById(R.id.connect_button);
        TextView logOutput = findViewById(R.id.log_output);
        TextView statusText = findViewById(R.id.status_text);
        View statusDot = findViewById(R.id.status_dot);
        com.google.android.material.switchmaterial.SwitchMaterial killSwitch = findViewById(R.id.kill_switch);

        
        android.content.SharedPreferences prefs = getPreferences(MODE_PRIVATE);
        String savedIp = prefs.getString("last_ip", "");
        if (!savedIp.isEmpty()) {
            ipInput.setText(savedIp);
        }

        
        connectButton.setOnClickListener(v -> {
            String ip = ipInput.getText().toString().trim();
            if (ip.isEmpty()) return;

            if (!isConnected) {
                prefs.edit().putString("last_ip", ip).apply();
                connectButton.setEnabled(false);
                log(logOutput, "[ ] Launching & connecting to " + ip + "...");

                new Thread(() -> {
                    
                    sendPromptToLaptop(ip, "LAUNCH_FREEMCP");

                    
                    try { Thread.sleep(1500); } catch (InterruptedException ignored) {}

                    
                    String response = sendPromptToLaptop(ip, "PING");
                    runOnUiThread(() -> {
                        if (response != null && response.trim().equals("PONG")) {
                            isConnected = true;
                            connectButton.setText("DISCONNECT");
                            connectButton.setBackgroundTintList(android.content.res.ColorStateList.valueOf(0xFFFF5252));
                            statusDot.setBackgroundResource(R.drawable.status_dot_green);
                            statusText.setText("LINKED");
                            log(logOutput, "[✓] Connected");
                            startHeartbeat(ip, statusDot, statusText, logOutput, connectButton);
                        } else {
                            log(logOutput, "[✗] Failed — check IP or service");
                            statusDot.setBackgroundResource(R.drawable.status_dot_red);
                            statusText.setText("FAILED");
                        }
                        connectButton.setEnabled(true);
                    });
                }).start();
            } else {
                isConnected = false;
                stopHeartbeat();
                connectButton.setText("CONNECT");
                connectButton.setBackgroundTintList(android.content.res.ColorStateList.valueOf(0xFF4CAF50));
                statusDot.setBackgroundResource(R.drawable.status_dot_red);
                statusText.setText("DISCONNECTED");
                log(logOutput, "[—] Disconnected");
            }
        });

        
        sendButton.setOnClickListener(v -> {
            String ip = ipInput.getText().toString().trim();
            String prompt = promptInput.getText().toString().trim();
            if (ip.isEmpty() || prompt.isEmpty()) return;

            log(logOutput, "YOU > " + prompt);
            sendButton.setEnabled(false);

            final String finalWirePrompt = prompt;
            new Thread(() -> {
                String response = sendPromptToLaptop(ip, finalWirePrompt);
                runOnUiThread(() -> {
                    log(logOutput, "ACE > " + response);
                    sendButton.setEnabled(true);
                    promptInput.setText("");
                });
            }).start();
        });


        
        killSwitch.setOnCheckedChangeListener((v, isChecked) -> {
            String ip = ipInput.getText().toString().trim();
            if (ip.isEmpty()) return;

            String cmd = isChecked ? "SET_KILL_SWITCH ON" : "SET_KILL_SWITCH OFF";
            log(logOutput, "[⛔] " + cmd);

            new Thread(() -> sendPromptToLaptop(ip, cmd)).start();
        });

        
        com.google.android.material.switchmaterial.SwitchMaterial charSwitch = findViewById(R.id.character_switch);
        charSwitch.setOnCheckedChangeListener((v, isChecked) -> {
            String ip = ipInput.getText().toString().trim();
            if (ip.isEmpty()) return;

            String cmd = isChecked ? "SET_CHARACTER_MODE ON" : "SET_CHARACTER_MODE OFF";
            log(logOutput, "[🎭] " + cmd);

            new Thread(() -> sendPromptToLaptop(ip, cmd)).start();
        });
    }

    private void startHeartbeat(String ip, View statusDot, TextView statusText, TextView logOutput,
                                com.google.android.material.button.MaterialButton connectButton) {
        stopHeartbeat();
        pingTimer = new Timer();
        pingTimer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                if (!isConnected) {
                    cancel();
                    return;
                }
                String response = sendPromptToLaptop(ip, "PING");
                runOnUiThread(() -> {
                    if (response != null && response.trim().equals("PONG")) {
                        statusDot.setBackgroundResource(R.drawable.status_dot_green);
                        statusText.setText("LINKED");
                    } else {
                        statusDot.setBackgroundResource(R.drawable.status_dot_red);
                        statusText.setText("LINK LOST");
                        log(logOutput, "[!] Heartbeat lost");
                        isConnected = false;
                        connectButton.setText("CONNECT");
                        connectButton.setBackgroundTintList(android.content.res.ColorStateList.valueOf(0xFF4CAF50));
                    }
                });
            }
        }, 8000, 12000);
    }

    private void stopHeartbeat() {
        if (pingTimer != null) {
            pingTimer.cancel();
            pingTimer = null;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopHeartbeat();
    }
}
