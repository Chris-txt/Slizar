#define IMM2D_WIDTH 150
#define IMM2D_HEIGHT 100
#define IMM2D_SCALE 7

#define IMM2D_IMPLEMENTATION
#include "immediate2d.h"
#include <string>
using namespace std;

#define NUM_ATTACCHI 12
#define NUM_RAGGI 3

static const Color DarkRed = MakeColor(88, 0, 0);
void DrawTitle(int b);
void DrawBackground(int blue);
void DrawScene(int enemyY, int enemyEalth, bool PlayerTurn, int playerEalth, int monetaOggetti, Image nemico);
int enemyAnimation(int enemyY, int& direction);
int qualeAzione(int mX, int mY);
void DrawAttack(int enemyY, int Danni);
int setRightAmountOfMoney(int SoldiOggetti);
void DrawAzione(int b, int& playerEalth, int& enemyEalth, int& SoldiOggetti, bool& PlayerTurn);
bool DrawSpecial(int b, int enemyEalth);
void Movement(int& PlayerX, int& PlayerY);
bool haPresoDanno(int AttackX[], int AttackY, int PlayerX, int PlayerY, int i, int raggioX[], bool SuperForma);
void DrawSuperFormCutscene();
void DrawDeath(bool isEnemyDead, int b);

static constexpr const char nemicopng[] = "Immagini/nemico.png";
static constexpr const char iconanemico[] = "Immagini/miniNemico.png";
static constexpr const char mortenemico[] = "Immagini/nemicoMorto.png";
static constexpr const char supernemico[] = "Immagini/nemicoFinale.png";
static constexpr const char pericolo[] = "Immagini/pericolo.png";
static constexpr const char barraspeciale[] = "Immagini/barraSpeciale.png";
static constexpr const char raggionemico[] = "Immagini/raggio.png";
static constexpr const char attacconemico[] = "Immagini/attaccoNemico.png";
static constexpr const char attaccosuper[] = "Immagini/attaccoSuper.png";
static constexpr const char schermotitolo[] = "Immagini/TitleScreen.png";

