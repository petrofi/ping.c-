#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>

#define PACKET_SIZE 64

int validateIpAddress(char *ipAddress) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }
    if (len == 1) {
        sum += *(unsigned char *) buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void sendPacket(int sockfd, struct sockaddr_in addr) {
    char packet[PACKET_SIZE];
    struct icmp *icmp_header = (struct icmp *) packet;

    icmp_header->icmp_type = ICMP_ECHO;
    icmp_header->icmp_code = 0;
    icmp_header->icmp_id = getpid() & 0xFFFF;
    icmp_header->icmp_seq = 0;
    icmp_header->icmp_cksum = 0;
    
    memset(packet + sizeof(struct icmp), 0, PACKET_SIZE - sizeof(struct icmp));
    icmp_header->icmp_cksum = checksum(packet, PACKET_SIZE);

    int bytes_sent = sendto(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr *) &addr, sizeof(addr));
    if (bytes_sent < 0) {
        printf("Error sending ICMP packet.\n");
    }
}

int receivePacket(int sockfd, struct sockaddr_in addr, struct timeval start_time) {
    char packet[PACKET_SIZE];
    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);
    
    int bytes_received = recvfrom(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr *) &from, &fromlen);
    if (bytes_received < 0) {
        printf("Error receiving ICMP packet or timeout.\n");
        return -1;
    }

    struct iphdr *ip_header = (struct iphdr *) packet;
    struct icmp *icmp_header = (struct icmp *) (packet + (ip_header->ihl * 4));

    if (icmp_header->icmp_type == ICMP_ECHOREPLY && icmp_header->icmp_id == (getpid() & 0xFFFF)) {
        struct timeval end_time;
        gettimeofday(&end_time, NULL);
        double elapsed_ms = (end_time.tv_sec - start_time.tv_sec) * 1000.0 + 
                            (end_time.tv_usec - start_time.tv_usec) / 1000.0;

        printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
               bytes_received - (ip_header->ihl * 4) - sizeof(struct icmp),
               inet_ntoa(from.sin_addr),
               icmp_header->icmp_seq,
               ip_header->ttl,
               elapsed_ms);
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <IP Address>\n", argv[0]);
        return 1;
    }

    if (!validateIpAddress(argv[1])) {
        printf("Invalid IP Address.\n");
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        printf("Socket creation failed. You might need root/sudo privileges.\n");
        return 1;
    }

    struct timeval tv_out;
    tv_out.tv_sec = 2;
    tv_out.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_out, sizeof tv_out);

    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &(dest_addr.sin_addr));

    printf("PING %s (%s) %d bytes of data.\n", argv[1], argv[1], PACKET_SIZE);

    for (int i = 0; i < 4; i++) {
        struct timeval start_time;
        gettimeofday(&start_time, NULL);
        
        sendPacket(sockfd, dest_addr);
        receivePacket(sockfd, dest_addr, start_time);
        
        sleep(1);
    }

    close(sockfd);
    return 0;
}
