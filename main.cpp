#include <iostream>
#include <conio.h>///_getch() uchun
#include <windows.h>///WinAPI, Konsol bilan ishlash imkoniyatini oshiradi(rang berish, tozalash)
#include <ctime>
#include <string.h>
#include <fstream>
#include <cmath>
using namespace std;

///Keling, konsolda olgan bilimlarimizga tayanib kichikroq o'yin tuzamiz :)
///Ushbu boshog'riqni o'qigingiz kelmasa, shunchaki o'tkazib yuboring

/**
Diqqat! Kodda izoh qoldirilmagan mavhum joylari mavjud!
*/
HANDLE handle=GetStdHandle(STD_OUTPUT_HANDLE);
string num="1";
void color(int n){SetConsoleTextAttribute(handle, n);}///Rangni almashtirish
/**
12-qizil
10-yashil
14-sariq
11-havorang

ranglarning tartibini shunchaki siklga qo'yib ham bilib olsa bo'ladi:
for(int i=x; i<y; i++)
{
    color(i);
    cout<<i<<endl;
}
*/

/**
Ilon harakatini chiqarishimiz uchun har safar konsolni tozalab boshqattan
chop etishimizga to'g'ri keladi, devorlar esa o'zgarmaydi, lekin system("cls")
ishlatilsa, ularni yana chop etishimizga to'g'ri keladi
Protsessor Hz katta bo'lsa, bu bilinmaydi, lekin oddiy hollarda o'sha devorlar
o'chib-yozilaverish natijasida "pirpirab" kishini asabiga tegishi mumkin
Bu yerda biroz dangasaroq yo'l tutamiz: devor ichinigina o'chiramiz
setCursor(x,y) - karetka(ruchka) ni x,y bo'yicha suradi, va o'sha surilgan joydan
boshlab yozib ketish mumkin, bunda koordinatalar boshi konsolning yuqori chap burchagi
*/
void setCursor(int x, int y){
        COORD Position;
        Position.X=x;
        Position.Y=y;
        SetConsoleCursorPosition(handle, Position);
}

int game();///Asosiy o'yin jarayoni ketadigan funksiya
int main();///Biz sevgan o'sha funksiya
void saveIt();///Rekorlarni saqlash va nazorat qilishni o'z zimmasiga oladi
int toInt(string s);///Berilgan string ni int tipida qaytaradi, uni kodlaringizda ishlatishingiz mumkin
void printMap();///Devor ichini qayta chop etib turadi
void changeDirect(char c);///yo'nalishni nazorat qiladi
int Move(int x, int y);///yo'nalishga qarab Ilonni bir birlikka surib beradi
void generateFood();///Ilonchamiz och qolib ketmasligi haqida qayg'uradi
void showDetails(int stopped=0);///Tabloni o'ngdagi kichkina ramkaga chop etadi

/**Keling, endi o'zgaruvchilar bilan sizni tanishtiraY:
area - o'zida o'yin maydoni haqida ma'lumotlar saqlaydi:
0 - bo'sh katak
N - ilon tanasi(N<len)
len - Ilon boshi, shuningdek ilon uzunligini ham bildiradi
-2 - ovqat

xpos va ypos bizga ilon boshining koordinatalari haqida so'zlaydi

direction - yo'nalishni bildiruvchi o'zgaruvchi, o'zida bir qancha qiymatlar saqlaydi:
1 - o'ngga
2 - tepaga
3 - chapga
4 - pastga

isAlive - Ilonimiz tirikmi-yo'qmi tekshiradi
speed - o'z nomiga ko'ra tezlikni bildiradi
score - to'plagan ochkolarni hisoblovchi
*/
int area[17][28], len, xpos, ypos, direction, score, speed=200;
bool isAlive;
int toInt(string s)
{
    int n=0;
    for(int i=0; i<s.size(); i++)
    n+=pow(10.,s.size()-i-1)*(s[i]-48);
    return n;
}
void saveIt()
{///Rekordlar bilan shug'ullanadi
    ///Ushbu funksiya izohlanmaydi!
    if(ifstream ("records.txt")==NULL)
    {
        ofstream f("records.txt");
        f<<"Python 200\nCanYouBeatMe 150\nS1mple 100\nTheFourth 90\nBlaBlaBla 79\nMaster 60\nCoder 45\nReachMyLevel 30\nDontWorry 28\nNOOB 10\n";
    }
    ifstream file("records.txt");
    int scores[10],pos=0;
    string s,records;
    getline(file,s,'\0');
    file.close();
    records=s;
    for(int i=0; i<10; i++)
    {
        s.erase(0,s.find(' ')+1);
        scores[i]=toInt(s.substr(0,s.find('\n')));
        s.erase(0,s.find('\n')+1);
        pos=(score>scores[i]&&!pos)?i+1:pos;
    }
    system("cls");
    if(!pos) return;
    color(11);
    cout<<"Tabriklaymiz :) Rekordlar jadvalimizdan "<<pos<<" - o'rinni\
    oldingiz! Iltimos, ismingizni probellarsiz kiriting, keyinchalik\
    xatolik bo'lmasligi uchun:\n\n";
    color(10);
    string name;
    cin>>name;
    ofstream f("records.txt");
    for(int i=0; i<10; i++)
    {
        if(i==(pos-1)){f<<name<<" "<<score<<endl;continue;}
        else f<<records.substr(0,records.find('\n')+1);
        records.erase(0,records.find('\n')+1);
    }
    f.close();
    cout<<"\nMa'lumotlar muvaffaqqiyatli saqlandi!";
   char c=_getch();
   while(c!=13) c=_getch();
   main();
}
void showDetails(int s){
    ///o'ng - 16, chap - 17, tepa - 30, past - 31, strelkalarning ASCII dagi kodi
    setCursor(31, 6);
    cout<<"         ";///10
    color(15);
    setCursor(31, 6);
    cout<<score;
    setCursor(34,8);
    cout<<(char)30;
    setCursor(34,9);
    cout<<"w";
    setCursor(31,10);
    cout<<(char)17<<"a   d"<<(char)16;
    setCursor(34,11);
    cout<<"s";
    setCursor(34,12);
    cout<<(char)31;
    setCursor(32,14);
    color(11);
    cout<<"Holati:";
    setCursor(30,15);
    if(s)
    {
        color(12);
        cout<<"Qotirilgan";
    }else{
        color(10);
        cout<<"Faol      ";
    }
}

