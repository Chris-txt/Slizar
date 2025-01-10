#define IMM2D_WIDTH 150
#define IMM2D_HEIGHT 100
#define IMM2D_SCALE 6

#define IMM2D_IMPLEMENTATION
#include "immediate2d.h"

#define NUM_ATTACCHI 12
#define NUM_OGGETTI 3

static const Color DarkRed = MakeColor(88, 0, 0);
void DrawTitle(int b);
void DrawBackground(int blue);
void DrawScene(int enemyY, int enemyEalth, bool PlayerTurn, int playerEalth, int monetaOggetti, Image nemico);
void enemyAnimation(int& enemyY, bool& direction);
int qualeAzione(int mX, int mY);
void DrawAttack(int enemyY);
void DrawAzione(int NumOggetti[], int b, int& playerEalth, int& enemyEalth, int& SoldiOggetti, bool& PlayerTurn);
bool DrawSpecial(int b, int enemyEalth);
void Movement(int& PlayerX, int& PlayerY);
bool haPresoDanno(int AttackX[], int AttackY, int PlayerX, int PlayerY, int i, int raggioX, bool SuperForma);
void DrawSuperFormCutscene();
void DrawDeath(bool isEnemyDead, int b);

static constexpr const char nemicopng[] = "nemico.png";
static constexpr const char iconanemico[] = "miniNemico.png";
static constexpr const char mortenemico[] = "nemicoMorto.png";
static constexpr const char supernemico[] = "nemicoFinale.png";
static constexpr const char pericolo[] = "pericolo.png";