void run() {
	UseDoubleBuffering(true);
	int b = RandomInt(0, 255);								//colore blu per lo sfondo
	int enemyY = 0;											//posizione verticale del nemico, serve per animazioni
	int Direction = 1;										//La direzione serve per decidere se il nemico si muove su o giù
	bool PlayerTurn = true;									//vede se è il turno del giocatore
	int enemyEalth = 100, playerEalth = 100;				//vita del nemico e del giocatore
	bool statoDelMouse = false;								//prevede che comandi collegati al mouse vengano ripetuti troppo velocemente
	int tempo;												//tempo del turno nemico
	int SoldiOggetti = 0;
	bool superFormaAttiva = false;							//indicatore per la forma finale del nemico attova, serve per non ripetere la cutscene
	Image nemico = LoadImage(nemicopng);					//carica immagine del nemico
	const Image raggio = LoadImage(raggionemico);
	const Image warning = LoadImage(pericolo);
	Image enemyAttack = LoadImage(attacconemico);

	DrawTitle(b);

	//battaglia
	while (enemyEalth > 0 && playerEalth > 0) {
		tempo = 350;												//La durata del turno nemico, non so in che misura però
		int PlayerX = 75, PlayerY = 75;								//posizione del giocatore
		int PreviousX = PlayerX, PreviousY = PlayerY;				//servono per evitare che il giocatore esca dal campo
		int AttackY = 30, AttackX[NUM_ATTACCHI];					//La posizione degli attacchi nemici
		int WarningX[NUM_RAGGI], raggioX[NUM_RAGGI];				//attacco speciale della nemico nella forma finale
		bool PlayerColpito = false;									//fa in modo che il player abbia un pò di tempo per riprendersi dopo aver preso danno
		int iframe = 0;												//periodo di immortalità dopo aver preso danno
		int Danni;

		for (int i = 0; i < NUM_ATTACCHI; i++) {
			AttackX[i] = RandomInt(1, IMM2D_WIDTH);
		}
		for (int i = 0;i < NUM_RAGGI;i++) {
			WarningX[i] = RandomInt(10, 40);
			raggioX[i] = WarningX[i];
		}

		while (!PlayerTurn) {

			//Disegni:
			DrawBackground(b);
			DrawScene(enemyY, enemyEalth, PlayerTurn, playerEalth, SoldiOggetti, nemico);

			//movimento del Player
			Movement(PlayerX, PlayerY);
			//Hitbox
			if (PlayerX > 21 && PlayerX < 129) {
				PreviousX = PlayerX;
			}
			else {
				PlayerX = PreviousX;
			}
			if (PlayerY > 51 && PlayerY < 79) {
				PreviousY = PlayerY;
			}
			else {
				PlayerY = PreviousY;
			}

			//attacchi:
			for (int i = 0; i < NUM_ATTACCHI; i++) {
				//DrawRectangle(AttackX[i], AttackY, 5, 5, White, White);
				DrawImage(AttackX[i], AttackY, enemyAttack);
				if (PlayerColpito == false) {
					if (haPresoDanno(AttackX, AttackY, PlayerX, PlayerY, i, raggioX, superFormaAttiva)) {
						playerEalth -= 10;
						PlayerColpito = true;
						PlayMusic(32, 200);
						iframe = 30;
					}
				}
			}
			if (enemyEalth <= 50) {
				for (int i = 0;i < NUM_RAGGI;i++) {
					DrawImage(raggioX[i] - 7, 0, raggio);
					DrawImage(WarningX [i] - 3, 70, warning);
				}
			}
			if (AttackY > 85) {
				for (int i = 0; i < NUM_ATTACCHI; i++) {		//genera posizioni di nuovi attacchi
					AttackX[i] = RandomInt(1, IMM2D_WIDTH);
				}
				AttackY = 20;
				for (int i = 0;i < NUM_RAGGI;i++) {				//genera posizioni di nuovi raggi
					raggioX[i] = WarningX[i];
					WarningX[i] = RandomInt(25, 125);
				}
				PlayMusic(53, 100);
			}
			
			//Icona player:
			DrawCircle(PlayerX, PlayerY, 2, Green, LightGreen);
			Present();
			
			AttackY++;
			tempo--;
			iframe--;
			if (tempo == 0 || playerEalth <= 0) {
				PlayerTurn = true;
			}
			if (iframe == 0) {
				PlayerColpito = false;
			}
			//Tocchi Finali:
			Wait(20);
			Clear();
		}

		SoldiOggetti = setRightAmountOfMoney(SoldiOggetti);

		while (PlayerTurn && playerEalth > 0) {
			//Disegni:
			DrawBackground(b);
			DrawScene(enemyY, enemyEalth, PlayerTurn, playerEalth, SoldiOggetti, nemico);

			//Animazioni:
			enemyY += enemyAnimation(enemyY, Direction);
			Present();

			//Azioni:
			if (LeftMousePressed()) {
				int mX = MouseX();
				int mY = MouseY();
				if (statoDelMouse == false) {
					//Attacco:
					if (qualeAzione(mX, mY) == 1) {
						if (superFormaAttiva) {
							Danni = RandomInt(5, 10);
						}
						else {
							Danni = RandomInt(10, 15);
						}
						DrawAttack(enemyY, Danni);
						PlayMusic(63, 200);
						PlayMusic(63, 100);
						Present();
						Wait(500);
						enemyEalth -= Danni;
						PlayerTurn = false;
						SoldiOggetti += 10;
					}
					else if (qualeAzione(mX, mY) == 2) {
						Clear();
						DrawAzione(b, playerEalth, enemyEalth, SoldiOggetti, PlayerTurn);
					}
					else if (qualeAzione(mX, mY) == 3) {
						bool FattoCentro = DrawSpecial(b, enemyEalth);
						DrawBackground(b);
						DrawScene(enemyY, enemyEalth, PlayerTurn, playerEalth, SoldiOggetti, nemico);
						if (FattoCentro) {
							Danni = RandomInt(15, 25);
							enemyEalth -= Danni;
							SoldiOggetti += 30;
							DrawString(92, 0, ":)", "Comic Sans MC", 13, Black, true);
							PlayMusic(56, 150);
							PlayMusic(65, 150);
							PlayMusic(70, 400);
						}
						else {
							Danni = RandomInt(0, 3);
							enemyEalth -= Danni;
							SoldiOggetti += RandomInt(0, 5);
							DrawString(90, 0, ":(", "Comic Sans MC", 15, Black, true);
							PlayMusic(37, 300);
							PlayMusic(32, 150);
						}
						DrawAttack(enemyY, Danni);
						Present();
						PlayerTurn = false;
						Wait(500);
					}
				}
				statoDelMouse = true;
			}
			else {
				statoDelMouse = false;
			}

			//Tocchi Finali:
			Wait(70);
			Clear();
		}

		SoldiOggetti = setRightAmountOfMoney(SoldiOggetti);
		if (superFormaAttiva == false && enemyEalth <= 50) {
			superFormaAttiva = true;
			nemico = LoadImage(supernemico);
			enemyAttack = LoadImage(attaccosuper);
			DrawSuperFormCutscene();
		}
	}
	
	if (enemyEalth <= 0) {
		DrawDeath(true, b);
	}
	else {
		DrawDeath(false, b);
	}
	CloseWindow();
}

