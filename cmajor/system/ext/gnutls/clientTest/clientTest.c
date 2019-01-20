#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <gnutls/gnutls.h>
#include <gnutls/x509.h>

int main()
{
    int result = 0;
    WSADATA wsaData;
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result < 0)
    {
        fprintf(stderr, "WSA Startup failed: %d\n", result);
        return 1;
    }
    gnutls_session_t session;
    gnutls_certificate_credentials_t xcred;
    result = gnutls_certificate_allocate_credentials(&xcred);
    if (result < 0)
    {
        fprintf(stderr, "gnutls_certificate_allocate_credentials failed: %d\n", result);
        return 1;
    }
    result = gnutls_certificate_set_x509_system_trust(xcred);
    if (result < 0)
    {
        fprintf(stderr, "gnutls_certificate_set_x509_system_trust failed: %d\n", result);
        return 1;
    }
    result = gnutls_init(&session, GNUTLS_CLIENT);
    if (result < 0)
    {
        fprintf(stderr, "gnutls_init failed: %d\n", result);
        return 1;
    }
    result = gnutls_server_name_set(session, GNUTLS_NAME_DNS, "sourceforge.net", strlen("sourceforge.net"));
    if (result < 0)
    {
        fprintf(stderr, "gnutls_server_name_set failed: %d\n", result);
        return 1;
    }
    result = gnutls_set_default_priority(session);
    if (result < 0)
    {
        fprintf(stderr, "gnutls_set_default_priority failed: %d\n", result);
        return 1;
    }
    result = gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, xcred);
    if (result < 0)
    {
        fprintf(stderr, "gnutls_credentials_set failed: %d\n", result);
        return 1;
    }
    gnutls_session_set_verify_cert(session, "sourceforge.net", 0);
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
    {
        fprintf(stderr, "socket failed: %d\n", result);
        return 1;
    }
    SOCKET c = 0;
    struct addrinfo hint;
    struct addrinfo* rp;
    struct addrinfo* res;
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_flags = 0;
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = IPPROTO_TCP;
    hint.ai_addrlen = 0;
    hint.ai_addr = 0;
    hint.ai_canonname = 0;
    hint.ai_next = 0;
    result = getaddrinfo("sourceforge.net", "443", &hint, &res);
    if (result != 0)
    {
        fprintf(stderr, "getaddrinfo failed: %d\n", result);
        return 1;
    }
    for (rp = res; rp != 0; rp = rp->ai_next)
    {
        SOCKET s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (s == -1)
        {
            continue;
        }
        int result = connect(s, rp->ai_addr, (int)rp->ai_addrlen);
        if (result == 0)
        {
            c = s;
        }
        else
        {
            freeaddrinfo(res);
            fprintf(stderr, "connect failed: %d\n", result);
            return 1;
        }
    }
    gnutls_transport_set_int(session, c);
    gnutls_handshake_set_timeout(session, GNUTLS_DEFAULT_HANDSHAKE_TIMEOUT);
    do 
    {
        result = gnutls_handshake(session);
    }
    while (result < 0 && gnutls_error_is_fatal(result) == 0);
    if (result < 0)
    {
        fprintf(stderr, "handshake failed: %d\n", result);
        return 1;
    }
    const char* msg = "GET / HTTP/1.1\r\nHost: sourceforge.net\r\n\r\n";
    result = gnutls_record_send(session, msg, strlen(msg));
    if (result < 0)
    {
        fprintf(stderr, "send failed: %d\n", result);
        return 1;
    }
    char buffer[1025];
    result = gnutls_record_recv(session, buffer, 1024);
    if (result < 0)
    {
        fprintf(stderr, "recv failed: %d\n", result);
        return 1;
    }
    result = gnutls_bye(session, GNUTLS_SHUT_RDWR);
    if (result < 0)
    {
        fprintf(stderr, "gnutls_bye failed: %d\n", result);
        return 1;
    }
    closesocket(c);
    closesocket(s);
    WSACleanup();
    return 0;
}