void printMap(){///doska ichini qayta yozib beradi
    for(int i=0; i<17;i++){
    setCursor(1, i+1);
    for(int j=0;j<28;j++){
        if(!area[i][j]) cout<<" ";
        if(area[i][j]==len){color(10);cout<<(char)1;}
        else if(area[i][j]>0){color(10);cout<<"o";}
        if(area[i][j]==-2){color(12);cout<<(char)3;}
        ///tepani optimal qilish mumkin
    }
    }
}

void changeDirect(char c){
    /**
    Bizning "c" o'zgaruvchimiz bosilgan knopka qiymatini o'zida saqlaydi
    va direction(yo'nalish) ni o'zgartirishga xizmat qiladi
    d - 100
    s - 115
    w - 119
    a - 97
    Eslatma: O'yin bosh harflarda boshqarishga moslashtirilmagan
    uni o'zingiz sozlab olishingiz mumkin!

    w, a, s, d va probeldan tashqari knopka bosilishi yo'nalishga halaqt bermaydi
    */
    switch(c){///case lar ichidagi shart ilon birdaniga qarama-qarshi yo'nalishda
        ///ketib qolmasligini ta'minlaydi
        case 100:
            if(direction!=3) direction=1;break;
        case 115:
            if(direction!=2) direction=4;break;
        case 119:
            if(direction!=4) direction=2;break;
        case 97:
            if(direction!=1) direction=3;break;
        case 32:///Agar probel( ) bosilsa, ilonni qotirish algoritmi ishga tushadi
            showDetails(1);
            char c=_getch();
            while(c!=32)
            {
                c=_getch();
            }
            showDetails();
            return;
    }
}

int Move(int x, int y){
    xpos+=x, ypos+=y;
    ///Agar keyingi katakda ilon tanasi yoki devor bo'lsa - GUMMM
    if(xpos<0||ypos<0||xpos>27||ypos>16||area[ypos][xpos]>0){isAlive=false; return 0xFF;}
    if(area[ypos][xpos]==-2){///Agar ovqat bo'lsa-chi? :P
        len++;
        score+=(380-speed)/75;
        generateFood();///Ovqat yeyildi, endi boshqasini chiqaramiz
        showDetails();
        ///Quyidagi algortmlar ilonni bir birlikka surish uchun ishlatiladi:
        for(int i=0; i<17; i++)
            for(int j=0; j<28; j++)
            if(area[i][j]>0) area[i][j]++;
    }
    area[ypos][xpos]=len;
    area[ypos][xpos]=len+1;
    for(int i=0; i<17; i++){
        for(int j=0; j<28; j++){
            if(area[i][j]>0) area[i][j]--;
        }
    }
}

int game(){
    color(14);
    system("cls");
    ///Quyidagi sikllarda o'yindagi devorlar tikkalanmoqda...
    for(int i=0; i<41;i++) cout<<"*";
    for(int i=0;i<17;i++){
        cout<<"\n*                            *          *";
        if(i==5){
            setCursor(32, i);
            color(11);
            cout<<"Ball:";
            color(14);
            setCursor(0, i+1);
        }
    }
    cout<<endl;
    for(int i=0;i<41;i++) cout<<"*";
    showDetails();
    while(isAlive){///Toki ilonimiz tirik ekan...
    printMap();
    if(kbhit()) changeDirect(_getch());///Agar biror knopka bosilsa, darhol yo'nalish aniqlansin
    _sleep(speed);///Konsolni ma'lum bir millisoniyaga "uxlatish" orqali
    ///ilonning tezligini nazorat qilamiz
    Move(1*(direction==1)-1*(direction==3), 1*(direction==4)-1*(direction==2));
    }
    ///isAlive=false, demak...
    color(12);
    setCursor(0, 24);
    cout<<"O'yin tugab qoldiyu xD"<<"\nBALINGIZ: "<<score<<"\nILON UZUNLIGI: "<<len<<" BIRLIK";
    char c=_getch();
    while(c!=13) c=_getch();
    saveIt();
    main();
}