void DrawTitle(int b)
{
	const Image iconaDelNemico = LoadImage(iconanemico);
	const Image SchermataTitolo = LoadImage(schermotitolo);
	while (LastKey() != Enter) {
		int r = RandomInt(0, 255);
		int g = RandomInt(0, 255);
		if (LastBufferedKey() == 'z') {
			DrawImage(0, 0, SchermataTitolo);
		}
		else {
			for (int i = 100;i > 0;i -= 10) {
				DrawCircle(75, 50, i, MakeColor(20, i, b), 0U);
			}
		}
		DrawString(75, 5, "Slizar", "Papyrus", 20, White, true);
		DrawRectangle(55, 45, 40, 40, Black, MakeColor(r, g, b));
		DrawImage(56, 46, iconaDelNemico);
		Present();
		Wait(150);
	}
	PlayMusic(58, 700);
	PlayMusic(52, 1100);
	for (int i = 0;i < 100;i += 10) {
		DrawCircle(75, 65, i, Black, 0U);
		Present();
		Wait(120);
	}
	Clear();
}

void DrawBackground(int blue)
{
	for (int i = 0; i < IMM2D_WIDTH; i += 5) {
		for (int j = 0; j < IMM2D_HEIGHT; j += 5) {
			DrawRectangle(i, j, 5, 5, MakeColor(i, j, blue), 0U);
		}
	}
}

void DrawScene(int enemyY, int enemyEalth, bool PlayerTurn, int playerEalth, int monetaOggetti, Image nemico)
{
	DrawImage(43, enemyY, nemico);							//disegna immagine del nemico
	DrawRectangle(24, 2, 101, 6, Red, LightGray);			//barra della vita persa nemica	
	DrawRectangle(25, 3, enemyEalth, 5, Green, 0U);			//barra della vita nemica
	DrawRectangle(25, 92, 100, 6, Red);						//barra della vita persa del giocatore
	DrawRectangle(25, 92, playerEalth, 6, Green);			//barra della vita del giocatore
	DrawRectangle(25, 88, 100, 3, DarkGray);				//barra dei soldi ottenibili
	DrawRectangle(25, 88, monetaOggetti, 3, LightCyan);		//barra dei soldi ottenuti
	if (PlayerTurn) {
		//Attacco
		DrawRectangle(5, 70, 45, 15, Red, LightRed);
		DrawString(5, 69, "Attacca", "Comic Sans MS", 8, LightRed);
		//Oggetti
		DrawRectangle(52, 70, 45, 15, Blue, LightBlue);
		DrawString(51, 69, "Oggetti", "Comic Sans MS", 8, LightBlue);
		//Mosse Speciali
		DrawRectangle(99, 70, 46, 15, Green, LightGreen);
		DrawString(98, 69, "Speciale", "Comic Sans MS", 8, LightGreen);
	}
	else {
		//Campo di Combattimento
		DrawRectangle(20, 50, 110, 30, Black, White);
	}
}

int enemyAnimation(int enemyY, int& direction)
{
	if (enemyY < -5) {
		direction = 1;
	}
	if (enemyY > 15) {
		direction = -1;
	}
	return direction;
}

int qualeAzione(int mX, int mY)
{
	if ((mX >= 5 && mX <= 50)&&(mY >= 70 && mY <= 85)) {
		return 1;
	}
	if ((mX >= 52 && mX <= 97) && (mY >= 70 && mY <= 85)) {
		return 2;
	}
	if ((mX >= 99 && mX <= 145) && (mY >= 70 && mY <= 85)) {
		return 3;
	}
	return 0;
}

void DrawAttack(int enemyY, int Danni)
{
	string DanniStr = to_string(Danni);
	for (int i = 0; i < 41; i++) {
		DrawPixel(52 + i, enemyY + i + 5, DarkRed);
		DrawPixel(53 + i, enemyY + i + 5, DarkRed);
		Present();
		Wait(7);
	}
	DrawString(75, 0, DanniStr.c_str(), "Comic Sans MC", 15, Black, true);
}

int setRightAmountOfMoney(int SoldiOggetti)
{
	if (SoldiOggetti > 100) {
		return 100;
	}
	return SoldiOggetti;
}

