#include <iostream>
#include <windows.h>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <fstream>
using namespace std;

//GLOBALNE VARIJABLE:

//Opcije igre, trajanje_igre u sekundama, razina max 14,pocetno kretanje
int trajanje_igre=15,razina=8,smjer_kretanja=2;//gornji lijevi kut {a,b}
int a=0,b=0;

//donji desni kut	{c,d}
int c=21,d=15;           	//duzine polja za kretanje zmije c-(a+1) (po x-osi) d-(b+1)(po y-osi)

bool kraj=false;
bool ispis=1;
bool alg_3=0;

//Prostor kretanja zmije (kutija)
char znak_okvir=char(178);

//Definicija zmije:    matricom
					//(1 ima zmije,0 nema zmije,3 okvir)
					//pocetne koordinata glave i repa(sredina polja)
					//znak za ispis tijela zmije)
					// pocetna duzina zmije
vector<vector<int> > zmija;    // [pos.Y][pos.X]
vector<int> zmija_red; //pomocni vektor (kolona, ne red)
int duzina_zmije=3;
int glava,rep;
char tijelo='o';

//Vektori: smjer (za smjer kretanja repa), krizanje (sadrzi koordinate na kojim je zmija skrenula)
vector<int> smjer;
vector<int> krizanje;

//definicija jabuke
char znak_jabuka=3;
int pos_jabuka;
int bro_jabuka=0;


//Deklaracije Koordinata konzole(pos) i ekrana konzole(screen)
HANDLE screen=GetStdHandle(STD_OUTPUT_HANDLE);					//pokazuje na konzolu
COORD pos;             											//struktura koordinata

void reset() // funkcija za vracanje varijabli na pocetno stanje
{
	smjer_kretanja=2; //pocetno kretanje
	//pocetna zmija
	glava=(c+a)/2*100+(d+b)/2;
	rep=((c+a)/2-duzina_zmije+1)*100+(d+b)/2;
	//brisemo vektore...
	smjer.clear();
	krizanje.clear();
	// ... i matricu
	zmija.clear();
	zmija_red.clear();
	//broj pojedenih jabuka 0
	bro_jabuka=0;
	//u slucaju da je zmija samu sebe pojela, kraj postavljamo na false
	kraj=false;
}

//FUNKCIJE:

//za kretanje glave zmije
void fglava();

//za kretanje repa zmije
void frep();

void skretanje();

//za ispis granica prostora za kretanje
void okvir();

//za ispis jabuke
void jabuka();

//provjera da li je zmija pojela jabuku
bool pojedena_jabuka();

//funkcija modua (a%b)
int mod(int a,int b);
// funkcija za alg. 1
bool skr_random();

//ALGORITMI:

void algoritam_1();        		//random kretanje
void algoritam_2();
void algoritam_3();

// MENI:

void izbor_algoritma();
void meni();
void poziv_algoritma(int n);
void algoritmiu1000 ();
void opcije();

int main()
{

	meni();
// ---------------------KRAJ-----------------------
	SetConsoleCursorPosition(screen,{0,d+2});
	cout << "       >>>DOVIDJENJA<<<    \n\n";
	return 0;
}

void fglava()
{
			//		1
			//	   	^
			//	0 <- -> 2
			//		v
			//		3
	SetConsoleTextAttribute(screen,14); 	//zuta boja
//Uzimanje smjera iz vektora smjera (posljednji element sadrzi taj podatak)
	int o=smjer.size();
	zmija[glava%100][glava/100]=1;
//Formiranje nove pozicije glave u zavisnosti od smjera
	if(smjer[o-1]==0)         		//lijevo
        glava-=100;
    else if(smjer[o-1]==1)			//gore
        glava--;
    else if(smjer[o-1]==2)			//desno
        glava+=100;
    else							//dole
        glava++;
    if(zmija[glava%100][glava/100]==3 or zmija[glava%100][glava/100]==1)
    	kraj=true;
	else
	{
//Uzimanje koordinata iz varijable glava,ispisivanje nove pozicije glave
    pos.X=glava/100;
    pos.Y=glava%100;
    if(ispis==1)
   	{
		SetConsoleCursorPosition(screen,pos);
    	Sleep(150-razina*10);
    	cout << tijelo;
	}
//Provjera da li je glava dostigla(pojela) jabuku (ako jest ne idemo u funkciju za brisanje repa - zmija se povecala)
    if(pojedena_jabuka()==0)
    	frep();
	}
	SetConsoleTextAttribute(screen,15); 
}