void generateFood(){
    srand(time(0));
    int foodX=rand()%28, foodY=rand()%17;
    while(area[foodY][foodX]){///Agar ovqat uchun tanlangan koordinata bo'sh bo'lmasa
        foodX=rand()%28;///yana boshqattan tasodifiy tanlaymiz
        foodY=rand()%17;
    }
    area[foodY][foodX]=-2;
}

int main()
{
    isAlive=1;
    len=3, xpos=10, ypos=7, direction=1, score=0;
    saveIt();
    for(int i=0; i<17;i++){
        for(int j=0; j<28; j++){
    if(i==ypos&&j>7&&j<=xpos) area[i][j]=j-7;
    else area[i][j]=0;
}
}
    generateFood();
    string menyu="Asosiy menyu:\n\
    1. O'yinni boshlash\n\
    2. Tezlikni o'zgartirish\n\
    3. Rekordlar\n\
    4. O'yin haqida\n\
    5. Chiqish\n", pointer="=>";
    menu:system("title Asosiy menyu");
    system("cls");
    color(10);
    for(int i=0; i<34; i++) cout<<" ";
    ///quyida siz ko'rgan strelkani surish algoritmi
    cout<<(menyu.substr(0, menyu.find(num)-2)+pointer+menyu.substr(menyu.find(num)-1,1000));
    while(1){
        char c=_getch();
        num=num[0]+1*(c==115||c==83)-1*(c==119||c==87)-5*(num[0]==53&&(c==115||c==83))+5*(num[0]==49&&(c==119||c==87));
        if(c==115||c==119||c==83||c==87) goto menu;
        if(c==13) break;///Agar enter bosilsam tanlangan punktga kiramiz
    }
    switch(atoi(num.c_str())){case 1: game();break;///o'yin
        case 4:///info
        system("cls&title O'yin haqida");
        color(10);
        cout<<"O'yin dasturchisi: NONE\nVersiya: NULL\nKod yozilgan vaqt: Sessiyada xD\n";
        color(12);
        cout<<"Boshqa hech narsa topilmadi! :(";
        _getch();
        goto menu;break;
        case 3:///Rekordlar
        system("cls&title Rekordlar");
        cout<<"TOP 10:\n\n";
        if(ifstream ("records.txt")==NULL)
        {
            saveIt();
        }
        ifstream f("records.txt");
        string s;
        getline(f,s,'\0');
        for(int i=1; i<11; i++)
        {
            cout<<i<<". ";
            cout<<s.substr(0,s.find('\n')+1);
            s.erase(0,s.find('\n')+1);
        }
        f.close();
        _getch();
        goto menu;
    }
    ///case 5 va case 2 lar switchda qandaydir xatolik berdi, nomlar to'qnashuvi
    ///bilan bog'liq, shuning uchun ularni alohida if orqali yozdim
    if(num[0]==53)
    {
        system("cls&title Chiqish");
        cout<<"O'yindan chiqib ketmoqchimisiz?\n";
        bool n=0;
        while(1)
        {   setCursor(2,1);
            if(n)color(244);
            else color(12);
            cout<<"Real hayotga qaytish\t";
            if(!n) color(242);
            else color(10);
            cout<<"O'yinga qaytish";
            char ch=_getch();
            if(ch==100||ch==68||ch==97||ch==65)n=!n;
            if(n&&ch==13) exit(0);
            if(!n&&ch==13){color(10);goto menu;}
        }
    }
    if(num[0]==50)
    {
        system("cls&title Tezlikni o'zgartirish");
        color(11);
        cout<<"\n\nHozirgi tezlik: ";
        while(1)
        {
            setCursor(0,3);
            color(10);
            cout<<(char)17<<"\t"<<7-speed/50<<"\t"<<(char)16;
            char c=_getch();
            if(c==13) goto menu;
            if(c==100||c==68) speed-=50*(speed>50);
            if(c==65||c==97) speed+=50*(speed<300);
            setCursor(0,2);
        }
    }
    return 0;
}
/**
Agar kodni o'qimasdan, sunchaki o'tkazib yuborgan bo'lsangiz,
    baribir o'qib chiqishni maslahat beraman(lOl)

Agar algoritmiga tushungan bo'lsangiz, bundan faqat xursand bo'laman
tushunmagan joylaringizni guruhda muhokama qilishni unutmang!

Imloviy xatoliklar va kod betartib yozilgani uchun uzr...
*/
