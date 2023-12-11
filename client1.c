#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>
#include <ctype.h>


int CountAnswer(int number_C,int guess,int *p_str, int *p_bal);
void ErrorHandling(const char* message);


int main(int argc, char* argv[]) {

    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAddr;

    char message[30];
    char tmp[100];
    int strLen;
    int number_C;

    // 숫자 설정 코드
    char numbers[100];
    int bolen = 1;
    while(bolen)
    {
        printf("\n(Player2)Please enter four digits : ");
        scanf(" %s",numbers);
        int n_i;
        int n_t = 0;
        int n_D = 1;
        for (n_i = 0; ((numbers[n_i] != '\0')&&(n_D==1)); ++n_i){
            bolen = 0;
            if (!(isdigit(numbers[n_i])))
            {
                printf("%d\n",n_i);
                printf("Player2 input Type Error : Retry\n");
                bolen = 1;
                n_t = 1;
                break;
            }
            int n_j;
            char n_d = numbers[n_i];
            for (n_j=n_i+1;numbers[n_j] != '\0';n_j++)
            {
                if (numbers[n_j]==n_d)
                {   
                    printf("Player2 input Overlap Error : Retry\n");
                    n_D = 0;
                    n_t=1;
                    bolen =1;
                    break;
                }
            }
                
        }
        if ((n_i != 4)&& (n_t==0))
        {
            printf("Player2 input length Error : Retry\n");
            bolen =1;
        }
    }

    number_C = atoi(numbers);

    if (argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }


    // 에러 목록
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (hSocket == INVALID_SOCKET)
        ErrorHandling("socket() error!");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));

    if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
        ErrorHandling("connect() error!");


    // 스트라이크, 볼 변수 정리
    int strike = 0 ,ball = 0;
    int *p_str ,*p_bal;
    p_str = &strike;
    p_bal = &ball;
    printf("Player2 : %d\n",number_C);

    while (1) {
        
        // 클라이언트로 메세지 전송
        int boolen = 1;
        while(boolen)
        {
            printf("\nmessage enter >>");
            scanf(" %s",tmp);
            int i;
            int t = 0;
            int num_D = 1;
            for (i = 0; ((tmp[i] != '\0')&&(num_D==1)); ++i){
                boolen = 0;
                if (!(isdigit(tmp[i])))
                {
                    printf("%d\n",i);
                    printf("Player2 input Type Error : Retry\n");
                    boolen = 1;
                    t = 1;
                    break;
                }
                int j;
                char tmp_d = tmp[i];
                for (j=i+1;tmp[j] != '\0';j++)
                {
                    if (tmp[j]==tmp_d)
                    {   
                        printf("Player2 input Overlap Error : Retry\n");
                        num_D = 0;
                        t=1;
                        boolen =1;
                        break;
                    }
                }
                
            }
            if ((i != 4)&& (t==0))
            {
                printf("Player2 input length Error : Retry\n");
                boolen =1;
            }
        }
        send(hSocket, tmp, strlen(tmp), 0);

        // 스트라이크 개수 입력 받기
        strLen = recv(hSocket, message, sizeof(message) - 1, 0);
        if (message[0] == 'Y')
        {
            printf("%s\n", message);
            break;
        }
        if (strLen <= 0)
            ErrorHandling("read() error!");
        printf("Strike Number : %s\n", message);

        // 볼 개수 입력 받기
        strLen = recv(hSocket, message, sizeof(message) - 1, 0);
        if (strLen <= 0)
            ErrorHandling("read() error!");
        printf("Ball number : %s\n", message);

        int num;
        strLen = recv(hSocket, message, sizeof(message) - 1, 0);
        if (strLen <= 0)
            ErrorHandling("read() error!");
        printf("Message from server: %s\n", message);
        num = atoi(message);

        // 볼과 스트라이크 개수 측정 함수
        CountAnswer(number_C,num,p_str, p_bal);
        printf("%d %d\n",*p_str,*p_bal);

        // 게임 끝 판정
        if (*p_str == 4) 
        {
            printf("Defeat");
            char GameWin[30] = "Your Winner!";
            send(hSocket, GameWin, strlen(GameWin) + 1, 0);
            break;
        }

        // 스트라이크 개수 전송
        char strikeStr[10];
        snprintf(strikeStr, sizeof(strikeStr), "%d", *p_str);
        send(hSocket, strikeStr, strlen(strikeStr) + 1, 0);

        // 볼 개수 전송
        char ballStr[10];
        snprintf(ballStr, sizeof(ballStr), "%d", *p_bal);
        send(hSocket, ballStr, strlen(ballStr) + 1, 0);


    }
    closesocket(hSocket);
    exit(EXIT_SUCCESS);
    WSACleanup();

    return 0;
}



void ErrorHandling(const char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}


int CountAnswer(int number_C,int guess,int *p_str, int *p_bal)
{
    int strike = 0;
    int ball = 0;
    

    int tempAnswer = number_C;
    int tempGuess = guess;

    for (int i = 0; i < 4; ++i) {
        if (tempAnswer % 10 == tempGuess % 10) {
            ++strike;
        }
        else {
            int temp = number_C;
            for (int j = 0; j < 4; ++j) {
                if (temp % 10 == tempGuess % 10) {
                    ++ball;
                    break;
                }
                temp /= 10;
            }
        }

        
        tempAnswer /= 10;
        tempGuess /= 10;
    }
    

    *p_str = strike;
    *p_bal = ball;

    return 0;
}



