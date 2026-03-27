#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <jni.h>
#include <netdb.h>
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_freemcp_app_MainActivity_sendPromptToLaptop(JNIEnv *env, jobject,
                                                     jstring ip,
                                                     jstring prompt) {

  const char *native_ip = env->GetStringUTFChars(ip, nullptr);
  const char *native_prompt = env->GetStringUTFChars(prompt, nullptr);
  std::string result = "Connection failed";

  std::string host_str = native_ip;
  std::string port_str = "9000";
  size_t last_colon = host_str.find_last_of(':');
  size_t first_colon = host_str.find_first_of(':');

  if (last_colon != std::string::npos) {
    if (first_colon == last_colon ||
        (host_str.front() == '[' && host_str.find(']') != std::string::npos &&
         host_str.find(']') < last_colon)) {
      port_str = host_str.substr(last_colon + 1);
      host_str = host_str.substr(0, last_colon);
      if (!host_str.empty() && host_str.front() == '[' &&
          host_str.back() == ']') {
        host_str = host_str.substr(1, host_str.length() - 2);
      }
    }
  }

  struct addrinfo hints, *res, *p;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(host_str.c_str(), port_str.c_str(), &hints, &res) == 0) {
    int sock = -1;
    int conn_result = -1;

    for (p = res; p != nullptr; p = p->ai_next) {
      sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      if (sock == -1)
        continue;

      int flags = fcntl(sock, F_GETFL, 0);
      fcntl(sock, F_SETFL, flags | O_NONBLOCK);

      conn_result = connect(sock, p->ai_addr, p->ai_addrlen);
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
          if (so_error != 0)
            conn_result = -1;
        } else {
          conn_result = -1;
        }
      }

      fcntl(sock, F_SETFL, flags);

      if (conn_result >= 0) {
        break;
      }
      close(sock);
      sock = -1;
    }

    if (sock != -1 && conn_result >= 0) {
      struct timeval tv;
      tv.tv_sec = 60;
      tv.tv_usec = 0;
      setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
      setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv, sizeof tv);

      send(sock, native_prompt, strlen(native_prompt), 0);

      char buffer[8192];
      std::string full_response = "";
      int bytes_read;
      while ((bytes_read = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        full_response.append(buffer, bytes_read);
      }
      if (!full_response.empty())
        result = full_response;
      else
        result = "Sent (no response)";

      close(sock);
    }

    freeaddrinfo(res);
  }

  env->ReleaseStringUTFChars(ip, native_ip);
  env->ReleaseStringUTFChars(prompt, native_prompt);
  return env->NewStringUTF(result.c_str());
}