void frep()
{
	//		1
	//	   	^
	//	0 <- -> 2
	//		v
	//		3

//brisanje posljednjeg repa
    pos.X=rep/100;    			//X koordinata
    pos.Y=rep%100;				//Y koordinata
    if(ispis==1)
	{
    	SetConsoleCursorPosition(screen,pos);
   	 	cout <<" ";
	}
    zmija[rep%100][rep/100]=0;
//spremanje novog repa (u ovisnosti smjera kretanja repa - vektor "smjer"
  	if(smjer[0]==0)
        rep-=100;
    else if(smjer[0]==1)
        rep--;
    else if(smjer[0]==2)
        rep+=100;
    else
        rep++;
//slucaj da je rep stigao do skretanja
    if(rep==krizanje[0])
    {
    	//rep je trenutno dosao do provg skretanja zmije
    	if(krizanje.size()==1)
    		krizanje.push_back(0);
    	//zamjena skretanja brisanje trenutnog i pomjeranje ostalih u vektoru "krizanje"
        for(int i=0;i<krizanje.size()-1;i++)
            	krizanje[i]=krizanje[i+1];
        krizanje.resize(krizanje.size()-1);
        //zamjena smjerova u vektoru "smjer"
        for (int i=0;i<smjer.size()-1;i++)
        	smjer[i]=smjer[i+1];
        smjer.resize(smjer.size()-1);
	}
}

void skretanje()
{
	// spremanje skretanje u vektor "krizanje", i spremanje smjera kretanja repa nakon skretanja u vektor "smjer"
	if(krizanje[0]==0)			//slucaj: zmija prvi put skrenula (ili rep prosao sva krizanja)
    {
        krizanje[0]=glava;
        smjer.push_back(smjer_kretanja);
    }

	else						//u ostalim slucajevima
    {
        krizanje.push_back(glava);
		smjer.push_back(smjer_kretanja);
    }
}

void jabuka()
{
	int y=rand()%(c-a-1)+a+1;
	int x=rand()%(d-b-1)+b+1;
	while(zmija[x][y]!=0)
	{

		y=rand()%(c-a-1)+a+1;
		x=rand()%(d-b-1)+b+1;
	}
	pos_jabuka=y*100+x;
	if(ispis==1)
	{
		SetConsoleCursorPosition(screen,{y,x});
		SetConsoleTextAttribute(screen,12);
		cout << znak_jabuka;
	}
}

bool pojedena_jabuka()
{
	if(glava==pos_jabuka)
	{
		jabuka();
		bro_jabuka++;
		alg_3=1;
		return true;
	}
	return false;
}

