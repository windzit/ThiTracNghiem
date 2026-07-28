#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <string.h>
#include <windows.h>
#include <limits.h>
#include <iostream>
using namespace std;

#define Enter 13
//const int WHITE=15;
#define PASSWORD "abcdef"
//const int WHITE=15;

bool CheckPhimChucNang(int  c ){
    if ( c== 0 || c == 224){
        return true;
    }
    return false;
}
bool CheckBackSpace( int c ){
    if ( c== 8 || c == 127){
        return true;
    }
    return false;

}

bool CheckChuCai(int c){
    if( (c >='a'&& c<='z' )|| c >='A'&& c <='Z'){
        return true;
    }
    return false;
}

bool CheckChuSo(int c){
    if( (c >='0'&& c<='9' )){
        return true;
    }
    return false;
}


int NhapChuSo(){
    char S[20];
    int i, hople,c;
    do{
        i = 0;
        hople = 1;
        cout <<"Nhap So Nguyen ( 0-9 / chap nhan so am)";
        while ((c= getch())!=Enter){
            if ( CheckPhimChucNang(c)){
                getch();
                continue;
            }
            if( CheckBackSpace(c)){
                if( i > 0){
                    S[--i]='\0';
                    printf("\b \b");
                }
                continue;
            }

            if ( i < 20 && ((CheckChuSo(c))||(i==1 && c=='-'))){
                S[i]= (char)c;
                printf("%c",(char)c);
                i++;
            }
            else{
                putchar('\a');
            }

        }
        S[i]='\0';
        if ( i ==0 ) hople = 0;
        if (!hople) cout <<"So vua nhap khong hop le ";
        else cout <<endl;
    }
    while(!hople);

    long val= stol(S);
    if (val >= INT_MAX) val = INT_MAX;
    if (val <= INT_MIN) val = INT_MIN;
    return val;
    
}


char* NhapChuoi(char*S , int gioihan){
    if ( S== NULL || gioihan ==0){
        if(S!=NULL)return S;
        return S;
    }
    int i , c , hople;
    do{
        i = 0;
        hople = 1;
        cout <<"Nhap Chuoi : " ;
        while((c=getch())!=Enter){
            if(CheckPhimChucNang(c)){
                getch();
                continue;
            }

            if(CheckBackSpace(c)){
                if ( i > 0){
                    S[--i]='\0';
                    printf("\b \b");
                }
                continue;
            }

            if(i < gioihan && ((CheckChuCai(c)) || c==' ') || CheckChuSo(c)){
                S[i]= (char)c;
                printf("%c",(char)c);
                i++;
            }

            else{
                putchar('\0');
            }
        }
        S[i]='\0';
        {
            int start ,end,w,len;
            start = 0;
            end = i - 1;
            while (S[start] ==' ')start++;
            while (end >= start && S[end]==' ')end--;
            len = end - start + 1;
            for ( int k =0 ; k < len; k++){
                S[k] =S[k+start];
            }
            int preSpace = 0;
            for (int k = 0 ; k < len ; k ++){
                if (S[k] ==' '){
                    if (!preSpace){
                        S[w++]=' ';
                        preSpace = 1;
                    }
                }
                else{
                    S[w++] =S[k];
                    preSpace =0;
                }
            }
            if ( w == 0){
                hople = 0;
            }
        }

        if(!hople)cout <<"Chuoi nhap vao khong hop le";
        else cout <<endl;

    }
    while(!hople);
    return S;

}

char* NhapMa(char* S, int gioihan){
    if ( S== NULL || gioihan ==0){
        if(S!=NULL)return S;
        return S;
    }
    int i , c , hople;
    do{
        i = 0;
        hople = 1;
        cout <<"Nhap Chuoi : " ;
        while((c=getch())!=Enter){
            if(CheckPhimChucNang(c)){
                getch();
                continue;
            }

            if(CheckBackSpace(c)){
                if ( i > 0){
                    S[--i]='\0';
                    printf("\b \b");
                }
                continue;
            }

            if(i < gioihan && ((CheckChuCai(c)) || CheckChuSo(c) || c=='-'|| c=='_')){
                S[i]= (char)c;
                printf("%c",(char)c);
                i++;
            }

            else{
                putchar('\0');
            }
        }
        S[i]='\0';
        if ( hople && i > 0){
            if (S[0] =='_' || S[0]=='-')hople = 0 ;
            if (S[i-1]== '_' || S[i-1]=='-')hople = 0;
        }
        if (hople ){
            if (strstr(S,"--")!=NULL || strstr(S,"_-")!=NULL||strstr(S,"-_")!=NULL||strstr(S,"__")!=NULL ) hople = 0;
        }
    }while(!hople);
    return S;
}