void run() {
	UseDoubleBuffering(true);
	int b = RandomInt(0, 255);								//colore sfondo
	int enemyY = 0;											//posizione verticale del nemico, serve per animazioni
	bool movingUP = true;									//movingUP serve per decidere se il nemico si muove su o giù
	bool PlayerTurn = true;									//vede se è il turno del giocatore
	int enemyEalth = 100, playerEalth = 100;				//vita del nemico e del giocatore
	bool statoDelMouse = false, PlayerColpito = false;		//prevede che alcuni comandi collegati al mouse e al player vengano ripetute troppo velocemente
	int tempo;												//tempo del turno nemico
	int NumPerOggetto[NUM_OGGETTI] = { 2,7,1 };				//La quantità disponibile per ogni oggetto
	int SoldiOggetti = 0;
	bool superFormaAttiva = false;							//indicatore per la forma finale del nemico attova, serve per non ripetere la cutscene
	Image nemico = LoadImage(nemicopng);					//carica immagine del nemico
	const Image warning = LoadImage(pericolo);

	DrawTitle(b);

	//battaglia
	while (enemyEalth > 0 && playerEalth > 0) {
		tempo = 350;											//La durata del turno nemico, non so in che misura però
		int PlayerX = 75, PlayerY = 75;							//posizione del giocatore
		int PreviousX = PlayerX, PreviousY = PlayerY;			//servono per evitare che il giocatore esca dal campo
		int AttackY = 30, AttackX[NUM_ATTACCHI];				//La posizione degli attacchi nemici
		int WarningX = RandomInt(25, 40), raggioX = WarningX;	//attacco speciale della nemico nella forma finale
		for (int i = 0; i < NUM_ATTACCHI; i++) {
			AttackX[i] = RandomInt(1, IMM2D_WIDTH);
		}

		while (!PlayerTurn) {

			//Disegni:
			DrawBackground(b);
			DrawScene(enemyY, enemyEalth, PlayerTurn, playerEalth, SoldiOggetti, nemico);
			DrawCircle(PlayerX, PlayerY, 2, Green, LightGreen);

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
				DrawRectangle(AttackX[i], AttackY, 5, 5, White, White);
				if (enemyEalth <= 50) {
					DrawImage(WarningX, 70, warning);
					DrawLine(raggioX, 0, raggioX, 100, 15, Yellow);
					DrawLine(raggioX, 0, raggioX, 100, 5, White);
				}
				if (PlayerColpito == false) {
					if (haPresoDanno(AttackX, AttackY, PlayerX, PlayerY, i, raggioX, superFormaAttiva)) {
						playerEalth -= 10;
						PlayerColpito = true;
					}
				}
			}
			if (AttackY > 85) {
				for (int i = 0; i < NUM_ATTACCHI; i++) {
					AttackX[i] = RandomInt(1, IMM2D_WIDTH);
				}
				AttackY = 20;
				raggioX = WarningX;
				WarningX = RandomInt(25, 125);
			}
			
			Present();
			AttackY++;

			tempo--;
			if (tempo == 0 || playerEalth <= 0) {
				PlayerTurn = true;
			}
			if (tempo % 50 == 0) {
				PlayerColpito = false;
			}
			//Tocchi Finali:
			Wait(25);
			Clear();
		}

		while (PlayerTurn && playerEalth > 0) {
			//Disegni:
			DrawBackground(b);
			DrawScene(enemyY, enemyEalth, PlayerTurn, playerEalth, SoldiOggetti, nemico);

			//Animazioni:
			enemyAnimation(enemyY, movingUP);
			Present();

			//Azioni:
			if (LeftMousePressed()) {
				int mX = MouseX();
				int mY = MouseY();
				if (statoDelMouse == false) {
					//Attacco:
					if (qualeAzione(mX, mY) == 1) {
						DrawAttack(enemyY);
						DrawString(75, 0, "15", "Comic Sans MC", 15, Black, true);
						Present();
						Wait(500);
						enemyEalth -= 15;
						PlayerTurn = false;
						SoldiOggetti += 10;
					}
					else if (qualeAzione(mX, mY) == 2) {
						Clear();
						DrawAzione(NumPerOggetto, b, playerEalth, enemyEalth, SoldiOggetti, PlayerTurn);
					}
					else if (qualeAzione(mX, mY) == 3) {
						bool FattoCentro = DrawSpecial(b, enemyEalth);
						DrawBackground(b);
						DrawScene(enemyY, enemyEalth, PlayerTurn, playerEalth, SoldiOggetti, nemico);
						DrawAttack(enemyY);
						if (FattoCentro) {
							enemyEalth -= RandomInt(15, 30);
							SoldiOggetti += 30;
							DrawString(75, 0, "Bravo", "Comic Sans MC", 13, Black, true);
						}
						else {
							enemyEalth -= RandomInt(0, 3);
							SoldiOggetti += RandomInt(0, 5);
							DrawString(75, 0, ":(", "Comic Sans MC", 15, Black, true);
						}
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

		if (superFormaAttiva == false && enemyEalth <= 50) {
			superFormaAttiva = true;
			nemico = LoadImage(supernemico);
			DrawSuperFormCutscene();
		}
	}
	
	if (enemyEalth <= 0) {
		DrawDeath(true, b);
	}
	else {
		DrawDeath(false, b);
	}
	Present();
}

void DrawTitle(int b)
{
	for (int i = 100;i > 0;i -= 10) {
		DrawCircle(75, 50, i, MakeColor(20, i, b), 0U);
	}
	DrawString(75, 5, "Slizar", "Papyrus", 20, White, true);
	const Image iconaDelNemico = LoadImage(iconanemico);
	while (LastKey() != Enter) {
		int r = RandomInt(0, 255);
		int g = RandomInt(0, 255);
		DrawRectangle(55, 45, 40, 40, Black, MakeColor(r, g, b));
		DrawImage(56, 46, iconaDelNemico);
		Present();
		Wait(150);
	}
	for (int i = 0;i < 100;i += 10) {
		DrawCircle(75, 65, i, Black, 0U);
		Present();
		Wait(120);
	}
	Clear();
}

void DrawBackground(int blue)
{
	Color stu;
	for (int i = 0; i < IMM2D_WIDTH; i += 5) {
		for (int j = 0; j < IMM2D_HEIGHT; j += 5) {
			stu = MakeColor(i, j, blue);
			DrawRectangle(i, j, 5, 5, stu, 0U);
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

void enemyAnimation(int& enemyY, bool& direction)
{
	if (direction) {
		enemyY--;
		if (enemyY < -5) {
			direction = false;
		}
	}
	else {
		enemyY++;
		if (enemyY > 15) {
			direction = true;
		}
	}
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

void DrawAttack(int enemyY)
{
	for (int i = 0; i < 41; i++) {
		DrawPixel(52 + i, enemyY + i + 5, DarkRed);
		DrawPixel(53 + i, enemyY + i + 5, DarkRed);
		Present();
		Wait(7);
	}
}

void DrawAzione(int NumOggetti[], int b, int& playerEalth, int& enemyEalth, int& SoldiOggetti, bool& PlayerTurn)
{
	//box
	DrawBackground(b);
	DrawRectangle(10, 5, 130, 80, Black, White);						//box
	DrawString(10, 72, "Premere ESC per uscire", "", 7, White);			//istruzione per uscire
	DrawString(10, 60, "tenere premuto per selezionare", "", 7, White);	//istruzioni
	DrawRectangle(25, 92, 100, 6, Red);									//barra della vita persa del giocatore
	DrawRectangle(25, 92, playerEalth, 6, Green);						//barra della vita del giocatore
	DrawRectangle(25, 88, 100, 3, DarkGray);							//barra dei soldi ottenibili
	DrawRectangle(25, 88, SoldiOggetti, 3, LightCyan);					//barra dei soldi ottenuti
	//oggetto 1
	DrawString(10, 5, "(1) Bomba", "Comic Sans MC", 10, LightRed);
	DrawString(120, 5, "10", "", 10, LightRed);
	//oggetto 2
	DrawString(10, 22, "(2) Cura", "Comic Sans MC", 10, LightGreen);
	DrawString(120, 22, "25", "", 10, LightGreen);
	//oggetto 3
	DrawString(10, 38, "(3) Super Cura", "Comic Sans MC", 10, LightGreen);
	DrawString(120, 38, "40", "", 10, LightGreen);
	
	Present();
	while (LastKey() != 27 && PlayerTurn) {
		if (LastKey() == '1' && SoldiOggetti >= 10) {
			SoldiOggetti -= 10;
			playerEalth -= 5;
			enemyEalth -= 20;
			PlayerTurn = false;
		}
		if (LastKey() == '2' && SoldiOggetti >= 15) {
			SoldiOggetti -= 15;
			playerEalth += 10;
			PlayerTurn = false;
		}
		if (LastKey() == '3' && SoldiOggetti >= 40) {
			SoldiOggetti -= 10;
			playerEalth += 35;
			PlayerTurn = false;
		}
		//DEBUG!! solo io posso sapere di questo comando
		if (LastKey() == 'z') {
			enemyEalth /= 2;
		}
	}
}

bool DrawSpecial(int b, int enemyEalth)
{
	int barraX = 10;
	bool versoDestra = true;
	while (LastKey() != 32) {
		DrawBackground(b);
		DrawRectangle(24, 2, 101, 6, Red, LightGray);
		DrawRectangle(25, 3, enemyEalth, 5, Green, 0U);
		DrawRectangle(10, 40, 90, 50, Red, White);
		DrawRectangle(101, 40, 10, 50, Green, White);
		DrawRectangle(112, 40, 20, 50, Red, White);
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

bool haPresoDanno(int AttackX[], int AttackY, int PlayerX, int PlayerY, int i, int raggioX, bool SuperForma)
{
	if (PlayerY<AttackY + 4 && PlayerY>AttackY - 5) {
		if (PlayerX > AttackX[i] - 4 && PlayerX < AttackX[i] + 8) {
			return true;
		}
	}
	if (SuperForma) {
		if (PlayerX > raggioX - 11 && PlayerX < raggioX + 10) {
			return true;
		}
	}
	return false;
}

void DrawSuperFormCutscene()
{
	const Image SiluetteNemico = LoadImage(mortenemico);
	Clear(DarkRed);
	DrawImage(43, 17, SiluetteNemico);
	Present();
	Wait(2000);
	for (int i = 100;i > 0;i -= 10) {
		DrawCircle(75, 50, i, MakeColor(255, 255 - i, 0), 0U);
	}
	DrawImage(43, 17, SiluetteNemico);
	Present();
	Wait(2000);
	DrawLine(74, 42, 74, 55, 2, Red);
	Present();
	Wait(2000);
}

void DrawDeath(bool isEnemyDead, int b)
{
	const Image NemicoUcciso = LoadImage(mortenemico);
	if (isEnemyDead) {
		Clear(Red);
		DrawImage(43, 17, NemicoUcciso);
		Present();
		Wait(2000);
		DrawLine(55, 20, 90, 80, 9, Red);
		Present();
		Wait(3000);
		Clear();
		DrawBackground(b);
		DrawString(75, 25, "Vittoria", "Comic Sans MC", 20, Green, true);
	}
	else {
		DrawBackground(b);
		DrawString(75, 25, "Game Over", "Comic Sans MC", 20, Red, true);
	}
}