bool skr_random()
{
	int x=glava/100, y=glava%100;
	if(smjer_kretanja==0)         		//lijevo
        x--;
    else if(smjer_kretanja==1)			//gore
        y--;
    else if(smjer_kretanja==2)			//desno
    	x++;
    else								//dole
        y++;
	if(zmija[y][x]!=0)
		return true;
	return false;
}
void okvir()
{
	for(int j=0;j<=c;j++)
		zmija_red.push_back(0);
	for(int i=0;i<=d;i++)	
		zmija.push_back(zmija_red);


	int x,y;	
	SetConsoleTextAttribute(screen,15);
	
//gornji okvir
	pos={a,b};
	if(ispis==1)
		SetConsoleCursorPosition(screen,pos);
	for(int j=a;j<c;j++)
	{
		x=pos.X;
		y=pos.Y;
		zmija[y][x]=3;                 //postavljamo okvir na vrijednost razlicitu od 0
		if(ispis==1)
			cout << znak_okvir;
		pos.X++;
	}
//lijevi okvir
	pos={a,b};
	for(int j=b;j<d;j++)
	{
		if(ispis==1)
			SetConsoleCursorPosition(screen,pos);
		x=pos.X;
		y=pos.Y;
		zmija[y][x]=3;
		if(ispis==1)
			cout << znak_okvir;
		pos.Y++;
	}
//donji okvir
	pos={a,d};
	if(ispis==1)											//(a,b)
		SetConsoleCursorPosition(screen,pos);               // O#################
	for(int j=a;j<c;j++)									// #		->X		#
	{														// # 				#
		x=pos.X;											// # |				#
		y=pos.Y;											// # Y   			#
		zmija[y][x]=3;										// #				#
		if(ispis==1)										// #				#
			cout << znak_okvir;								// #				#
		pos.X++;											// #				#
															// #################O (c,d)
	}
//desni okvir
	pos={c,b};
	for(int j=b;j<=d;j++)
	{
		if(ispis==1)
			SetConsoleCursorPosition(screen,pos);
		x=pos.X;
		y=pos.Y;
		zmija[y][x]=3;
		if(ispis==1)
			cout << znak_okvir;
		pos.Y++;
	}
}

int mod(int a,int b)
{
	int m=a%b;
	if (m<0)
		m=m+b;
	return m;
}

//ALGORITMI:

void algoritam_1()			//RANDOM
{
//	Random kretanje jednom u tri puta (vjerojatnoca 1/3)
		int p=1,s;
		p=rand()%4;
		if( skr_random()==1)
			p=1;
		if(p==1)
		{
			s=rand()%2;

			if(s==0)
				smjer_kretanja=mod(smjer_kretanja+1,4);
			else if(s==1)
				smjer_kretanja=mod(smjer_kretanja-1,4);
			if(skr_random()==1)
				smjer_kretanja=(smjer_kretanja+2)%4;
			skretanje();
		}
}


void algoritam_2()
{
	int pom=smjer_kretanja;
	if(glava/100==c-1 and smjer_kretanja==2 )  //zmija dolazi do desnog ruba skrece gore
	{
		smjer_kretanja=1;
		skretanje();
	}
	else if(glava/100==a+1 and glava%100==d-2)   //donji lijevi kut zmija skrece desno
		{
			smjer_kretanja=3;
			skretanje();
			fglava();
			smjer_kretanja=2;
			skretanje();
		}
//kretanje gore,dole , iskljucuje slucaj kada zmija dode do donjeg lijevog kuta
	else if((glava%100==d-2 and glava/100!=c-1) or glava%100==b+1)
	{
		smjer_kretanja=0;
		skretanje();
		fglava();
		smjer_kretanja=mod(pom+2,4);
		skretanje();
	}
}
void algoritam_3() 
{
	int pom=smjer_kretanja;	
	if((glava/100>pos_jabuka/100 and smjer_kretanja==2) or (glava/100<pos_jabuka/100 and smjer_kretanja==0))
	{
		smjer_kretanja++;
		if(skr_random()==1)
			smjer_kretanja=mod(smjer_kretanja-2,4);
		while(skr_random()==1)
		{
			smjer_kretanja=pom;
			fglava();
			smjer_kretanja++;
			if(skr_random()==1)
				smjer_kretanja=mod(smjer_kretanja-2,4);
		}
		fglava();
		skretanje();
		int pom1=smjer_kretanja;
		smjer_kretanja=(pom+2)%4;
		while(skr_random()==1)
		{
			smjer_kretanja=pom1;
			fglava();
			smjer_kretanja=(pom+2)%4;
		}
		skretanje();
	}
	if(glava/100==pos_jabuka/100)
	{
		pom=smjer_kretanja;
		if(glava%100<pos_jabuka%100)
			smjer_kretanja=3;
		else
			smjer_kretanja=1;	
		skretanje();
		fglava();
	}
	if(alg_3==1 and smjer_kretanja%2==1)
	{
		if(glava/100<pos_jabuka/100)
			smjer_kretanja=2;
		if(glava/100>pos_jabuka/100)
			smjer_kretanja=0;	
		if(skr_random()==1)
			smjer_kretanja=mod(smjer_kretanja-2,4);
		skretanje();
		alg_3=0;
	}
}