void DrawAzione(int b, int& playerEalth, int& enemyEalth, int& SoldiOggetti, bool& PlayerTurn)
{
	//box
	string oggetti[3] = { "(1) Bomba","(2) Cura","(3) Super Cura" };
	string costo[3] = { "10","25","40" };
	DrawBackground(b);
	DrawRectangle(10, 5, 130, 80, Black, White);						//box
	DrawString(10, 72, "Premere ESC per uscire", "", 7, White);			//istruzione per uscire
	DrawRectangle(25, 92, 100, 6, Red);									//barra della vita persa del giocatore
	DrawRectangle(25, 92, playerEalth, 6, Green);						//barra della vita del giocatore
	DrawRectangle(25, 88, 100, 3, DarkGray);							//barra dei soldi ottenibili
	DrawRectangle(25, 88, SoldiOggetti, 3, LightCyan);					//barra dei soldi ottenuti
	
	int j = 5;
	for (int i = 0;i < 3;i++) {
		DrawString(10, j, oggetti[i].c_str(), "Comic Sans MC", 10, LightGreen);
		DrawString(120, j, costo[i].c_str(), "Comic Sans MC", 10, LightGreen);
		j += 20;
	}

	Present();
	char ultimo = LastKey();
	while (ultimo != 27 && PlayerTurn) {
		ultimo = LastKey();
		if (ultimo == '1' && SoldiOggetti >= 10) {
			SoldiOggetti -= 10;
			playerEalth -= 5;
			enemyEalth -= 20;
			PlayerTurn = false;
		}
		if (ultimo == '2' && SoldiOggetti >= 15) {
			SoldiOggetti -= 15;
			playerEalth += 15;
			PlayerTurn = false;
		}
		if (ultimo == '3' && SoldiOggetti >= 35) {
			SoldiOggetti -= 35;
			playerEalth += 40;
			PlayerTurn = false;
		}
		//DEBUG!! solo io posso sapere di questo comando
		if (ultimo == 'z') {
			enemyEalth -= 50;
			PlayerTurn = false;
		}
	}
}

bool DrawSpecial(int b, int enemyEalth)
{
	int barraX = 10;
	bool versoDestra = true;
	const Image barraMinigioco = LoadImage(barraspeciale);
	while (LastKey() != 32) {
		DrawBackground(b);
		DrawImage(10, 40, barraMinigioco);
		DrawString(5, 15, "Premi spazio per confermare", "Comic Sans MC", 8, Black);
		if (versoDestra) {
			barraX += 8;
		}
		else {
			barraX -= 8;
		}
		DrawLine(barraX, 35, barraX, 95, 3, Black);
		if (barraX >= 120) {
			versoDestra = false;
		}
		if (barraX <= 10) {
			versoDestra = true;
		}
		Present();
		Clear();
		Wait(40);
	}

	if (barraX > 100 && barraX < 111) {
		return true;
	}
	return false;
}

void Movement(int& PlayerX, int& PlayerY)
{
	char Movement = LastKey();
	if (Movement == Up) {
		PlayerY -= 2;
	}
	if (Movement == Down) {
		PlayerY += 2;
	}
	if (Movement == Right) {
		PlayerX += 2;
	}
	if (Movement == Left) {
		PlayerX -= 2;
	}
}

bool haPresoDanno(int AttackX[], int AttackY, int PlayerX, int PlayerY, int i, int raggioX[], bool SuperForma)
{
	if (PlayerY<AttackY + 4 && PlayerY>AttackY - 5) {
		if (PlayerX > AttackX[i] - 3 && PlayerX < AttackX[i] + 7) {
			return true;
		}
	}
	if (SuperForma && i < NUM_ATTACCHI) {
		if (PlayerX > raggioX[i] - 8 && PlayerX < raggioX[i] + 12) {
			return true;
		}
	}
	return false;
}

void DrawSuperFormCutscene()
{
	const Image SiluetteNemico = LoadImage(mortenemico);
	Clear(DarkRed);
	DrawImage(44, 18, SiluetteNemico);
	Present();
	Wait(1000);
	for (int i = 90;i > 0;i -= 10) {
		Clear(DarkRed);
		DrawCircle(75, 50, i, MakeColor(255, 255 - i, 0), 0U);
		DrawImage(44, 18, SiluetteNemico);
		Present();
		PlayMusic(75, 70);
		Wait(500);
	}
	PlayMusic(1500, 10);
}

void DrawDeath(bool isEnemyDead, int b)
{
	const Image NemicoUcciso = LoadImage(mortenemico);
	if (isEnemyDead) {
		Clear(Red);
		DrawImage(43, 17, NemicoUcciso);
		Present();
		PlayMusic(45, 2000);
		Wait(2000);
		DrawLine(55, 20, 90, 80, 9, Red);
		Present();
		PlayMusic(30, 3000);
		Wait(3000);
		Clear();
		DrawBackground(b);
		DrawString(75, 25, "Vittoria", "Comic Sans MC", 20, Green, true);
		PlayMusic(56, 300);
		PlayMusic(67, 300);
		PlayMusic(67, 200);
		PlayMusic(80, 100);
	}
	else {
		DrawBackground(b);
		DrawString(75, 25, "Game Over", "Comic Sans MC", 20, Red, true);
		PlayMusic(45, 2000);
		PlayMusic(30, 3000);
	}
	DrawString(75, 60, "Premi ENTER per chiudere", "Comic Sans MC", 8, White, true);
	Present();
	while (LastKey() != Enter) {}
}
