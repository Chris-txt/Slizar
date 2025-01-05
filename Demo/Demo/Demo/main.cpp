#define IMM2D_WIDTH 150
#define IMM2D_HEIGHT 100
#define IMM2D_SCALE 5

#define IMM2D_IMPLEMENTATION
#include "immediate2d.h"

#define NUM_ATTACCHI 12
#define NUM_OGGETTI 3

void DrawBackground(int blue);
void DrawScene(int enemyY, int enemyEalth, bool PlayerTurn, int playerEalth);
void enemyAnimation(int& enemyY, bool& direction);
int qualeAzione(int mX, int mY);
void DrawAttack(int enemyY);
void DrawAzione(int NumOggetti[], int b);
void Movement(int& PlayerX, int& PlayerY);
bool haPresoDanno(int AttackX[], int AttackY, int PlayerX, int PlayerY, int i);

void run() {
	int b = RandomInt(0, 255);								//colore sfondo
	int enemyY = 40;										//posizione verticale del nemico, serve per animazioni
	bool movingUP = true;									//movingUP serve per decidere se il nemico si muove su o giù
	bool PlayerTurn = true;									//vede se è il turno del giocatore
	int enemyEalth = 100, playerEalth = 100;				//vita del nemico e del giocatore
	bool statoDelMouse = false, PlayerColpito = false;		//prevede che alcuni comandi collegati al mouse e al player vengano ripetute troppo velocemente
	int tempo;												//tempo del turno nemico
	int NumPerOggetto[NUM_OGGETTI] = { 2,7,1 };				//La quantità disponibile per ogni oggetto

	//battaglia
	while (enemyEalth > 0 && playerEalth > 0) {
		tempo = 350;										//La durata del turno nemico, non so in che misura però
		int PlayerX = 75, PlayerY = 75;						//posizione del giocatore
		int PreviousX = PlayerX, PreviousY = PlayerY;		//servono per evitare che il giocatore esca dal campo
		int AttackY = 30, AttackX[NUM_ATTACCHI];				//La posizione degli attacchi nemici
		for (int i = 0; i < NUM_ATTACCHI; i++) {
			AttackX[i] = RandomInt(1, IMM2D_WIDTH);
		}

		while (PlayerTurn == false) {
			UseDoubleBuffering(true);
			//Disegni:
			DrawBackground(b);
			DrawScene(enemyY, enemyEalth, PlayerTurn, playerEalth);
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
				if (PlayerColpito == false) {
					if (haPresoDanno(AttackX, AttackY, PlayerX, PlayerY, i)) {
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
			}
			
			Present();
			AttackY++;

			tempo--;
			if (tempo <= 0) {
				PlayerTurn = true;
			}
			if (tempo % 50 == 0) {
				PlayerColpito = false;
			}
			//Tocchi Finali:
			Wait(25);
			Clear();
		}
		while (PlayerTurn) {
			UseDoubleBuffering(true);
			//Disegni:
			DrawBackground(b);
			DrawScene(enemyY, enemyEalth, PlayerTurn, playerEalth);

			//Animazioni:
			enemyAnimation(enemyY, movingUP);
			Present();

			//Azioni:
			if (LeftMousePressed()) {
				int mX = MouseX();
				int mY = MouseY();
				if (statoDelMouse == false) {
					//Attacco:
					if (qualeAzione(mX, mY)==1) {
						UseDoubleBuffering(false);
						DrawAttack(enemyY);
						enemyEalth -= 15;
						PlayerTurn = false;
					}
					else if (qualeAzione(mX, mY) == 2) {
						DrawAzione(NumPerOggetto, b);
					}
				}
				statoDelMouse = true;
			}
			else {
				statoDelMouse = false;
			}

			//Tocchi Finali:
			Wait(120);
			Clear();
		}
	}
	DrawBackground(b);
	if (enemyEalth <= 0) {
		DrawString(75, 25, "You Won", "Comic Sans MC", 20, White, true);
	}
	else {
		DrawString(75, 25, "You Died", "Comic Sans MC", 20, White, true);
	}
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

void DrawScene(int enemyY, int enemyEalth, bool PlayerTurn, int playerEalth)
{
	//Nemico
	DrawCircle(75, enemyY, 20, Red, LightRed);				//nemico											  
	DrawCircle(75, enemyY, 7, White, LightGray);			//occhio del nemico (non che ci sono altri occhi)
	DrawLine(75, enemyY - 5, 75, enemyY + 5, 1, Black);		//pupilla del nemico
	DrawRectangle(24, 2, 101, 6, Red, LightGray);			//barra della vita persa nemica	
	DrawRectangle(25, 3, enemyEalth, 5, Green, 0U);			//barra della vita nemica
	DrawRectangle(25, 93, 100, 5, Red, Red);				//barra della vita persa del giocatore
	DrawRectangle(25, 93, playerEalth, 5, Green, Green);	//barra della vita del giocatore
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
		if (enemyY < 30) {
			direction = false;
		}
	}
	else {
		enemyY++;
		if (enemyY > 46) {
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
	return 0;
}

void DrawAttack(int enemyY)
{
	Color DarkRed = MakeColor(88, 0, 0);
	for (int i = 0; i < 41; i++) {
		//DrawLine(50, enemyY - 20, 51 + i, enemyY + i, 2, Black);
		DrawPixel(52 + i, enemyY - 20 + i, DarkRed);
		DrawPixel(52 + i, enemyY - 21 + i, DarkRed);
		Wait(5);
	}
	DrawString(75, 0, "15", "Comic Sans MC", 15, Black, true);
	Wait(500);
}

void DrawAzione(int NumOggetti[], int b)
{
	while (LastKey() != 27) {
		DrawRectangle(10, 5, 130, 80, Black, White);
		DrawString(7, 85, "Premere ESC per uscire", "", 7, Black);
		//oggetto 1
		DrawString(10, 7, "Bomba", "Papyrus", 10, White);
		DrawString(120, 7, "5", "", 10, White);
		//oggetto 2
		DrawString(10, 24, "Cura", "Papyrus", 10, White);
		DrawString(120, 24, "10", "", 10, White);
		//oggetto 3
		DrawString(10, 40, "Super Cura", "Papyrus", 10, White);
		DrawString(120, 40, "30", "", 10, White);
		
		Present();
		Clear();
		DrawBackground(b);
	}
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

bool haPresoDanno(int AttackX[], int AttackY, int PlayerX, int PlayerY, int i)
{
	if (PlayerY<AttackY + 4 && PlayerY>AttackY - 5) {
		if (PlayerX > AttackX[i] - 4 && PlayerX < AttackX[i] + 7) {
			return true;
		}
	}
	return false;
}