// MENI
void meni ()
{
    int odabir = 0;
    while(odabir!=4)
	{
		system("cls");
		SetConsoleTextAttribute(screen,15);
		cout << "DOBRODOSLI U IGRU SNAKE! \n\n\n";
   		cout << "1. ALGORITMI\n2. ALGORITMI U 1000 PONAVLJANJA\n3. OPCIJE\n4. IZLAZ\n\n";
   		cout << "ZA ZELJENI ODABIR PRITISNITE BROJ 1-4: ";
	    cin >> odabir;
	    if (odabir == 1)
	    {
	       	izbor_algoritma();
	    }
	    else if(odabir == 2)
	    {
	       	algoritmiu1000();
	    }
	    else if(odabir == 3)
	        opcije();
    }
}

void izbor_algoritma()
{
	SetConsoleTextAttribute(screen,15);
    int al=0;
    while (al!=4)
    {
        system("cls");
        cout << "ODABERITE ALGORITAM! \n1. ALGORITAM 1 \n2. ALGORITAM 2 \n3. ALGORITAM 3 \n4. IZLAZ \n(UNESITE PRITISKOM NA BROJ):  ";
        cin >> al;
        if(al==1 or al==2 or al==3)
       		poziv_algoritma(al);
    }
}

void poziv_algoritma(int n)
{
	reset();
	if(ispis==1)
		system ("cls");
	smjer.push_back(smjer_kretanja);		//pocetno kretanje repa (udesno)
	krizanje.push_back(0);					//koordinata {0,0} (0=0*100+0) kao prva koordinata promjene smjera



//Deklarisanje vremena:
	clock_t vrijeme;
	vrijeme=clock();
	srand(time(0));

//POCETAK IGRE:
	okvir();								//ispis granica

//ispis pocetne zmije:
	if(ispis==1)
		SetConsoleCursorPosition(screen,{rep/100,rep%100});
	SetConsoleTextAttribute(screen,14);		// boja zmije zuta
	int pom=rep;
	while(pom!=glava+100)
	{
		if(ispis==1)
			cout << tijelo;
		zmija[pom%100][pom/100]=1;
		pom+=100;
	}
	if(ispis==1)
		Sleep(1000);

	jabuka();

	pom=clock()/1000;

	while(clock()/1000-pom<trajanje_igre or ispis==0)
	{

//nacin kretanja
		if(n==1)
			algoritam_1();
		if(n==2)
			algoritam_2();
		if(n==3)
			algoritam_3();
//pokretanje:
		fglava();
//Slucaj da je zmija udarila u zid
		if(kraj==true)
			break;
		zmija[glava%100][glava/100]=1;
		if(ispis==1)
		{
			SetConsoleTextAttribute(screen,3);
			SetConsoleCursorPosition(screen,{c+2,0});
			cout << "PROTEKLO VRIJEME: " << clock()/1000-pom;
			SetConsoleTextAttribute(screen,15);

		}
	}
}

