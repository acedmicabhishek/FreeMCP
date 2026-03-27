#include <jni.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_freemcp_app_MainActivity_sendPromptToLaptop(
        JNIEnv* env,
        jobject,
        jstring ip,
        jstring prompt) {
    
    const char* native_ip = env->GetStringUTFChars(ip, nullptr);
    const char* native_prompt = env->GetStringUTFChars(prompt, nullptr);
    std::string result = "Connection failed";
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock != -1) {
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(9000);

        if (inet_pton(AF_INET, native_ip, &server_addr.sin_addr) > 0) {

            
            int flags = fcntl(sock, F_GETFL, 0);
            fcntl(sock, F_SETFL, flags | O_NONBLOCK);

            int conn_result = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
            if (conn_result < 0 && errno == EINPROGRESS) {
                fd_set write_fds;
                FD_ZERO(&write_fds);
                FD_SET(sock, &write_fds);
                struct timeval conn_tv;
                conn_tv.tv_sec = 5;
                conn_tv.tv_usec = 0;
                conn_result = select(sock + 1, nullptr, &write_fds, nullptr, &conn_tv);
                if (conn_result > 0) {
                    int so_error;
                    socklen_t len = sizeof(so_error);
                    getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
                    if (so_error != 0) conn_result = -1;
                } else {
                    conn_result = -1; 
                }
            }

            
            fcntl(sock, F_SETFL, flags);

            if (conn_result >= 0) {
                struct timeval tv;
                tv.tv_sec = 60;
                tv.tv_usec = 0;
                setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
                setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);

                send(sock, native_prompt, strlen(native_prompt), 0);
                
                char buffer[8192];
                std::string full_response = "";
                int bytes_read;
                while ((bytes_read = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
                    full_response.append(buffer, bytes_read);
                }
                if (!full_response.empty()) result = full_response;
                else result = "Sent (no response)";
            }
        }
        close(sock);
    }
    
    env->ReleaseStringUTFChars(ip, native_ip);
    env->ReleaseStringUTFChars(prompt, native_prompt);
    return env->NewStringUTF(result.c_str());
}