void algoritmiu1000()
{	
    int brojac[3]={0,0,0};
    ispis=0;
    system("cls");
    ofstream snejk;
// prolaz kroz algoritme
	for(int j=0;j<3;j++)
	{
		SetConsoleTextAttribute(screen,15);
	    for(int i=0; i<100; i++)
	    {
	        poziv_algoritma(j+1);
	        brojac[j] = brojac[j]+bro_jabuka;
	        SetConsoleCursorPosition(screen,{0,0});
	        cout << i;
	    }
	     system("cls");
	}
//spremanje rezultata u fajl
    snejk.open("Snejk.txt");
    
    	snejk << "Ukupan broj pojedenih jabuka u prvom algoritmu iznosi: " << brojac[0];
    	cout << "\nUkupan broj pojedenih jabuka u prvom algoritmu iznosi: " << brojac[0];
        snejk << "\n\n\n---------------------------------------------------------\n\n\n";
      
	    snejk << "Ukupan broj pojedenih jabuka u drugom algoritmu iznosi: " << brojac[1];
        cout << "\nUkupan broj pojedenih jabuka u drugom algoritmu iznosi: " << brojac[1];
        snejk << "\n\n\n---------------------------------------------------------\n\n\n";
      
	    snejk << "Ukupan broj pojedenih jabuka u trecem algoritmu iznosi: " << brojac[2];
        cout << "\nUkupan broj pojedenih jabuka u trecem algoritmu iznosi: " << brojac[2] << endl;
        snejk << "\n\n\n---------------------------------------------------------\n\n\n";
   
    snejk.close();
    system("pause");
	ispis=1;
}

void opcije()
{
	SetConsoleTextAttribute(screen,15);
	cout << "        PROMJENA OPCIJA!!         \n\n";
	int e=1;
	while(e!=6)
	{
		system("cls");
		cout << "        PROMJENA OPCIJA!!         \n\n";
		cout << "1. POCETNA DUZINA ZMIJE\n2. BRZINA ZMIJE\n3. TRAJANJE IGRE\n4. OKVIR\n5. SPREMI\\UZMI OPCIJE\n6. IZLAZ\n\n(UNESITE PRITISKOM NA BROJ) ";
		cin >> e;
		if(e==1)
		{
			int pom=0;
			int unos=0;
			while(unos!=2)
			{
				system("cls");
				cout << "1. PROMJENA DUZINE POCETNE ZMIJE\n     Trenutno: "<< duzina_zmije << "\n2. IZLAZ\n";
				cin >> unos;
				if(unos==1)
				{
					system("cls");
					cout << "\nUnesite broj za promjenu (max" << (c-(a+1))/2 << "): ";
					cin >> pom;
					if(pom>(c-(a+1))/2 or pom<=1)
					{
						cout << "\nNemoguc unos!!\n";
						Sleep(2000);
					}
					else
						duzina_zmije=pom;
				}
			}

		}
		else if(e==2)
		{
			int pom=0;
			int unos=0;
			while(unos!=2)
			{
				system("cls");
				cout << "1. PROMJENA BRZINE KRETANJA ZMIJE\n     Trenutna razina: "<< razina<< "\n2. IZLAZ\n";
				cin >> unos;
				if(unos==1)
				{
					system("cls");
					cout << "\nUnesite broj za promjenu (max" << 14 << ", 14 najbrze): ";
					cin >> pom;
					if(pom>15 or pom<=0)
					{
						cout << "\nNemoguc unos!!\n";
						Sleep(2000);
					}
					else
						razina=pom;
				}
			}
		}
		else if(e==3)
		{
			int pom=0;
			int unos=0;
			while(unos!=2)
			{
				system("cls");
				cout << "1. PROMJENA TRAJANJA IGRE U SIMULACIJI ALGORITAMA\n     Trenutno: "<< trajanje_igre<< " sekundi\n2. IZLAZ\n";
				cin >> unos;
				if(unos==1)
				{
					system("cls");
					cout << "\nUnesite broj za promjenu (u sekundama) ";
					cin >> pom;
					if(pom<=0)
					{
						cout << "\nNemoguc unos!!\n";
						Sleep(2000);
					}
					else
						trajanje_igre=pom;
				}
			}
		}
		else if(e==4)
		{
			int pom=0,pom1=0,pom2=0,pom3=0;
			int unos=0,greska;
			while(unos!=2)
			{
				greska=0;
				system("cls");
				cout << "1. PROMJENA POLJA\n    Trenutne dimenzije: "<< c-(a+1) << "x" << d-(b+1)
				   << "\n    Trenutna pozicija: (" << a <<","  << b << ")" << " (" << c <<","  << d << ")"
				   << "\n2. IZLAZ\n";
				cin >> unos;
				if(unos==1)
				{
					system("cls");
					cout << "\nUnesite poziciju gornjeg lijevog kuta (a,b): ";
					cin >> pom >> pom1;
					if(pom<0 or pom1<0 or pom>80 or pom1>80)
					{
						cout << "\nNemoguc unos!!\n";
						Sleep(2000);
						greska++;
					}
					cout << "\nUnesite poziciju donjeg desnog kuta (c,d): ";
					cin >> pom2 >> pom3;
					if(pom2<0 or pom3<0 or pom2>60 or pom3>60)
					{
						cout << "\nNemoguc unos!!\n";
						Sleep(2000);
						greska++;
					}
					if((pom+pom2)%2==0 or pom>=pom2 or pom1>=pom3 or 2*(pom2-pom)<=duzina_zmije)
					{
						if((pom+pom2)%2==0)
							cout << "\nNemoguc unos, suma koordinata x u obje pozicije ne smije biti parna !!\n";
						else if(pom>=pom2)
							cout << "\nNemoguc unos, prva koordinata x veca od druge\n";
						else if(pom1>=pom3)
							cout << "\nNemoguc unos, prva koordinata y veca od druge\n";
						else
							cout << "\nNemoguc unos, duzina x ose premala\n";
						Sleep(3000);
						greska++;
					}
					if(greska==0)
					{
						int r=0;
						int a1=a;
						int b1=b;
						int c1=c;
						int d1=d;
						a=pom;
						b=pom1;
						c=pom2;
						d=pom3;
						system("cls");
						okvir();
						cout << "IZGLED OKVIRA\n";
						Sleep(3000);
						system("cls");
						cout << "Zelite li izvrsiti promjenu: \n1. DA\n2. NE \n";
						cin >> r;
						if(r==2)
						{
							a=a1;
							b=b1;
							c=c1;
							d=d1;
						}
					}
				}
			}
		}
		if(e==5)
		{
			ofstream spremi_opcije;
			ifstream uzmi_opcije;
			int pom=0;
			while(pom!=3)
			{
				system("cls");
				cout << "1. SPREMI TRENUTNE OPCIJE\n2. UCITAJ PRETHODNO SPREMLJENE OPCIJE\n3. IZLAZ\n";
				cin >> pom;
				if(pom==1)
				{
					system("cls");
					spremi_opcije.open("opcije.txt");
					spremi_opcije << a << " " << b << " " << c << " "  << d << " " << razina << " ";
					spremi_opcije << trajanje_igre << " " << duzina_zmije;
					spremi_opcije.close();
					cout << "\nUSPJESNO SPREMLJENE OPCIJE!\n";
					Sleep(3000);
				}
				if(pom==2)
				{
					system("cls");
					uzmi_opcije.open("opcije.txt");
					if(!uzmi_opcije.fail())
					{
						uzmi_opcije >> a  >> b  >> c >> d >> razina >> trajanje_igre >> duzina_zmije;
						uzmi_opcije.close();
						cout << "\nUSPJESNO PREUZETE SPREMLJENE OPCIJE!\n";
					}
					else
						cout << "\nNEMA SPREMLJENIH OPCIJA!\n";
					Sleep(3000);
				}
			}
		}
	}
}
